#include <fstream>
#include <sstream>
#include <chrono>
#include <cassert>
#include <iostream>
#include <cassert>
#include <algorithm>

#ifdef USE_BOOST_CONVERT
    #include <boost/locale/encoding_utf.hpp>
#else
    #include <codecvt>
#endif

#include "utils.hpp"

#include <contrib/cityhash/city.h>


namespace NJamSpell {

std::string LoadFile(const std::string& fileName) {
    std::ifstream in(fileName, std::ios::binary);
    std::ostringstream out;
    out << in.rdbuf();
    return out.str();
}

void SaveFile(const std::string& fileName, const std::string& data) {
    std::ofstream out(fileName, std::ios::binary);
    out << data;
}

TTokenizer::TTokenizer()
    : Locale("en_US.utf-8")
{
}

bool TTokenizer::LoadAlphabet(const std::string& alphabetFile) {
    std::string data = LoadFile(alphabetFile);
    if (data.empty()) {
        return false;
    }
    std::wstring wdata = UTF8ToWide(data);
    if (wdata.empty()) {
        return false;
    }
    ToLower(wdata);
    std::unordered_set<wchar_t> alphabet;
    for (auto chr: wdata) {
        if (chr == 10 || chr == 13) {
            continue;
        }
        alphabet.insert(chr);
    }
    if (alphabet.empty()) {
        return false;
    }
    Alphabet = alphabet;
    return true;
}

TSentences TTokenizer::Process(const std::wstring& originalText) const {
    if (originalText.empty()) {
        return TSentences();
    }

    TSentences sentences;

    TWords currSentence;
    TWord currWord;

    for (size_t i = 0; i < originalText.size(); ++i) {
        wchar_t letter = std::tolower(originalText[i], Locale);
        if (Alphabet.find(letter) != Alphabet.end()) {
            if (currWord.Ptr == nullptr) {
                currWord.Ptr = &originalText[i];
            }
            currWord.Len += 1;
        } else {
            if (currWord.Ptr != nullptr) {
                currSentence.push_back(currWord);
                currWord = TWord();
            }
        }
        if (letter == L'?' || letter == L'!' || letter == L'.') {
            if (!currSentence.empty()) {
                sentences.push_back(currSentence);
                currSentence.clear();
            }
        }
    }
    if (currWord.Ptr != nullptr) {
        currSentence.push_back(currWord);
    }
    if (!currSentence.empty()) {
        sentences.push_back(currSentence);
    }

    return sentences;
}

void TTokenizer::Clear() {
    Alphabet.clear();
}

const std::unordered_set<wchar_t>& TTokenizer::GetAlphabet() const {
    return Alphabet;
}

std::wstring UTF8ToWide(const std::string& text) {
#ifdef USE_BOOST_CONVERT
    using boost::locale::conv::utf_to_utf;
    return utf_to_utf<wchar_t>(text.c_str(), text.c_str() + text.size());
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(text);
#endif
}

std::string WideToUTF8(const std::wstring& text) {
#ifdef USE_BOOST_CONVERT
    using boost::locale::conv::utf_to_utf;
    return utf_to_utf<char>(text.c_str(), text.c_str() + text.size());
#else
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(text);
#endif
}

uint64_t GetCurrentTimeMs() {
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return ms.count();
}

static const std::locale GLocale("en_US.UTF-8");
static const std::ctype<wchar_t>& GWctype = std::use_facet<std::ctype<wchar_t>>(GLocale);

void ToLower(std::wstring& text) {
    std::transform(text.begin(), text.end(), text.begin(), [](wchar_t wch) {
        return GWctype.tolower(wch);
    });
}

wchar_t MakeUpperIfRequired(wchar_t orig, wchar_t sample) {
    if (GWctype.toupper(sample) == sample) {
        return GWctype.toupper(orig);
    }
    return orig;
}

uint16_t CityHash16(const std::string& str) {
    uint32_t hash = ::CityHash32(&str[0], str.size());
    return hash % std::numeric_limits<uint16_t>::max();
}

uint16_t CityHash16(const char* str, size_t size) {
    uint32_t hash = ::CityHash32(str, size);
    return hash % std::numeric_limits<uint16_t>::max();
}

uint32_t CityHash32(const char *str, size_t size) {
    return ::CityHash32(str, size);
}

} // NJamSpell

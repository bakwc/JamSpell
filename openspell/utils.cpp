#include <fstream>
#include <sstream>
#include <codecvt>
#include <chrono>
#include <cassert>
#include <iostream>

#include "utils.hpp"


namespace NOpenSpell {

std::string LoadFile(const std::string& fileName) {
    std::ifstream in(fileName, std::ios::binary);
    std::ostringstream out;
    out << in.rdbuf();
    return out.str();
}

TTokenizer::TTokenizer()
    : Locale("en_US.utf-8")
{
}

void TTokenizer::LoadAlphabet(const std::string& alphabetFile) {
    std::string data = LoadFile(alphabetFile);
    std::wstring wdata = UTF8ToWide(data);
    Alphabet.clear();
    for (auto chr: wdata) {
        Alphabet.insert(chr);
    }
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

std::wstring UTF8ToWide(const std::string& text) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring res = converter.from_bytes(text);
    return res;
}

std::uint64_t GetCurrentTimeMs() {
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return ms.count();
}


} // NOpenSpell

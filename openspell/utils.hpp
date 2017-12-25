#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <locale>

#include "saveload.hpp"

namespace NOpenSpell {

struct TWord {
    TWord() = default;
    TWord(const wchar_t* ptr, size_t len)
        : Ptr(ptr)
        , Len(len)
    {
    }
    TWord(const std::wstring& w)
        : Ptr(&w[0])
        , Len(w.size())
    {
    }
    bool operator ==(const TWord& other) const {
        return (Ptr == other.Ptr && Len == other.Len);
    }
    const wchar_t* Ptr = nullptr;
    size_t Len = 0;
};

struct TWordHashPtr {
public:
  std::size_t operator()(const TWord& x) const {
      return (size_t)x.Ptr;
  }
};

using TWords = std::vector<TWord>;
using TSentences = std::vector<TWords>;

class TTokenizer {
public:
    TTokenizer();
    void LoadAlphabet(const std::string& alphabetFile);
    TSentences Process(const std::wstring& originalText) const;
    void Clear();

    const std::unordered_set<wchar_t>& GetAlphabet() const;

    SAVELOAD(Alphabet)
private:
    std::unordered_set<wchar_t> Alphabet;
    std::locale Locale;
};

std::string LoadFile(const std::string& fileName);
std::wstring UTF8ToWide(const std::string& text);
std::string WideToUTF8(const std::wstring& text);
std::uint64_t GetCurrentTimeMs();
void ToLower(std::wstring& text);
wchar_t MakeUpperIfRequired(wchar_t orig, wchar_t sample);

} // NOpenSpell

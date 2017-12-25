#pragma once

#include <unordered_map>
#include <vector>
#include <utility>
#include <string>

#include "saveload.hpp"
#include "utils.hpp"


namespace NOpenSpell {

const uint64_t MAGIC_BYTE = 8559322735408079685L;
const uint16_t VERSION = 4;
constexpr double DEFAULT_K = 0.05;

using TWordId = uint32_t;
using TCount = uint32_t;
using TGram1Key = TWordId;
using TGram2Key = std::pair<TWordId, TWordId>;
using TGram3Key = std::tuple<TWordId, TWordId, TWordId>;
using TWordIds = std::vector<TWordId>;
using TIdSentences = std::vector<TWordIds>;

struct TGram2KeyHash {
public:
  std::size_t operator()(const TGram2Key& x) const {
      return (size_t)x.first ^ ((size_t)x.second << 16);
  }
};

struct TGram3KeyHash {
public:
  std::size_t operator()(const TGram3Key& x) const {
    return (size_t)std::get<0>(x) ^
            ((size_t)std::get<1>(x) << 16) ^
            ((size_t)std::get<2>(x) << 32);
  }
};

class TLangModel {
public:
    void Train(const std::string& fileName, const std::string& alphabetFile);
    double Score(const TWords& words) const;
    double Score(const std::wstring& str) const;
    TWord GetWord(const std::wstring& word) const;
    const std::unordered_set<wchar_t>& GetAlphabet() const;
    TSentences Tokenize(const std::wstring& text) const;

    void Save(const std::string& modelFileName) const;
    bool Load(const std::string& modelFileName);
    void Clear();

    const std::unordered_map<std::wstring, TWordId>& GetWordToId();

    SAVELOAD(K, WordToId, LastWordID, TotalWords, Grams1, Grams2, Grams3, Tokenizer)
private:
    TIdSentences ConvertToIds(const TSentences& sentences);
    TWordId GetWordId(const TWord& word);
    TWordId GetWordId(const TWord& word) const;

    double GetGram1Prob(TWordId word) const;
    double GetGram2Prob(TWordId word1, TWordId word2) const;
    double GetGram3Prob(TWordId word1, TWordId word2, TWordId word3) const;

private:
    const TWordId UnknownWordId = std::numeric_limits<TWordId>::max();
    double K = DEFAULT_K;
    std::unordered_map<std::wstring, TWordId> WordToId;
    TWordId LastWordID = 0;
    TWordId TotalWords = 0;
    std::unordered_map<TGram1Key, TCount> Grams1;
    std::unordered_map<TGram2Key, TCount, TGram2KeyHash> Grams2;
    std::unordered_map<TGram3Key, TCount, TGram3KeyHash> Grams3;
    TTokenizer Tokenizer;
};


} // NOpenSpell

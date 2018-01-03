#pragma once

#include <unordered_map>
#include <vector>
#include <utility>
#include <string>
#include <limits>

#include <contrib/handypack/handypack.hpp>
#include "utils.hpp"
#include "perfect_hash.hpp"


namespace NOpenSpell {


constexpr uint64_t LANG_MODEL_MAGIC_BYTE = 8559322735408079685L;
constexpr uint16_t LANG_MODEL_VERSION = 11;
constexpr double LANG_MODEL_DEFAULT_K = 0.05;

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
    bool Train(const std::string& fileName, const std::string& alphabetFile);
    double Score(const TWords& words) const;
    double Score(const std::wstring& str) const;
    TWord GetWord(const std::wstring& word) const;
    const std::unordered_set<wchar_t>& GetAlphabet() const;
    TSentences Tokenize(const std::wstring& text) const;

    bool Dump(const std::string& modelFileName) const;
    bool Load(const std::string& modelFileName);
    void Clear();

    const std::unordered_map<std::wstring, TWordId>& GetWordToId();

    TWordId GetWordId(const TWord& word);
    TWordId GetWordIdNoCreate(const TWord& word) const;
    TWord GetWordById(TWordId wid) const;
    TCount GetWordCount(TWordId wid) const;

    uint64_t GetCheckSum() const;

    HANDYPACK(WordToId, LastWordID, TotalWords, VocabSize,
              PerfectHash, Buckets, NPlus1, NPlus2, Tokenizer, CheckSum)
private:
    TIdSentences ConvertToIds(const TSentences& sentences);

    double GetGram1Prob(TWordId word) const;
    double GetGram2Prob(TWordId word1, TWordId word2) const;
    double GetGram3Prob(TWordId word1, TWordId word2, TWordId word3) const;

    TCount GetGram1HashCount(TWordId word) const;
    TCount GetGram2HashCount(TWordId word1, TWordId word2) const;
    TCount GetGram3HashCount(TWordId word1, TWordId word2, TWordId word3) const;

    double PAbsDiscount1(TWordId word1) const;
    double PAbsDiscount2(TWordId word1, TWordId word2) const;
    double PAbsDiscount3(TWordId word1, TWordId word2, TWordId word3) const;

private:
    const TWordId UnknownWordId = std::numeric_limits<TWordId>::max();
    double K = LANG_MODEL_DEFAULT_K;
    std::unordered_map<std::wstring, TWordId> WordToId;
    std::vector<const std::wstring*> IdToWord;
    TWordId LastWordID = 0;
    TWordId TotalWords = 0;
    TWordId VocabSize = 0;
    TTokenizer Tokenizer;
    std::vector<std::pair<uint16_t, TCount>> Buckets;
    std::unordered_map<TWordId, TCount> NPlus1;
    std::unordered_map<TGram2Key, TCount, TGram2KeyHash> NPlus2;
    TPerfectHash PerfectHash;
    uint64_t CheckSum;
};


} // NOpenSpell

#include <algorithm>
#include <fstream>

#include "spell_corrector.hpp"

namespace NJamSpell {


static std::vector<std::wstring> GetDeletes1(const std::wstring& w) {
    std::vector<std::wstring> results;
    for (size_t i = 0; i < w.size(); ++i) {
        auto nw = w.substr(0, i) + w.substr(i+1);
        if (!nw.empty()) {
            results.push_back(nw);
        }
    }
    return results;
}

static std::vector<std::vector<std::wstring>> GetDeletes2(const std::wstring& w) {
    std::vector<std::vector<std::wstring>> results;
    for (size_t i = 0; i < w.size(); ++i) {
        auto nw = w.substr(0, i) + w.substr(i+1);
        if (!nw.empty()) {
            std::vector<std::wstring> currResults = GetDeletes1(nw);
            currResults.push_back(nw);
            results.push_back(currResults);
        }
    }
    return results;
}

bool TSpellCorrector::LoadLangModel(const std::string& modelFile) {
    if (!LangModel.Load(modelFile)) {
        return false;
    }
    std::string cacheFile = modelFile + ".spell";
    if (!LoadCache(cacheFile)) {
        PrepareCache();
        SaveCache(cacheFile);
    }
    return true;
}

bool TSpellCorrector::TrainLangModel(const std::string& textFile, const std::string& alphabetFile, const std::string& modelFile) {
    if (!LangModel.Train(textFile, alphabetFile)) {
        return false;
    }
    PrepareCache();
    if (!LangModel.Dump(modelFile)) {
        return false;
    }
    std::string cacheFile = modelFile + ".spell";
    if (!SaveCache(cacheFile)) {
        return false;
    }
    return true;
}

struct TScoredWord {
    TWord Word;
    double Score = 0;
};

TWords TSpellCorrector::GetCandidatesRaw(const TWords& sentence, size_t position) const {
    if (position >= sentence.size()) {
        return TWords();
    }

    TWord w = sentence[position];

    TWords candidates = Edits2(w);

    bool firstLevel = true;
    bool knownWord = false;
    if (candidates.empty()) {
        candidates = Edits(w);
        firstLevel = false;
    }

    if (candidates.empty()) {
        return candidates;
    }

    {
        TWord c = LangModel.GetWord(std::wstring(w.Ptr, w.Len));
        if (c.Ptr && c.Len) {
            w = c;
            candidates.push_back(c);
            knownWord = true;
        } else {
            candidates.push_back(w);
        }
    }

    std::unordered_set<TWord, TWordHashPtr> uniqueCandidates(candidates.begin(), candidates.end());

    FilterCandidatesByFrequency(uniqueCandidates, w);

    std::vector<TScoredWord> scoredCandidates;
    scoredCandidates.reserve(uniqueCandidates.size());

    for (TWord cand: uniqueCandidates) {
        TWords candSentence;
        for (size_t i = 0; i < sentence.size(); ++i) {
            if (i == position) {
                candSentence.push_back(cand);
            } else if ((i < position && i + 2 >= position) ||
                       (i > position && i <= position + 2))
            {
                candSentence.push_back(sentence[i]);
            }
        }

        TScoredWord scored;
        scored.Word = cand;
        scored.Score = LangModel.Score(candSentence);
        if (!(scored.Word == w)) {
            if (knownWord) {
                if (firstLevel) {
                    scored.Score -= KnownWordsPenalty;
                } else {
                    scored.Score *= 50.0;
                }
            } else {
                scored.Score -= UnknownWordsPenalty;
            }
        }
        scoredCandidates.push_back(scored);
    }

    std::sort(scoredCandidates.begin(), scoredCandidates.end(), [](TScoredWord w1, TScoredWord w2) {
        return w1.Score > w2.Score;
    });

    candidates.clear();
    for (auto s: scoredCandidates) {
        candidates.push_back(s.Word);
    }
    return candidates;
}

void TSpellCorrector::FilterCandidatesByFrequency(std::unordered_set<TWord, TWordHashPtr>& uniqueCandidates, TWord origWord) const {
    if (uniqueCandidates.size() <= MaxCandiatesToCheck) {
        return;
    }

    using TCountCand = std::pair<TCount, TWord>;
    std::vector<TCountCand> candidateCounts;
    for (auto&& c: uniqueCandidates) {
        TCount cnt = LangModel.GetWordCount(LangModel.GetWordIdNoCreate(c));
        candidateCounts.push_back(std::make_pair(cnt, c));
    }
    uniqueCandidates.clear();
    std::stable_sort(candidateCounts.begin(), candidateCounts.end(), [](const TCountCand& a, const TCountCand& b) {
        return a.first > b.first;
    });

    for (size_t i = 0; i < MaxCandiatesToCheck; ++ i) {
        uniqueCandidates.insert(candidateCounts[i].second);
    }
    uniqueCandidates.insert(origWord);
}

std::vector<std::wstring> TSpellCorrector::GetCandidates(const std::vector<std::wstring>& sentence, size_t position) const {
    TWords words;
    for (auto&& w: sentence) {
        words.push_back(TWord(w));
    }
    TWords candidates = GetCandidatesRaw(words, position);
    std::vector<std::wstring> results;
    for (auto&& c: candidates) {
        results.push_back(std::wstring(c.Ptr, c.Len));
    }
    return results;
}

std::wstring TSpellCorrector::FixFragment(const std::wstring& text) const {
    TSentences origSentences = LangModel.Tokenize(text);
    std::wstring lowered = text;
    ToLower(lowered);
    TSentences sentences = LangModel.Tokenize(lowered);
    std::wstring result;
    size_t origPos = 0;
    for (size_t i = 0; i < sentences.size(); ++i) {
        TWords words = sentences[i];
        const TWords& origWords = origSentences[i];
        for (size_t j = 0; j < words.size(); ++j) {
            TWord orig = origWords[j];
            TWord lowered = words[j];
            TWords candidates = GetCandidatesRaw(words, j);
            if (candidates.size() > 0) {
                words[j] = candidates[0];
            }
            size_t currOrigPos = orig.Ptr - &text[0];
            while (origPos < currOrigPos) {
                result.push_back(text[origPos]);
                origPos += 1;
            }
            std::wstring newWord = std::wstring(words[j].Ptr, words[j].Len);
            std::wstring origWord = std::wstring(orig.Ptr, orig.Len);
            std::wstring origLowered = std::wstring(lowered.Ptr, lowered.Len);
            if (newWord != origLowered) {
                for (size_t k = 0; k < newWord.size(); ++k) {
                    size_t n = k < origWord.size() ? k : origWord.size() - 1;
                    wchar_t newChar = newWord[k];
                    wchar_t origChar = origWord[n];
                    result.push_back(MakeUpperIfRequired(newChar, origChar));
                }
            } else {
                result += origWord;
            }
            origPos += orig.Len;
        }
    }
    while (origPos < text.size()) {
        result.push_back(text[origPos]);
        origPos += 1;
    }
    return result;
}

std::wstring TSpellCorrector::FixFragmentNormalized(const std::wstring& text) const {
    std::wstring lowered = text;
    ToLower(lowered);
    TSentences sentences = LangModel.Tokenize(lowered);
    std::wstring result;
    for (size_t i = 0; i < sentences.size(); ++i) {
        TWords words = sentences[i];
        for (size_t i = 0; i < words.size(); ++i) {
            TWords candidates = GetCandidatesRaw(words, i);
            if (candidates.size() > 0) {
                words[i] = candidates[0];
            }
            result += std::wstring(words[i].Ptr, words[i].Len) + L" ";
        }
        if (words.size() > 0) {
            result.resize(result.size() - 1);
            result += L". ";
        }
    }
    if (!result.empty()) {
        result.resize(result.size() - 1);
    }
    return result;
}

void TSpellCorrector::SetPenalty(double knownWordsPenaly, double unknownWordsPenalty) {
    KnownWordsPenalty = knownWordsPenaly;
    UnknownWordsPenalty = unknownWordsPenalty;
}

void TSpellCorrector::SetMaxCandiatesToCheck(size_t maxCandidatesToCheck) {
    MaxCandiatesToCheck = maxCandidatesToCheck;
}

template<typename T>
inline void AddVec(T& target, const T& source) {
    target.insert(target.end(), source.begin(), source.end());
}

TWords TSpellCorrector::Edits(const TWord& word) const {
    std::wstring w(word.Ptr, word.Len);
    TWords result;

    std::vector<std::vector<std::wstring>> cands = GetDeletes2(w);
    cands.push_back(std::vector<std::wstring>({w}));

    for (auto&& w1: cands) {
        for (auto&& w: w1) {
            TWord c = LangModel.GetWord(w);
            if (c.Ptr && c.Len) {
                result.push_back(c);
            }
            std::string s = WideToUTF8(w);
            if (Deletes1->Contains(s)) {
                Inserts(w, result);
            }
            if (Deletes2->Contains(s)) {
                Inserts2(w, result);
            }
        }
    }

    return result;
}

TWords TSpellCorrector::Edits2(const TWord& word, bool lastLevel) const {
    std::wstring w(word.Ptr, word.Len);
    TWords result;

    for (size_t i = 0; i < w.size() + 1; ++i) {
        // delete
        if (i < w.size()) {
            std::wstring s = w.substr(0, i) + w.substr(i+1);
            TWord c = LangModel.GetWord(s);
            if (c.Ptr && c.Len) {
                result.push_back(c);
            }
            if (!lastLevel) {
                AddVec(result, Edits2(TWord(s)));
            }
        }

        // transpose
        if (i + 1 < w.size()) {
            std::wstring s = w.substr(0, i);
            s += w.substr(i + 1, 1);
            s += w.substr(i, 1);
            if (i + 2 < w.size()) {
                s += w.substr(i+2);
            }
            TWord c = LangModel.GetWord(s);
            if (c.Ptr && c.Len) {
                result.push_back(c);
            }
            if (!lastLevel) {
                AddVec(result, Edits2(TWord(s)));
            }
        }

        // replace
        if (i < w.size()) {
            for (auto&& ch: LangModel.GetAlphabet()) {
                std::wstring s = w.substr(0, i) + ch + w.substr(i+1);
                TWord c = LangModel.GetWord(s);
                if (c.Ptr && c.Len) {
                    result.push_back(c);
                }
                if (!lastLevel) {
                    AddVec(result, Edits2(TWord(s)));
                }
            }
        }

        // inserts
        {
            for (auto&& ch: LangModel.GetAlphabet()) {
                std::wstring s = w.substr(0, i) + ch + w.substr(i);
                TWord c = LangModel.GetWord(s);
                if (c.Ptr && c.Len) {
                    result.push_back(c);
                }
                if (!lastLevel) {
                    AddVec(result, Edits2(TWord(s)));
                }
            }
        }
    }

    return result;
}

void TSpellCorrector::Inserts(const std::wstring& w, TWords& result) const {
    for (size_t i = 0; i < w.size() + 1; ++i) {
        for (auto&& ch: LangModel.GetAlphabet()) {
            std::wstring s = w.substr(0, i) + ch + w.substr(i);
            TWord c = LangModel.GetWord(s);
            if (c.Ptr && c.Len) {
                result.push_back(c);
            }
        }
    }
}

void TSpellCorrector::Inserts2(const std::wstring& w, TWords& result) const {
    for (size_t i = 0; i < w.size() + 1; ++i) {
        for (auto&& ch: LangModel.GetAlphabet()) {
            std::wstring s = w.substr(0, i) + ch + w.substr(i);
            if (Deletes1->Contains(WideToUTF8(s))) {
                Inserts(s, result);
            }
        }
    }
}

void TSpellCorrector::PrepareCache() {
    auto&& wordToId = LangModel.GetWordToId();
    size_t n = 0;
    size_t s = 0;
    for (auto&& it: wordToId) {
        n += 1;
        s += it.first.size();
        if (n > 3000) {
            break;
        }
    }
    size_t avgWordLen = std::max(int(double(s) / n) + 1, 1);
    size_t avgWordLenMinusOne = std::max(size_t(1), avgWordLen - 1);

    uint64_t deletes1size = wordToId.size() * avgWordLen;
    uint64_t deletes2size = wordToId.size() * avgWordLen * avgWordLenMinusOne;
    deletes1size = std::max(uint64_t(1000), deletes1size);
    deletes1size = std::max(uint64_t(1000), deletes1size);

    double falsePositiveProb = 0.001;
    Deletes1.reset(new TBloomFilter(deletes1size, falsePositiveProb));
    Deletes2.reset(new TBloomFilter(deletes2size, falsePositiveProb));

    uint64_t deletes1real = 0;
    uint64_t deletes2real = 0;

    for (auto&& it: wordToId) {
        auto deletes = GetDeletes2(it.first);
        for (auto&& w1: deletes) {
            Deletes1->Insert(WideToUTF8(w1.back()));
            deletes1real += 1;
            for (size_t i = 0; i < w1.size() - 1; ++i) {
                Deletes2->Insert(WideToUTF8(w1[i]));
                deletes2real += 1;
            }
        }
    }
}

constexpr uint64_t SPELL_CHECKER_CACHE_MAGIC_BYTE = 3811558393781437494L;
constexpr uint16_t SPELL_CHECKER_CACHE_VERSION = 1;

bool TSpellCorrector::LoadCache(const std::string& cacheFile) {
    std::ifstream in(cacheFile, std::ios::binary);
    if (!in.is_open()) {
        return false;
    }
    uint16_t version = 0;
    uint64_t magicByte = 0;
    NHandyPack::Load(in, magicByte);
    if (magicByte != SPELL_CHECKER_CACHE_MAGIC_BYTE) {
        return false;
    }
    NHandyPack::Load(in, version);
    if (version != SPELL_CHECKER_CACHE_VERSION) {
        return false;
    }
    uint64_t checkSum = 0;
    NHandyPack::Load(in, checkSum);
    if (checkSum != LangModel.GetCheckSum()) {
        return false;
    }
    std::unique_ptr<TBloomFilter> deletes1(new TBloomFilter());
    std::unique_ptr<TBloomFilter> deletes2(new TBloomFilter());
    deletes1->Load(in);
    deletes2->Load(in);
    magicByte = 0;
    NHandyPack::Load(in, magicByte);
    if (magicByte != SPELL_CHECKER_CACHE_MAGIC_BYTE) {
        return false;
    }
    Deletes1 = std::move(deletes1);
    Deletes2 = std::move(deletes2);
    return true;
}

bool TSpellCorrector::SaveCache(const std::string& cacheFile) {
    std::ofstream out(cacheFile, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }
    if (!Deletes1 || !Deletes2) {
        return false;
    }
    NHandyPack::Dump(out, SPELL_CHECKER_CACHE_MAGIC_BYTE);
    NHandyPack::Dump(out, SPELL_CHECKER_CACHE_VERSION);
    NHandyPack::Dump(out, LangModel.GetCheckSum());
    Deletes1->Dump(out);
    Deletes2->Dump(out);
    NHandyPack::Dump(out, SPELL_CHECKER_CACHE_MAGIC_BYTE);
    return true;
}


} // NJamSpell

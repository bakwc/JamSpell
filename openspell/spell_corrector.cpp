#include "spell_corrector.hpp"

namespace NOpenSpell {


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

static std::vector<std::wstring> GetDeletes2(const std::wstring& w) {
    std::vector<std::wstring> results;
    for (size_t i = 0; i < w.size(); ++i) {
        auto nw = w.substr(0, i) + w.substr(i+1);
        if (!nw.empty()) {
            std::vector<std::wstring> currResults = GetDeletes1(nw);
            results.insert(results.end(), currResults.begin(), currResults.end());
        }
    }
    return results;
}

bool TSpellCorrector::LoadLangModel(const std::string& modelFile) {
    if (!LangModel.Load(modelFile)) {
        return false;
    }

    auto&& wordToId = LangModel.GetWordToId();
    for (auto&& it: wordToId) {
        TWord wid(it.first);
        auto deletes1 = GetDeletes1(it.first);
        auto deletes2 = GetDeletes2(it.first);
        for (auto&& w: deletes1) {
            Deletes1[w].insert(wid);
        }
        for (auto&& w: deletes2) {
            Deletes2[w].insert(wid);
        }
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
    TWords candidates = Edits(w);
    bool firstLevel = true;
    if (candidates.empty()) {
        candidates = Edits(w, false);
        firstLevel = false;
    }

    {
        TWord c = LangModel.GetWord(std::wstring(w.Ptr, w.Len));
        if (c.Ptr && c.Len) {
            w = c;
            candidates.push_back(c);
        }
    }

    if (candidates.empty()) {
        return candidates;
    }

    std::unordered_set<TWord, TWordHashPtr> uniqueCandidates(candidates.begin(), candidates.end());

    std::vector<TScoredWord> scoredCandidates;
    scoredCandidates.reserve(candidates.size());
    for (TWord cand: uniqueCandidates) {
        TWords candSentence;
        for (size_t i = 0; i < sentence.size(); ++i) {
            if (i == position) {
                candSentence.push_back(cand);
            } else {
                candSentence.push_back(sentence[i]);
            }
        }

        TScoredWord scored;
        scored.Word = cand;
        scored.Score = LangModel.Score(candSentence);
        if (!(scored.Word == w)) {
            if (firstLevel) {
                scored.Score *= 1.045;
            } else {
                scored.Score *= 50.0;
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

template<typename T>
inline void AddVec(T& target, const T& source) {
    target.insert(target.end(), source.begin(), source.end());
}

TWords TSpellCorrector::Edits(const TWord& word, bool lastLevel) const {
    std::wstring w(word.Ptr, word.Len);
    TWords result;

    std::vector<std::wstring> cands = GetDeletes1(w);
    cands.push_back(w);
    if (!lastLevel) {
        std::vector<std::wstring> deletes2 = GetDeletes2(w);
        cands.insert(cands.end(), deletes2.begin(), deletes2.end());
    }

    for (auto&& w: cands) {
        TWord c = LangModel.GetWord(w);
        if (c.Ptr && c.Len) {
            result.push_back(c);
        }
        auto it = Deletes1.find(w);
        if (it != Deletes1.end()) {
            for (auto c1:it->second) {
                result.push_back(c1);
            }
        }
        if (!lastLevel) {
            it = Deletes2.find(w);
            if (it != Deletes2.end()) {
                for (auto c1:it->second) {
                    result.push_back(c1);
                }
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
                AddVec(result, Edits(TWord(s)));
            }
        }

        // transpose
        if (i + 2 < w.size()) {
            std::wstring s = w.substr(0, i);
            s += w.substr(i + 1, 1);
            s += w.substr(i, 1);
            if (i + 3 < w.size()) {
                s += w.substr(i+2);
            }
            TWord c = LangModel.GetWord(s);
            if (c.Ptr && c.Len) {
                result.push_back(c);
            }
            if (!lastLevel) {
                AddVec(result, Edits(TWord(s)));
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
                    AddVec(result, Edits(TWord(s)));
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
                    AddVec(result, Edits(TWord(s)));
                }
            }
        }
    }

    return result;
}


} // NOpenSpell

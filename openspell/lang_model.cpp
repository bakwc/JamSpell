#include <iostream>
#include <cassert>
#include <fstream>

#include "lang_model.hpp"


namespace NOpenSpell {


void TLangModel::Train(const std::string& fileName, const std::string& alphabetFile) {
    std::cerr << "[info] loading text" << std::endl;
    Tokenizer.LoadAlphabet(alphabetFile);
    std::wstring trainText = UTF8ToWide(LoadFile(fileName));
    TSentences sentences = Tokenizer.Process(trainText);
    TIdSentences sentenceIds = ConvertToIds(sentences);

    assert(sentences.size() == sentenceIds.size());

    std::cerr << "[info] generating N-grams" << std::endl;
    uint64_t lastTime = GetCurrentTimeMs();
    size_t total = sentenceIds.size();
    for (size_t i = 0; i < total; ++i) {
        const TWordIds& words = sentenceIds[i];
        for (auto&& w: words) {
            Grams1[w] += 1;
            TotalWords += 1;
        }
        for (size_t j = 0; j < words.size() - 1; ++j) {
            TGram2Key key(words[j], words[j+1]);
            Grams2[key] += 1;
        }
        for (size_t j = 0; j < words.size() - 2; ++j) {
            TGram3Key key(words[j], words[j+1], words[j+2]);
            Grams3[key] += 1;
        }
        uint64_t currTime = GetCurrentTimeMs();
        if (currTime - lastTime > 4000) {
            std::cerr << "[info] processed " << (100.0 * float(i) / float(total));
            lastTime = currTime;
        }
    }

    std::cerr << "[info] finished training" << std::endl;
}

double TLangModel::Score(const TWords& words) const {
    TWordIds sentence;
    for (auto&& w: words) {
        sentence.push_back(GetWordId(w));
    }
    sentence.push_back(UnknownWordId);
    sentence.push_back(UnknownWordId);

    double result = 0;
    for (size_t i = 0; i < sentence.size() - 2; ++i) {
        result += log(GetGram1Prob(sentence[i]));
        result += log(GetGram2Prob(sentence[i], sentence[i + 1]));
        result += log(GetGram3Prob(sentence[i], sentence[i + 1], sentence[i + 2]));
    }
    return result;
}

void TLangModel::Save(const std::string& modelFileName) const {
    std::ofstream out(modelFileName, std::ios::binary);
    Save(out);
}

void TLangModel::Load(const std::string& modelFileName) {
    std::ifstream in(modelFileName, std::ios::binary);
    Load(in);
}

TIdSentences TLangModel::ConvertToIds(const TSentences& sentences) {
    TIdSentences newSentences;
    for (size_t i = 0; i < sentences.size(); ++i) {
        const TWords& words = sentences[i];
        TWordIds wordIds;
        for (size_t j = 0; j < words.size(); ++j) {
            const TWord& word = words[i];
            wordIds.push_back(GetWordId(word));
        }
        newSentences.push_back(wordIds);
    }
    return newSentences;
}

TWordId TLangModel::GetWordId(const TWord& word) {
    std::wstring w(word.Ptr, word.Len);
    auto it = WordToId.find(w);
    if (it != WordToId.end()) {
        return it->second;
    }
    TWordId wordId = LastWordID;
    ++LastWordID;
    WordToId[w] = wordId;
    return wordId;
}

TWordId TLangModel::GetWordId(const TWord& word) const {
    std::wstring w(word.Ptr, word.Len);
    auto it = WordToId.find(w);
    if (it != WordToId.end()) {
        return it->second;
    }
    return UnknownWordId;
}

double TLangModel::GetGram1Prob(TWordId word) const {
    double countsGram1 = 0;
    auto it = Grams1.find(word);
    if (it != Grams1.end()) {
        countsGram1 = it->second;
    }
    countsGram1 += K;
    double vocabSize = Grams1.size();
    return countsGram1 / (TotalWords + vocabSize);
}

double TLangModel::GetGram2Prob(TWordId word1, TWordId word2) const {
    double countsGram1 = 0;
    auto it = Grams1.find(word1);
    if (it != Grams1.end()) {
        countsGram1 = it->second;
    }
    countsGram1 += TotalWords;
    double countsGram2 = 0;
    auto jt = Grams2.find(TGram2Key(word1, word2));
    if (jt != Grams2.end()) {
        countsGram2 = jt->second;
    }
    countsGram2 += K;
    return countsGram2 / countsGram1;
}

double TLangModel::GetGram3Prob(TWordId word1, TWordId word2, TWordId word3) const {
    double countsGram2 = 0;
    auto it = Grams2.find(TGram2Key(word1, word2));
    if (it != Grams2.end()) {
        countsGram2 = it->second;
    }
    countsGram2 += TotalWords;
    double countsGram3 = 0;
    auto jt = Grams3.find(TGram3Key(word1, word2, word3));
    if (jt != Grams3.end()) {
        countsGram3 = jt->second;
    }
    countsGram3 += K;
    return countsGram3 / countsGram2;
}

} // NOpenSpell

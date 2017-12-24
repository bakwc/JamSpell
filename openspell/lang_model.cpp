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


} // NOpenSpell

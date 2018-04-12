#pragma once

#include <memory>

#include "lang_model.hpp"
#include "bloom_filter.hpp"

namespace NJamSpell {


class TSpellCorrector {
public:
    bool LoadLangModel(const std::string& modelFile);
    bool TrainLangModel(const std::string& textFile, const std::string& alphabetFile, const std::string& modelFile);
    NJamSpell::TWords GetCandidatesRaw(const NJamSpell::TWords& sentence, size_t position) const;
    std::vector<std::wstring> GetCandidates(const std::vector<std::wstring>& sentence, size_t position) const;
    std::wstring FixFragment(const std::wstring& text) const;
    std::wstring FixFragmentNormalized(const std::wstring& text) const;
    void SetPenalty(double knownWordsPenaly, double unknownWordsPenalty);
    void SetMaxCandiatesToCheck(size_t maxCandidatesToCheck);
private:
    void FilterCandidatesByFrequency(std::unordered_set<NJamSpell::TWord, NJamSpell::TWordHashPtr>& uniqueCandidates, NJamSpell::TWord origWord) const;
    NJamSpell::TWords Edits(const NJamSpell::TWord& word) const;
    NJamSpell::TWords Edits2(const NJamSpell::TWord& word, bool lastLevel = true) const;
    void Inserts(const std::wstring& w, NJamSpell::TWords& result) const;
    void Inserts2(const std::wstring& w, NJamSpell::TWords& result) const;
    void PrepareCache();
    bool LoadCache(const std::string& cacheFile);
    bool SaveCache(const std::string& cacheFile);
private:
    TLangModel LangModel;
    std::unique_ptr<TBloomFilter> Deletes1;
    std::unique_ptr<TBloomFilter> Deletes2;
    double KnownWordsPenalty = 20.0;
    double UnknownWordsPenalty = 5.0;
    size_t MaxCandiatesToCheck = 14;
};


} // NJamSpell

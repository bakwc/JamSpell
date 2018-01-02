#pragma once

#include <memory>

#include "lang_model.hpp"
#include "bloom_filter.hpp"

namespace NOpenSpell {


class TSpellCorrector {
public:
    bool LoadLangModel(const std::string& modelFile);
    bool TrainLangModel(const std::string& textFile, const std::string& alphabetFile);
    NOpenSpell::TWords GetCandidatesRaw(const NOpenSpell::TWords& sentence, size_t position) const;
    std::vector<std::wstring> GetCandidates(const std::vector<std::wstring>& sentence, size_t position) const;
    std::wstring FixFragment(const std::wstring& text) const;
    std::wstring FixFragmentNormalized(const std::wstring& text) const;
private:
    NOpenSpell::TWords Edits(const NOpenSpell::TWord& word) const;
    NOpenSpell::TWords Edits2(const NOpenSpell::TWord& word, bool lastLevel = true) const;
    void Inserts(const std::wstring& w, NOpenSpell::TWords& result) const;
    void Inserts2(const std::wstring& w, NOpenSpell::TWords& result) const;
    void PrepareCache();
    bool LoadCache(const std::string& cacheFile);
    bool SaveCache(const std::string& cacheFile);
private:
    TLangModel LangModel;
    std::unique_ptr<TBloomFilter> Deletes1;
    std::unique_ptr<TBloomFilter> Deletes2;
};


} // NOpenSpell

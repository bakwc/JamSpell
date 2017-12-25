#pragma once

#include "lang_model.hpp"

namespace NOpenSpell {


class TSpellCorrector {
public:
    bool LoadLangModel(const std::string& modelFile);
    TWords GetCandidatesRaw(const TWords& sentence, size_t position) const;
    std::vector<std::wstring> GetCandidates(const std::vector<std::wstring>& sentence, size_t position) const;
    std::wstring FixFragment(const std::wstring& text) const;
    std::wstring FixFragmentNormalized(const std::wstring& text) const;
private:
    TWords Edits(const TWord& word, bool lastLevel = true) const;
    TWords Edits2(const TWord& word, bool lastLevel = true) const;
private:
    TLangModel LangModel;
    std::unordered_map<std::wstring, std::unordered_set<TWord, TWordHashPtr>> Deletes1;
    std::unordered_map<std::wstring, std::unordered_set<TWord, TWordHashPtr>> Deletes2;
};


} // NOpenSpell

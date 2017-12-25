#pragma once

#include "lang_model.hpp"

namespace NOpenSpell {


class TSpellCorrector {
public:
    bool LoadLangModel(const std::string& modelFile);
    TWords Correct(const TWords& sentence, size_t position) const;
    std::wstring Correct(const std::wstring& text) const;
    std::wstring CorrectNormalize(const std::wstring& text) const;
private:
    TWords Edits(const TWord& word, bool lastLevel = true) const;
private:
    TLangModel LangModel;
};


} // NOpenSpell

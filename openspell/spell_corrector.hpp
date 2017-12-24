#include "lang_model.hpp"

namespace NOpenSpell {


class TSpellCorrector {
public:
    bool LoadLangModel(const std::string& modelFile);
    TWords Correct(const TWords& sentence, size_t position);
private:
    TWords Edits(const TWord& word, bool lastLevel = true);
private:
    TLangModel LangModel;
};


} // NOpenSpell

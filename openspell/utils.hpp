#include <string>
#include <vector>
#include <unordered_set>
#include <locale>

#include "saveload.hpp"

namespace NOpenSpell {

struct TWord {
    const wchar_t* Ptr = nullptr;
    size_t Len = 0;
};

using TWords = std::vector<TWord>;
using TSentences = std::vector<TWords>;

class TTokenizer {
public:
    TTokenizer();
    void LoadAlphabet(const std::string& alphabetFile);
    TSentences Process(const std::wstring& originalText) const;

    SAVELOAD(Alphabet)
private:
    std::unordered_set<wchar_t> Alphabet;
    std::locale Locale;
};

std::string LoadFile(const std::string& fileName);
std::wstring UTF8ToWide(const std::string& text);
std::uint64_t GetCurrentTimeMs();

} // NOpenSpell

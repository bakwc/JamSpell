#include <unordered_map>
#include <vector>
#include <utility>
#include <string>

namespace NOpenSpell {

using TWordId = uint32_t;
using TCount = uint32_t;
using TGram1Key = TWordId;
using TGram2Key = std::pair<TWordId, TWordId>;
using TGram3Key = std::tuple<TWordId, TWordId, TWordId>;

struct TGram2KeyHash {
public:
  std::size_t operator()(const TGram2Key& x) const {
    return std::hash<TWordId>()(x.first) ^
           std::hash<TWordId>()(x.second);
  }
};

struct TGram3KeyHash {
public:
  std::size_t operator()(const TGram3Key& x) const {
    return std::hash<TWordId>()(std::get<0>(x)) ^
           std::hash<TWordId>()(std::get<1>(x)) ^
           std::hash<TWordId>()(std::get<2>(x));
  }
};

class TLangModel {
public:
private:
    std::unordered_map<std::string, TWordId> WordToId;
    std::vector<std::string*> IdToWord;
    TWordId LastWordID;
    TWordId TotalWords;
    std::unordered_map<TGram1Key, TCount> Grams1;
    std::unordered_map<TGram2Key, TCount, TGram2KeyHash> Grams2;
    std::unordered_map<TGram3Key, TCount, TGram3KeyHash> Grams3;
};

} // NOpenSpell

#pragma once

#include <ostream>

namespace NOpenSpell {

class TPerfectHash {
public:
    TPerfectHash();
    TPerfectHash(const TPerfectHash& other) = delete;
    ~TPerfectHash();
    void Save(std::ostream& out) const;
    void Load(std::istream& in);
    void Init(const std::vector<std::string>& keys);
    uint32_t Hash(const std::string& value) const;
    uint32_t BucketsNumber() const;
private:
    void* Phf; // sort of forward declaration
};

} // NOpenSpell

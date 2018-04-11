#pragma once

#include <ostream>

namespace NJamSpell {

class TPerfectHash {
public:
    TPerfectHash();
    TPerfectHash(const TPerfectHash& other) = delete;
    ~TPerfectHash();
    void Dump(std::ostream& out) const;
    void Load(std::istream& in);
    bool Init(const std::vector<std::string>& keys);
    void Clear();
    uint32_t Hash(const std::string& value) const;
    uint32_t Hash(const char* value, size_t size) const;
    uint32_t BucketsNumber() const;
private:
    void* Phf; // sort of forward declaration
};

} // NJamSpell

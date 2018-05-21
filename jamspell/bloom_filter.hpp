#pragma once
#include <memory>
#include <string>

namespace NJamSpell {

class TBloomFilter {
public:
    TBloomFilter();
    TBloomFilter(uint64_t elements, double falsePositiveRate);
    ~TBloomFilter();
    void Insert(const std::string& element);
    bool Contains(const std::string& element) const;
    void Dump(std::ostream& out) const;
    void Load(std::istream& in);
private:
    struct Impl;
    std::unique_ptr<Impl> BloomFilter;
};

} // NJamSpell

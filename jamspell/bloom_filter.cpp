#include <cassert>

#include "bloom_filter.hpp"

#include <contrib/bloom/bloom_filter.hpp>
#include <contrib/handypack/handypack.hpp>

namespace NJamSpell {

struct TBloomFilter::Impl: public bloom_filter {
    Impl(): bloom_filter() {}
    Impl(const bloom_parameters& params): bloom_filter(params) {}
    Impl(const TBloomFilter::Impl& bloomFilter): bloom_filter(bloomFilter) {}
    ~Impl() {}
    void Dump(std::ostream& out) const {
        NHandyPack::Dump(out, salt_, bit_table_, salt_count_, table_size_,
                        projected_element_count_, inserted_element_count_,
                        random_seed_, desired_false_positive_probability_);
    }
    void Load(std::istream& in) {
        NHandyPack::Load(in, salt_, bit_table_, salt_count_, table_size_,
                        projected_element_count_, inserted_element_count_,
                        random_seed_, desired_false_positive_probability_);
    }
};

TBloomFilter::TBloomFilter() {
    BloomFilter.reset(new TBloomFilter::Impl());
}

TBloomFilter::TBloomFilter(uint64_t elements, double falsePositiveRate) {
    bloom_parameters parameters;
    parameters.projected_element_count = elements;
    parameters.false_positive_probability = falsePositiveRate;
    parameters.random_seed = 42;
    parameters.compute_optimal_parameters();
    assert(!(!parameters));
    BloomFilter.reset(new TBloomFilter::Impl(parameters));
}

TBloomFilter::~TBloomFilter() {
}

void TBloomFilter::Insert(const std::string& element) {
    BloomFilter->insert(element);
}

bool TBloomFilter::Contains(const std::string& element) const {
    return BloomFilter->contains(element);
}

void TBloomFilter::Dump(std::ostream& out) const {
    BloomFilter->Dump(out);
}

void TBloomFilter::Load(std::istream& in) {
    BloomFilter->Load(in);
}

} // NJamSpell

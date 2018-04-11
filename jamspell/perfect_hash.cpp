#include <contrib/handypack/handypack.hpp>
#include <contrib/phf/phf.h>

#include "perfect_hash.hpp"

#include <cassert>

namespace NJamSpell {

void TPerfectHash::Dump(std::ostream& out) const {
    const phf& perfHash = *(const phf*)Phf;
    NHandyPack::Dump(out, perfHash.d_max,
                         perfHash.g_op,
                         perfHash.m,
                         perfHash.r,
                         perfHash.seed,
                         perfHash.nodiv);
    out.write((const char*)perfHash.g, perfHash.r * sizeof(uint32_t));
}

void TPerfectHash::Load(std::istream& in) {
    Clear();
    Phf = new phf();
    phf& perfHash = *(phf*)Phf;
    NHandyPack::Load(in, perfHash.d_max,
                        perfHash.g_op,
                        perfHash.m,
                        perfHash.r,
                        perfHash.seed,
                        perfHash.nodiv);
    perfHash.g = (uint32_t*)calloc(perfHash.r, sizeof(uint32_t));
    in.read((char*)perfHash.g, perfHash.r * sizeof(uint32_t));
}

bool TPerfectHash::Init(const std::vector<std::string>& keys) {
    std::vector<phf_string_t> keysForPhf;
    keysForPhf.reserve(keys.size());
    for (const std::string& s: keys) {
        keysForPhf.push_back({&s[0], s.size()});
    }

    phf* tempPhf = new phf();
    phf_error_t res = PHF::init<phf_string_t, false>(tempPhf, &keysForPhf[0], keysForPhf.size(), 4, 80, 42);
    if (res != 0) {
        PHF::destroy(tempPhf);
        delete tempPhf;
        return false;
    }
    Clear();
    Phf = tempPhf;
    return true;
}

void TPerfectHash::Clear() {
    if (!Phf) {
        return;
    }
    PHF::destroy((phf*)Phf);
    delete (phf*)Phf;
}

uint32_t TPerfectHash::Hash(const std::string& value) const {
    assert(Phf && "Not initialized");
    return PHF::hash<std::string>((phf*)Phf, value);
}

uint32_t TPerfectHash::Hash(const char* value, size_t size) const {
    assert(Phf && "Not initialized");
    phf_string_t phfValue = {value, size};
    return PHF::hash<phf_string_t>((phf*)Phf, phfValue);
}

uint32_t TPerfectHash::BucketsNumber() const {
    const phf* p = (phf*)Phf;
    return p->m;
}

TPerfectHash::TPerfectHash()
    : Phf(nullptr)
{
}

TPerfectHash::~TPerfectHash() {
    Clear();
}

} // NJamSpell

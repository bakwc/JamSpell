#include <contrib/saveload/saveload.hpp>
#include <contrib/phf/phf.h>

#include "perfect_hash.hpp"

namespace NOpenSpell {

void TPerfectHash::Save(std::ostream& out) const {
    const phf& perfHash = *(const phf*)Phf;
    NSaveLoad::Save(out, perfHash.d_max,
                         perfHash.g_op,
                         perfHash.m,
                         perfHash.r,
                         perfHash.seed,
                         perfHash.nodiv);
    out.write((const char*)perfHash.g, perfHash.r * sizeof(uint32_t));
}

void TPerfectHash::Load(std::istream& in) {
    Phf = new phf();
    phf& perfHash = *(phf*)Phf;
    NSaveLoad::Load(in, perfHash.d_max,
                        perfHash.g_op,
                        perfHash.m,
                        perfHash.r,
                        perfHash.seed,
                        perfHash.nodiv);
    perfHash.g = (uint32_t*)calloc(perfHash.r, sizeof(uint32_t));
    in.read((char*)perfHash.g, perfHash.r * sizeof(uint32_t));
}

void TPerfectHash::Init(const std::vector<std::string>& keys) {
    Phf = new phf();
    phf_error_t res = PHF::init<std::string, false>((phf*)Phf, &keys[0], keys.size(), 4, 80, 42);
    std::cerr << " Train result: " << res << "\n";
}

uint32_t TPerfectHash::Hash(const std::string& value) const {
    return PHF::hash<std::string>((phf*)Phf, value);
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
    if (!Phf) {
        return;
    }
    PHF::destroy((phf*)Phf);
    delete (phf*)Phf;
}

} // NOpenSpell

#include <gtest/gtest.h>

#include <jamspell/perfect_hash.hpp>
#include <contrib/handypack/handypack.hpp>

TEST(PerfetHashTest, basicFlow) {

    NJamSpell::TPerfectHash ph;
    std::vector<std::string> keys = {
        "key1",
        "key2",
        "key3",
        "abc",
        "654321",
    };
    ph.Init(keys);

    ASSERT_TRUE(ph.BucketsNumber() < uint32_t(2.0 * keys.size()));
    std::set<size_t> backetsUsed;

    for (auto&& s: keys) {
        backetsUsed.insert(ph.Hash(s));
    }

    ASSERT_EQ(keys.size(), backetsUsed.size());

    std::string serialized;
    {
        std::stringbuf buf;
        std::ostream out(&buf);
        ph.Dump(out);
        serialized = buf.str();
    }

    NJamSpell::TPerfectHash ph2;
    {
        NHandyPack::imemstream in(&serialized[0], serialized.size());
        ph2.Load(in);
    }

    backetsUsed.clear();
    for (auto&& s: keys) {
        backetsUsed.insert(ph2.Hash(s));
    }
    ASSERT_EQ(keys.size(), backetsUsed.size());
}

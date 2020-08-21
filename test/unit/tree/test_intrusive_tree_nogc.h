#ifndef CDSUNIT_TREE_TEST_INTRUSIVE_TREE_NOGC_H
#define CDSUNIT_TREE_TEST_INTRUSIVE_TREE_NOGC_H

#include "test_intrusive_tree.h"
#include "cds_test/fixture.h"
#include <iostream>
namespace cds
{
namespace intrusive
{
}
} // namespace cds

namespace cds_test
{

namespace ci = cds::intrusive;

class intrusive_tree_nogc : public intrusive_tree
{
    typedef intrusive_tree base_class;

  protected:
    template <class Tree>
    void test(Tree &t)
    {
        ASSERT_TRUE(t.empty());
        ASSERT_CONTAINER_SIZE(t, 0);
        size_t const nTreeSize = kSize;

        typedef typename Tree::value_type value_type;

        std::vector<value_type> data;
        std::vector<size_t> indices;

        data.reserve(kSize);
        indices.reserve(kSize);
        for (size_t key = 0; key < kSize; ++key)
        {
            data.push_back(value_type(static_cast<int>(key)));
            indices.push_back(key);
        }
        shuffle(indices.begin(), indices.end());
        // insert/find
        for (auto idx : indices)
        {
            auto &i = data[idx];

            ASSERT_FALSE(t.contains(i.nKey));
            ASSERT_FALSE(t.contains(i));
            ASSERT_FALSE(t.contains(other_item(i.key()), other_less()));
            ASSERT_FALSE(t.find(i.nKey, [](value_type &, int) {}));
            ASSERT_FALSE(t.find_with(other_item(i.key()), other_less(), [](value_type &, other_item const &) {}));

            std::pair<bool, bool> updResult;

            updResult = t.update(i, [](bool, value_type &, value_type &) {
                ASSERT_TRUE(false);
            },
                                 false);
            EXPECT_FALSE(updResult.first);
            EXPECT_FALSE(updResult.second);

            switch (i.key() % 3)
            {
            case 0:
                ASSERT_TRUE(t.insert(i));
                ASSERT_FALSE(t.insert(i));
                updResult = t.update(i, [](bool bNew, value_type &val, value_type &arg) {
                    EXPECT_FALSE(bNew);
                    EXPECT_EQ(&val, &arg);
                },
                                     false);
                EXPECT_TRUE(updResult.first);
                EXPECT_FALSE(updResult.second);
                break;
            case 1:
                EXPECT_EQ(i.nUpdateNewCount, 0u);
                ASSERT_TRUE(t.insert(i, [](value_type &v) { ++v.nUpdateNewCount; }));
                EXPECT_EQ(i.nUpdateNewCount, 1u);
                ASSERT_FALSE(t.insert(i, [](value_type &v) { ++v.nUpdateNewCount; }));
                EXPECT_EQ(i.nUpdateNewCount, 1u);
                i.nUpdateNewCount = 0;
                break;
            case 2:
                updResult = t.update(i, [](bool, value_type &, value_type &) {
                    EXPECT_TRUE(false);
                },
                                     false);
                EXPECT_FALSE(updResult.first);
                EXPECT_FALSE(updResult.second);

                EXPECT_EQ(i.nUpdateNewCount, 0u);
                updResult = t.update(i, [](bool bNew, value_type &val, value_type &arg) {
                    EXPECT_TRUE(bNew);
                    EXPECT_EQ(&val, &arg);
                    ++val.nUpdateNewCount;
                });
                EXPECT_TRUE(updResult.first);
                EXPECT_TRUE(updResult.second);
                EXPECT_EQ(i.nUpdateNewCount, 1u);
                i.nUpdateNewCount = 0;

                EXPECT_EQ(i.nUpdateCount, 0u);
                updResult = t.update(i, [](bool bNew, value_type &val, value_type &arg) {
                    EXPECT_FALSE(bNew);
                    EXPECT_EQ(&val, &arg);
                    ++val.nUpdateCount;
                },
                                     false);
                EXPECT_TRUE(updResult.first);
                EXPECT_FALSE(updResult.second);
                EXPECT_EQ(i.nUpdateCount, 1u);
                i.nUpdateCount = 0;

                break;
            }

            ASSERT_TRUE(t.contains(i.nKey));
            ASSERT_TRUE(t.contains(i));
            ASSERT_TRUE(t.contains(other_item(i.key()), other_less()));
            EXPECT_EQ(i.nFindCount, 0u);
            ASSERT_TRUE(t.find(i.nKey, [](value_type &v, int) { ++v.nFindCount; }));
            EXPECT_EQ(i.nFindCount, 1u);
            ASSERT_TRUE(t.find_with(other_item(i.key()), other_less(), [](value_type &v, other_item const &) { ++v.nFindCount; }));
            EXPECT_EQ(i.nFindCount, 2u);
            ASSERT_TRUE(t.find(i, [](value_type &v, value_type &) { ++v.nFindCount; }));
            EXPECT_EQ(i.nFindCount, 3u);
        }

        ASSERT_FALSE(t.empty());
        ASSERT_CONTAINER_SIZE(t, nTreeSize);

        std::for_each(data.begin(), data.end(), [](value_type &v) { v.clear_stat(); });
    }
};
} // namespace cds_test

#endif

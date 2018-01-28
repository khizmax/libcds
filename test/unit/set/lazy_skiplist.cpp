#include <cds_test/ext_gtest.h>

#include <cds/gc/dhp.h>
#include <cds/container/lazy_skip_list_set_dhp.h>

template<typename T>
using Set = cds::container::LazySkipListSet<cds::gc::DHP, T>;

typedef int value_type;

TEST(LazyTest, test_insert) {
    Set<value_type> set;

    ASSERT_TRUE(set.insert(42));
    ASSERT_TRUE(set.insert(41));
    ASSERT_TRUE(set.insert(43));

    ASSERT_FALSE(set.insert(42));
    ASSERT_FALSE(set.insert(43.0f));
    ASSERT_FALSE(set.insert(41));

    ASSERT_TRUE(set.insert(-41));
}

TEST(LazyTest, test_contains) {
    Set<value_type> set;

    ASSERT_FALSE(set.contains(42));
    ASSERT_FALSE(set.contains(43));

    set.insert(42);
    ASSERT_TRUE(set.contains(42));
    ASSERT_FALSE(set.contains(43));

    set.insert(43);
    ASSERT_TRUE(set.contains(42));
    ASSERT_TRUE(set.contains(43));
}

TEST(LazyTest, test_remove) {
    Set<value_type> set;

    ASSERT_FALSE(set.remove(42));

    set.insert(42);
    ASSERT_TRUE(set.remove(42));
    ASSERT_FALSE(set.remove(42));
}

TEST(LazyTest, test_empty) {
    Set<value_type> set;

    ASSERT_TRUE(set.empty());

    set.insert(42);
    ASSERT_FALSE(set.empty());

    set.insert(43);
    ASSERT_FALSE(set.empty());

    set.remove(42);
    ASSERT_FALSE(set.empty());

    set.remove(43);
    ASSERT_TRUE(set.empty());
}

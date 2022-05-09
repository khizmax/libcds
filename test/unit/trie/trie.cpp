// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds_test/ext_gtest.h>
#include <cds/container/hamt.h>


namespace {


    TEST(TRIE, HAPPY_FLOW__LOOKUP) {
    // arrange
    cds::container::Hamt<int, string, int> trie;
//
    trie.insert("k1", 1);
    trie.insert("k2", 2);
    trie.insert("k3", 3);
    trie.insert("k4", 4);
    trie.insert("k5", 5);
    trie.insert("k6", 6);
    trie.insert("k7", 7);
    trie.insert("k8", 8);
    trie.insert("k9", 9);
    trie.insert("k10", 10);
    trie.insert("k11", 11);
    trie.insert("k12", 12);
    trie.insert("k13", 13);
    trie.insert("k14", 14);
    trie.insert("k15", 15);
    trie.insert("k16", 16);
    trie.insert("k17", 17);
    trie.insert("k18", 18);
    trie.insert("k19", 19);
    trie.insert("k20", 20);
    trie.insert("k21", 21);
    trie.insert("k22", 22);
    trie.insert("k23", 23);
    trie.insert("k24", 24);
    trie.insert("k25", 25);

    // act & assert
    ASSERT_EQ(trie
    .lookup("k1").value, 1);
    ASSERT_EQ(trie
    .lookup("k2").value, 2);
    ASSERT_EQ(trie
    .lookup("k3").value, 3);
    ASSERT_EQ(trie
    .lookup("k4").value, 4);
    ASSERT_EQ(trie
    .lookup("k5").value, 5);
    ASSERT_EQ(trie
    .lookup("k6").value, 6);
    ASSERT_EQ(trie
    .lookup("k7").value, 7);
    ASSERT_EQ(trie
    .lookup("k8").value, 8);
    ASSERT_EQ(trie
    .lookup("k9").value, 9);
    ASSERT_EQ(trie
    .lookup("k10").value, 10);
    ASSERT_EQ(trie
    .lookup("k11").value, 11);
    ASSERT_EQ(trie
    .lookup("k12").value, 12);
    ASSERT_EQ(trie
    .lookup("k13").value, 13);
    ASSERT_EQ(trie
    .lookup("k14").value, 14);
    ASSERT_EQ(trie
    .lookup("k15").value, 15);
    ASSERT_EQ(trie
    .lookup("k16").value, 16);
    ASSERT_EQ(trie
    .lookup("k17").value, 17);
    ASSERT_EQ(trie
    .lookup("k18").value, 18);
    ASSERT_EQ(trie
    .lookup("k19").value, 19);
    ASSERT_EQ(trie
    .lookup("k20").value, 20);
    ASSERT_EQ(trie
    .lookup("k21").value, 21);
    ASSERT_EQ(trie
    .lookup("k22").value, 22);
    ASSERT_EQ(trie
    .lookup("k23").value, 23);
    ASSERT_EQ(trie
    .lookup("k24").value, 24);
    ASSERT_EQ(trie
    .lookup("k25").value, 25);

    ASSERT_EQ(trie
    .lookup("k223"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k224"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k225"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k226"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k227"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k228"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k229"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2210"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2211"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2212"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2213"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2214"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2215"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2216"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2217"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2218"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2229"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2220"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2222"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2223"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2224"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2225"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2226"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k227"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k2228"), cds::container::hamt::LOOKUP_NOT_FOUND);
    ASSERT_EQ(trie
    .lookup("k22d29"), cds::container::hamt::LOOKUP_NOT_FOUND);
}
}// namespace

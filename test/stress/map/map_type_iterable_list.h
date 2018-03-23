// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TYPE_ITERABLE_LIST_H
#define CDSUNIT_MAP_TYPE_ITERABLE_LIST_H

#include "map_type.h"

#include <cds/container/iterable_kvlist_hp.h>
#include <cds/container/iterable_kvlist_dhp.h>

#include <cds_test/stat_iterable_list_out.h>

namespace map {

    template <typename Key, typename Value>
    struct iterable_list_type
    {
        typedef typename map_type_base<Key, Value>::key_compare compare;
        typedef typename map_type_base<Key, Value>::key_less    less;

        struct traits_IterableList_cmp :
            public cc::iterable_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::IterableKVList< cds::gc::HP,  Key, Value, traits_IterableList_cmp > IterableList_HP_cmp;
        typedef cc::IterableKVList< cds::gc::DHP, Key, Value, traits_IterableList_cmp > IterableList_DHP_cmp;

        struct traits_IterableList_cmp_stat: public traits_IterableList_cmp
        {
            typedef cc::iterable_list::stat<> stat;
        };
        typedef cc::IterableKVList< cds::gc::HP, Key, Value, traits_IterableList_cmp_stat > IterableList_HP_cmp_stat;
        typedef cc::IterableKVList< cds::gc::DHP, Key, Value, traits_IterableList_cmp_stat > IterableList_DHP_cmp_stat;

        struct traits_IterableList_cmp_seqcst :
            public cc::iterable_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::IterableKVList< cds::gc::HP,  Key, Value, traits_IterableList_cmp_seqcst > IterableList_HP_cmp_seqcst;
        typedef cc::IterableKVList< cds::gc::DHP, Key, Value, traits_IterableList_cmp_seqcst > IterableList_DHP_cmp_seqcst;


        struct traits_IterableList_less :
            public cc::iterable_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::IterableKVList< cds::gc::HP,  Key, Value, traits_IterableList_less > IterableList_HP_less;
        typedef cc::IterableKVList< cds::gc::DHP, Key, Value, traits_IterableList_less > IterableList_DHP_less;

        struct traits_IterableList_less_stat: public traits_IterableList_less
        {
            typedef cc::iterable_list::stat<> stat;
        };
        typedef cc::IterableKVList< cds::gc::HP, Key, Value, traits_IterableList_less_stat > IterableList_HP_less_stat;
        typedef cc::IterableKVList< cds::gc::DHP, Key, Value, traits_IterableList_less_stat > IterableList_DHP_less_stat;

        struct traits_IterableList_less_seqcst :
            public cc::iterable_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::IterableKVList< cds::gc::HP,  Key, Value, traits_IterableList_less_seqcst > IterableList_HP_less_seqcst;
        typedef cc::IterableKVList< cds::gc::DHP, Key, Value, traits_IterableList_less_seqcst > IterableList_DHP_less_seqcst;

    };

} // namespace map

#endif // ifndef CDSUNIT_MAP_TYPE_ITERABLE_LIST_H

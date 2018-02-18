// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TYPE_ITRERABLE_LIST_H
#define CDSUNIT_SET_TYPE_ITRERABLE_LIST_H

#include "set_type.h"

#include <cds/container/iterable_list_hp.h>
#include <cds/container/iterable_list_dhp.h>
//#include <cds/container/michael_list_rcu.h>

namespace set {

    template <typename Key, typename Val>
    struct iterable_list_type
    {
        typedef typename set_type_base< Key, Val >::key_val key_val;
        typedef typename set_type_base< Key, Val >::compare compare;
        typedef typename set_type_base< Key, Val >::less    less;

        struct traits_IterableList_cmp:
            public cc::iterable_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_cmp > IterableList_HP_cmp;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_cmp > IterableList_DHP_cmp;

        struct traits_IterableList_cmp_stat: public traits_IterableList_cmp
        {
            typedef cc::iterable_list::stat<> stat;
        };
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_cmp_stat > IterableList_HP_cmp_stat;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_cmp_stat > IterableList_DHP_cmp_stat;

        struct traits_IterableList_cmp_seqcst : public traits_IterableList_cmp
        {
            typedef co::v::sequential_consistent memory_model;
        };
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_cmp_seqcst > IterableList_HP_cmp_seqcst;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_cmp_seqcst > IterableList_DHP_cmp_seqcst;

        struct traits_IterableList_less :
            public cc::iterable_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_less > IterableList_HP_less;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_less > IterableList_DHP_less;

        struct traits_IterableList_less_stat : public traits_IterableList_less
        {
            typedef cc::iterable_list::stat<> stat;
        };
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_less_stat > IterableList_HP_less_stat;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_less_stat > IterableList_DHP_less_stat;

        struct traits_IterableList_less_seqcst :
            public cc::iterable_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_less_seqcst > IterableList_HP_less_seqcst;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_less_seqcst > IterableList_DHP_less_seqcst;

    };

} // namespace set

#endif // #ifndef CDSUNIT_SET_TYPE_ITRERABLE_LIST_H

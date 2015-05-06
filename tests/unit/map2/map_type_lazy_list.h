//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_LAZY_LIST_H
#define CDSUNIT_MAP_TYPE_LAZY_LIST_H

#include "map2/map_type.h"

#include <cds/container/lazy_kvlist_hp.h>
#include <cds/container/lazy_kvlist_dhp.h>
#include <cds/container/lazy_kvlist_rcu.h>
#include <cds/container/lazy_kvlist_nogc.h>

#include "michael_alloc.h"

namespace map2 {

    template <typename Key, typename Value>
    struct lazy_list_type
    {
        typedef typename map_type_base<Key, Value>::compare compare;
        typedef typename map_type_base<Key, Value>::less    less;
        typedef typename map_type_base<Key, Value>::equal_to equal_to;

        struct traits_LazyList_cmp_stdAlloc:
            public cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_HP_cmp_stdAlloc;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_DHP_cmp_stdAlloc;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_NOGC_cmp_stdAlloc;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_GPI_cmp_stdAlloc;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_GPB_cmp_stdAlloc;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_SHB_cmp_stdAlloc;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_SHT_cmp_stdAlloc;
#endif

        struct traits_LazyList_unord_stdAlloc :
            public cc::lazy_list::make_traits<
                co::equal_to< equal_to >
                ,co::sort< false >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_unord_stdAlloc > LazyList_NOGC_unord_stdAlloc;

        struct traits_LazyList_cmp_stdAlloc_seqcst :
            public cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_HP_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_DHP_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_NOGC_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_GPI_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_GPB_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_GPT_cmp_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_SHB_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_SHT_cmp_stdAlloc_seqcst;
#endif

        struct traits_LazyList_cmp_michaelAlloc :
            public cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_HP_cmp_michaelAlloc;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_DHP_cmp_michaelAlloc;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_NOGC_cmp_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_GPI_cmp_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_GPB_cmp_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_SHB_cmp_michaelAlloc;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_SHT_cmp_michaelAlloc;
#endif
        struct traits_LazyList_less_stdAlloc :
            public cc::lazy_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_stdAlloc > LazyList_HP_less_stdAlloc;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_stdAlloc > LazyList_DHP_less_stdAlloc;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_stdAlloc > LazyList_NOGC_less_stdAlloc;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_GPI_less_stdAlloc;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_GPB_less_stdAlloc;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_SHB_less_stdAlloc;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_SHT_less_stdAlloc;
#endif

        struct traits_LazyList_less_stdAlloc_seqcst :
            public cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_HP_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_DHP_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_NOGC_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_SHT_less_stdAlloc_seqcst;
#endif

        struct traits_LazyList_less_michaelAlloc :
            public cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_HP_less_michaelAlloc;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_DHP_less_michaelAlloc;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_NOGC_less_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_GPI_less_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_GPB_less_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_SHB_less_michaelAlloc;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_SHT_less_michaelAlloc;
#endif
    };

}   // namespace map2

#endif // ifndef CDSUNIT_MAP_TYPE_LAZY_LIST_H

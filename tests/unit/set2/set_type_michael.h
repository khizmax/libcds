//$$CDS-header$$

#ifndef CDSUNIT_SET_TYPE_MICHAEL_H
#define CDSUNIT_SET_TYPE_MICHAEL_H

#include "set2/set_type_michael_list.h"
#include "set2/set_type_lazy_list.h"

#include <cds/container/michael_set.h>
#include <cds/container/michael_set_rcu.h>

#include "michael_alloc.h"

namespace set2 {

    template <typename Key, typename Val>
    struct set_type< cc::michael_set::implementation_tag, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::compare compare;
        typedef typename base_class::less less;
        typedef typename base_class::hash hash;

        // ***************************************************************************
        // MichaelHashSet based on MichaelList

        typedef michael_list_type< Key, Val > ml;

        struct traits_MichaelSet_stdAlloc :
            public cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        {};
        typedef cc::MichaelHashSet< cds::gc::HP,  typename ml::MichaelList_HP_cmp_stdAlloc,  traits_MichaelSet_stdAlloc > MichaelSet_HP_cmp_stdAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_DHP_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPI_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPB_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHB_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHT_cmp_stdAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, typename ml::MichaelList_HP_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_HP_less_stdAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_DHP_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPI_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPB_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHB_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHT_less_stdAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, typename ml::MichaelList_HP_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_HP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_DHP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHT_less_stdAlloc_seqcst;
#endif

        struct traits_MichaelSet_michaelAlloc :
            public cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::MichaelHashSet< cds::gc::HP,  typename ml::MichaelList_HP_cmp_michaelAlloc,  traits_MichaelSet_michaelAlloc > MichaelSet_HP_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_DHP_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPI_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPB_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_SHB_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_SHT_cmp_michaelAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, typename ml::MichaelList_HP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_HP_less_michaelAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_DHP_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPI_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPB_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_SHB_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_SHT_less_michaelAlloc;
#endif


        // ***************************************************************************
        // MichaelHashSet based on LazyList

        typedef lazy_list_type< Key, Val > ll;

        typedef cc::MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_HP_cmp_stdAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_DHP_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPI_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPB_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHB_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHT_cmp_stdAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_HP_less_stdAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_DHP_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPI_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPB_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHB_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHT_less_stdAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_HP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_DHP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHT_less_stdAlloc_seqcst;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_HP_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_DHP_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPI_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPB_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_SHB_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_SHT_cmp_michaelAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_HP_less_michaelAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_DHP_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPI_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPB_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_SHB_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_SHT_less_michaelAlloc;
#endif
    };

} // namespace set2

#endif // #ifndef CDSUNIT_SET_TYPE_MICHAEL_H

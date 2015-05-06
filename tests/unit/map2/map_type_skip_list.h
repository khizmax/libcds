//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_SKIP_LIST_H
#define CDSUNIT_MAP_TYPE_SKIP_LIST_H

#include "map2/map_type.h"

#include <cds/container/skip_list_map_hp.h>
#include <cds/container/skip_list_map_dhp.h>
#include <cds/container/skip_list_map_rcu.h>
#include <cds/container/skip_list_map_nogc.h>

#include "print_skip_list_stat.h"

namespace map2 {

    template <typename Key, typename Value>
    struct map_type< cc::skip_list::implementation_tag, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;

        class traits_SkipListMap_less_pascal: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_hp_less_pascal;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_dhp_less_pascal;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_nogc_less_pascal;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_gpi_less_pascal;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_gpb_less_pascal;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_gpt_less_pascal;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_shb_less_pascal;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_sht_less_pascal;
#endif

        class traits_SkipListMap_less_pascal_seqcst: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_hp_less_pascal_seqcst;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_dhp_less_pascal_seqcst;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_nogc_less_pascal_seqcst;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_gpi_less_pascal_seqcst;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_gpb_less_pascal_seqcst;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_gpt_less_pascal_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_shb_less_pascal_seqcst;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_sht_less_pascal_seqcst;
#endif

        class traits_SkipListMap_less_pascal_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_hp_less_pascal_stat;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_dhp_less_pascal_stat;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_nogc_less_pascal_stat;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_gpi_less_pascal_stat;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_gpb_less_pascal_stat;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_gpt_less_pascal_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_shb_less_pascal_stat;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_sht_less_pascal_stat;
#endif

        class traits_SkipListMap_cmp_pascal: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_hp_cmp_pascal;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_dhp_cmp_pascal;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_nogc_cmp_pascal;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_gpi_cmp_pascal;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_gpb_cmp_pascal;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_gpt_cmp_pascal;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_shb_cmp_pascal;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_sht_cmp_pascal;
#endif

        class traits_SkipListMap_cmp_pascal_stat: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_hp_cmp_pascal_stat;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_dhp_cmp_pascal_stat;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_nogc_cmp_pascal_stat;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_gpi_cmp_pascal_stat;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_gpb_cmp_pascal_stat;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_gpt_cmp_pascal_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_shb_cmp_pascal_stat;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_sht_cmp_pascal_stat;
#endif

        class traits_SkipListMap_less_xorshift: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_hp_less_xorshift;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_dhp_less_xorshift;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_nogc_less_xorshift;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_gpi_less_xorshift;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_gpb_less_xorshift;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_gpt_less_xorshift;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_shb_less_xorshift;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_sht_less_xorshift;
#endif

        class traits_SkipListMap_less_xorshift_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_hp_less_xorshift_stat;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_dhp_less_xorshift_stat;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_nogc_less_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_gpi_less_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_gpb_less_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_gpt_less_xorshift_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_shb_less_xorshift_stat;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_sht_less_xorshift_stat;
#endif

        class traits_SkipListMap_cmp_xorshift: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_hp_cmp_xorshift;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_dhp_cmp_xorshift;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_nogc_cmp_xorshift;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_gpi_cmp_xorshift;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_gpb_cmp_xorshift;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_gpt_cmp_xorshift;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_shb_cmp_xorshift;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_sht_cmp_xorshift;
#endif

        class traits_SkipListMap_cmp_xorshift_stat: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_hp_cmp_xorshift_stat;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_dhp_cmp_xorshift_stat;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_nogc_cmp_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_gpi_cmp_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_gpb_cmp_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_gpt_cmp_xorshift_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_shb_cmp_xorshift_stat;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_sht_cmp_xorshift_stat;
#endif

    };

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( cc::SkipListMap< GC, K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() );
    }

}   // namespace map2

#endif // ifndef CDSUNIT_MAP_TYPE_SKIP_LIST_H

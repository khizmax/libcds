#ifndef CDSUNIT_SET_TYPE_LAZY_SKIP_LIST_H
#define CDSUNIT_SET_TYPE_LAZY_SKIP_LIST_H

#include "set_type.h"

#include <cds/container/lazy_skip_list_set_hp.h>
#include <cds/container/lazy_skip_list_set_dhp.h>

#include <cds_test/stat_lazyskiplist_out.h>

namespace set {

    template <typename GC, typename T, typename Traits = cc::lazy_skip_list::traits >
    class LazySkipListSet : public cc::LazySkipListSet<GC, T, Traits>
    {
        typedef cc::LazySkipListSet<GC, T, Traits> base_class;
    public:
        template <typename Config>
        LazySkipListSet( Config const& /*cfg*/ )
        {}

        // for testing
        static constexpr bool const c_bExtractSupported = true;
        static constexpr bool const c_bLoadFactorDepended = false;
        static constexpr bool const c_bEraseExactKey = false;
    };

    struct tag_LazySkipListSet;

    template <typename Key, typename Val>
    struct set_type< tag_LazySkipListSet, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::compare compare;
        typedef typename base_class::less less;
        typedef typename base_class::hash hash;

        class traits_LazySkipListSet_less_turbo32: public cc::lazy_skip_list::make_traits <
                co::less< less >
                ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::turbo32 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_turbo32 > LazySkipListSet_hp_less_turbo32;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_turbo32 > LazySkipListSet_dhp_less_turbo32;

        class traits_LazySkipListSet_less_turbo24: public cc::lazy_skip_list::make_traits <
                co::less< less >
                ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::turbo24 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_turbo24 > LazySkipListSet_hp_less_turbo24;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_turbo24 > LazySkipListSet_dhp_less_turbo24;

        class traits_LazySkipListSet_less_turbo16: public cc::lazy_skip_list::make_traits <
                co::less< less >
                ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::turbo16 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_turbo16 > LazySkipListSet_hp_less_turbo16;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_turbo16 > LazySkipListSet_dhp_less_turbo16;

        class traits_LazySkipListSet_less_turbo32_seqcst: public cc::lazy_skip_list::make_traits <
                co::less< less >
                ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::turbo32 >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_turbo32_seqcst > LazySkipListSet_hp_less_turbo32_seqcst;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_turbo32_seqcst > LazySkipListSet_dhp_less_turbo32_seqcst;

        class traits_LazySkipListSet_less_turbo32_stat: public cc::lazy_skip_list::make_traits <
                co::less< less >
                ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::turbo32 >
                ,co::stat< cc::lazy_skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_turbo32_stat > LazySkipListSet_hp_less_turbo32_stat;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_turbo32_stat > LazySkipListSet_dhp_less_turbo32_stat;

        class traits_LazySkipListSet_less_turbo24_stat: public cc::lazy_skip_list::make_traits <
                co::less< less >
                ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::turbo24 >
                ,co::stat< cc::lazy_skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_turbo24_stat > LazySkipListSet_hp_less_turbo24_stat;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_turbo24_stat > LazySkipListSet_dhp_less_turbo24_stat;

        class traits_LazySkipListSet_less_turbo16_stat: public cc::lazy_skip_list::make_traits <
                co::less< less >
                ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::turbo16 >
                ,co::stat< cc::lazy_skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_turbo16_stat > LazySkipListSet_hp_less_turbo16_stat;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_turbo16_stat > LazySkipListSet_dhp_less_turbo16_stat;

        class traits_LazySkipListSet_cmp_turbo32: public cc::lazy_skip_list::make_traits <
            co::compare< compare >
            ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::turbo32 >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_cmp_turbo32 > LazySkipListSet_hp_cmp_turbo32;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_cmp_turbo32 > LazySkipListSet_dhp_cmp_turbo32;

        class traits_LazySkipListSet_cmp_turbo32_stat: public cc::lazy_skip_list::make_traits <
            co::compare< compare >
            ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::turbo32 >
            ,co::stat< cc::lazy_skip_list::stat<> >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_cmp_turbo32_stat > LazySkipListSet_hp_cmp_turbo32_stat;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_cmp_turbo32_stat > LazySkipListSet_dhp_cmp_turbo32_stat;

        class traits_LazySkipListSet_less_xorshift32: public cc::lazy_skip_list::make_traits <
            co::less< less >
            ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::xorshift32 >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_xorshift32 > LazySkipListSet_hp_less_xorshift32;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_xorshift32 > LazySkipListSet_dhp_less_xorshift32;

        class traits_LazySkipListSet_less_xorshift24: public cc::lazy_skip_list::make_traits <
            co::less< less >
            ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::xorshift24 >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_xorshift24 > LazySkipListSet_hp_less_xorshift24;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_xorshift24 > LazySkipListSet_dhp_less_xorshift24;

        class traits_LazySkipListSet_less_xorshift16: public cc::lazy_skip_list::make_traits <
            co::less< less >
            , cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::xorshift16 >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_xorshift16 > LazySkipListSet_hp_less_xorshift16;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_xorshift16 > LazySkipListSet_dhp_less_xorshift16;

        class traits_LazySkipListSet_less_xorshift32_stat: public cc::lazy_skip_list::make_traits <
            co::less< less >
            ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::xorshift32 >
            ,co::stat< cc::lazy_skip_list::stat<> >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_xorshift32_stat > LazySkipListSet_hp_less_xorshift32_stat;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_xorshift32_stat > LazySkipListSet_dhp_less_xorshift32_stat;

        class traits_LazySkipListSet_less_xorshift24_stat: public cc::lazy_skip_list::make_traits <
            co::less< less >
            , cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::xorshift24 >
            , co::stat< cc::lazy_skip_list::stat<> >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_xorshift24_stat > LazySkipListSet_hp_less_xorshift24_stat;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_xorshift24_stat > LazySkipListSet_dhp_less_xorshift24_stat;

        class traits_LazySkipListSet_less_xorshift16_stat: public cc::lazy_skip_list::make_traits <
            co::less< less >
            , cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::xorshift16 >
            , co::stat< cc::lazy_skip_list::stat<> >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_less_xorshift16_stat > LazySkipListSet_hp_less_xorshift16_stat;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_less_xorshift16_stat > LazySkipListSet_dhp_less_xorshift16_stat;

        class traits_LazySkipListSet_cmp_xorshift32: public cc::lazy_skip_list::make_traits <
            co::compare< compare >
            ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::xorshift32 >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_cmp_xorshift32 > LazySkipListSet_hp_cmp_xorshift32;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_cmp_xorshift32 > LazySkipListSet_dhp_cmp_xorshift32;

        class traits_LazySkipListSet_cmp_xorshift32_stat: public cc::lazy_skip_list::make_traits <
            co::compare< compare >
            ,cc::lazy_skip_list::random_level_generator< cc::lazy_skip_list::xorshift32 >
            ,co::stat< cc::lazy_skip_list::stat<> >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef LazySkipListSet< cds::gc::HP, key_val, traits_LazySkipListSet_cmp_xorshift32_stat > LazySkipListSet_hp_cmp_xorshift32_stat;
        typedef LazySkipListSet< cds::gc::DHP, key_val, traits_LazySkipListSet_cmp_xorshift32_stat > LazySkipListSet_dhp_cmp_xorshift32_stat;
    };

    template <typename GC, typename T, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, LazySkipListSet<GC, T, Traits> const& s )
    {
        o << s.statistics();
    }

} // namespace set

#define CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipList_set_type, key_type, value_type ) \
    TEST_F( fixture, LazySkipList_set_type ) \
    { \
        typedef set::set_type< tag_LazySkipListSet, key_type, value_type >::LazySkipList_set_type set_type; \
        test_case<set_type>(); \
    }


#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 1
#   define CDSSTRESS_LazySkipListSet_HP_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_less_turbo32_seqcst,      key_type, value_type ) \
        CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_less_turbo32_seqcst,     key_type, value_type ) \

#else
#   define CDSSTRESS_LazySkipListSet_HP_2( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL == 1
#   define CDSSTRESS_LazySkipListSet_HP_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_less_turbo32,            key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_less_turbo32_stat,        key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_cmp_turbo32,              key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_cmp_turbo32_stat,        key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_less_xorshift32,          key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_less_xorshift32_stat,      key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_cmp_xorshift32,            key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_cmp_xorshift32_stat,      key_type, value_type ) \

#else
#   define CDSSTRESS_LazySkipListSet_HP_1( fixture, test_case, key_type, value_type )
#endif


#define CDSSTRESS_LazySkipListSet_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_less_turbo32,             key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_less_turbo24,             key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_less_turbo16,             key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_less_turbo32_stat,       key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_less_turbo24_stat,       key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_less_turbo16_stat,       key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_cmp_turbo32,             key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_cmp_turbo32_stat,         key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_less_xorshift32,           key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_less_xorshift24,           key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_less_xorshift16,           key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_less_xorshift32_stat,     key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_less_xorshift24_stat,     key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_less_xorshift16_stat,     key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_dhp_cmp_xorshift32,           key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_case( fixture, test_case, LazySkipListSet_hp_cmp_xorshift32_stat,       key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_HP_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_HP_2( fixture, test_case, key_type, value_type ) \


#define CDSSTRESS_LazySkipListSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_LazySkipListSet_HP( fixture, test_case, key_type, value_type ) \

#endif // #ifndef CDSUNIT_SET_TYPE_LAZY_SKIP_LIST_H

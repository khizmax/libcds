//$$CDS-header$$

#include "map/hdr_map.h"
#include <cds/container/lazy_list_nogc.h>
#include <cds/container/split_list_map_nogc.h>

namespace map {

    namespace {
        struct nogc_cmp_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag                   ordered_list;
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;
            typedef cc::opt::v::relaxed_ordering        memory_model;
            enum { dynamic_bucket_table = false };

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef HashMapHdrTest::cmp   compare;
            };
        };

        struct nogc_less_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag                   ordered_list;
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;
            typedef cc::opt::v::sequential_consistent   memory_model;
            enum { dynamic_bucket_table = false };

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef HashMapHdrTest::less   less;
            };
        };

        struct nogc_cmpmix_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag                   ordered_list;
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef HashMapHdrTest::cmp   compare;
                typedef std::less<HashMapHdrTest::key_type>     less;
            };
        };

        struct nogc_cmpmix_stat_traits : public nogc_cmpmix_traits
        {
            typedef cc::split_list::stat<> stat;
        };
    }

    void HashMapHdrTest::Split_Lazy_nogc_cmp()
    {
        // traits-based version
        typedef cc::SplitListMap< cds::gc::nogc, key_type, value_type, nogc_cmp_traits > map_type;
        test_int_nogc< map_type >();

        // option-based version
        typedef cc::SplitListMap< cds::gc::nogc,
            key_type,
            value_type,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::opt::memory_model< cc::opt::v::relaxed_ordering >
                ,cc::split_list::dynamic_bucket_table< true >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                        cc::opt::compare< cmp >
                    >::type
                >
            >::type
        > opt_map;
        test_int_nogc< opt_map >();
    }

    void HashMapHdrTest::Split_Lazy_nogc_less()
    {
        // traits-based version
        typedef cc::SplitListMap< cds::gc::nogc, key_type, value_type, nogc_less_traits > map_type;
        test_int_nogc< map_type >();

        // option-based version
        typedef cc::SplitListMap< cds::gc::nogc,
            key_type,
            value_type,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::opt::memory_model< cc::opt::v::relaxed_ordering >
                ,cc::split_list::dynamic_bucket_table< true >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                        cc::opt::less< less >
                    >::type
                >
            >::type
        > opt_map;
        test_int_nogc< opt_map >();
    }

    void HashMapHdrTest::Split_Lazy_nogc_cmpmix()
    {
        // traits-based version
        typedef cc::SplitListMap< cds::gc::nogc, key_type, value_type, nogc_cmpmix_traits > map_type;
        test_int_nogc< map_type >();

        // option-based version
        typedef cc::SplitListMap< cds::gc::nogc,
            key_type,
            value_type,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                    cc::opt::less< std::less<key_type> >
                        ,cc::opt::compare< cmp >
                    >::type
                >
            >::type
        > opt_map;
        test_int_nogc< opt_map >();
    }

    void HashMapHdrTest::Split_Lazy_nogc_cmpmix_stat()
    {
        // traits-based version
        typedef cc::SplitListMap< cds::gc::nogc, key_type, value_type, nogc_cmpmix_stat_traits > map_type;
        test_int_nogc< map_type >();

        // option-based version
        typedef cc::SplitListMap< cds::gc::nogc,
            key_type,
            value_type,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cds::opt::stat< cc::split_list::stat<> >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                    cc::opt::less< std::less<key_type> >
                        ,cc::opt::compare< cmp >
                    >::type
                >
            >::type
        > opt_map;
        test_int_nogc< opt_map >();
    }

} // namespace map


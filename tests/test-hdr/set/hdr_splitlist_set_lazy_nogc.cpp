//$$CDS-header$$

#include "set/hdr_set.h"
#include <cds/container/lazy_list_nogc.h>
#include <cds/container/split_list_set_nogc.h>

namespace set {

    namespace {
        struct nogc_cmp_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag                   ordered_list;
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;
            typedef cc::opt::v::relaxed_ordering        memory_model;
            enum { dynamic_bucket_table = false };

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
            };
        };

        struct nogc_less_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag                ordered_list;
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;
            typedef cc::opt::v::sequential_consistent                      memory_model;
            enum { dynamic_bucket_table = false };

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
            };
        };

        struct nogc_cmpmix_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag                ordered_list;
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
                typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
            };
        };

        struct nogc_cmpmix_stat_traits : public nogc_cmpmix_traits
        {
            typedef cc::split_list::stat<> stat;
        };
    }

    void HashSetHdrTest::Split_Lazy_nogc_cmp()
    {
        // traits-based version
        typedef cc::SplitListSet< cds::gc::nogc, item, nogc_cmp_traits > set;

        test_int_nogc< set >();

        // option-based version
        typedef cc::SplitListSet< cds::gc::nogc, item,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::opt::memory_model< cc::opt::v::relaxed_ordering >
                ,cc::split_list::dynamic_bucket_table< true >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                        cc::opt::compare< cmp<item> >
                    >::type
                >
            >::type
        > opt_set;
        test_int_nogc< opt_set >();
    }

    void HashSetHdrTest::Split_Lazy_nogc_less()
    {
        // traits-based version
        typedef cc::SplitListSet< cds::gc::nogc, item, nogc_less_traits > set;

        test_int_nogc< set >();

        // option-based version
        typedef cc::SplitListSet< cds::gc::nogc, item,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::opt::memory_model< cc::opt::v::sequential_consistent >
                ,cc::split_list::dynamic_bucket_table< false >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                        cc::opt::less< less<item> >
                    >::type
                >
            >::type
        > opt_set;
        test_int_nogc< opt_set >();
    }

    void HashSetHdrTest::Split_Lazy_nogc_cmpmix()
    {
        // traits-based version
        typedef cc::SplitListSet< cds::gc::nogc, item, nogc_cmpmix_traits > set;
        test_int_nogc< set >();

        // option-based version
        typedef cc::SplitListSet< cds::gc::nogc, item,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                        cc::opt::less< less<item> >
                        ,cc::opt::compare< cmp<item> >
                    >::type
                >
            >::type
        > opt_set;
        test_int_nogc< opt_set >();
    }

    void HashSetHdrTest::Split_Lazy_nogc_cmpmix_stat()
    {
        // traits-based version
        typedef cc::SplitListSet< cds::gc::nogc, item, nogc_cmpmix_stat_traits > set;
        test_int_nogc< set >();

        // option-based version
        typedef cc::SplitListSet< cds::gc::nogc, item,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::opt::hash< hash_int >
                ,cds::opt::stat< cc::split_list::stat<>>
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::split_list::ordered_list_traits<
                    cc::lazy_list::make_traits<
                        cc::opt::less< less<item> >
                        ,cc::opt::compare< cmp<item> >
                    >::type
                >
            >::type
        > opt_set;
        test_int_nogc< opt_set >();
    }

} // namespace set



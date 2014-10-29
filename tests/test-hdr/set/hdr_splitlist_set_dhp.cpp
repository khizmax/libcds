//$$CDS-header$$

#include "set/hdr_set.h"
#include <cds/container/michael_list_dhp.h>
#include <cds/container/split_list_set.h>

namespace set {

    namespace {
        struct DHP_cmp_traits: public cc::split_list::traits
        {
            typedef cc::michael_list_tag                ordered_list;
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;
            typedef cc::opt::v::relaxed_ordering        memory_model;
            enum { dynamic_bucket_table = false };

            struct ordered_list_traits: public cc::michael_list::traits
            {
                typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
            };
        };

        struct DHP_less_traits: public cc::split_list::traits
        {
            typedef cc::michael_list_tag                ordered_list;
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;
            typedef cc::opt::v::sequential_consistent   memory_model;
            enum { dynamic_bucket_table = false };

            struct ordered_list_traits: public cc::michael_list::traits
            {
                typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
            };
        };

        struct DHP_cmpmix_traits: public cc::split_list::traits
        {
            typedef cc::michael_list_tag                ordered_list;
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;

            struct ordered_list_traits: public cc::michael_list::traits
            {
                typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
                typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
            };
        };

        struct DHP_cmpmix_stat_traits : public DHP_cmpmix_traits
        {
            typedef cc::split_list::stat<> stat;
        };
    }

    void HashSetHdrTest::Split_DHP_cmp()
    {
        // traits-based version
        typedef cc::SplitListSet< cds::gc::DHP, item, DHP_cmp_traits > set;

        test_int< set >();

        // option-based version
        typedef cc::SplitListSet< cds::gc::DHP, item,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::opt::memory_model< cc::opt::v::relaxed_ordering >
                ,cc::split_list::dynamic_bucket_table< true >
                ,cc::split_list::ordered_list_traits<
                    cc::michael_list::make_traits<
                        cc::opt::compare< cmp<item> >
                    >::type
                >
            >::type
        > opt_set;
        test_int< opt_set >();
    }

    void HashSetHdrTest::Split_DHP_less()
    {
        // traits-based version
        typedef cc::SplitListSet< cds::gc::DHP, item, DHP_less_traits > set;

        test_int< set >();

        // option-based version
        typedef cc::SplitListSet< cds::gc::DHP, item,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::opt::memory_model< cc::opt::v::sequential_consistent >
                ,cc::split_list::dynamic_bucket_table< false >
                ,cc::split_list::ordered_list_traits<
                    cc::michael_list::make_traits<
                        cc::opt::less< less<item> >
                    >::type
                >
            >::type
        > opt_set;
        test_int< opt_set >();
    }

    void HashSetHdrTest::Split_DHP_cmpmix()
    {
        // traits-based version
        typedef cc::SplitListSet< cds::gc::DHP, item, DHP_cmpmix_traits > set;
        test_int< set >();

        // option-based version
        typedef cc::SplitListSet< cds::gc::DHP, item,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::split_list::ordered_list_traits<
                    cc::michael_list::make_traits<
                        cc::opt::less< less<item> >
                        ,cc::opt::compare< cmp<item> >
                    >::type
                >
            >::type
        > opt_set;
        test_int< opt_set >();
    }

    void HashSetHdrTest::Split_DHP_cmpmix_stat()
    {
        // traits-based version
        typedef cc::SplitListSet< cds::gc::DHP, item, DHP_cmpmix_stat_traits > set;
        test_int< set >();

        // option-based version
        typedef cc::SplitListSet< cds::gc::DHP, item,
            cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
                ,cc::split_list::ordered_list_traits<
                    cc::michael_list::make_traits<
                        cc::opt::less< less<item> >
                        ,cc::opt::compare< cmp<item> >
                    >::type
                >
                ,cds::opt::stat< cc::split_list::stat<> >
            >::type
        > opt_set;
        test_int< opt_set >();
    }


} // namespace set



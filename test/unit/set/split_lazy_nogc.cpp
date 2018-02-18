// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_set_nogc.h"

#include <cds/container/lazy_list_nogc.h>
#include <cds/container/split_list_set_nogc.h>
#include <cds/intrusive/free_list.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class SplitListLazySet_NoGC : public cds_test::container_set_nogc
    {
    protected:
        typedef cds_test::container_set_nogc base_class;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    TEST_F( SplitListLazySet_NoGC, compare )
    {
        typedef cc::SplitListSet< gc_type, int_item,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::lazy_list_tag >
                , cds::opt::hash< hash_int >
                , cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        cds::opt::compare< cmp >
                    >::type
                >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, less )
    {
        typedef cc::SplitListSet< gc_type, int_item,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::lazy_list_tag >
                , cds::opt::hash< hash_int >
                , cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        cds::opt::less< less >
                    >::type
                >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, cmpmix )
    {
        typedef cc::SplitListSet< gc_type, int_item,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::lazy_list_tag >
                , cds::opt::hash< hash_int >
                , cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        cds::opt::less< less >
                        , cds::opt::compare< cmp >
                    >::type
                >
            >::type
        > set_type;

        set_type s( kSize, 1 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, item_counting )
    {
        struct set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef base_class::less less;
                typedef cds::backoff::empty back_off;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 3 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, stat )
    {
        struct set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::split_list::stat<> stat;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef base_class::less less;
                typedef cds::opt::v::sequential_consistent memory_model;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 4 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, back_off )
    {
        struct set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::opt::v::sequential_consistent memory_model;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::empty back_off;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, mutex )
    {
        struct set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::opt::v::sequential_consistent memory_model;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
                typedef std::mutex lock_type;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 3 );
        test( s );
    }

    struct set_static_traits: public cc::split_list::traits
    {
        static bool const dynamic_bucket_table = false;
    };

    TEST_F( SplitListLazySet_NoGC, static_bucket_table )
    {
        struct set_traits: public set_static_traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 4 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, static_bucket_table_free_list )
    {
        struct set_traits: public set_static_traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::FreeList free_list;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 4 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, free_list )
    {
        struct set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::FreeList free_list;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::empty back_off;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }


} // namespace

// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_set_hp.h"

#include <cds/container/lazy_list_dhp.h>
#include <cds/container/michael_set.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class MichaelLazySet_DHP : public cds_test::container_set_hp
    {
    protected:
        typedef cds_test::container_set_hp base_class;

        void SetUp()
        {
            typedef cc::LazyList< gc_type, int_item > list_type;
            typedef cc::MichaelHashSet< gc_type, list_type >   set_type;

            cds::gc::dhp::smr::construct( set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };

    TEST_F( MichaelLazySet_DHP, compare )
    {
        typedef cc::LazyList< gc_type, int_item,
            typename cc::lazy_list::make_traits<
                cds::opt::compare< cmp >
            >::type
        > list_type;

        typedef cc::MichaelHashSet< gc_type, list_type,
            typename cc::michael_set::make_traits<
                cds::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( MichaelLazySet_DHP, less )
    {
        typedef cc::LazyList< gc_type, int_item,
            typename cc::lazy_list::make_traits<
                cds::opt::less< base_class::less >
            >::type
        > list_type;

        typedef cc::MichaelHashSet< gc_type, list_type,
            typename cc::michael_set::make_traits<
                cds::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( MichaelLazySet_DHP, cmpmix )
    {
        struct list_traits : public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cmp compare;
        };
        typedef cc::LazyList< gc_type, int_item, list_traits > list_type;

        typedef cc::MichaelHashSet< gc_type, list_type,
            typename cc::michael_set::make_traits<
                cds::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( MichaelLazySet_DHP, item_counting )
    {
        struct list_traits : public cc::lazy_list::traits
        {
            typedef cmp compare;
        };
        typedef cc::LazyList< gc_type, int_item, list_traits > list_type;

        struct set_traits: public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 3 );
        test( s );
    }

    TEST_F( MichaelLazySet_DHP, backoff )
    {
        struct list_traits : public cc::lazy_list::traits
        {
            typedef cmp compare;
            typedef cds::backoff::make_exponential_t<cds::backoff::pause, cds::backoff::yield> back_off;
        };
        typedef cc::LazyList< gc_type, int_item, list_traits > list_type;

        struct set_traits : public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 4 );
        test( s );
    }

    TEST_F( MichaelLazySet_DHP, seq_cst )
    {
        struct list_traits : public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::LazyList< gc_type, int_item, list_traits > list_type;

        struct set_traits : public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 4 );
        test( s );
    }

    TEST_F( MichaelLazySet_DHP, mutex )
    {
        struct list_traits: public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef std::mutex lock_type;
        };
        typedef cc::LazyList< gc_type, int_item, list_traits > list_type;

        struct set_traits: public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 4 );
        test( s );
    }

    TEST_F( MichaelLazySet_DHP, stat )
    {
        struct list_traits: public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyList< gc_type, int_item, list_traits > list_type;

        struct set_traits: public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 4 );
        test( s );
        EXPECT_GE( s.statistics().m_nInsertSuccess, 0u );
    }

    TEST_F( MichaelLazySet_DHP, wrapped_stat )
    {
        struct list_traits: public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cc::lazy_list::wrapped_stat<> stat;
        };
        typedef cc::LazyList< gc_type, int_item, list_traits > list_type;

        struct set_traits: public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 4 );
        test( s );
        EXPECT_GE( s.statistics().m_nInsertSuccess, 0u );
    }

} // namespace

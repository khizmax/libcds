// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_michael_iterable_hp.h"

#include <cds/intrusive/iterable_list_dhp.h>
#include <cds/intrusive/michael_set.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveMichaelIterableSet_DHP : public cds_test::intrusive_set_hp
    {
    protected:
        typedef cds_test::intrusive_set_hp base_class;

    protected:
        void SetUp()
        {
            struct list_traits : public ci::iterable_list::traits
            {};
            typedef ci::IterableList< gc_type, item_type, list_traits > list_type;
            typedef ci::MichaelHashSet< gc_type, list_type > set_type;

            cds::gc::dhp::smr::construct( set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };


    TEST_F( IntrusiveMichaelIterableSet_DHP, cmp )
    {
        typedef ci::IterableList< gc_type
            , item_type
            ,ci::iterable_list::make_traits<
                ci::opt::compare< cmp<item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        > bucket_type;

        typedef ci::MichaelHashSet< gc_type, bucket_type,
            ci::michael_set::make_traits<
                ci::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveMichaelIterableSet_DHP, less )
    {
        typedef ci::IterableList< gc_type
            , item_type
            ,ci::iterable_list::make_traits<
                ci::opt::less< less<item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        > bucket_type;

        typedef ci::MichaelHashSet< gc_type, bucket_type,
            ci::michael_set::make_traits<
                ci::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveMichaelIterableSet_DHP, cmpmix )
    {
        struct list_traits : public ci::iterable_list::traits
        {
            typedef base_class::less<item_type> less;
            typedef cmp<item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::IterableList< gc_type, item_type, list_traits > bucket_type;

        struct set_traits : public ci::michael_set::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::MichaelHashSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveMichaelIterableSet_DHP, stat )
    {
        struct list_traits: public ci::iterable_list::traits
        {
            typedef base_class::less<item_type> less;
            typedef mock_disposer disposer;
            typedef ci::iterable_list::stat<> stat;
        };
        typedef ci::IterableList< gc_type, item_type, list_traits > bucket_type;

        struct set_traits: public ci::michael_set::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::MichaelHashSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
        EXPECT_GE( s.statistics().m_nInsertSuccess, 0u );
    }

    TEST_F( IntrusiveMichaelIterableSet_DHP, wrapped_stat )
    {
        struct list_traits: public ci::iterable_list::traits
        {
            typedef base_class::less<item_type> less;
            typedef mock_disposer disposer;
            typedef ci::iterable_list::wrapped_stat<> stat;
        };
        typedef ci::IterableList< gc_type, item_type, list_traits > bucket_type;

        struct set_traits: public ci::michael_set::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::MichaelHashSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
        EXPECT_GE( s.statistics().m_nInsertSuccess, 0u );
    }

    TEST_F( IntrusiveMichaelIterableSet_DHP, derived_list )
    {
        class bucket_type: public ci::IterableList< gc_type
            , item_type
            ,ci::iterable_list::make_traits<
                ci::opt::compare< cmp<item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        >
        {};

        typedef ci::MichaelHashSet< gc_type, bucket_type,
            ci::michael_set::make_traits<
                ci::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

} // namespace

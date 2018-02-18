// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_set_nogc.h"

#include <cds/intrusive/lazy_list_nogc.h>
#include <cds/intrusive/michael_set_nogc.h>

#include <mutex>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::nogc gc_type;

    class IntrusiveMichaelLazySet_NoGC : public cds_test::intrusive_set_nogc
    {
    protected:
        typedef cds_test::intrusive_set_nogc base_class;

    protected:
        typedef typename base_class::base_int_item< ci::lazy_list::node<gc_type>>   base_item_type;
        typedef typename base_class::base_int_item< ci::lazy_list::node<gc_type, std::mutex>>   base_mutex_item_type;
        typedef typename base_class::member_int_item< ci::lazy_list::node<gc_type>> member_item_type;
        typedef typename base_class::member_int_item< ci::lazy_list::node<gc_type, std::mutex>> member_mutex_item_type;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };


    TEST_F( IntrusiveMichaelLazySet_NoGC, base_cmp )
    {
        typedef ci::LazyList< gc_type
            , base_item_type
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< ci::opt::gc< gc_type > > >
                ,ci::opt::compare< cmp<base_item_type> >
                ,ci::opt::disposer< mock_disposer >
                ,ci::opt::back_off< cds::backoff::pause >
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

    TEST_F( IntrusiveMichaelLazySet_NoGC, base_less )
    {
        typedef ci::LazyList< gc_type
            , base_item_type
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< ci::opt::gc< gc_type >>>
                ,ci::opt::less< less<base_item_type> >
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

    TEST_F( IntrusiveMichaelLazySet_NoGC, base_cmpmix )
    {
        struct list_traits : public ci::lazy_list::traits
        {
            typedef ci::lazy_list::base_hook< ci::opt::gc<gc_type>> hook;
            typedef base_class::less<base_item_type> less;
            typedef cmp<base_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::LazyList< gc_type, base_item_type, list_traits > bucket_type;

        struct set_traits : public ci::michael_set::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::MichaelHashSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveMichaelLazySet_NoGC, base_mutex )
    {
        struct list_traits : public ci::lazy_list::traits
        {
            typedef ci::lazy_list::base_hook< ci::opt::gc<gc_type>, ci::opt::lock_type<std::mutex>> hook;
            typedef base_class::less<base_mutex_item_type> less;
            typedef cmp<base_mutex_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::LazyList< gc_type, base_mutex_item_type, list_traits > bucket_type;

        struct set_traits : public ci::michael_set::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::MichaelHashSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveMichaelLazySet_NoGC, base_stat )
    {
        struct list_traits: public ci::lazy_list::traits
        {
            typedef ci::lazy_list::base_hook< ci::opt::gc<gc_type>> hook;
            typedef base_class::less<base_item_type> less;
            typedef mock_disposer disposer;
            typedef ci::lazy_list::stat<> stat;
        };
        typedef ci::LazyList< gc_type, base_item_type, list_traits > bucket_type;

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

    TEST_F( IntrusiveMichaelLazySet_NoGC, base_wrapped_stat )
    {
        struct list_traits: public ci::lazy_list::traits
        {
            typedef ci::lazy_list::base_hook< ci::opt::gc<gc_type>> hook;
            typedef base_class::less<base_item_type> less;
            typedef mock_disposer disposer;
            typedef ci::lazy_list::wrapped_stat<> stat;
        };
        typedef ci::LazyList< gc_type, base_item_type, list_traits > bucket_type;

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

    TEST_F( IntrusiveMichaelLazySet_NoGC, member_cmp )
    {
        typedef ci::LazyList< gc_type
            ,member_item_type
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( member_item_type, hMember ),
                    ci::opt::gc<gc_type>
                > >
                ,ci::opt::compare< cmp<member_item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        >    bucket_type;

        typedef ci::MichaelHashSet< gc_type, bucket_type,
            ci::michael_set::make_traits<
                ci::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveMichaelLazySet_NoGC, member_less )
    {
        typedef ci::LazyList< gc_type
            , member_item_type
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( member_item_type, hMember ),
                    ci::opt::gc<gc_type>
                > >
                ,ci::opt::less< less<member_item_type> >
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

    TEST_F( IntrusiveMichaelLazySet_NoGC, member_cmpmix )
    {
        struct list_traits : public ci::lazy_list::traits
        {
            typedef ci::lazy_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<gc_type>> hook;
            typedef base_class::less<member_item_type> less;
            typedef cmp<member_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::LazyList< gc_type, member_item_type, list_traits > bucket_type;

        struct set_traits : public ci::michael_set::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::MichaelHashSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveMichaelLazySet_NoGC, member_mutex )
    {
        struct list_traits : public ci::lazy_list::traits
        {
            typedef ci::lazy_list::member_hook< offsetof( member_mutex_item_type, hMember ), ci::opt::gc<gc_type>, ci::opt::lock_type<std::mutex>> hook;
            typedef base_class::less<member_mutex_item_type> less;
            typedef cmp<member_mutex_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::LazyList< gc_type, member_mutex_item_type, list_traits > bucket_type;

        struct set_traits : public ci::michael_set::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::MichaelHashSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveMichaelLazySet_NoGC, member_stat )
    {
        struct list_traits: public ci::lazy_list::traits
        {
            typedef ci::lazy_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<gc_type>> hook;
            typedef cmp<member_item_type> compare;
            typedef mock_disposer disposer;
            typedef ci::lazy_list::stat<> stat;
        };
        typedef ci::LazyList< gc_type, member_item_type, list_traits > bucket_type;

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

    TEST_F( IntrusiveMichaelLazySet_NoGC, member_wrapped_stat )
    {
        struct list_traits: public ci::lazy_list::traits
        {
            typedef ci::lazy_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<gc_type>> hook;
            typedef cmp<member_item_type> compare;
            typedef mock_disposer disposer;
            typedef ci::lazy_list::wrapped_stat<> stat;
        };
        typedef ci::LazyList< gc_type, member_item_type, list_traits > bucket_type;

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

} // namespace

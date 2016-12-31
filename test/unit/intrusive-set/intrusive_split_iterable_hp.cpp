/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "test_intrusive_split_iterable_set_hp.h"

#include <cds/intrusive/iterable_list_hp.h>
#include <cds/intrusive/split_list.h>
#include <cds/intrusive/free_list.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveSplitListIterableSet_HP : public cds_test::intrusive_split_iterable_set_hp
    {
    protected:
        typedef cds_test::intrusive_split_iterable_set_hp base_class;
        typedef base_class::item_type< ci::split_list::node<void>> item_type;

    protected:
        void SetUp()
        {
            struct list_traits : public ci::iterable_list::traits
            {};
            typedef ci::IterableList< gc_type, item_type, list_traits > list_type;
            typedef ci::SplitListSet< gc_type, list_type >   set_type;

            // +3 - for iterators
            cds::gc::hp::GarbageCollector::Construct( set_type::c_nHazardPtrCount + 3, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };


    TEST_F( IntrusiveSplitListIterableSet_HP, cmp )
    {
        typedef ci::IterableList< gc_type
            , item_type
            ,ci::iterable_list::make_traits<
                ci::opt::compare< cmp<item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        > bucket_type;

        typedef ci::SplitListSet< gc_type, bucket_type,
            ci::split_list::make_traits<
                ci::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListIterableSet_HP, less )
    {
        typedef ci::IterableList< gc_type
            , item_type
            ,ci::iterable_list::make_traits<
                ci::opt::less< less<item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        > bucket_type;

        typedef ci::SplitListSet< gc_type, bucket_type,
            ci::split_list::make_traits<
                ci::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListIterableSet_HP, cmpmix )
    {
        struct list_traits : public ci::iterable_list::traits
        {
            typedef base_class::less<item_type> less;
            typedef cmp<item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::IterableList< gc_type, item_type, list_traits > bucket_type;

        struct set_traits : public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListIterableSet_HP, free_list )
    {
        struct list_traits: public ci::iterable_list::traits
        {
            typedef base_class::less<item_type> less;
            typedef cmp<item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::IterableList< gc_type, item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef ci::FreeList  free_list;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListIterableSet_HP, static_bucket_table )
    {
        struct list_traits: public ci::iterable_list::traits
        {
            typedef base_class::less<item_type> less;
            typedef cmp<item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::IterableList< gc_type, item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            enum {
                dynamic_bucket_table = false
            };
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListIterableSet_HP, static_bucket_table_free_list )
    {
        struct list_traits: public ci::iterable_list::traits
        {
            typedef base_class::less<item_type> less;
            typedef cmp<item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::IterableList< gc_type, item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            enum {
                dynamic_bucket_table = false
            };
            typedef ci::FreeList free_list;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListIterableSet_HP, list_stat )
    {
        struct list_traits: public ci::iterable_list::traits
        {
            typedef base_class::less<item_type> less;
            typedef cmp<item_type> compare;
            typedef mock_disposer disposer;
            typedef ci::iterable_list::stat<> stat;
        };
        typedef ci::IterableList< gc_type, item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
        EXPECT_GE( s.list_statistics().m_nInsertSuccess, 0u );
    }

    TEST_F( IntrusiveSplitListIterableSet_HP, stat )
    {
        struct list_traits: public ci::iterable_list::traits
        {
            typedef base_class::less<item_type> less;
            typedef cmp<item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::IterableList< gc_type, item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            typedef ci::split_list::stat<> stat;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
        EXPECT_GE( s.statistics().m_nInsertSuccess, 0u );
    }

    TEST_F( IntrusiveSplitListIterableSet_HP, derived_list )
    {
        class bucket_type: public ci::IterableList< gc_type
            , item_type
            ,ci::iterable_list::make_traits<
                ci::opt::compare< cmp<item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        >
        {};

        typedef ci::SplitListSet< gc_type, bucket_type,
            ci::split_list::make_traits<
                ci::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }


} // namespace

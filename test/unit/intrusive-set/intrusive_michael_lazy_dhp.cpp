/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#include "test_intrusive_set_hp.h"

#include <cds/intrusive/lazy_list_dhp.h>
#include <cds/intrusive/michael_set.h>

#include <mutex>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveMichaelLazySet_DHP : public cds_test::intrusive_set_hp
    {
    protected:
        typedef cds_test::intrusive_set_hp base_class;

    protected:
        typedef typename base_class::base_int_item< ci::lazy_list::node<gc_type>>   base_item_type;
        typedef typename base_class::base_int_item< ci::lazy_list::node<gc_type, std::mutex>>   base_mutex_item_type;
        typedef typename base_class::member_int_item< ci::lazy_list::node<gc_type>> member_item_type;
        typedef typename base_class::member_int_item< ci::lazy_list::node<gc_type, std::mutex>> member_mutex_item_type;

        void SetUp()
        {
            struct list_traits : public ci::lazy_list::traits
            {
                typedef ci::lazy_list::base_hook< ci::opt::gc<gc_type>> hook;
            };
            typedef ci::LazyList< gc_type, base_item_type, list_traits > list_type;
            typedef ci::MichaelHashSet< gc_type, list_type >   set_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };


    TEST_F( IntrusiveMichaelLazySet_DHP, base_cmp )
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

    TEST_F( IntrusiveMichaelLazySet_DHP, base_less )
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

    TEST_F( IntrusiveMichaelLazySet_DHP, base_cmpmix )
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

    TEST_F( IntrusiveMichaelLazySet_DHP, base_mutex )
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


    TEST_F( IntrusiveMichaelLazySet_DHP, member_cmp )
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

    TEST_F( IntrusiveMichaelLazySet_DHP, member_less )
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

    TEST_F( IntrusiveMichaelLazySet_DHP, member_cmpmix )
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

    TEST_F( IntrusiveMichaelLazySet_DHP, member_mutex )
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

} // namespace

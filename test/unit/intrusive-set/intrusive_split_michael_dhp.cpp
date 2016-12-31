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

#include "test_intrusive_set_hp.h"

#include <cds/intrusive/michael_list_dhp.h>
#include <cds/intrusive/split_list.h>
#include <cds/intrusive/free_list.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveSplitListSet_DHP : public cds_test::intrusive_set_hp
    {
    protected:
        typedef cds_test::intrusive_set_hp base_class;

    protected:
        typedef typename base_class::base_int_item< ci::split_list::node< ci::michael_list::node<gc_type>>>   base_item_type;
        typedef typename base_class::member_int_item< ci::split_list::node< ci::michael_list::node<gc_type>>> member_item_type;

        void SetUp()
        {
            struct list_traits : public ci::michael_list::traits
            {
                typedef ci::michael_list::base_hook< ci::opt::gc<gc_type>> hook;
            };
            typedef ci::MichaelList< gc_type, base_item_type, list_traits > list_type;
            typedef ci::SplitListSet< gc_type, list_type >   set_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };


    TEST_F( IntrusiveSplitListSet_DHP, base_cmp )
    {
        typedef ci::MichaelList< gc_type
            , base_item_type
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< ci::opt::gc< gc_type > > >
                ,ci::opt::compare< cmp<base_item_type> >
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

    TEST_F( IntrusiveSplitListSet_DHP, base_less )
    {
        typedef ci::MichaelList< gc_type
            , base_item_type
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< ci::opt::gc< gc_type >>>
                ,ci::opt::less< less<base_item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        > bucket_type;

        typedef ci::SplitListSet< gc_type, bucket_type,
            ci::split_list::make_traits<
                ci::opt::hash< hash_int >
                ,ci::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, base_cmpmix )
    {
        struct list_traits : public ci::michael_list::traits
        {
            typedef ci::michael_list::base_hook< ci::opt::gc<gc_type>> hook;
            typedef base_class::less<base_item_type> less;
            typedef cmp<base_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::MichaelList< gc_type, base_item_type, list_traits > bucket_type;

        struct set_traits : public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            typedef ci::split_list::stat<> stat;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, base_static_bucket_table )
    {
        struct list_traits: public ci::michael_list::traits
        {
            typedef ci::michael_list::base_hook< ci::opt::gc<gc_type>> hook;
            typedef base_class::less<base_item_type> less;
            typedef cmp<base_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::MichaelList< gc_type, base_item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            typedef ci::split_list::stat<> stat;
            enum {
                dynamic_bucket_table = false
            };
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, base_static_bucket_table_free_list )
    {
        struct list_traits: public ci::michael_list::traits
        {
            typedef ci::michael_list::base_hook< ci::opt::gc<gc_type>> hook;
            typedef cmp<base_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::MichaelList< gc_type, base_item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            typedef ci::split_list::stat<> stat;
            enum {
                dynamic_bucket_table = false
            };
            typedef ci::FreeList free_list;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, base_free_list )
    {
        struct list_traits: public ci::michael_list::traits
        {
            typedef ci::michael_list::base_hook< ci::opt::gc<gc_type>> hook;
            typedef base_class::less<base_item_type> less;
            typedef mock_disposer disposer;
        };
        typedef ci::MichaelList< gc_type, base_item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            typedef ci::split_list::stat<> stat;
            typedef ci::FreeList free_list;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, member_cmp )
    {
        typedef ci::MichaelList< gc_type
            ,member_item_type
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( member_item_type, hMember ),
                    ci::opt::gc<gc_type>
                > >
                ,ci::opt::compare< cmp<member_item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< gc_type, bucket_type,
            ci::split_list::make_traits<
                ci::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, member_less )
    {
        typedef ci::MichaelList< gc_type
            , member_item_type
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( member_item_type, hMember ),
                    ci::opt::gc<gc_type>
                > >
                ,ci::opt::less< less<member_item_type> >
                ,ci::opt::disposer< mock_disposer >
            >::type
        > bucket_type;

        typedef ci::SplitListSet< gc_type, bucket_type,
            ci::split_list::make_traits<
                ci::opt::hash< hash_int >
                ,ci::opt::back_off< cds::backoff::pause >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, member_cmpmix )
    {
        struct list_traits : public ci::michael_list::traits
        {
            typedef ci::michael_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<gc_type>> hook;
            typedef base_class::less<member_item_type> less;
            typedef cmp<member_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::MichaelList< gc_type, member_item_type, list_traits > bucket_type;

        struct set_traits : public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, member_static_bucket_table )
    {
        struct list_traits: public ci::michael_list::traits
        {
            typedef ci::michael_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<gc_type>> hook;
            typedef base_class::less<member_item_type> less;
            typedef cmp<member_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::MichaelList< gc_type, member_item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            enum {
                dynami_bucket_table = false
            };
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, member_static_bucket_table_free_list )
    {
        struct list_traits: public ci::michael_list::traits
        {
            typedef ci::michael_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<gc_type>> hook;
            typedef base_class::less<member_item_type> less;
            typedef mock_disposer disposer;
        };
        typedef ci::MichaelList< gc_type, member_item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            enum {
                dynami_bucket_table = false
            };
            typedef ci::FreeList free_list;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_DHP, member_free_list )
    {
        struct list_traits: public ci::michael_list::traits
        {
            typedef ci::michael_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<gc_type>> hook;
            typedef base_class::less<member_item_type> less;
            typedef mock_disposer disposer;
        };
        typedef ci::MichaelList< gc_type, member_item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            typedef ci::FreeList free_list;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

} // namespace

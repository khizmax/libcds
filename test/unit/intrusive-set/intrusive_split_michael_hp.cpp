// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_set_hp.h"

#include <cds/intrusive/michael_list_hp.h>
#include <cds/intrusive/split_list.h>
#include <cds/intrusive/free_list.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveSplitListSet_HP : public cds_test::intrusive_set_hp
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

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( set_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };


    TEST_F( IntrusiveSplitListSet_HP, base_cmp )
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

    TEST_F( IntrusiveSplitListSet_HP, base_less )
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

    TEST_F( IntrusiveSplitListSet_HP, base_cmpmix )
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

    TEST_F( IntrusiveSplitListSet_HP, base_static_bucket_table )
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

    TEST_F( IntrusiveSplitListSet_HP, base_static_bucket_table_free_list )
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
            enum {
                dynamic_bucket_table = false
            };
            typedef ci::FreeList free_list;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_HP, base_free_list )
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
            typedef ci::FreeList free_list;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_HP, base_bit_reversal_swar )
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
            typedef cds::algo::bit_reversal::swar bit_reversal;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_HP, base_bit_reversal_lookup )
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
            typedef cds::algo::bit_reversal::lookup bit_reversal;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_HP, base_bit_reversal_muldiv )
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
            typedef cds::algo::bit_reversal::muldiv bit_reversal;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_HP, member_cmp )
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

    TEST_F( IntrusiveSplitListSet_HP, member_less )
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

    TEST_F( IntrusiveSplitListSet_HP, member_cmpmix )
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

    TEST_F( IntrusiveSplitListSet_HP, member_static_bucket_table )
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
                dynamic_bucket_table = false
            };
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_HP, member_static_bucket_table_free_list )
    {
        struct list_traits: public ci::michael_list::traits
        {
            typedef ci::michael_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<gc_type>> hook;
            typedef cmp<member_item_type> compare;
            typedef mock_disposer disposer;
        };
        typedef ci::MichaelList< gc_type, member_item_type, list_traits > bucket_type;

        struct set_traits: public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            enum {
                dynamic_bucket_table = false
            };
            typedef ci::FreeList free_list;
        };
        typedef ci::SplitListSet< gc_type, bucket_type, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( IntrusiveSplitListSet_HP, member_free_list )
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

// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_set_hp.h"

#include <cds/intrusive/skip_list_hp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveSkipListSet_HP : public cds_test::intrusive_set_hp
    {
    protected:
        typedef cds_test::intrusive_set_hp base_class;

    protected:
        typedef typename base_class::base_int_item< ci::skip_list::node< gc_type>>   base_item_type;
        typedef typename base_class::member_int_item< ci::skip_list::node< gc_type>> member_item_type;

        void SetUp()
        {
            typedef ci::SkipListSet< gc_type, base_item_type,
                typename ci::skip_list::make_traits<
                    ci::opt::hook<ci::skip_list::base_hook< ci::opt::gc< gc_type >>>
                    ,ci::opt::disposer<mock_disposer>
                    ,ci::opt::compare<mock_disposer>
                >::type
            > set_type;

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


    TEST_F( IntrusiveSkipListSet_HP, base_cmp )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<base_item_type> compare;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, base_less )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef base_class::less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, base_cmpmix )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<base_item_type> compare;
            typedef base_class::less<base_item_type> less;
            typedef ci::skip_list::stat<> stat;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, base_xorshift32 )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<base_item_type> compare;
            typedef ci::skip_list::xorshift32 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, base_xorshift24 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<base_item_type> compare;
            typedef ci::skip_list::xorshift24 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, base_xorshift16 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<base_item_type> compare;
            typedef ci::skip_list::xorshift16 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, base_turbo32 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<base_item_type> compare;
            typedef ci::skip_list::turbo32 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, base_turbo24 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<base_item_type> compare;
            typedef ci::skip_list::turbo24 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, base_turbo16 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<base_item_type> compare;
            typedef ci::skip_list::turbo16 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, member_cmp )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof(member_item_type, hMember), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<member_item_type> compare;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, member_less )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef base_class::less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef ci::opt::v::sequential_consistent memory_model;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, member_cmpmix )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<member_item_type> compare;
            typedef base_class::less<member_item_type> less;
            typedef ci::skip_list::stat<> stat;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, member_xorshift32 )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<member_item_type> compare;
            typedef ci::skip_list::xorshift32 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, member_xorshift24 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<member_item_type> compare;
            typedef ci::skip_list::xorshift24 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, member_xorshift16 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<member_item_type> compare;
            typedef ci::skip_list::xorshift16 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, member_turbo32 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<member_item_type> compare;
            typedef ci::skip_list::turbo32 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, member_turbo24 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<member_item_type> compare;
            typedef ci::skip_list::turbo24 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveSkipListSet_HP, member_turbo16 )
    {
        struct traits: public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<member_item_type> compare;
            typedef ci::skip_list::turbo16 random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

} // namespace

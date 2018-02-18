// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_set_nogc.h"

#include <cds/intrusive/skip_list_nogc.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::nogc gc_type;

    class IntrusiveSkipListSet_NoGC : public cds_test::intrusive_set_nogc
    {
    protected:
        typedef cds_test::intrusive_set_nogc base_class;

    protected:
        typedef typename base_class::base_int_item< ci::skip_list::node< gc_type>>   base_item_type;
        typedef typename base_class::member_int_item< ci::skip_list::node< gc_type>> member_item_type;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };


    TEST_F( IntrusiveSkipListSet_NoGC, base_cmp )
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

    TEST_F( IntrusiveSkipListSet_NoGC, base_less )
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

    TEST_F( IntrusiveSkipListSet_NoGC, base_cmpmix )
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

    TEST_F( IntrusiveSkipListSet_NoGC, base_xorshift32 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, base_xorshift24 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, base_xorshift16 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, base_turbo32 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, base_turbo24 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, base_turbo16 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, member_cmp )
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

    TEST_F( IntrusiveSkipListSet_NoGC, member_less )
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

    TEST_F( IntrusiveSkipListSet_NoGC, member_cmpmix )
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

    TEST_F( IntrusiveSkipListSet_NoGC, member_xorshift32 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, member_xorshift24 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, member_xorshift16 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, member_turbo32 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, member_turbo24 )
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

    TEST_F( IntrusiveSkipListSet_NoGC, member_turbo16 )
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

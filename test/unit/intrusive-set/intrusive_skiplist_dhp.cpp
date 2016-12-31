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

#include <cds/intrusive/skip_list_dhp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveSkipListSet_DHP : public cds_test::intrusive_set_hp
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

            cds::gc::dhp::GarbageCollector::Construct( 16, set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };


    TEST_F( IntrusiveSkipListSet_DHP, base_cmp )
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

    TEST_F( IntrusiveSkipListSet_DHP, base_less )
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

    TEST_F( IntrusiveSkipListSet_DHP, base_cmpmix )
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

    TEST_F( IntrusiveSkipListSet_DHP, base_xorshift )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<base_item_type> compare;
            typedef ci::skip_list::xorshift random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, base_item_type, traits > set_type;

        set_type s;
        test( s );
    }


    TEST_F( IntrusiveSkipListSet_DHP, member_cmp )
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

    TEST_F( IntrusiveSkipListSet_DHP, member_less )
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

    TEST_F( IntrusiveSkipListSet_DHP, member_cmpmix )
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

    TEST_F( IntrusiveSkipListSet_DHP, member_xorshift )
    {
        struct traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp<member_item_type> compare;
            typedef ci::skip_list::xorshift random_level_generator;
        };

        typedef ci::SkipListSet< gc_type, member_item_type, traits > set_type;

        set_type s;
        test( s );
    }

} // namespace

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

#include "test_intrusive_list_hp.h"
#include <cds/intrusive/lazy_list_dhp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveLazyList_DHP : public cds_test::intrusive_list_hp
    {
    public:
        typedef cds_test::intrusive_list_hp::base_item< ci::lazy_list::node< gc_type>> base_item;
        typedef cds_test::intrusive_list_hp::member_item< ci::lazy_list::node< gc_type>> member_item;

        typedef cds_test::intrusive_list_hp::base_item< ci::lazy_list::node< gc_type, std::mutex>> base_mutex_item;
        typedef cds_test::intrusive_list_hp::member_item< ci::lazy_list::node< gc_type, std::mutex>> member_mutex_item;

    protected:
        void SetUp()
        {
            struct traits: public ci::lazy_list::traits
            {
                typedef ci::lazy_list::base_hook< cds::opt::gc< gc_type >> hook;
            };
            typedef ci::LazyList< gc_type, base_item, traits > list_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, list_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };

    TEST_F( IntrusiveLazyList_DHP, base_hook )
    {
        typedef ci::LazyList< gc_type, base_item,
            typename ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< cds::opt::gc< gc_type >>>
                ,ci::opt::disposer< mock_disposer >
                ,cds::opt::less< less< base_item >>
            >::type
       > list_type;

       list_type l;
       test_common( l );
       test_ordered_iterator( l );
       test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, base_hook_cmp )
    {
        typedef ci::LazyList< gc_type, base_item,
            typename ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< cds::opt::gc< gc_type >>>
                , ci::opt::disposer< mock_disposer >
                , cds::opt::compare< cmp< base_item >>
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, base_hook_item_counting )
    {
        struct traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_common::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::LazyList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, base_hook_mutex )
    {
        struct traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< cds::opt::gc< gc_type >, cds::opt::lock_type< std::mutex>> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_mutex_item > compare;
            typedef intrusive_list_common::less< base_mutex_item > less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::LazyList< gc_type, base_mutex_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, base_hook_backoff )
    {
        struct traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_common::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
        };
        typedef ci::LazyList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, base_hook_seqcst )
    {
        struct traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_common::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef ci::LazyList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, base_hook_stat )
    {
        struct traits: public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_common::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::lazy_list::stat<> stat;
        };
        typedef ci::LazyList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, base_hook_wrapped_stat )
    {
        struct traits: public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef intrusive_list_common::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::lazy_list::wrapped_stat<> stat;
        };
        typedef ci::LazyList< gc_type, base_item, traits > list_type;

        cds::intrusive::lazy_list::stat<> st;
        list_type l( st );
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, member_hook )
    {
        typedef ci::LazyList< gc_type, member_item,
            typename ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >>>
                ,ci::opt::disposer< mock_disposer >
                ,cds::opt::less< less< member_item >>
            >::type
       > list_type;

       list_type l;
       test_common( l );
       test_ordered_iterator( l );
       test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, member_hook_cmp )
    {
        typedef ci::LazyList< gc_type, member_item,
            typename ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >>>
                ,ci::opt::disposer< mock_disposer >
                ,cds::opt::compare< cmp< member_item >>
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, member_hook_item_counting )
    {
        struct traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_common::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::LazyList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, member_hook_seqcst )
    {
        struct traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_common::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef ci::LazyList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, member_hook_mutex )
    {
        struct traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::member_hook< offsetof( member_mutex_item, hMember ), cds::opt::gc< gc_type >, cds::opt::lock_type< std::mutex >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_mutex_item > compare;
            typedef intrusive_list_common::less< member_mutex_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef ci::LazyList< gc_type, member_mutex_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, member_hook_back_off )
    {
        struct traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_common::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef ci::LazyList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, member_hook_stat )
    {
        struct traits: public ci::lazy_list::traits {
            typedef ci::lazy_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_common::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::lazy_list::stat<> stat;
        };
        typedef ci::LazyList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveLazyList_DHP, member_hook_wrapped_stat )
    {
        struct traits: public ci::lazy_list::traits {
            typedef ci::lazy_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_common::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::lazy_list::wrapped_stat<> stat;
        };
        typedef ci::LazyList< gc_type, member_item, traits > list_type;

        cds::intrusive::lazy_list::stat<> st;
        list_type l( st );
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

} // namespace

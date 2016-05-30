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

#include "test_intrusive_list_hp.h"
#include <cds/intrusive/michael_list_hp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveMichaelList_HP : public cds_test::intrusive_list_hp
    {
    public:
        typedef cds_test::intrusive_list_hp::base_item< ci::michael_list::node< gc_type>> base_item;
        typedef cds_test::intrusive_list_hp::member_item< ci::michael_list::node< gc_type>> member_item;

    protected:
        void SetUp()
        {
            struct traits: public ci::michael_list::traits
            {
                typedef ci::michael_list::base_hook< cds::opt::gc< gc_type >> hook;
            };
            typedef ci::MichaelList< gc_type, base_item, traits > list_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( list_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

    TEST_F( IntrusiveMichaelList_HP, base_hook )
    {
        typedef ci::MichaelList< gc_type, base_item,
            typename ci::michael_list::make_traits< 
                ci::opt::hook< ci::michael_list::base_hook< cds::opt::gc< gc_type >>>
                ,ci::opt::disposer< mock_disposer >
                ,cds::opt::less< less< base_item >>
            >::type 
       > list_type;

       list_type l;
       test_common( l );
       test_ordered_iterator( l );
       test_hp( l );
    }

    TEST_F( IntrusiveMichaelList_HP, base_hook_cmp )
    {
        typedef ci::MichaelList< gc_type, base_item,
            typename ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< cds::opt::gc< gc_type >>>
                , ci::opt::disposer< mock_disposer >
                , cds::opt::compare< cmp< base_item >>
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveMichaelList_HP, base_hook_item_counting )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_common::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::MichaelList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveMichaelList_HP, base_hook_backoff )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_common::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
        };
        typedef ci::MichaelList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveMichaelList_HP, base_hook_seqcst )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_common::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef ci::MichaelList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveMichaelList_HP, member_hook )
    {
        typedef ci::MichaelList< gc_type, member_item,
            typename ci::michael_list::make_traits< 
                ci::opt::hook< ci::michael_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >>>
                ,ci::opt::disposer< mock_disposer >
                ,cds::opt::less< less< member_item >>
            >::type 
       > list_type;

       list_type l;
       test_common( l );
       test_ordered_iterator( l );
       test_hp( l );
    }

    TEST_F( IntrusiveMichaelList_HP, member_hook_cmp )
    {
        typedef ci::MichaelList< gc_type, member_item,
            typename ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >>>
                ,ci::opt::disposer< mock_disposer >
                ,cds::opt::compare< cmp< member_item >>
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveMichaelList_HP, member_hook_item_counting )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_common::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::MichaelList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveMichaelList_HP, member_hook_seqcst )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_common::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef ci::MichaelList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveMichaelList_HP, member_hook_back_off )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_common::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef ci::MichaelList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

} // namespace

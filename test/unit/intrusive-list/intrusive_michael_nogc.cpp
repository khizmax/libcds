// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_list_nogc.h"
#include <cds/intrusive/michael_list_nogc.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::nogc gc_type;

    class IntrusiveMichaelList_NOGC : public cds_test::intrusive_list_nogc
    {
    public:
        typedef cds_test::intrusive_list_nogc::base_item< ci::michael_list::node< gc_type>> base_item;
        typedef cds_test::intrusive_list_nogc::member_item< ci::michael_list::node< gc_type>> member_item;
    };

    TEST_F( IntrusiveMichaelList_NOGC, base_hook )
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
    }

    TEST_F( IntrusiveMichaelList_NOGC, base_hook_cmp )
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
    }

    TEST_F( IntrusiveMichaelList_NOGC, base_hook_item_counting )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_nogc::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::MichaelList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
    }

    TEST_F( IntrusiveMichaelList_NOGC, base_hook_backoff )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_nogc::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
        };
        typedef ci::MichaelList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
    }

    TEST_F( IntrusiveMichaelList_NOGC, base_hook_seqcst )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::base_hook< cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< base_item > compare;
            typedef intrusive_list_nogc::less< base_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef ci::MichaelList< gc_type, base_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
    }

    TEST_F( IntrusiveMichaelList_NOGC, member_hook )
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
    }

    TEST_F( IntrusiveMichaelList_NOGC, member_hook_cmp )
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
    }

    TEST_F( IntrusiveMichaelList_NOGC, member_hook_item_counting )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_nogc::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::MichaelList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
    }

    TEST_F( IntrusiveMichaelList_NOGC, member_hook_seqcst )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_nogc::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef ci::MichaelList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
    }

    TEST_F( IntrusiveMichaelList_NOGC, member_hook_back_off )
    {
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::member_hook< offsetof( member_item, hMember ), cds::opt::gc< gc_type >> hook;
            typedef mock_disposer disposer;
            typedef cmp< member_item > compare;
            typedef intrusive_list_nogc::less< member_item > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef ci::MichaelList< gc_type, member_item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
    }

} // namespace

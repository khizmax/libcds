//$$CDS-header$$

#include "set/hdr_intrusive_skiplist_set.h"

#include <cds/intrusive/skip_list_hp.h>
#include "map/print_skiplist_stat.h"

namespace set {
        void IntrusiveSkipListSet::skiplist_hp_member_cmp()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_less()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_cmpmix()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_cmp_stat()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_less_stat()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_cmpmix_stat()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_cmp_xorshift()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_less_xorshift()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_cmpmix_xorshift()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

        void IntrusiveSkipListSet::skiplist_hp_member_cmp_xorshift_stat()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_less_xorshift_stat()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_cmpmix_xorshift_stat()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }


    void IntrusiveSkipListSet::skiplist_hp_member_cmp_pascal()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_less_pascal()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_cmpmix_pascal()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

        void IntrusiveSkipListSet::skiplist_hp_member_cmp_pascal_stat()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_less_pascal_stat()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_member_cmpmix_pascal_stat()
    {
        typedef member_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::member_hook< offsetof(item, hMember), co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

} // namespace set

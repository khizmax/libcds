//$$CDS-header$$

#include "set/hdr_intrusive_skiplist_set.h"

#include <cds/intrusive/skip_list_nogc.h>
#include "map/print_skiplist_stat.h"

namespace set {

    void IntrusiveSkipListSet::skiplist_nogc_base_cmp()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_less()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_cmpmix()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::compare< cmp<item> >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_cmp_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::compare< cmp<item> >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_less_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_cmpmix_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_cmp_xorshift()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_less_xorshift()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_cmpmix_xorshift()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

        void IntrusiveSkipListSet::skiplist_nogc_base_cmp_xorshift_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_less_xorshift_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_cmpmix_xorshift_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }


    //*********
    void IntrusiveSkipListSet::skiplist_nogc_base_cmp_pascal()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_less_pascal()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_cmpmix_pascal()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

        void IntrusiveSkipListSet::skiplist_nogc_base_cmp_pascal_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_less_pascal_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_nogc_base_cmpmix_pascal_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::nogc> > item;

        typedef ci::SkipListSet< cds::gc::nogc, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist_nogc<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

}   // namespace set

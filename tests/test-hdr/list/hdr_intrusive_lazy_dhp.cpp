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

#include "list/hdr_intrusive_lazy.h"
#include <cds/intrusive/lazy_list_dhp.h>

namespace ordlist {
    void IntrusiveLazyListHeaderTest::DHP_base_cmp()
    {
        typedef base_int_item< cds::gc::DHP > item;
        struct traits : public ci::lazy_list::traits
        {
            typedef ci::lazy_list::base_hook< co::gc<cds::gc::DHP> > hook;
            typedef cmp<item> compare;
            typedef faked_disposer disposer;
        };
        typedef ci::LazyList< cds::gc::DHP, item, traits > list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_base_less()
    {
        typedef base_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_base_cmpmix()
    {
        typedef base_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_base_ic()
    {
        typedef base_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_member_cmp()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_member_less()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_member_cmpmix()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_member_ic()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_int<list>();
    }

} // namespace ordlist

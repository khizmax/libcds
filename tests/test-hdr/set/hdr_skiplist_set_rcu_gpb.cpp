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

#include "set/hdr_skiplist_set_rcu.h"

#include <cds/urcu/general_buffered.h>
#include <cds/container/skip_list_set_rcu.h>

#include "unit/michael_alloc.h"
#include "map/print_skiplist_stat.h"

namespace set {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_less()
    {
        struct set_traits : public cc::skip_list::traits
        {
            typedef SkipListSetRCUHdrTest::less<item> less;
            typedef simple_item_counter item_counter;
        };
        typedef cc::SkipListSet< rcu_type, item, set_traits > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_cmp()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
            co::compare< cmp<item > >
            ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_cmpless()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
            co::less< less<item > >
            ,co::compare< cmp<item > >
            ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_less_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_cmp_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_cmpless_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_xorshift_less()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_xorshift_cmp()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_xorshift_cmpless()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_xorshift_less_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_xorshift_cmp_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_xorshift_cmpless_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_turbopas_less()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_turbopas_cmp()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_turbopas_cmpless()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_turbopas_less_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_turbopas_cmp_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_turbopas_cmpless_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_michaelalloc_less()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_michaelalloc_cmp()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_michaelalloc_cmpless()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_michaelalloc_less_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_michaelalloc_cmp_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPB_michaelalloc_cmpless_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }
} // namespace set

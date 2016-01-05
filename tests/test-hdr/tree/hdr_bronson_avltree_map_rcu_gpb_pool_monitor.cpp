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

#include "tree/hdr_bronson_avltree_map.h"
#include <cds/urcu/general_buffered.h>
#include <cds/container/bronson_avltree_map_rcu.h>
#include <cds/sync/pool_monitor.h>
#include <cds/memory/vyukov_queue_pool.h>

#include "unit/print_bronsonavltree_stat.h"
#include "unit/print_sync_monitor_stat.h"

namespace tree {
    namespace cc = cds::container;
    namespace co = cds::opt;
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;

        struct print_stat {
            template <typename Tree>
            void operator()( Tree const& t ) const
            {
                std::cout << t.statistics();
                std::cout << t.monitor().statistics();
            }
        };

        typedef cds::memory::vyukov_queue_pool< std::mutex > simple_pool;
        typedef cds::memory::lazy_vyukov_queue_pool< std::mutex > lazy_pool;
        typedef cds::memory::bounded_vyukov_queue_pool< std::mutex > bounded_pool;
    } // namespace

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_gpb_pool_monitor_less()
    {
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,co::sync_monitor< cds::sync::pool_monitor<simple_pool> >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_gpb_pool_monitor_less_stat()
    {
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,co::sync_monitor< cds::sync::pool_monitor<lazy_pool, cds::opt::none, true >>
                ,cc::bronson_avltree::relaxed_insert< false >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
    }

        void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_gpb_pool_monitor_cmp_ic_stat()
    {
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,co::sync_monitor< cds::sync::pool_monitor<bounded_pool, cds::opt::none, true >>
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_gpb_pool_monitor_cmp_ic_stat_yield()
    {
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,co::back_off< cds::backoff::yield >
                ,co::sync_monitor< cds::sync::pool_monitor<lazy_pool, cds::opt::none, true >>
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_gpb_pool_monitor_less_relaxed_insert()
    {
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,cc::bronson_avltree::relaxed_insert< true >
                ,co::sync_monitor< cds::sync::pool_monitor<lazy_pool> >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_gpb_pool_monitor_less_relaxed_insert_stat()
    {
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,cc::bronson_avltree::relaxed_insert< true >
                ,co::sync_monitor< cds::sync::pool_monitor<simple_pool, cds::opt::none, true >>
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
    }

} // namespace tree

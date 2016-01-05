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
#include <cds/urcu/signal_threaded.h>
#include <cds/container/bronson_avltree_map_rcu.h>

#include "unit/print_bronsonavltree_stat.h"

namespace tree {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace cc = cds::container;
    namespace co = cds::opt;
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_threaded<> > rcu_type;

        struct print_stat {
            template <typename Tree>
            void operator()( Tree const& t )
            {
                std::cout << t.statistics();
            }
        };
    } // namespace
#endif

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,cc::bronson_avltree::relaxed_insert< false >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_less_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,cc::bronson_avltree::relaxed_insert< false >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_cmp_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::stat< cc::bronson_avltree::stat<> >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_cmpless()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::less< std::less<key_type> >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_less_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_cmp_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_cmp_ic_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::bronson_avltree::stat<> >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_cmp_ic_stat_yield()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,co::back_off< cds::backoff::yield >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_less_relaxed_insert()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,cc::bronson_avltree::relaxed_insert< true >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_sht_less_relaxed_insert_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,cc::bronson_avltree::relaxed_insert< true >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

} // namespace tree

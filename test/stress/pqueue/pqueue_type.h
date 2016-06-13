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

#ifndef CDSSTRESS_PQUEUE_TYPES_H
#define CDSSTRESS_PQUEUE_TYPES_H

#include <cds/urcu/general_instant.h>
#include <cds/urcu/general_buffered.h>
#include <cds/urcu/general_threaded.h>
#include <cds/urcu/signal_buffered.h>
#include <cds/urcu/signal_threaded.h>

#include <cds/container/mspriority_queue.h>
#include <cds/container/fcpriority_queue.h>

#include <cds/container/ellen_bintree_set_hp.h>
#include <cds/container/ellen_bintree_set_dhp.h>
#include <cds/container/ellen_bintree_set_rcu.h>

#include <cds/container/skip_list_set_hp.h>
#include <cds/container/skip_list_set_dhp.h>
#include <cds/container/skip_list_set_rcu.h>

#include <cds/sync/spinlock.h>

#include <queue>
#include <vector>
#include <deque>
#include <mutex> //unique_lock

#include <boost/container/stable_vector.hpp>
#include <boost/container/deque.hpp>

#include <cds_test/stress_test.h>
#include <cds_test/stat_ellenbintree_out.h>
#include <cds_test/stat_skiplist_out.h>
#include <cds_test/stat_flat_combining_out.h>

namespace pqueue {
    namespace cc = cds::container;
    namespace co = cds::opt;

    namespace details {
        template <typename T, typename Container, typename Lock, typename Less = std::less<typename Container::value_type> >
        class StdPQueue
        {
        public:
            typedef T value_type;
            typedef std::priority_queue<value_type, Container, Less> pqueue_type;

        private:
            pqueue_type     m_PQueue;
            mutable Lock    m_Lock;

            typedef std::unique_lock<Lock> scoped_lock;

        public:
            bool push( value_type const& val )
            {
                scoped_lock l( m_Lock );
                m_PQueue.push( val );
                return true;
            }

            bool pop( value_type& dest )
            {
                scoped_lock l( m_Lock );
                if ( !m_PQueue.empty() ) {
                    dest = m_PQueue.top();
                    m_PQueue.pop();
                    return true;
                }
                return false;
            }

            template <typename Q, typename MoveFunc>
            bool pop_with( Q& dest, MoveFunc f )
            {
                scoped_lock l( m_Lock );
                if ( !m_PQueue.empty() ) {
                    f( dest, m_PQueue.top() );
                    m_PQueue.pop();
                    return true;
                }
                return false;
            }

            void clear()
            {
                scoped_lock l( m_Lock );
                while ( !m_PQueue.empty() )
                    m_PQueue.pop();
            }

            template <typename Func>
            void clear_with( Func f )
            {
                scoped_lock l( m_Lock );
                while ( !m_PQueue.empty() ) {
                    f( m_PQueue.top() );
                    m_PQueue.pop();
                }
            }

            bool empty() const
            {
                return m_PQueue.empty();
            }

            size_t size() const
            {
                return m_PQueue.size();
            }

            cds::opt::none statistics() const
            {
                return cds::opt::none();
            }
        };

        // EllenBinTree priority queue
        template <typename GC>
        struct EllenBinTreePQueue_pop_max
        {
            template <typename T, typename Tree>
            bool operator()( T& dest, Tree& container ) const
            {
                typename Tree::guarded_ptr gp( container.extract_max() );
                if ( gp )
                    dest = *gp;
                return !gp.empty();
            }
        };

        template <typename RCU>
        struct EllenBinTreePQueue_pop_max< cds::urcu::gc<RCU> >
        {
            template <typename T, typename Tree>
            bool operator()( T& dest, Tree& container ) const
            {
                typename Tree::exempt_ptr ep( container.extract_max() );
                if ( ep )
                    dest = *ep;
                return !ep.empty();
            }
        };

        template <typename GC>
        struct EllenBinTreePQueue_pop_min
        {
            template <typename T, typename Tree>
            bool operator()( T& dest, Tree& container ) const
            {
                typename Tree::guarded_ptr gp( container.extract_min() );
                if ( gp )
                    dest = *gp;
                return !gp.empty();
            }
        };

        template <typename RCU>
        struct EllenBinTreePQueue_pop_min< cds::urcu::gc<RCU> >
        {
            template <typename T, typename Tree>
            bool operator()( T& dest, Tree& container ) const
            {
                typename Tree::exempt_ptr ep( container.extract_min() );
                if ( ep )
                    dest = *ep;
                return !ep.empty();
            }
        };

        template <typename GC, typename Key, typename T, typename Traits, bool Max = true>
        class EllenBinTreePQueue : protected cds::container::EllenBinTreeSet< GC, Key, T, Traits >
        {
            typedef cds::container::EllenBinTreeSet< GC, Key, T, Traits > base_class;
            template <typename GC2> friend struct EllenBinTreePQueue_pop_max;
            template <typename GC2> friend struct EllenBinTreePQueue_pop_min;

        public:
            typedef T value_type;

            bool push( value_type const& val )
            {
                return base_class::insert( val );
            }

            bool pop( value_type& dest )
            {
                return Max ? EllenBinTreePQueue_pop_max< typename base_class::gc >()(dest, *this)
                    : EllenBinTreePQueue_pop_min< typename base_class::gc >()(dest, *this);
            }

            void clear()
            {
                base_class::clear();
            }

            bool empty() const
            {
                return base_class::empty();
            }

            size_t size() const
            {
                return base_class::size();
            }

            typename base_class::stat const& statistics() const
            {
                return base_class::statistics();
            }
        };


        // SkipList property queue
        template <typename GC>
        struct SkipListPQueue_pop_max
        {
            template <typename T, typename Set>
            bool operator()( T& dest, Set& container ) const
            {
                typename Set::guarded_ptr gp( container.extract_max() );
                if ( gp )
                    dest = *gp;
                return !gp.empty();
            }
        };

        template <typename RCU>
        struct SkipListPQueue_pop_max< cds::urcu::gc<RCU> >
        {
            template <typename T, typename Set>
            bool operator()( T& dest, Set& container ) const
            {
                typename Set::exempt_ptr ep( container.extract_max() );
                if ( ep )
                    dest = *ep;
                return !ep.empty();
            }
        };

        template <typename GC>
        struct SkipListPQueue_pop_min
        {
            template <typename T, typename Set>
            bool operator()( T& dest, Set& container ) const
            {
                typename Set::guarded_ptr gp( container.extract_min() );
                if ( gp )
                    dest = *gp;
                return !gp.empty();
            }
        };

        template <typename RCU>
        struct SkipListPQueue_pop_min< cds::urcu::gc<RCU> >
        {
            template <typename T, typename Set>
            bool operator()( T& dest, Set& container ) const
            {
                typename Set::exempt_ptr ep( container.extract_min() );
                if ( ep )
                    dest = *ep;
                return !ep.empty();
            }
        };

        template <typename GC, typename T, typename Traits, bool Max = true>
        class SkipListPQueue : protected cds::container::SkipListSet< GC, T, Traits >
        {
            typedef cds::container::SkipListSet< GC, T, Traits > base_class;
            template <typename GC2> friend struct SkipListPQueue_pop_max;
            template <typename GC2> friend struct SkipListPQueue_pop_min;

        public:
            typedef T value_type;

            bool push( value_type const& val )
            {
                return base_class::insert( val );
            }

            bool pop( value_type& dest )
            {
                return Max ? SkipListPQueue_pop_max< typename base_class::gc >()(dest, *this)
                    : SkipListPQueue_pop_min< typename base_class::gc >()(dest, *this);
            }

            void clear()
            {
                base_class::clear();
            }

            bool empty() const
            {
                return base_class::empty();
            }

            size_t size() const
            {
                return base_class::size();
            }

            typename base_class::stat const& statistics() const
            {
                return base_class::statistics();
            }
        };

    } // namespace details

    template <typename Value>
    struct Types
    {
        static size_t const c_nBoundedCapacity = 1024 * 1024 * 16;

        typedef std::less<Value>    less;

        struct cmp {
            int operator()( Value const& v1, Value const& v2 ) const
            {
                return less()( v1, v2 ) ? -1 : less()( v2, v1 ) ? 1 : 0;
            }
        };

        typedef cds::urcu::gc< cds::urcu::general_instant<> >   rcu_gpi;
        typedef cds::urcu::gc< cds::urcu::general_buffered<> >  rcu_gpb;
        typedef cds::urcu::gc< cds::urcu::general_threaded<> >  rcu_gpt;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cds::urcu::gc< cds::urcu::signal_buffered<> >  rcu_shb;
        typedef cds::urcu::gc< cds::urcu::signal_threaded<> >  rcu_sht;
#endif


        // MSPriorityQueue
        struct traits_MSPriorityQueue_static_less : public
            cc::mspriority_queue::make_traits <
                co::buffer < co::v::initialized_static_buffer< char, c_nBoundedCapacity > >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_static_less > MSPriorityQueue_static_less;

        struct traits_MSPriorityQueue_static_less_stat : public cc::mspriority_queue::traits
        {
            typedef co::v::initialized_static_buffer< char, c_nBoundedCapacity > buffer;
            typedef cc::mspriority_queue::stat<> stat;
        };
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_static_less_stat > MSPriorityQueue_static_less_stat;

        struct traits_MSPriorityQueue_static_cmp : public
            cc::mspriority_queue::make_traits <
                co::buffer< co::v::initialized_static_buffer< char, c_nBoundedCapacity > >
                , co::compare < cmp >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_static_cmp > MSPriorityQueue_static_cmp;

        struct traits_MSPriorityQueue_static_mutex : public
            cc::mspriority_queue::make_traits<
                co::buffer< co::v::initialized_static_buffer< char, c_nBoundedCapacity > >
                , co::lock_type<std::mutex>
            >::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_static_mutex > MSPriorityQueue_static_mutex;

        struct traits_MSPriorityQueue_dyn_less : public
            cc::mspriority_queue::make_traits<
                co::buffer< co::v::initialized_dynamic_buffer< char > >
            >::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_dyn_less > MSPriorityQueue_dyn_less;

        struct traits_MSPriorityQueue_dyn_less_stat : public
            cc::mspriority_queue::make_traits <
                co::buffer< co::v::initialized_dynamic_buffer< char > >
                , co::stat < cc::mspriority_queue::stat<> >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_dyn_less_stat > MSPriorityQueue_dyn_less_stat;

        struct traits_MSPriorityQueue_dyn_cmp : public
            cc::mspriority_queue::make_traits <
                co::buffer< co::v::initialized_dynamic_buffer< char > >
                , co::compare < cmp >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_dyn_cmp > MSPriorityQueue_dyn_cmp;

        struct traits_MSPriorityQueue_dyn_mutex : public
            cc::mspriority_queue::make_traits <
                co::buffer< co::v::initialized_dynamic_buffer< char > >
                , co::lock_type < std::mutex >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_dyn_mutex > MSPriorityQueue_dyn_mutex;


        // Priority queue based on EllenBinTreeSet
        struct traits_EllenBinTree_max :
            public cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< typename Value::key_extractor >
                ,cc::opt::less< std::less<Value> >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        {};
        typedef details::EllenBinTreePQueue< cds::gc::HP, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_HP_max;
        typedef details::EllenBinTreePQueue< cds::gc::DHP, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_DHP_max;
        typedef details::EllenBinTreePQueue< rcu_gpi, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_gpi_max;
        typedef details::EllenBinTreePQueue< rcu_gpb, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_gpb_max;
        typedef details::EllenBinTreePQueue< rcu_gpt, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_gpt_max;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef details::EllenBinTreePQueue< rcu_shb, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_shb_max;
        typedef details::EllenBinTreePQueue< rcu_sht, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_sht_max;
#endif

        struct traits_EllenBinTree_max_stat :
            public cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< typename Value::key_extractor >
                ,cc::opt::less< std::less<Value> >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        {};
        typedef details::EllenBinTreePQueue< cds::gc::HP, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_HP_max_stat;
        typedef details::EllenBinTreePQueue< cds::gc::DHP, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_DHP_max_stat;
        typedef details::EllenBinTreePQueue< rcu_gpi, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_gpi_max_stat;
        typedef details::EllenBinTreePQueue< rcu_gpb, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_gpb_max_stat;
        typedef details::EllenBinTreePQueue< rcu_gpt, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_gpt_max_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef details::EllenBinTreePQueue< rcu_shb, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_shb_max_stat;
        typedef details::EllenBinTreePQueue< rcu_sht, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_sht_max_stat;
#endif

        struct traits_EllenBinTree_min :
            public cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< typename Value::key_extractor >
                ,cc::opt::less< std::greater<Value> >
            >::type
        {};
        typedef details::EllenBinTreePQueue< cds::gc::HP, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_HP_min;
        typedef details::EllenBinTreePQueue< cds::gc::DHP, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_DHP_min;
        typedef details::EllenBinTreePQueue< rcu_gpi, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_gpi_min;
        typedef details::EllenBinTreePQueue< rcu_gpb, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_gpb_min;
        typedef details::EllenBinTreePQueue< rcu_gpt, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_gpt_min;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef details::EllenBinTreePQueue< rcu_shb, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_shb_min;
        typedef details::EllenBinTreePQueue< rcu_sht, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_sht_min;
#endif

        struct traits_EllenBinTree_min_stat :
            public cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< typename Value::key_extractor >
                ,cc::opt::less< std::greater<Value> >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        {};
        typedef details::EllenBinTreePQueue< cds::gc::HP, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_HP_min_stat;
        typedef details::EllenBinTreePQueue< cds::gc::DHP, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_DHP_min_stat;
        typedef details::EllenBinTreePQueue< rcu_gpi, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_gpi_min_stat;
        typedef details::EllenBinTreePQueue< rcu_gpb, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_gpb_min_stat;
        typedef details::EllenBinTreePQueue< rcu_gpt, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_gpt_min_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef details::EllenBinTreePQueue< rcu_shb, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_shb_min_stat;
        typedef details::EllenBinTreePQueue< rcu_sht, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_sht_min_stat;
#endif

        // Priority queue based on SkipListSet
        struct traits_SkipList_max :
            public cc::skip_list::make_traits <
            cc::opt::less < std::less<Value> >
            > ::type
        {};
        typedef details::SkipListPQueue< cds::gc::HP, Value, traits_SkipList_max > SkipList_HP_max;
        typedef details::SkipListPQueue< cds::gc::DHP, Value, traits_SkipList_max > SkipList_DHP_max;
        typedef details::SkipListPQueue< rcu_gpi, Value, traits_SkipList_max > SkipList_RCU_gpi_max;
        typedef details::SkipListPQueue< rcu_gpb, Value, traits_SkipList_max > SkipList_RCU_gpb_max;
        typedef details::SkipListPQueue< rcu_gpt, Value, traits_SkipList_max > SkipList_RCU_gpt_max;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef details::SkipListPQueue< rcu_shb, Value, traits_SkipList_max > SkipList_RCU_shb_max;
        typedef details::SkipListPQueue< rcu_sht, Value, traits_SkipList_max > SkipList_RCU_sht_max;
#endif

        struct traits_SkipList_max_stat :
            public cc::skip_list::make_traits<
                cc::opt::less< std::less<Value> >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        {};
        typedef details::SkipListPQueue< cds::gc::HP, Value, traits_SkipList_max_stat > SkipList_HP_max_stat;
        typedef details::SkipListPQueue< cds::gc::DHP, Value, traits_SkipList_max_stat > SkipList_DHP_max_stat;
        typedef details::SkipListPQueue< rcu_gpi, Value, traits_SkipList_max_stat > SkipList_RCU_gpi_max_stat;
        typedef details::SkipListPQueue< rcu_gpb, Value, traits_SkipList_max_stat > SkipList_RCU_gpb_max_stat;
        typedef details::SkipListPQueue< rcu_gpt, Value, traits_SkipList_max_stat > SkipList_RCU_gpt_max_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef details::SkipListPQueue< rcu_shb, Value, traits_SkipList_max_stat > SkipList_RCU_shb_max_stat;
        typedef details::SkipListPQueue< rcu_sht, Value, traits_SkipList_max_stat > SkipList_RCU_sht_max_stat;
#endif

        struct traits_SkipList_min :
            public cc::skip_list::make_traits<
                cc::opt::less< std::greater<Value> >
            >::type
        {};
        typedef details::SkipListPQueue< cds::gc::HP, Value, traits_SkipList_min, false > SkipList_HP_min;
        typedef details::SkipListPQueue< cds::gc::DHP, Value, traits_SkipList_min, false > SkipList_DHP_min;
        typedef details::SkipListPQueue< rcu_gpi, Value, traits_SkipList_min, false > SkipList_RCU_gpi_min;
        typedef details::SkipListPQueue< rcu_gpb, Value, traits_SkipList_min, false > SkipList_RCU_gpb_min;
        typedef details::SkipListPQueue< rcu_gpt, Value, traits_SkipList_min, false > SkipList_RCU_gpt_min;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef details::SkipListPQueue< rcu_shb, Value, traits_SkipList_min, false > SkipList_RCU_shb_min;
        typedef details::SkipListPQueue< rcu_sht, Value, traits_SkipList_min, false > SkipList_RCU_sht_min;
#endif

        struct traits_SkipList_min_stat :
            public cc::skip_list::make_traits<
                cc::opt::less< std::greater<Value> >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        {};
        typedef details::SkipListPQueue< cds::gc::HP, Value, traits_SkipList_min_stat, false > SkipList_HP_min_stat;
        typedef details::SkipListPQueue< cds::gc::DHP, Value, traits_SkipList_min_stat, false > SkipList_DHP_min_stat;
        typedef details::SkipListPQueue< rcu_gpi, Value, traits_SkipList_min_stat, false > SkipList_RCU_gpi_min_stat;
        typedef details::SkipListPQueue< rcu_gpb, Value, traits_SkipList_min_stat, false > SkipList_RCU_gpb_min_stat;
        typedef details::SkipListPQueue< rcu_gpt, Value, traits_SkipList_min_stat, false > SkipList_RCU_gpt_min_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef details::SkipListPQueue< rcu_shb, Value, traits_SkipList_min_stat, false > SkipList_RCU_shb_min_stat;
        typedef details::SkipListPQueue< rcu_sht, Value, traits_SkipList_min_stat, false > SkipList_RCU_sht_min_stat;
#endif


        // FCPriorityQueue
        struct traits_FCPQueue_stat : public
            cds::container::fcpqueue::make_traits <
            cds::opt::stat < cds::container::fcpqueue::stat<> >
            > ::type
        {};

        typedef cds::container::FCPriorityQueue< Value >    FCPQueue_vector;
        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value>
            ,traits_FCPQueue_stat
        >    FCPQueue_vector_stat;

        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, std::deque<Value> >
        > FCPQueue_deque;
        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, std::deque<Value> >
            ,traits_FCPQueue_stat
        > FCPQueue_deque_stat;

        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, boost::container::deque<Value> >
        > FCPQueue_boost_deque;
        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, boost::container::deque<Value> >
            ,traits_FCPQueue_stat
        > FCPQueue_boost_deque_stat;

        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, boost::container::stable_vector<Value> >
        > FCPQueue_boost_stable_vector;
        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, boost::container::stable_vector<Value> >
            ,traits_FCPQueue_stat
        > FCPQueue_boost_stable_vector_stat;

        /// Standard priority_queue
        typedef details::StdPQueue< Value, std::vector<Value>, cds::sync::spin> StdPQueue_vector_spin;
        typedef details::StdPQueue< Value, std::vector<Value>, std::mutex >  StdPQueue_vector_mutex;
        typedef details::StdPQueue< Value, std::deque<Value>, cds::sync::spin> StdPQueue_deque_spin;
        typedef details::StdPQueue< Value, std::deque<Value>,  std::mutex >  StdPQueue_deque_mutex;
    };

}   // namespace pqueue


// *********************************************
// Priority queue statistics
namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::opt::none )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::fcpqueue::empty_stat const& )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::fcpqueue::stat<> const& s )
    {
        return o 
            << CDSSTRESS_STAT_OUT( s, m_nPush )
            << CDSSTRESS_STAT_OUT( s, m_nPushMove )
            << CDSSTRESS_STAT_OUT( s, m_nPop )
            << CDSSTRESS_STAT_OUT( s, m_nFailedPop )
            << static_cast<cds::algo::flat_combining::stat<> const&>(s);
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::mspriority_queue::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::mspriority_queue::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nPushCount )
            << CDSSTRESS_STAT_OUT( s, m_nPopCount )
            << CDSSTRESS_STAT_OUT( s, m_nPushFailCount )
            << CDSSTRESS_STAT_OUT( s, m_nPopFailCount )
            << CDSSTRESS_STAT_OUT( s, m_nPushHeapifySwapCount )
            << CDSSTRESS_STAT_OUT( s, m_nPopHeapifySwapCount );
    }

} // namespace cds_test

#endif // #ifndef CDSSTRESS_PQUEUE_TYPES_H

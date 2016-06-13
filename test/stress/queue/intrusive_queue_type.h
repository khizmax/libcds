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

#ifndef CDSSTRESS_INTRUSIVE_QUEUE_TYPES_H
#define CDSSTRESS_INTRUSIVE_QUEUE_TYPES_H

#include <cds/intrusive/msqueue.h>
#include <cds/intrusive/moir_queue.h>
#include <cds/intrusive/optimistic_queue.h>
#include <cds/intrusive/tsigas_cycle_queue.h>
#include <cds/intrusive/vyukov_mpmc_cycle_queue.h>
#include <cds/intrusive/basket_queue.h>
#include <cds/intrusive/fcqueue.h>
#include <cds/intrusive/segmented_queue.h>

#include <cds/gc/hp.h>
#include <cds/gc/dhp.h>

#include <boost/intrusive/slist.hpp>

#include <cds_test/stress_test.h>
#include <cds_test/stat_flat_combining_out.h>
#include "print_stat.h"

namespace queue {

    namespace details {
        struct empty_stat {};

        template <typename T, typename Lock=std::mutex>
        class BoostSList
        {
            typedef boost::intrusive::slist< T, boost::intrusive::cache_last<true> >    slist_type;
            typedef Lock lock_type;
            typedef std::lock_guard<lock_type> lock_guard;

            slist_type  m_List;
            mutable lock_type m_Lock;
        public:
            typedef T value_type;

        public:
            bool push( value_type& v )
            {
                lock_guard l( m_Lock );
                m_List.push_back( v );
                return true;
            }

            bool enqueue( value_type& v )
            {
                return push( v );
            }

            value_type * pop()
            {
                lock_guard l( m_Lock );
                if ( m_List.empty() )
                    return nullptr;
                value_type& v = m_List.front();
                m_List.pop_front();
                return &v;
            }
            value_type * deque()
            {
                return pop();
            }

            bool empty() const
            {
                lock_guard l( m_Lock );
                return m_List.empty();
            }

            size_t size() const
            {
                lock_guard l( m_Lock );
                return m_List.size();
            }

            empty_stat statistics() const
            {
                return empty_stat();
            }
        };
    } // namespace details

    template <typename T>
    struct Types 
    {
        // MSQueue, MoirQueue
        struct traits_MSQueue_HP : public cds::intrusive::msqueue::traits
        {
            typedef cds::intrusive::msqueue::base_hook< cds::opt::gc< cds::gc::HP > > hook;
        };
        typedef cds::intrusive::MSQueue< cds::gc::HP, T, traits_MSQueue_HP > MSQueue_HP;
        typedef cds::intrusive::MoirQueue< cds::gc::HP, T, traits_MSQueue_HP > MoirQueue_HP;

        struct traits_MSQueue_HP_seqcst : public cds::intrusive::msqueue::traits
        {
            typedef cds::intrusive::msqueue::base_hook< cds::opt::gc< cds::gc::HP > > hook;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::intrusive::MSQueue< cds::gc::HP, T, traits_MSQueue_HP_seqcst > MSQueue_HP_seqcst;
        typedef cds::intrusive::MoirQueue< cds::gc::HP, T, traits_MSQueue_HP_seqcst > MoirQueue_HP_seqcst;

        struct traits_MSQueue_DHP : public cds::intrusive::msqueue::traits
        {
            typedef cds::intrusive::msqueue::base_hook< cds::opt::gc< cds::gc::DHP > > hook;
        };
        typedef cds::intrusive::MSQueue< cds::gc::DHP, T, traits_MSQueue_DHP > MSQueue_DHP;
        typedef cds::intrusive::MoirQueue< cds::gc::DHP, T, traits_MSQueue_DHP > MoirQueue_DHP;

        struct traits_MSQueue_DHP_seqcst : public cds::intrusive::msqueue::traits
        {
            typedef cds::intrusive::msqueue::base_hook< cds::opt::gc< cds::gc::DHP > > hook;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::intrusive::MSQueue< cds::gc::DHP, T, traits_MSQueue_DHP_seqcst > MSQueue_DHP_seqcst;
        typedef cds::intrusive::MoirQueue< cds::gc::DHP, T, traits_MSQueue_DHP_seqcst > MoirQueue_DHP_seqcst;

        // MSQueue + item counter
        struct traits_MSQueue_HP_ic : public cds::intrusive::msqueue::traits
        {
            typedef cds::intrusive::msqueue::base_hook< cds::opt::gc< cds::gc::HP > > hook;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::intrusive::MSQueue< cds::gc::HP, T, traits_MSQueue_HP_ic > MSQueue_HP_ic;
        typedef cds::intrusive::MoirQueue< cds::gc::HP, T, traits_MSQueue_HP_ic > MoirQueue_HP_ic;

        struct traits_MSQueue_DHP_ic : public cds::intrusive::msqueue::traits
        {
            typedef cds::intrusive::msqueue::base_hook< cds::opt::gc< cds::gc::DHP > > hook;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::intrusive::MSQueue< cds::gc::DHP, T, traits_MSQueue_DHP_ic > MSQueue_DHP_ic;
        typedef cds::intrusive::MoirQueue< cds::gc::DHP, T, traits_MSQueue_DHP_ic > MoirQueue_DHP_ic;

        // MSQueue + stat
        struct traits_MSQueue_HP_stat : public cds::intrusive::msqueue::traits
        {
            typedef cds::intrusive::msqueue::base_hook< cds::opt::gc< cds::gc::HP > > hook;
            typedef cds::intrusive::msqueue::stat<> stat;
        };
        typedef cds::intrusive::MSQueue< cds::gc::HP, T, traits_MSQueue_HP_stat > MSQueue_HP_stat;
        typedef cds::intrusive::MoirQueue< cds::gc::HP, T, traits_MSQueue_HP_stat > MoirQueue_HP_stat;

        struct traits_MSQueue_DHP_stat : public cds::intrusive::msqueue::traits
        {
            typedef cds::intrusive::msqueue::base_hook< cds::opt::gc< cds::gc::DHP > > hook;
            typedef cds::intrusive::msqueue::stat<> stat;
        };
        typedef cds::intrusive::MSQueue< cds::gc::DHP, T, traits_MSQueue_DHP_stat > MSQueue_DHP_stat;
        typedef cds::intrusive::MoirQueue< cds::gc::DHP, T, traits_MSQueue_DHP_stat > MoirQueue_DHP_stat;


        // OptimisticQueue
        struct traits_OptimisticQueue_HP : public cds::intrusive::optimistic_queue::traits
        {
            typedef cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::HP > > hook;
        };
        typedef cds::intrusive::OptimisticQueue< cds::gc::HP, T, traits_OptimisticQueue_HP > OptimisticQueue_HP;

        struct traits_OptimisticQueue_HP_seqcst : public
            cds::intrusive::optimistic_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
                , cds::opt::memory_model< cds::opt::v::sequential_consistent >
            >::type
        {};
        typedef cds::intrusive::OptimisticQueue< cds::gc::HP, T, traits_OptimisticQueue_HP_seqcst > OptimisticQueue_HP_seqcst;

        struct traits_OptimisticQueue_DHP : public cds::intrusive::optimistic_queue::traits
        {
            typedef cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::DHP > > hook;
        };
        typedef cds::intrusive::OptimisticQueue< cds::gc::DHP, T, traits_OptimisticQueue_DHP > OptimisticQueue_DHP;

        struct traits_OptimisticQueue_DHP_seqcst: public
            cds::intrusive::optimistic_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::DHP > > >
                , cds::opt::memory_model< cds::opt::v::sequential_consistent >
            >::type
        {};
        typedef cds::intrusive::OptimisticQueue< cds::gc::DHP, T, traits_OptimisticQueue_DHP_seqcst > OptimisticQueue_DHP_seqcst;

        // OptimisticQueue + item counter
        struct traits_OptimisticQueue_HP_ic: public
            cds::intrusive::optimistic_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
                , cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cds::intrusive::OptimisticQueue< cds::gc::HP, T, traits_OptimisticQueue_HP_ic > OptimisticQueue_HP_ic;

        struct traits_OptimisticQueue_DHP_ic: public
            cds::intrusive::optimistic_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::DHP > > >
                , cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cds::intrusive::OptimisticQueue< cds::gc::DHP, T, traits_OptimisticQueue_DHP_ic > OptimisticQueue_DHP_ic;

        // OptimisticQueue + stat
        struct traits_OptimisticQueue_HP_stat: public
            cds::intrusive::optimistic_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
                , cds::opt::stat< cds::intrusive::optimistic_queue::stat<> >
            >::type
        {};
        typedef cds::intrusive::OptimisticQueue< cds::gc::HP, T, traits_OptimisticQueue_HP_stat > OptimisticQueue_HP_stat;

        struct traits_OptimisticQueue_DHP_stat: public
            cds::intrusive::optimistic_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::DHP > > >
                , cds::opt::stat< cds::intrusive::optimistic_queue::stat<> >
            >::type
        {};
        typedef cds::intrusive::OptimisticQueue< cds::gc::DHP, T, traits_OptimisticQueue_DHP_stat > OptimisticQueue_DHP_stat;

        // TsigasCycleQueue
        class TsigasCycleQueue_dyn
            : public cds::intrusive::TsigasCycleQueue< T,
                typename cds::intrusive::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< int > >
                >::type
            >
        {
            typedef cds::intrusive::TsigasCycleQueue< T,
                typename cds::intrusive::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< int > >
                >::type
            > base_class;
        public:
            TsigasCycleQueue_dyn()
                : base_class( 1024 * 64 )
            {}

            TsigasCycleQueue_dyn( size_t nCapacity )
                : base_class( nCapacity )
            {}

            cds::opt::none statistics() const
            {
                return cds::opt::none();
            }
        };

        class TsigasCycleQueue_dyn_ic
            : public cds::intrusive::TsigasCycleQueue< T,
                typename cds::intrusive::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< int > >
                    ,cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            >
        {
            typedef cds::intrusive::TsigasCycleQueue< T,
                typename cds::intrusive::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< int > >
                    ,cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > base_class;
        public:
            TsigasCycleQueue_dyn_ic()
                : base_class( 1024 * 64 )
            {}
            TsigasCycleQueue_dyn_ic( size_t nCapacity )
                : base_class( nCapacity )
            {}

            cds::opt::none statistics() const
            {
                return cds::opt::none();
            }
        };

        // VyukovMPMCCycleQueue
        struct traits_VyukovMPMCCycleQueue_dyn : public cds::intrusive::vyukov_queue::traits
        {
            typedef cds::opt::v::uninitialized_dynamic_buffer< int > buffer;
        };
        class VyukovMPMCCycleQueue_dyn
            : public cds::intrusive::VyukovMPMCCycleQueue< T, traits_VyukovMPMCCycleQueue_dyn >
        {
            typedef cds::intrusive::VyukovMPMCCycleQueue< T, traits_VyukovMPMCCycleQueue_dyn > base_class;
        public:
            VyukovMPMCCycleQueue_dyn()
                : base_class( 1024 * 64 )
            {}
            VyukovMPMCCycleQueue_dyn( size_t nCapacity )
                : base_class( nCapacity )
            {}

            cds::opt::none statistics() const
            {
                return cds::opt::none();
            }
        };

        struct traits_VyukovMPMCCycleQueue_dyn_ic : public traits_VyukovMPMCCycleQueue_dyn
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        class VyukovMPMCCycleQueue_dyn_ic
            : public cds::intrusive::VyukovMPMCCycleQueue< T, traits_VyukovMPMCCycleQueue_dyn_ic >
        {
            typedef cds::intrusive::VyukovMPMCCycleQueue< T, traits_VyukovMPMCCycleQueue_dyn_ic > base_class;
        public:
            VyukovMPMCCycleQueue_dyn_ic()
                : base_class( 1024 * 64 )
            {}
            VyukovMPMCCycleQueue_dyn_ic( size_t nCapacity )
                : base_class( nCapacity )
            {}

            cds::opt::none statistics() const
            {
                return cds::opt::none();
            }
        };

        // BasketQueue
        struct traits_BasketQueue_HP : public
            cds::intrusive::basket_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
            > ::type
        {};
        typedef cds::intrusive::BasketQueue< cds::gc::HP, T, traits_BasketQueue_HP > BasketQueue_HP;

        struct traits_BasketQueue_HP_seqcst: public
            cds::intrusive::basket_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
                , cds::opt::memory_model< cds::opt::v::sequential_consistent >
            > ::type
        {};
        typedef cds::intrusive::BasketQueue<cds::gc::HP, T, traits_BasketQueue_HP_seqcst > BasketQueue_HP_seqcst;

        struct traits_BasketQueue_DHP : public
            cds::intrusive::basket_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::DHP > > >
            > ::type
        {};
        typedef cds::intrusive::BasketQueue< cds::gc::DHP, T, traits_BasketQueue_DHP > BasketQueue_DHP;

        struct traits_BasketQueue_DHP_seqcst: public
            cds::intrusive::basket_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::DHP > > >
                , cds::opt::memory_model< cds::opt::v::sequential_consistent >
            > ::type
        {};
        typedef cds::intrusive::BasketQueue< cds::gc::DHP, T, traits_BasketQueue_DHP_seqcst > BasketQueue_DHP_seqcst;

        // BasketQueue + item counter
        struct traits_BasketQueue_HP_ic : public
            cds::intrusive::basket_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            > ::type
        {};
        typedef cds::intrusive::BasketQueue< cds::gc::HP, T, traits_BasketQueue_HP_ic > BasketQueue_HP_ic;

        struct traits_BasketQueue_DHP_ic : public
            cds::intrusive::basket_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::DHP > > >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            > ::type
        {};
        typedef cds::intrusive::BasketQueue< cds::gc::DHP, T, traits_BasketQueue_DHP_ic > BasketQueue_DHP_ic;

        // BasketQueue + stat
        struct traits_BasketQueue_HP_stat : public
            cds::intrusive::basket_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
                , cds::opt::stat< cds::intrusive::basket_queue::stat<> >
            > ::type
        {};
        typedef cds::intrusive::BasketQueue< cds::gc::HP, T, traits_BasketQueue_HP_stat > BasketQueue_HP_stat;

        struct traits_BasketQueue_DHP_stat : public
            cds::intrusive::basket_queue::make_traits <
                cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::DHP > > >
                , cds::opt::stat< cds::intrusive::basket_queue::stat<> >
            > ::type
        {};
        typedef cds::intrusive::BasketQueue< cds::gc::DHP, T, traits_BasketQueue_DHP_stat > BasketQueue_DHP_stat;

        // FCQueue
        class traits_FCQueue_delay2:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::backoff< cds::backoff::delay_of<2>>>
            >::type
        {};
        class traits_FCQueue_delay2_elimination:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::backoff< cds::backoff::delay_of<2>>>
                ,cds::opt::enable_elimination< true >
            >::type
        {};
        class traits_FCQueue_delay2_elimination_stat:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::backoff< cds::backoff::delay_of<2>>>
                ,cds::opt::stat< cds::intrusive::fcqueue::stat<> >
                ,cds::opt::enable_elimination< true >
            >::type
        {};
        class traits_FCQueue_expbackoff_elimination:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::elimination_backoff< cds::backoff::Default >
            >::type
        {};
        class traits_FCQueue_expbackoff_elimination_stat:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::stat< cds::intrusive::fcqueue::stat<> >
                ,cds::opt::elimination_backoff< cds::backoff::Default >
            >::type
        {};

        class traits_FCQueue_wait_ss:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<>>
            >::type
        {};
        struct traits_FCQueue_wait_ss_stat: traits_FCQueue_wait_ss
        {
            typedef cds::intrusive::fcqueue::stat<> stat;
        };
        class traits_FCQueue_wait_sm:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<>>
            >::type
        {};
        struct traits_FCQueue_wait_sm_stat: traits_FCQueue_wait_sm
        {
            typedef cds::intrusive::fcqueue::stat<> stat;
        };
        class traits_FCQueue_wait_mm:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            >::type
        {};
        struct traits_FCQueue_wait_mm_stat: traits_FCQueue_wait_mm
        {
            typedef cds::intrusive::fcqueue::stat<> stat;
        };

        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_delay2 > FCQueue_list_delay2;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_delay2_elimination > FCQueue_list_delay2_elimination;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_delay2_elimination_stat > FCQueue_list_delay2_elimination_stat;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_expbackoff_elimination > FCQueue_list_expbackoff_elimination;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_expbackoff_elimination_stat > FCQueue_list_expbackoff_elimination_stat;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_wait_ss > FCQueue_list_wait_ss;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_wait_ss_stat > FCQueue_list_wait_ss_stat;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_wait_sm > FCQueue_list_wait_sm;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_wait_sm_stat > FCQueue_list_wait_sm_stat;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_wait_mm > FCQueue_list_wait_mm;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_wait_mm_stat > FCQueue_list_wait_mm_stat;

        // SegmentedQueue
        class traits_SegmentedQueue_spin_stat:
            public cds::intrusive::segmented_queue::make_traits<
                cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
            >::type
        {};
        class traits_SegmentedQueue_spin_padding :
            public cds::intrusive::segmented_queue::make_traits<
            cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        {};
        class traits_SegmentedQueue_mutex_stat :
            public cds::intrusive::segmented_queue::make_traits<
                cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
                ,cds::opt::lock_type< std::mutex >
            >::type
        {};
        class traits_SegmentedQueue_mutex:
            public cds::intrusive::segmented_queue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        {};
        class traits_SegmentedQueue_mutex_padding:
            public cds::intrusive::segmented_queue::make_traits<
                cds::opt::lock_type< std::mutex >
                ,cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        {};

        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T >  SegmentedQueue_HP_spin;
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T, traits_SegmentedQueue_spin_padding >  SegmentedQueue_HP_spin_padding;
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T, traits_SegmentedQueue_spin_stat >  SegmentedQueue_HP_spin_stat;
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T, traits_SegmentedQueue_mutex >  SegmentedQueue_HP_mutex;
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T, traits_SegmentedQueue_mutex_padding >  SegmentedQueue_HP_mutex_padding;
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T, traits_SegmentedQueue_mutex_stat >  SegmentedQueue_HP_mutex_stat;

        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, T >  SegmentedQueue_DHP_spin;
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, T, traits_SegmentedQueue_spin_padding >  SegmentedQueue_DHP_spin_padding;
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, T, traits_SegmentedQueue_spin_stat >  SegmentedQueue_DHP_spin_stat;
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, T, traits_SegmentedQueue_mutex >  SegmentedQueue_DHP_mutex;
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, T, traits_SegmentedQueue_mutex_padding >  SegmentedQueue_DHP_mutex_padding;
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, T, traits_SegmentedQueue_mutex_stat >  SegmentedQueue_DHP_mutex_stat;

        // Boost SList
        typedef details::BoostSList< T, std::mutex >      BoostSList_mutex;
        typedef details::BoostSList< T, cds::sync::spin > BoostSList_spin;
    };
} // namespace queue

namespace cds_test {

    // cds::container::fcqueue::stat
    template <typename Counter>
    static inline property_stream& operator <<( property_stream& o, cds::intrusive::fcqueue::stat<Counter> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nEnqueue )
            << CDSSTRESS_STAT_OUT( s, m_nDequeue )
            << CDSSTRESS_STAT_OUT( s, m_nFailedDeq )
            << CDSSTRESS_STAT_OUT( s, m_nCollided )
            << static_cast<cds::algo::flat_combining::stat<> const&>(s);
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::fcqueue::empty_stat const& /*s*/ )
    {
        return o;
    }

} // namespace cds_test

#endif // #ifndef CDSSTRESS_INTRUSIVE_QUEUE_TYPES_H

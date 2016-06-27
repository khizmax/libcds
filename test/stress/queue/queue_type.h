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

#ifndef CDSSTRESS_QUEUE_TYPES_H
#define CDSSTRESS_QUEUE_TYPES_H

#include <cds/container/msqueue.h>
#include <cds/container/moir_queue.h>
#include <cds/container/rwqueue.h>
#include <cds/container/optimistic_queue.h>
#include <cds/container/tsigas_cycle_queue.h>
#include <cds/container/vyukov_mpmc_cycle_queue.h>
#include <cds/container/basket_queue.h>
#include <cds/container/fcqueue.h>
#include <cds/container/fcdeque.h>
#include <cds/container/segmented_queue.h>

#include <cds/gc/hp.h>
#include <cds/gc/dhp.h>

#include "std_queue.h"
#include "lock/win32_lock.h"
#include "framework/michael_alloc.h"

#include <boost/container/deque.hpp>

#include <cds_test/stress_test.h>
#include <cds_test/stat_flat_combining_out.h>
#include "print_stat.h"

namespace queue {

    namespace details {
        template <typename T, typename Traits=cds::container::fcdeque::traits, class Deque=std::deque<T> >
        class FCDequeL: public cds::container::FCDeque<T, Deque, Traits >
        {
            typedef cds::container::FCDeque<T, Deque, Traits > base_class;
        public:
            FCDequeL()
            {}

            FCDequeL(
                unsigned int nCompactFactor     ///< Flat combining: publication list compacting factor
                ,unsigned int nCombinePassCount ///< Flat combining: number of combining passes for combiner thread
                )
                : base_class( nCompactFactor, nCombinePassCount )
            {}

            bool push( T const& v )
            {
                return base_class::push_front( v );
            }
            bool enqueue( T const& v )
            {
                return push( v );
            }

            bool pop( T& v )
            {
                return base_class::pop_back( v );
            }
            bool deque( T& v )
            {
                return pop(v);
            }
        };

        template <typename T, typename Traits=cds::container::fcdeque::traits, class Deque = std::deque<T> >
        class FCDequeR: public cds::container::FCDeque<T, Deque, Traits >
        {
            typedef cds::container::FCDeque<T, Deque, Traits > base_class;
        public:
            FCDequeR()
            {}

            FCDequeR(
                unsigned int nCompactFactor     ///< Flat combining: publication list compacting factor
                ,unsigned int nCombinePassCount ///< Flat combining: number of combining passes for combiner thread
                )
                : base_class( nCompactFactor, nCombinePassCount )
            {}

            bool push( T const& v )
            {
                return base_class::push_back( v );
            }
            bool enqueue( T const& v )
            {
                return push( v );
            }

            bool pop( T& v )
            {
                return base_class::pop_front( v );
            }
            bool deque( T& v )
            {
                return pop(v);
            }
        };

    } // namespace details

    template <typename Value>
    struct Types {

        // MSQueue
        typedef cds::container::MSQueue<cds::gc::HP,  Value > MSQueue_HP;
        typedef cds::container::MSQueue<cds::gc::DHP, Value > MSQueue_DHP;
        typedef cds::container::MoirQueue<cds::gc::HP, Value > MoirQueue_HP;
        typedef cds::container::MoirQueue<cds::gc::DHP, Value > MoirQueue_DHP;

        struct traits_MSQueue_michaelAlloc : public cds::container::msqueue::traits
        {
            typedef memory::MichaelAllocator<int>  allocator;
        };
        typedef cds::container::MSQueue<cds::gc::HP,  Value, traits_MSQueue_michaelAlloc > MSQueue_HP_michaelAlloc;
        typedef cds::container::MSQueue<cds::gc::DHP, Value, traits_MSQueue_michaelAlloc > MSQueue_DHP_michaelAlloc;
        typedef cds::container::MoirQueue<cds::gc::HP, Value, traits_MSQueue_michaelAlloc > MoirQueue_HP_michaelAlloc;
        typedef cds::container::MoirQueue<cds::gc::DHP, Value, traits_MSQueue_michaelAlloc > MoirQueue_DHP_michaelAlloc;

        struct traits_MSQueue_seqcst : public
            cds::container::msqueue::make_traits <
                cds::opt::memory_model < cds::opt::v::sequential_consistent >
            > ::type
        {};
        typedef cds::container::MSQueue< cds::gc::HP,  Value, traits_MSQueue_seqcst > MSQueue_HP_seqcst;
        typedef cds::container::MSQueue< cds::gc::DHP, Value, traits_MSQueue_seqcst > MSQueue_DHP_seqcst;
        typedef cds::container::MoirQueue< cds::gc::HP, Value, traits_MSQueue_seqcst > MoirQueue_HP_seqcst;
        typedef cds::container::MoirQueue< cds::gc::DHP, Value, traits_MSQueue_seqcst > MoirQueue_DHP_seqcst;

        // MSQueue + item counter
        struct traits_MSQueue_ic : public
            cds::container::msqueue::make_traits <
                cds::opt::item_counter < cds::atomicity::item_counter >
            >::type
        {};
        typedef cds::container::MSQueue< cds::gc::HP,  Value, traits_MSQueue_ic > MSQueue_HP_ic;
        typedef cds::container::MSQueue< cds::gc::DHP, Value, traits_MSQueue_ic > MSQueue_DHP_ic;
        typedef cds::container::MoirQueue< cds::gc::HP, Value, traits_MSQueue_ic > MoirQueue_HP_ic;
        typedef cds::container::MoirQueue< cds::gc::DHP, Value, traits_MSQueue_ic > MoirQueue_DHP_ic;

        // MSQueue + stat
        struct traits_MSQueue_stat: public
            cds::container::msqueue::make_traits <
                cds::opt::stat< cds::container::msqueue::stat<> >
            >::type
        {};
        typedef cds::container::MSQueue< cds::gc::HP,  Value, traits_MSQueue_stat > MSQueue_HP_stat;
        typedef cds::container::MSQueue< cds::gc::DHP, Value, traits_MSQueue_stat > MSQueue_DHP_stat;
        typedef cds::container::MoirQueue< cds::gc::HP, Value, traits_MSQueue_stat > MoirQueue_HP_stat;
        typedef cds::container::MoirQueue< cds::gc::DHP, Value, traits_MSQueue_stat > MoirQueue_DHP_stat;


        // OptimisticQueue
        typedef cds::container::OptimisticQueue< cds::gc::HP, Value > OptimisticQueue_HP;
        typedef cds::container::OptimisticQueue< cds::gc::DHP, Value > OptimisticQueue_DHP;

        struct traits_OptimisticQueue_michaelAlloc : public cds::container::optimistic_queue::traits
        {
            typedef memory::MichaelAllocator<int> allocator;
        };
        typedef cds::container::OptimisticQueue< cds::gc::HP,  Value, traits_OptimisticQueue_michaelAlloc > OptimisticQueue_HP_michaelAlloc;
        typedef cds::container::OptimisticQueue< cds::gc::DHP, Value, traits_OptimisticQueue_michaelAlloc > OptimisticQueue_DHP_michaelAlloc;

        struct traits_OptimisticQueue_seqcst : public cds::container::optimistic_queue::traits
        {
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::container::OptimisticQueue< cds::gc::HP,  Value, traits_OptimisticQueue_seqcst > OptimisticQueue_HP_seqcst;
        typedef cds::container::OptimisticQueue< cds::gc::DHP, Value, traits_OptimisticQueue_seqcst > OptimisticQueue_DHP_seqcst;

        struct traits_OptimisticQueue_ic : public cds::container::optimistic_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::OptimisticQueue< cds::gc::HP,  Value, traits_OptimisticQueue_ic > OptimisticQueue_HP_ic;
        typedef cds::container::OptimisticQueue< cds::gc::DHP, Value, traits_OptimisticQueue_ic > OptimisticQueue_DHP_ic;

        struct traits_OptimisticQueue_stat : public
            cds::container::optimistic_queue::make_traits <
                cds::opt::stat < cds::intrusive::optimistic_queue::stat<> >
            > ::type
        {};
        typedef cds::container::OptimisticQueue< cds::gc::HP,  Value, traits_OptimisticQueue_stat > OptimisticQueue_HP_stat;
        typedef cds::container::OptimisticQueue< cds::gc::DHP, Value, traits_OptimisticQueue_stat > OptimisticQueue_DHP_stat;


        // TsigasCycleQueue

        class TsigasCycleQueue_dyn
            : public cds::container::TsigasCycleQueue< Value,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< int > >
                >::type
            >
        {
            typedef cds::container::TsigasCycleQueue< Value,
                typename cds::container::tsigas_queue::make_traits<
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

        class TsigasCycleQueue_dyn_michaelAlloc
            : public cds::container::TsigasCycleQueue< Value,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< int > >
                    ,cds::opt::allocator< memory::MichaelAllocator<int> >
                >::type
            >
        {
            typedef cds::container::TsigasCycleQueue< Value,
                typename cds::container::tsigas_queue::make_traits<
                   cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< int > >
                    , cds::opt::allocator< memory::MichaelAllocator<int> >
                >::type
            > base_class;
        public:
            TsigasCycleQueue_dyn_michaelAlloc()
                : base_class( 1024 * 64 )
            {}

            TsigasCycleQueue_dyn_michaelAlloc( size_t nCapacity )
                : base_class( nCapacity )
            {}

            cds::opt::none statistics() const
            {
                return cds::opt::none();
            }
        };

        class TsigasCycleQueue_dyn_ic
            : public cds::container::TsigasCycleQueue< Value,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< int > >
                    ,cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            >
        {
            typedef cds::container::TsigasCycleQueue< Value,
                typename cds::container::tsigas_queue::make_traits<
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
        struct traits_VyukovMPMCCycleQueue_dyn : public cds::container::vyukov_queue::traits
        {
            typedef cds::opt::v::uninitialized_dynamic_buffer< int > buffer;
        };
        class VyukovMPMCCycleQueue_dyn
            : public cds::container::VyukovMPMCCycleQueue< Value, traits_VyukovMPMCCycleQueue_dyn >
        {
            typedef cds::container::VyukovMPMCCycleQueue< Value, traits_VyukovMPMCCycleQueue_dyn > base_class;
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

        struct traits_VyukovMPMCCycleQueue_dyn_michaelAlloc : public cds::container::vyukov_queue::traits
        {
            typedef cds::opt::v::uninitialized_dynamic_buffer< int, memory::MichaelAllocator<int> > buffer;
        };
        class VyukovMPMCCycleQueue_dyn_michaelAlloc
            : public cds::container::VyukovMPMCCycleQueue< Value, traits_VyukovMPMCCycleQueue_dyn_michaelAlloc >
        {
            typedef cds::container::VyukovMPMCCycleQueue< Value, traits_VyukovMPMCCycleQueue_dyn_michaelAlloc > base_class;
        public:
            VyukovMPMCCycleQueue_dyn_michaelAlloc()
                : base_class( 1024 * 64 )
            {}
            VyukovMPMCCycleQueue_dyn_michaelAlloc( size_t nCapacity )
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
            : public cds::container::VyukovMPMCCycleQueue< Value, traits_VyukovMPMCCycleQueue_dyn_ic >
        {
            typedef cds::container::VyukovMPMCCycleQueue< Value, traits_VyukovMPMCCycleQueue_dyn_ic > base_class;
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

        typedef cds::container::BasketQueue< cds::gc::HP , Value > BasketQueue_HP;
        typedef cds::container::BasketQueue< cds::gc::DHP, Value > BasketQueue_DHP;

        struct traits_BasketQueue_michaelAlloc : public cds::container::basket_queue::traits
        {
            typedef memory::MichaelAllocator<int> allocator;
        };
        typedef cds::container::BasketQueue< cds::gc::HP,  Value, traits_BasketQueue_michaelAlloc > BasketQueue_HP_michaelAlloc;
        typedef cds::container::BasketQueue< cds::gc::DHP, Value, traits_BasketQueue_michaelAlloc > BasketQueue_DHP_michaelAlloc;

        struct traits_BasketQueue_seqcst : public cds::container::basket_queue::traits
        {
            typedef cds::opt::v::sequential_consistent mamory_model;
        };
        typedef cds::container::BasketQueue< cds::gc::HP,  Value, traits_BasketQueue_seqcst > BasketQueue_HP_seqcst;
        typedef cds::container::BasketQueue< cds::gc::DHP, Value, traits_BasketQueue_seqcst > BasketQueue_DHP_seqcst;

        struct traits_BasketQueue_ic : public cds::container::basket_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::BasketQueue< cds::gc::HP,  Value, traits_BasketQueue_ic >BasketQueue_HP_ic;
        typedef cds::container::BasketQueue< cds::gc::DHP, Value, traits_BasketQueue_ic >BasketQueue_DHP_ic;

        struct traits_BasketQueue_stat : public cds::container::basket_queue::traits
        {
            typedef cds::container::basket_queue::stat<> stat;
        };
        typedef cds::container::BasketQueue< cds::gc::HP,  Value, traits_BasketQueue_stat > BasketQueue_HP_stat;
        typedef cds::container::BasketQueue< cds::gc::DHP, Value, traits_BasketQueue_stat > BasketQueue_DHP_stat;


        // RWQueue
        typedef cds::container::RWQueue< Value > RWQueue_Spin;

        struct traits_RWQueue_Spin_ic : public cds::container::rwqueue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::RWQueue< Value, traits_RWQueue_Spin_ic > RWQueue_Spin_ic;

        struct traits_RWQueue_mutex : public
            cds::container::rwqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        {};
        typedef cds::container::RWQueue< Value, traits_RWQueue_mutex > RWQueue_mutex;

        // FCQueue
        struct traits_FCQueue_stat:
            public cds::container::fcqueue::make_traits<
                cds::opt::stat< cds::container::fcqueue::stat<> >
            >::type
        {};
        struct traits_FCQueue_single_mutex_single_condvar:
            public cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<>>
            >::type
        {};
        struct traits_FCQueue_single_mutex_single_condvar_stat: traits_FCQueue_single_mutex_single_condvar
        {
            typedef cds::container::fcqueue::stat<> stat;
        };
        struct traits_FCQueue_single_mutex_multi_condvar:
            public cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<>>
            >::type
        {};
        struct traits_FCQueue_single_mutex_multi_condvar_stat: traits_FCQueue_single_mutex_multi_condvar
        {
            typedef cds::container::fcqueue::stat<> stat;
        };
        struct traits_FCQueue_multi_mutex_multi_condvar:
            public cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            >::type
        {};
        struct traits_FCQueue_multi_mutex_multi_condvar_stat: traits_FCQueue_multi_mutex_multi_condvar
        {
            typedef cds::container::fcqueue::stat<> stat;
        };
        struct traits_FCQueue_elimination:
            public cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        {};
        struct traits_FCQueue_elimination_stat:
            public cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::stat< cds::container::fcqueue::stat<> >
            >::type
        {};

        typedef cds::container::FCQueue< Value > FCQueue_deque;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_stat > FCQueue_deque_stat;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_single_mutex_single_condvar> FCQueue_deque_wait_ss;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_single_mutex_single_condvar_stat> FCQueue_deque_wait_ss_stat;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_single_mutex_multi_condvar> FCQueue_deque_wait_sm;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_single_mutex_multi_condvar_stat> FCQueue_deque_wait_sm_stat;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_multi_mutex_multi_condvar> FCQueue_deque_wait_mm;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_multi_mutex_multi_condvar_stat> FCQueue_deque_wait_mm_stat;

        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_elimination > FCQueue_deque_elimination;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_elimination_stat > FCQueue_deque_elimination_stat;

        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value>>> FCQueue_list;
        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value>>, traits_FCQueue_stat> FCQueue_list_stat;
        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value>>, traits_FCQueue_single_mutex_single_condvar> FCQueue_list_wait_ss;
        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value>>, traits_FCQueue_single_mutex_single_condvar_stat> FCQueue_list_wait_ss_stat;
        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value>>, traits_FCQueue_single_mutex_multi_condvar> FCQueue_list_wait_sm;
        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value>>, traits_FCQueue_single_mutex_multi_condvar_stat> FCQueue_list_wait_sm_stat;
        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value>>, traits_FCQueue_multi_mutex_multi_condvar> FCQueue_list_wait_mm;
        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value>>, traits_FCQueue_multi_mutex_multi_condvar_stat> FCQueue_list_wait_mm_stat;

        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value> >, traits_FCQueue_elimination > FCQueue_list_elimination;
        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value> >, traits_FCQueue_elimination_stat > FCQueue_list_elimination_stat;


   // FCDeque
        struct traits_FCDeque_stat:
            public cds::container::fcdeque::make_traits<
                cds::opt::stat< cds::container::fcdeque::stat<> >
            >::type
        {};
        struct traits_FCDeque_elimination:
            public cds::container::fcdeque::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        {};
        struct traits_FCDeque_elimination_stat:
            public cds::container::fcdeque::make_traits<
                cds::opt::stat< cds::container::fcdeque::stat<> >,
                cds::opt::enable_elimination< true >
            >::type
        {};
        struct traits_FCDeque_mutex:
            public cds::container::fcdeque::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        {};

        struct traits_FCDeque_wait_ss: cds::container::fcdeque::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<> wait_strategy;
        };
        struct traits_FCDeque_wait_ss_stat: traits_FCDeque_wait_ss
        {
            typedef cds::container::fcdeque::stat<> stat;
        };
        struct traits_FCDeque_wait_sm: cds::container::fcdeque::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<> wait_strategy;
        };
        struct traits_FCDeque_wait_sm_stat: traits_FCDeque_wait_sm
        {
            typedef cds::container::fcdeque::stat<> stat;
        };
        struct traits_FCDeque_wait_mm: cds::container::fcdeque::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<> wait_strategy;
        };
        struct traits_FCDeque_wait_mm_stat: traits_FCDeque_wait_mm
        {
            typedef cds::container::fcdeque::stat<> stat;
        };

        typedef details::FCDequeL< Value > FCDequeL_default;
        typedef details::FCDequeL< Value, traits_FCDeque_mutex > FCDequeL_mutex;
        typedef details::FCDequeL< Value, traits_FCDeque_stat > FCDequeL_stat;
        typedef details::FCDequeL< Value, traits_FCDeque_wait_ss > FCDequeL_wait_ss;
        typedef details::FCDequeL< Value, traits_FCDeque_wait_ss_stat > FCDequeL_wait_ss_stat;
        typedef details::FCDequeL< Value, traits_FCDeque_wait_sm > FCDequeL_wait_sm;
        typedef details::FCDequeL< Value, traits_FCDeque_wait_sm_stat > FCDequeL_wait_sm_stat;
        typedef details::FCDequeL< Value, traits_FCDeque_wait_mm > FCDequeL_wait_mm;
        typedef details::FCDequeL< Value, traits_FCDeque_wait_mm_stat > FCDequeL_wait_mm_stat;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination > FCDequeL_elimination;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination_stat > FCDequeL_elimination_stat;

        typedef details::FCDequeL< Value, cds::container::fcdeque::traits, boost::container::deque<Value> > FCDequeL_boost;
        typedef details::FCDequeL< Value, traits_FCDeque_stat, boost::container::deque<Value> > FCDequeL_boost_stat;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination, boost::container::deque<Value> > FCDequeL_boost_elimination;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination_stat, boost::container::deque<Value> > FCDequeL_boost_elimination_stat;

        typedef details::FCDequeR< Value > FCDequeR_default;
        typedef details::FCDequeR< Value, traits_FCDeque_mutex > FCDequeR_mutex;
        typedef details::FCDequeR< Value, traits_FCDeque_stat > FCDequeR_stat;
        typedef details::FCDequeR< Value, traits_FCDeque_wait_ss > FCDequeR_wait_ss;
        typedef details::FCDequeR< Value, traits_FCDeque_wait_ss_stat > FCDequeR_wait_ss_stat;
        typedef details::FCDequeR< Value, traits_FCDeque_wait_sm > FCDequeR_wait_sm;
        typedef details::FCDequeR< Value, traits_FCDeque_wait_sm_stat > FCDequeR_wait_sm_stat;
        typedef details::FCDequeR< Value, traits_FCDeque_wait_mm > FCDequeR_wait_mm;
        typedef details::FCDequeR< Value, traits_FCDeque_wait_mm_stat > FCDequeR_wait_mm_stat;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination > FCDequeR_elimination;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination_stat > FCDequeR_elimination_stat;

        typedef details::FCDequeR< Value, cds::container::fcdeque::traits, boost::container::deque<Value> > FCDequeR_boost;
        typedef details::FCDequeR< Value, traits_FCDeque_stat, boost::container::deque<Value> > FCDequeR_boost_stat;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination, boost::container::deque<Value> > FCDequeR_boost_elimination;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination_stat, boost::container::deque<Value> > FCDequeR_boost_elimination_stat;

        // STL
        typedef StdQueue_deque<Value>               StdQueue_deque_Spinlock;
        typedef StdQueue_list<Value>                StdQueue_list_Spinlock;
        typedef StdQueue_deque<Value, std::mutex>   StdQueue_deque_Mutex;
        typedef StdQueue_list<Value, std::mutex>    StdQueue_list_Mutex;
#ifdef UNIT_LOCK_WIN_CS
        typedef StdQueue_deque<Value, lock::win::CS>    StdQueue_deque_WinCS;
        typedef StdQueue_list<Value, lock::win::CS>     StdQueue_list_WinCS;
        typedef StdQueue_deque<Value, lock::win::Mutex> StdQueue_deque_WinMutex;
        typedef StdQueue_list<Value, lock::win::Mutex>  StdQueue_list_WinMutex;
#endif

        // SegmentedQueue
        class traits_SegmentedQueue_spin_stat:
            public cds::container::segmented_queue::make_traits<
                cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
            >::type
        {};
        class traits_SegmentedQueue_spin_padding:
            public cds::container::segmented_queue::make_traits<
                cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        {};
        class traits_SegmentedQueue_mutex_stat:
            public cds::container::segmented_queue::make_traits<
                cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
                ,cds::opt::lock_type< std::mutex >
            >::type
        {};
        class traits_SegmentedQueue_mutex:
            public cds::container::segmented_queue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        {};
        class traits_SegmentedQueue_mutex_padding:
            public cds::container::segmented_queue::make_traits<
                cds::opt::lock_type< std::mutex >
                , cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        {};

        typedef cds::container::SegmentedQueue< cds::gc::HP, Value >  SegmentedQueue_HP_spin;
        typedef cds::container::SegmentedQueue< cds::gc::HP, Value, traits_SegmentedQueue_spin_padding >  SegmentedQueue_HP_spin_padding;
        typedef cds::container::SegmentedQueue< cds::gc::HP, Value, traits_SegmentedQueue_spin_stat >  SegmentedQueue_HP_spin_stat;
        typedef cds::container::SegmentedQueue< cds::gc::HP, Value, traits_SegmentedQueue_mutex >  SegmentedQueue_HP_mutex;
        typedef cds::container::SegmentedQueue< cds::gc::HP, Value, traits_SegmentedQueue_mutex_padding >  SegmentedQueue_HP_mutex_padding;
        typedef cds::container::SegmentedQueue< cds::gc::HP, Value, traits_SegmentedQueue_mutex_stat >  SegmentedQueue_HP_mutex_stat;

        typedef cds::container::SegmentedQueue< cds::gc::DHP, Value >  SegmentedQueue_DHP_spin;
        typedef cds::container::SegmentedQueue< cds::gc::DHP, Value, traits_SegmentedQueue_spin_padding >  SegmentedQueue_DHP_spin_padding;
        typedef cds::container::SegmentedQueue< cds::gc::DHP, Value, traits_SegmentedQueue_spin_stat >  SegmentedQueue_DHP_spin_stat;
        typedef cds::container::SegmentedQueue< cds::gc::DHP, Value, traits_SegmentedQueue_mutex >  SegmentedQueue_DHP_mutex;
        typedef cds::container::SegmentedQueue< cds::gc::DHP, Value, traits_SegmentedQueue_mutex_padding >  SegmentedQueue_DHP_mutex_padding;
        typedef cds::container::SegmentedQueue< cds::gc::DHP, Value, traits_SegmentedQueue_mutex_stat >  SegmentedQueue_DHP_mutex_stat;
    };
}


// *********************************************
// Queue statistics
namespace cds_test {

    template <typename Counter>
    static inline property_stream& operator <<( property_stream& o, cds::container::fcqueue::stat<Counter> const& s )
    {
            return o
                << CDSSTRESS_STAT_OUT( s, m_nEnqueue )
                << CDSSTRESS_STAT_OUT( s, m_nEnqMove )
                << CDSSTRESS_STAT_OUT( s, m_nDequeue )
                << CDSSTRESS_STAT_OUT( s, m_nFailedDeq )
                << CDSSTRESS_STAT_OUT( s, m_nCollided )
                << static_cast<cds::algo::flat_combining::stat<> const&>(s);
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::fcqueue::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::fcdeque::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::fcdeque::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nPushFront )
            << CDSSTRESS_STAT_OUT( s, m_nPushFrontMove )
            << CDSSTRESS_STAT_OUT( s, m_nPushBack )
            << CDSSTRESS_STAT_OUT( s, m_nPushBackMove )
            << CDSSTRESS_STAT_OUT( s, m_nPopFront )
            << CDSSTRESS_STAT_OUT( s, m_nFailedPopFront )
            << CDSSTRESS_STAT_OUT( s, m_nPopBack )
            << CDSSTRESS_STAT_OUT( s, m_nFailedPopBack )
            << CDSSTRESS_STAT_OUT( s, m_nCollided )
            << static_cast<cds::algo::flat_combining::stat<> const&>(s);
    }

} // namespace cds_test

#define CDSSTRESS_Queue_F( test_fixture, type_name, level ) \
    TEST_F( test_fixture, type_name ) \
    { \
        if ( !check_detail_level( level )) return; \
        typedef queue::Types< value_type >::type_name queue_type; \
        queue_type queue; \
        test( queue ); \
    }

#define CDSSTRESS_MSQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_HP,                0 ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_HP_michaelAlloc,   0 ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_HP_seqcst,         2 ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_HP_ic,             1 ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_HP_stat,           0 ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_DHP,               0 ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_DHP_michaelAlloc,  0 ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_DHP_seqcst,        2 ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_DHP_ic,            1 ) \
    CDSSTRESS_Queue_F( test_fixture, MSQueue_DHP_stat,          0 )

#define CDSSTRESS_MoirQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_HP,              0 ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_HP_michaelAlloc, 0 ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_HP_seqcst,       2 ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_HP_ic,           1 ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_HP_stat,         0 ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_DHP,             0 ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_DHP_michaelAlloc,0 ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_DHP_seqcst,      2 ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_DHP_ic,          1 ) \
    CDSSTRESS_Queue_F( test_fixture, MoirQueue_DHP_stat,        0 )

#define CDSSTRESS_OptimsticQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_HP,                0 ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_HP_michaelAlloc,   0 ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_HP_seqcst,         2 ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_HP_ic,             1 ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_HP_stat,           0 ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_DHP,               0 ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_DHP_michaelAlloc,  0 ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_DHP_seqcst,        2 ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_DHP_ic,            1 ) \
    CDSSTRESS_Queue_F( test_fixture, OptimisticQueue_DHP_stat,          0 )

#define CDSSTRESS_BasketQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_HP,                0 ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_HP_michaelAlloc,   0 ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_HP_seqcst,         2 ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_HP_ic,             1 ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_HP_stat,           0 ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_DHP,               0 ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_DHP_michaelAlloc,  0 ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_DHP_seqcst,        2 ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_DHP_ic,            1 ) \
    CDSSTRESS_Queue_F( test_fixture, BasketQueue_DHP_stat,          0 )

#define CDSSTRESS_FCQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque,                 0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque_stat,            0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque_wait_ss,         1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque_wait_ss_stat,    0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque_wait_sm,         1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque_wait_sm_stat,    0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque_wait_mm,         1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque_wait_mm_stat,    0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque_elimination,     1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_deque_elimination_stat,0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list,                  0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list_stat,             0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list_wait_ss,          1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list_wait_ss_stat,     0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list_wait_sm,          1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list_wait_sm_stat,     0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list_wait_mm,          1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list_wait_mm_stat,     0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list_elimination,      1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCQueue_list_elimination_stat, 0 )

#define CDSSTRESS_FCDeque( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_default,              0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_mutex,                0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_stat,                 0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_wait_ss,              1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_wait_ss_stat,         0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_wait_sm,              1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_wait_sm_stat,         0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_wait_mm,              1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_wait_mm_stat,         0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_elimination,          1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_elimination_stat,     0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_boost,                1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_boost_stat,           0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_boost_elimination,    1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeL_boost_elimination_stat, 1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_default,              0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_mutex,                0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_stat,                 0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_wait_ss,              1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_wait_ss_stat,         0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_wait_sm,              1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_wait_sm_stat,         0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_wait_mm,              1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_wait_mm_stat,         0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_elimination,          1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_elimination_stat,     0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_boost,                1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_boost_stat,           0 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_boost_elimination,    1 ) \
    CDSSTRESS_Queue_F( test_fixture, FCDequeR_boost_elimination_stat, 1 )

#define CDSSTRESS_RWQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, RWQueue_Spin,      0 ) \
    CDSSTRESS_Queue_F( test_fixture, RWQueue_Spin_ic,   1 ) \
    CDSSTRESS_Queue_F( test_fixture, RWQueue_mutex,     0 )

#define CDSSTRESS_SegmentedQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_HP_spin,            0 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_HP_spin_padding,    0 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_HP_spin_stat,       0 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_HP_mutex,           0 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_HP_mutex_padding,   1 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_HP_mutex_stat,      0 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_DHP_spin,           0 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_DHP_spin_padding,   1 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_DHP_spin_stat,      0 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_DHP_mutex,          0 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_DHP_mutex_padding,  1 ) \
    CDSSTRESS_Queue_F( test_fixture, SegmentedQueue_DHP_mutex_stat,     0 )


#define CDSSTRESS_TsigasQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, TsigasCycleQueue_dyn,              0 ) \
    CDSSTRESS_Queue_F( test_fixture, TsigasCycleQueue_dyn_michaelAlloc, 0 ) \
    CDSSTRESS_Queue_F( test_fixture, TsigasCycleQueue_dyn_ic,           1 )

#define CDSSTRESS_VyukovQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, VyukovMPMCCycleQueue_dyn,              0 ) \
    CDSSTRESS_Queue_F( test_fixture, VyukovMPMCCycleQueue_dyn_michaelAlloc, 0 ) \
    CDSSTRESS_Queue_F( test_fixture, VyukovMPMCCycleQueue_dyn_ic,           1 )

#define CDSSTRESS_StdQueue( test_fixture ) \
    CDSSTRESS_Queue_F( test_fixture, StdQueue_deque_Spinlock,   0 ) \
    CDSSTRESS_Queue_F( test_fixture, StdQueue_list_Spinlock,    0 ) \
    CDSSTRESS_Queue_F( test_fixture, StdQueue_deque_Mutex,      1 ) \
    CDSSTRESS_Queue_F( test_fixture, StdQueue_list_Mutex,       1 )

#endif // #ifndef CDSSTRESS_QUEUE_TYPES_H

//$$CDS-header$$

#ifndef __CDSUNIT_INTRUSIVE_QUEUE_TYPES_H
#define __CDSUNIT_INTRUSIVE_QUEUE_TYPES_H

#include <cds/intrusive/msqueue.h>
#include <cds/intrusive/moir_queue.h>
#include <cds/intrusive/optimistic_queue.h>
#include <cds/intrusive/tsigas_cycle_queue.h>
#include <cds/intrusive/vyukov_mpmc_cycle_queue.h>
#include <cds/intrusive/basket_queue.h>
#include <cds/intrusive/fcqueue.h>
#include <cds/intrusive/michael_deque.h>
#include <cds/intrusive/segmented_queue.h>

#include <cds/gc/hp.h>
#include <cds/gc/hrc.h>
#include <cds/gc/ptb.h>

#include <boost/intrusive/slist.hpp>

#include "print_deque_stat.h"
#include "print_segmentedqueue_stat.h"

namespace queue {

    namespace details {
        struct empty_stat {};

        // MichaelDeque, push right/pop left
        template <typename GC, typename T, CDS_DECL_OPTIONS10>
        class MichaelDequeR: public cds::intrusive::MichaelDeque< GC, T, CDS_OPTIONS10>
        {
            typedef cds::intrusive::MichaelDeque< GC, T, CDS_OPTIONS10> base_class;
        public:
            MichaelDequeR( size_t nMaxItemCount )
                : base_class( (unsigned int) nMaxItemCount, 4 )
            {}
            MichaelDequeR()
                : base_class( 64 * 1024, 4 )
            {}

            bool push( T& v )
            {
                return base_class::push_back( v );
            }
            bool enqueue( T& v )
            {
                return push( v );
            }

            T * pop()
            {
                return base_class::pop_front();
            }
            T * deque()
            {
                return pop();
            }
        };

        // MichaelDeque, push left/pop right
        template <typename GC, typename T, CDS_DECL_OPTIONS10>
        class MichaelDequeL: public cds::intrusive::MichaelDeque< GC, T, CDS_OPTIONS10>
        {
            typedef cds::intrusive::MichaelDeque< GC, T, CDS_OPTIONS10> base_class;
        public:
            MichaelDequeL( size_t nMaxItemCount )
                : base_class( (unsigned int) nMaxItemCount, 4 )
            {}
            MichaelDequeL()
                : base_class( 64 * 1024, 4 )
            {}

            bool push( T& v )
            {
                return base_class::push_front( v );
            }
            bool enqueue( T& v )
            {
                return push( v );
            }

            T * pop()
            {
                return base_class::pop_back();
            }
            T * deque()
            {
                return pop();
            }
        };

        template <typename T, typename Lock=cds_std::mutex>
        class BoostSList
        {
            typedef boost::intrusive::slist< T, boost::intrusive::cache_last<true> >    slist_type;
            typedef Lock lock_type;
            typedef cds_std::lock_guard<lock_type> lock_guard;

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
                    return cds::nullptr;
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
    }

    template <typename T>
    struct Types {

        // MSQueue
        typedef cds::intrusive::MSQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HP > > >
        >   MSQueue_HP;

        typedef cds::intrusive::MSQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MSQueue_HP_seqcst;

        typedef cds::intrusive::MSQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HRC > > >
        >   MSQueue_HRC;

        typedef cds::intrusive::MSQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HRC > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MSQueue_HRC_seqcst;

        typedef cds::intrusive::MSQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::PTB > > >
        >   MSQueue_PTB;

        typedef cds::intrusive::MSQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MSQueue_PTB_seqcst;

        // MSQueue + item counter
        typedef cds::intrusive::MSQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MSQueue_HP_ic;

        typedef cds::intrusive::MSQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HRC > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MSQueue_HRC_ic;

        typedef cds::intrusive::MSQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MSQueue_PTB_ic;

        // MSQueue + stat
        typedef cds::intrusive::MSQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MSQueue_HP_stat;

        typedef cds::intrusive::MSQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HRC > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MSQueue_HRC_stat;

        typedef cds::intrusive::MSQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MSQueue_PTB_stat;


        // MoirQueue
        typedef cds::intrusive::MoirQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HP > > >
        >   MoirQueue_HP;

        typedef cds::intrusive::MoirQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MoirQueue_HP_seqcst;

        typedef cds::intrusive::MoirQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HRC > > >
        >   MoirQueue_HRC;

        typedef cds::intrusive::MoirQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HRC > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MoirQueue_HRC_seqcst;

        typedef cds::intrusive::MoirQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::PTB > > >
        >   MoirQueue_PTB;

        typedef cds::intrusive::MoirQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MoirQueue_PTB_seqcst;

        // MoirQueue + item counter
        typedef cds::intrusive::MoirQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MoirQueue_HP_ic;

        typedef cds::intrusive::MoirQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HRC > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MoirQueue_HRC_ic;

        typedef cds::intrusive::MoirQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MoirQueue_PTB_ic;

        // MoirQueue + stat
        typedef cds::intrusive::MoirQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MoirQueue_HP_stat;

        typedef cds::intrusive::MoirQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::HRC > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MoirQueue_HRC_stat;

        typedef cds::intrusive::MoirQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::single_link::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MoirQueue_PTB_stat;

        // OptimisticQueue
        typedef cds::intrusive::OptimisticQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
        >   OptimisticQueue_HP;

        typedef cds::intrusive::OptimisticQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   OptimisticQueue_HP_seqcst;

        typedef cds::intrusive::OptimisticQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::PTB > > >
        >   OptimisticQueue_PTB;

        typedef cds::intrusive::OptimisticQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   OptimisticQueue_PTB_seqcst;


        // OptimisticQueue + item counter
        typedef cds::intrusive::OptimisticQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   OptimisticQueue_HP_ic;

        typedef cds::intrusive::OptimisticQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   OptimisticQueue_PTB_ic;

        // OptimisticQueue + stat
        typedef cds::intrusive::OptimisticQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   OptimisticQueue_HP_stat;

        typedef cds::intrusive::OptimisticQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   OptimisticQueue_PTB_stat;

        // TsigasCycleQueue
        class TsigasCycleQueue_dyn
            : public cds::intrusive::TsigasCycleQueue< T
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
            >
        {
            typedef cds::intrusive::TsigasCycleQueue< T
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
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
            : public cds::intrusive::TsigasCycleQueue< T
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        {
            typedef cds::intrusive::TsigasCycleQueue< T
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
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
        class VyukovMPMCCycleQueue_dyn
            : public cds::intrusive::VyukovMPMCCycleQueue< T
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
            >
        {
            typedef cds::intrusive::VyukovMPMCCycleQueue< T
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
            > base_class;
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

        class VyukovMPMCCycleQueue_dyn_ic
            : public cds::intrusive::VyukovMPMCCycleQueue< T
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        {
            typedef cds::intrusive::VyukovMPMCCycleQueue< T
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            > base_class;
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


        // MichaelDeque
        typedef details::MichaelDequeR< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::HP > > >
        >    MichaelDequeR_HP;
        typedef details::MichaelDequeR< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >    MichaelDequeR_HP_ic;
        typedef details::MichaelDequeR< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        >    MichaelDequeR_HP_stat;

        typedef details::MichaelDequeR< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::PTB > > >
        >    MichaelDequeR_PTB;
        typedef details::MichaelDequeR< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >    MichaelDequeR_PTB_ic;
        typedef details::MichaelDequeR< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        >    MichaelDequeR_PTB_stat;

        typedef details::MichaelDequeL< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::HP > > >
        >    MichaelDequeL_HP;
        typedef details::MichaelDequeL< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >    MichaelDequeL_HP_ic;
        typedef details::MichaelDequeL< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        >    MichaelDequeL_HP_stat;


        typedef details::MichaelDequeL< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::PTB > > >
        >    MichaelDequeL_PTB;
        typedef details::MichaelDequeL< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >    MichaelDequeL_PTB_ic;
        typedef details::MichaelDequeL< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::michael_deque::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        >    MichaelDequeL_PTB_stat;

        // BasketQueue
        typedef cds::intrusive::BasketQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
        >   BasketQueue_HP;

        typedef cds::intrusive::BasketQueue<cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   BasketQueue_HP_seqcst;

        typedef cds::intrusive::BasketQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HRC > > >
        >   BasketQueue_HRC;

        typedef cds::intrusive::BasketQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HRC > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   BasketQueue_HRC_seqcst;

        typedef cds::intrusive::BasketQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::PTB > > >
        >   BasketQueue_PTB;

        typedef cds::intrusive::BasketQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   BasketQueue_PTB_seqcst;

        // BasketQueue + item counter
        typedef cds::intrusive::BasketQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   BasketQueue_HP_ic;

        typedef cds::intrusive::BasketQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HRC > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   BasketQueue_HRC_ic;

        typedef cds::intrusive::BasketQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   BasketQueue_PTB_ic;

        // BasketQueue + stat
        typedef cds::intrusive::BasketQueue< cds::gc::HP, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HP > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   BasketQueue_HP_stat;

        typedef cds::intrusive::BasketQueue< cds::gc::HRC, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::HRC > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   BasketQueue_HRC_stat;

        typedef cds::intrusive::BasketQueue< cds::gc::PTB, T
            ,cds::intrusive::opt::hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc< cds::gc::PTB > > >
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   BasketQueue_PTB_stat;

        // FCQueue
        class traits_FCQueue_delay2:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::back_off< cds::backoff::delay_of<2> >
            >::type
        {};
        class traits_FCQueue_delay2_elimination:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::back_off< cds::backoff::delay_of<2> >
                ,cds::opt::enable_elimination< true >
            >::type
        {};
        class traits_FCQueue_delay2_elimination_stat:
            public cds::intrusive::fcqueue::make_traits<
                cds::opt::back_off< cds::backoff::delay_of<2> >
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

        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_delay2 > FCQueue_list_delay2;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_delay2_elimination > FCQueue_list_delay2_elimination;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_delay2_elimination_stat > FCQueue_list_delay2_elimination_stat;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_expbackoff_elimination > FCQueue_list_expbackoff_elimination;
        typedef cds::intrusive::FCQueue< T, boost::intrusive::list<T>, traits_FCQueue_expbackoff_elimination_stat > FCQueue_list_expbackoff_elimination_stat;

        // SegmentedQueue
        class traits_SegmentedQueue_spin_stat:
            public cds::intrusive::segmented_queue::make_traits<
                cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
            >::type
        {};
        class traits_SegmentedQueue_mutex_stat:
            public cds::intrusive::segmented_queue::make_traits<
                cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
                ,cds::opt::lock_type< cds_std::mutex >
            >::type
        {};
        class traits_SegmentedQueue_mutex:
            public cds::intrusive::segmented_queue::make_traits<
                cds::opt::lock_type< cds_std::mutex >
            >::type
        {};

        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T >  SegmentedQueue_HP_spin;
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T, traits_SegmentedQueue_spin_stat >  SegmentedQueue_HP_spin_stat;
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T, traits_SegmentedQueue_mutex >  SegmentedQueue_HP_mutex;
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, T, traits_SegmentedQueue_mutex_stat >  SegmentedQueue_HP_mutex_stat;

        typedef cds::intrusive::SegmentedQueue< cds::gc::PTB, T >  SegmentedQueue_PTB_spin;
        typedef cds::intrusive::SegmentedQueue< cds::gc::PTB, T, traits_SegmentedQueue_spin_stat >  SegmentedQueue_PTB_spin_stat;
        typedef cds::intrusive::SegmentedQueue< cds::gc::PTB, T, traits_SegmentedQueue_mutex >  SegmentedQueue_PTB_mutex;
        typedef cds::intrusive::SegmentedQueue< cds::gc::PTB, T, traits_SegmentedQueue_mutex_stat >  SegmentedQueue_PTB_mutex_stat;

        // Boost SList
        typedef details::BoostSList< T, cds_std::mutex >    BoostSList_mutex;
        typedef details::BoostSList< T, cds::lock::Spin >   BoostSList_spin;
    };
}


// *********************************************
// Queue statistics
namespace std {

    // cds::intrusive::queue_stat
    template <typename Counter>
    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::queue_stat<Counter> const& s )
    {
        return o
            << "\tStatistics:\n"
            << "\t\t     Enqueue count: " << s.m_EnqueueCount.get() << "\n"
            << "\t\t      Enqueue race: " << s.m_EnqueueRace.get()  << "\n"
            << "\t\t     Dequeue count: " << s.m_DequeueCount.get() << "\n"
            << "\t\t      Dequeue race: " << s.m_DequeueRace.get()  << "\n"
            << "\t\tAdvance tail error: " << s.m_AdvanceTailError.get() << "\n"
            << "\t\t          Bad tail: " << s.m_BadTail.get() << "\n"
;
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::queue_dummy_stat const& s )
    {
        return o;
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::opt::none )
    {
        return o;
    }

    // cds::intrusive::optimistic_queue::stat
    template <typename Counter>
    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::optimistic_queue::stat<Counter> const& s )
    {
        return o
            << static_cast<cds::intrusive::queue_stat<Counter> const&>( s )
            << "\t\t"
            << "\t\t    fix list call: " << s.m_FixListCount.get() << "\n";
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::optimistic_queue::dummy_stat const& s )
    {
        return o;
    }

    // cds::intrusive::basket_queue::stat
    template <typename Counter>
    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::basket_queue::stat<Counter> const& s )
    {
        return o
            << static_cast<cds::intrusive::queue_stat<Counter> const&>( s )
            << "\t\tTry Add basket count: " << s.m_TryAddBasket.get() << "\n"
            << "\t\t    Add basket count: " << s.m_AddBasketCount.get() << "\n";
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::basket_queue::dummy_stat const& s )
    {
        return o;
    }

    // cds::intrusive::fcqueue::stat
    template <typename Counter>
    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::fcqueue::stat<Counter> const& s )
    {
            return o << "\tStatistics:\n"
                << "\t                    Push: " << s.m_nEnqueue.get()           << "\n"
                << "\t                     Pop: " << s.m_nDequeue.get()           << "\n"
                << "\t               FailedPop: " << s.m_nFailedDeq.get()         << "\n"
                << "\t  Collided push/pop pair: " << s.m_nCollided.get()          << "\n"
                << "\tFlat combining statistics:\n"
                << "\t        Combining factor: " << s.combining_factor()         << "\n"
                << "\t         Operation count: " << s.m_nOperationCount.get()    << "\n"
                << "\t      Combine call count: " << s.m_nCombiningCount.get()    << "\n"
                << "\t        Compact pub-list: " << s.m_nCompactPublicationList.get() << "\n"
                << "\t   Deactivate pub-record: " << s.m_nDeactivatePubRecord.get()    << "\n"
                << "\t     Activate pub-record: " << s.m_nActivatePubRecord.get() << "\n"
                << "\t       Create pub-record: " << s.m_nPubRecordCreated.get()  << "\n"
                << "\t       Delete pub-record: " << s.m_nPubRecordDeteted.get()  << "\n"
                << "\t      Acquire pub-record: " << s.m_nAcquirePubRecCount.get()<< "\n"
                << "\t      Release pub-record: " << s.m_nReleasePubRecCount.get()<< "\n";
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::fcqueue::empty_stat const& s )
    {
        return o;
    }

    static inline std::ostream& operator <<( std::ostream& o, queue::details::empty_stat const& s )
    {
        return o;
    }

} // namespace std

#endif // #ifndef __CDSUNIT_INTRUSIVE_QUEUE_TYPES_H

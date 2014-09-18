//$$CDS-header$$

#ifndef __CDSUNIT_QUEUE_TYPES_H
#define __CDSUNIT_QUEUE_TYPES_H

#include <cds/container/msqueue.h>
#include <cds/container/moir_queue.h>
#include <cds/container/rwqueue.h>
#include <cds/container/optimistic_queue.h>
#include <cds/container/tsigas_cycle_queue.h>
#include <cds/container/vyukov_mpmc_cycle_queue.h>
#include <cds/container/basket_queue.h>
#include <cds/container/fcqueue.h>
#include <cds/container/fcdeque.h>
#include <cds/container/michael_deque.h>
#include <cds/container/segmented_queue.h>

#include <cds/gc/hp.h>
#include <cds/gc/hrc.h>
#include <cds/gc/ptb.h>

#include "queue/std_queue.h"
#include "lock/win32_lock.h"
#include "michael_alloc.h"
#include "print_deque_stat.h"
#include "print_segmentedqueue_stat.h"

#include <boost/container/deque.hpp>

namespace queue {

    namespace details {
        // MichaelDeque, push right/pop left
        template <typename GC, typename T, CDS_DECL_OPTIONS7>
        class MichaelDequeR: public cds::container::MichaelDeque< GC, T, CDS_OPTIONS7>
        {
            typedef cds::container::MichaelDeque< GC, T, CDS_OPTIONS7> base_class;
        public:
            MichaelDequeR( size_t nMaxItemCount )
                : base_class( (unsigned int) nMaxItemCount, 4 )
            {}
            MichaelDequeR()
                : base_class( 64 * 1024, 4 )
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

        // MichaelDeque, push left/pop right
        template <typename GC, typename T, CDS_DECL_OPTIONS7>
        class MichaelDequeL: public cds::container::MichaelDeque< GC, T, CDS_OPTIONS7>
        {
            typedef cds::container::MichaelDeque< GC, T, CDS_OPTIONS7> base_class;
        public:
            MichaelDequeL( size_t nMaxItemCount )
                : base_class( (unsigned int) nMaxItemCount, 4 )
            {}
            MichaelDequeL()
                : base_class( 64 * 1024, 4 )
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

        template <typename T, typename Traits=cds::container::fcdeque::type_traits, class Deque=std::deque<T> >
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

        template <typename T, typename Traits=cds::container::fcdeque::type_traits, class Deque = std::deque<T> >
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

    }

    template <typename Value>
    struct Types {

        // MSQueue
        typedef cds::container::MSQueue<
            cds::gc::HP , Value
        >   MSQueue_HP;

        typedef cds::container::MSQueue<
            cds::gc::HP , Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   MSQueue_HP_michaelAlloc;

        typedef cds::container::MSQueue<
            cds::gc::HP, Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MSQueue_HP_seqcst;

        typedef cds::container::MSQueue< cds::gc::HRC,
            Value
        >   MSQueue_HRC;

        typedef cds::container::MSQueue< cds::gc::HRC,
            Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   MSQueue_HRC_michaelAlloc;

        typedef cds::container::MSQueue< cds::gc::HRC,
            Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MSQueue_HRC_seqcst;

        typedef cds::container::MSQueue< cds::gc::PTB,
            Value
        >   MSQueue_PTB;

        typedef cds::container::MSQueue< cds::gc::PTB,
            Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   MSQueue_PTB_michaelAlloc;

        typedef cds::container::MSQueue< cds::gc::PTB,
            Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MSQueue_PTB_seqcst;

        // MSQueue + item counter
        typedef cds::container::MSQueue< cds::gc::HP,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MSQueue_HP_ic;

        typedef cds::container::MSQueue< cds::gc::HRC,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MSQueue_HRC_ic;

        typedef cds::container::MSQueue< cds::gc::PTB,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MSQueue_PTB_ic;

        // MSQueue + stat
        typedef cds::container::MSQueue< cds::gc::HP,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MSQueue_HP_stat;

        typedef cds::container::MSQueue< cds::gc::HRC,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MSQueue_HRC_stat;

        typedef cds::container::MSQueue< cds::gc::PTB,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MSQueue_PTB_stat;


        // MoirQueue
        typedef cds::container::MoirQueue< cds::gc::HP,
            Value
        >   MoirQueue_HP;

        typedef cds::container::MoirQueue< cds::gc::HP,
            Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   MoirQueue_HP_michaelAlloc;

        typedef cds::container::MoirQueue< cds::gc::HP,
            Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MoirQueue_HP_seqcst;

        typedef cds::container::MoirQueue< cds::gc::HRC,
            Value
        >   MoirQueue_HRC;

        typedef cds::container::MoirQueue< cds::gc::HRC,
            Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   MoirQueue_HRC_michaelAlloc;

        typedef cds::container::MoirQueue< cds::gc::HRC,
            Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MoirQueue_HRC_seqcst;

        typedef cds::container::MoirQueue< cds::gc::PTB,
            Value
        >   MoirQueue_PTB;

        typedef cds::container::MoirQueue< cds::gc::PTB,
            Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   MoirQueue_PTB_michaelAlloc;

        typedef cds::container::MoirQueue< cds::gc::PTB,
            Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   MoirQueue_PTB_seqcst;

        // MoirQueue + item counter
        typedef cds::container::MoirQueue< cds::gc::HP,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MoirQueue_HP_ic;

        typedef cds::container::MoirQueue< cds::gc::HRC,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MoirQueue_HRC_ic;

        typedef cds::container::MoirQueue< cds::gc::PTB,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   MoirQueue_PTB_ic;

        // MoirQueue + stat
        typedef cds::container::MoirQueue< cds::gc::HP,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MoirQueue_HP_stat;

        typedef cds::container::MoirQueue< cds::gc::HRC,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MoirQueue_HRC_stat;

        typedef cds::container::MoirQueue< cds::gc::PTB,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   MoirQueue_PTB_stat;

        // OptimisticQueue
        typedef cds::container::OptimisticQueue< cds::gc::HP,
            Value
        >   OptimisticQueue_HP;

        typedef cds::container::OptimisticQueue< cds::gc::HP,
            Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   OptimisticQueue_HP_michaelAlloc;

        typedef cds::container::OptimisticQueue< cds::gc::HP,
            Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   OptimisticQueue_HP_seqcst;

        typedef cds::container::OptimisticQueue< cds::gc::PTB,
            Value
        >   OptimisticQueue_PTB;

        typedef cds::container::OptimisticQueue< cds::gc::PTB,
            Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   OptimisticQueue_PTB_michaelAlloc;

        typedef cds::container::OptimisticQueue< cds::gc::PTB,
            Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   OptimisticQueue_PTB_seqcst;


        // OptimisticQueue + item counter
        typedef cds::container::OptimisticQueue< cds::gc::HP,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   OptimisticQueue_HP_ic;

        typedef cds::container::OptimisticQueue< cds::gc::PTB,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   OptimisticQueue_PTB_ic;

        // OptimisticQueue + stat
        typedef cds::container::OptimisticQueue< cds::gc::HP,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   OptimisticQueue_HP_stat;

        typedef cds::container::OptimisticQueue< cds::gc::PTB,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   OptimisticQueue_PTB_stat;

        // TsigasCycleQueue
        class TsigasCycleQueue_dyn
            : public cds::container::TsigasCycleQueue<
                Value
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
            >
        {
            typedef cds::container::TsigasCycleQueue<
                Value
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

        class TsigasCycleQueue_dyn_michaelAlloc
            : public cds::container::TsigasCycleQueue<
                Value
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                ,cds::opt::allocator< memory::MichaelAllocator<int> >
            >
        {
            typedef cds::container::TsigasCycleQueue<
                Value
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                ,cds::opt::allocator< memory::MichaelAllocator<int> >
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
            : public cds::container::TsigasCycleQueue<
                Value
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        {
            typedef cds::container::TsigasCycleQueue<
                Value
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
            : public cds::container::VyukovMPMCCycleQueue<
                Value
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
            >
        {
            typedef cds::container::VyukovMPMCCycleQueue<
                Value
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

        class VyukovMPMCCycleQueue_dyn_michaelAlloc
            : public cds::container::VyukovMPMCCycleQueue<
            Value
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int, memory::MichaelAllocator<int> > >
            >
        {
            typedef cds::container::VyukovMPMCCycleQueue<
                Value
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int, memory::MichaelAllocator<int> > >
            > base_class;
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

        class VyukovMPMCCycleQueue_dyn_ic
            : public cds::container::VyukovMPMCCycleQueue<
                Value
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        {
            typedef cds::container::VyukovMPMCCycleQueue<
                Value
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
        typedef details::MichaelDequeR< cds::gc::HP, Value >    MichaelDequeR_HP;
        typedef details::MichaelDequeR< cds::gc::HP, Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >    MichaelDequeR_HP_ic;
        typedef details::MichaelDequeR< cds::gc::HP, Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >    MichaelDequeR_HP_michaelAlloc;
        typedef details::MichaelDequeR< cds::gc::HP, Value
            ,cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        >    MichaelDequeR_HP_stat;

        typedef details::MichaelDequeR< cds::gc::PTB, Value >    MichaelDequeR_PTB;
        typedef details::MichaelDequeR< cds::gc::PTB, Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >    MichaelDequeR_PTB_ic;
        typedef details::MichaelDequeR< cds::gc::PTB, Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >    MichaelDequeR_PTB_michaelAlloc;
        typedef details::MichaelDequeR< cds::gc::PTB, Value
            ,cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        >    MichaelDequeR_PTB_stat;

        typedef details::MichaelDequeL< cds::gc::HP, Value >    MichaelDequeL_HP;
        typedef details::MichaelDequeL< cds::gc::HP, Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >    MichaelDequeL_HP_ic;
        typedef details::MichaelDequeL< cds::gc::HP, Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >    MichaelDequeL_HP_michaelAlloc;
        typedef details::MichaelDequeL< cds::gc::HP, Value
            ,cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        >    MichaelDequeL_HP_stat;


        typedef details::MichaelDequeL< cds::gc::PTB, Value >    MichaelDequeL_PTB;
        typedef details::MichaelDequeL< cds::gc::PTB, Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >    MichaelDequeL_PTB_ic;
        typedef details::MichaelDequeL< cds::gc::PTB, Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >    MichaelDequeL_PTB_michaelAlloc;
        typedef details::MichaelDequeL< cds::gc::PTB, Value
            ,cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        >    MichaelDequeL_PTB_stat;

        // BasketQueue
        typedef cds::container::BasketQueue<
            cds::gc::HP , Value
        >   BasketQueue_HP;

        typedef cds::container::BasketQueue<
            cds::gc::HP , Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   BasketQueue_HP_michaelAlloc;

        typedef cds::container::BasketQueue<
            cds::gc::HP, Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   BasketQueue_HP_seqcst;

        typedef cds::container::BasketQueue< cds::gc::HRC,
            Value
        >   BasketQueue_HRC;

        typedef cds::container::BasketQueue< cds::gc::HRC,
            Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   BasketQueue_HRC_michaelAlloc;

        typedef cds::container::BasketQueue< cds::gc::HRC,
            Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   BasketQueue_HRC_seqcst;

        typedef cds::container::BasketQueue< cds::gc::PTB,
            Value
        >   BasketQueue_PTB;

        typedef cds::container::BasketQueue< cds::gc::PTB,
            Value
            ,cds::opt::allocator< memory::MichaelAllocator<int> >
        >   BasketQueue_PTB_michaelAlloc;

        typedef cds::container::BasketQueue< cds::gc::PTB,
            Value
            ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
        >   BasketQueue_PTB_seqcst;

        // BasketQueue + item counter
        typedef cds::container::BasketQueue< cds::gc::HP,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   BasketQueue_HP_ic;

        typedef cds::container::BasketQueue< cds::gc::HRC,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   BasketQueue_HRC_ic;

        typedef cds::container::BasketQueue< cds::gc::PTB,
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        >   BasketQueue_PTB_ic;

        // BasketQueue + stat
        typedef cds::container::BasketQueue< cds::gc::HP,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   BasketQueue_HP_stat;

        typedef cds::container::BasketQueue< cds::gc::HRC,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   BasketQueue_HRC_stat;

        typedef cds::container::BasketQueue< cds::gc::PTB,
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        >   BasketQueue_PTB_stat;


        // RWQueue
        typedef cds::container::RWQueue<
            Value
        > RWQueue_Spin;

        typedef cds::container::RWQueue<
            Value
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        > RWQueue_Spin_ic;

        typedef cds::container::RWQueue<
            Value
            ,cds::opt::stat< cds::intrusive::queue_stat<> >
        > RWQueue_Spin_stat;

        // FCQueue
        class traits_FCQueue_elimination:
            public cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        {};
        class traits_FCQueue_elimination_stat:
            public cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::stat< cds::container::fcqueue::stat<> >
            >::type
        {};

        typedef cds::container::FCQueue< Value > FCQueue_deque;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_elimination > FCQueue_deque_elimination;
        typedef cds::container::FCQueue< Value, std::queue<Value>, traits_FCQueue_elimination_stat > FCQueue_deque_elimination_stat;

        typedef cds::container::FCQueue< Value, std::queue<Value, std::list<Value> > > FCQueue_list;
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
                cds::opt::lock_type< cds_std::mutex >
            >::type
        {};

        typedef details::FCDequeL< Value > FCDequeL_default;
        typedef details::FCDequeL< Value, traits_FCDeque_mutex > FCDequeL_mutex;
        typedef details::FCDequeL< Value, traits_FCDeque_stat > FCDequeL_stat;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination > FCDequeL_elimination;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination_stat > FCDequeL_elimination_stat;

        typedef details::FCDequeL< Value, cds::container::fcdeque::type_traits, boost::container::deque<Value> > FCDequeL_boost;
        typedef details::FCDequeL< Value, traits_FCDeque_stat, boost::container::deque<Value> > FCDequeL_boost_stat;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination, boost::container::deque<Value> > FCDequeL_boost_elimination;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination_stat, boost::container::deque<Value> > FCDequeL_boost_elimination_stat;

        typedef details::FCDequeR< Value > FCDequeR_default;
        typedef details::FCDequeR< Value, traits_FCDeque_mutex > FCDequeR_mutex;
        typedef details::FCDequeR< Value, traits_FCDeque_stat > FCDequeR_stat;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination > FCDequeR_elimination;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination_stat > FCDequeR_elimination_stat;

        typedef details::FCDequeR< Value, cds::container::fcdeque::type_traits, boost::container::deque<Value> > FCDequeR_boost;
        typedef details::FCDequeR< Value, traits_FCDeque_stat, boost::container::deque<Value> > FCDequeR_boost_stat;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination, boost::container::deque<Value> > FCDequeR_boost_elimination;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination_stat, boost::container::deque<Value> > FCDequeR_boost_elimination_stat;

        typedef StdQueue_deque<Value>               StdQueue_deque_Spinlock;
        typedef StdQueue_list<Value>                StdQueue_list_Spinlock;
        typedef StdQueue_deque<Value, cds_std::mutex> StdQueue_deque_BoostMutex;
        typedef StdQueue_list<Value, cds_std::mutex>  StdQueue_list_BoostMutex;
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
        class traits_SegmentedQueue_mutex_stat:
            public cds::container::segmented_queue::make_traits<
                cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
                ,cds::opt::lock_type< cds_std::mutex >
            >::type
        {};
        class traits_SegmentedQueue_mutex:
            public cds::container::segmented_queue::make_traits<
                cds::opt::lock_type< cds_std::mutex >
            >::type
        {};

        typedef cds::container::SegmentedQueue< cds::gc::HP, Value >  SegmentedQueue_HP_spin;
        typedef cds::container::SegmentedQueue< cds::gc::HP, Value, traits_SegmentedQueue_spin_stat >  SegmentedQueue_HP_spin_stat;
        typedef cds::container::SegmentedQueue< cds::gc::HP, Value, traits_SegmentedQueue_mutex >  SegmentedQueue_HP_mutex;
        typedef cds::container::SegmentedQueue< cds::gc::HP, Value, traits_SegmentedQueue_mutex_stat >  SegmentedQueue_HP_mutex_stat;

        typedef cds::container::SegmentedQueue< cds::gc::PTB, Value >  SegmentedQueue_PTB_spin;
        typedef cds::container::SegmentedQueue< cds::gc::PTB, Value, traits_SegmentedQueue_spin_stat >  SegmentedQueue_PTB_spin_stat;
        typedef cds::container::SegmentedQueue< cds::gc::PTB, Value, traits_SegmentedQueue_mutex >  SegmentedQueue_PTB_mutex;
        typedef cds::container::SegmentedQueue< cds::gc::PTB, Value, traits_SegmentedQueue_mutex_stat >  SegmentedQueue_PTB_mutex_stat;


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

    // cds::container::fcqueue::stat
    template <typename Counter>
    static inline std::ostream& operator <<( std::ostream& o, cds::container::fcqueue::stat<Counter> const& s )
    {
            return o << "\tStatistics:\n"
                << "\t                    Push: " << s.m_nEnqueue.get()           << "\n"
                << "\t                PushMove: " << s.m_nEnqMove.get()           << "\n"
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

    static inline std::ostream& operator <<( std::ostream& o, cds::container::fcqueue::empty_stat const& s )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::container::fcdeque::empty_stat const& s )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::container::fcdeque::stat<> const& s )
    {
        return o << "\tStatistics:\n"
            << "\t              Push front: " << s.m_nPushFront.get()         << "\n"
            << "\t         Push front move: " << s.m_nPushFrontMove.get()     << "\n"
            << "\t               Push back: " << s.m_nPushBack.get()          << "\n"
            << "\t          Push back move: " << s.m_nPushBackMove.get()      << "\n"
            << "\t               Pop front: " << s.m_nPopFront.get()          << "\n"
            << "\t        Failed pop front: " << s.m_nFailedPopFront.get()    << "\n"
            << "\t                Pop back: " << s.m_nPopBack.get()           << "\n"
            << "\t         Failed pop back: " << s.m_nFailedPopBack.get()     << "\n"
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

}

#endif // #ifndef __CDSUNIT_QUEUE_TYPES_H

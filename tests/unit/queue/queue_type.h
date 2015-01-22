//$$CDS-header$$

#ifndef CDSUNIT_QUEUE_TYPES_H
#define CDSUNIT_QUEUE_TYPES_H

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

#include "queue/std_queue.h"
#include "lock/win32_lock.h"
#include "michael_alloc.h"
#include "print_segmentedqueue_stat.h"

#include <boost/container/deque.hpp>

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
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                >::type
            >
        {
            typedef cds::container::TsigasCycleQueue< Value,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
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
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                    ,cds::opt::allocator< memory::MichaelAllocator<int> >
                >::type
            >
        {
            typedef cds::container::TsigasCycleQueue< Value,
                typename cds::container::tsigas_queue::make_traits<
                   cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
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
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                    ,cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            >
        {
            typedef cds::container::TsigasCycleQueue< Value,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
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
            typedef cds::opt::v::dynamic_buffer< int > buffer;
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
            typedef cds::opt::v::dynamic_buffer< int, memory::MichaelAllocator<int> > buffer;
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
                cds::opt::lock_type< std::mutex >
            >::type
        {};

        typedef details::FCDequeL< Value > FCDequeL_default;
        typedef details::FCDequeL< Value, traits_FCDeque_mutex > FCDequeL_mutex;
        typedef details::FCDequeL< Value, traits_FCDeque_stat > FCDequeL_stat;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination > FCDequeL_elimination;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination_stat > FCDequeL_elimination_stat;

        typedef details::FCDequeL< Value, cds::container::fcdeque::traits, boost::container::deque<Value> > FCDequeL_boost;
        typedef details::FCDequeL< Value, traits_FCDeque_stat, boost::container::deque<Value> > FCDequeL_boost_stat;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination, boost::container::deque<Value> > FCDequeL_boost_elimination;
        typedef details::FCDequeL< Value, traits_FCDeque_elimination_stat, boost::container::deque<Value> > FCDequeL_boost_elimination_stat;

        typedef details::FCDequeR< Value > FCDequeR_default;
        typedef details::FCDequeR< Value, traits_FCDeque_mutex > FCDequeR_mutex;
        typedef details::FCDequeR< Value, traits_FCDeque_stat > FCDequeR_stat;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination > FCDequeR_elimination;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination_stat > FCDequeR_elimination_stat;

        typedef details::FCDequeR< Value, cds::container::fcdeque::traits, boost::container::deque<Value> > FCDequeR_boost;
        typedef details::FCDequeR< Value, traits_FCDeque_stat, boost::container::deque<Value> > FCDequeR_boost_stat;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination, boost::container::deque<Value> > FCDequeR_boost_elimination;
        typedef details::FCDequeR< Value, traits_FCDeque_elimination_stat, boost::container::deque<Value> > FCDequeR_boost_elimination_stat;

        typedef StdQueue_deque<Value>               StdQueue_deque_Spinlock;
        typedef StdQueue_list<Value>                StdQueue_list_Spinlock;
        typedef StdQueue_deque<Value, std::mutex>   StdQueue_deque_BoostMutex;
        typedef StdQueue_list<Value, std::mutex>    StdQueue_list_BoostMutex;
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
namespace std {

    template <typename Counter>
    static inline std::ostream& operator <<(std::ostream& o, cds::container::basket_queue::stat<Counter> const& s)
    {
        return o
            << "\tStatistics:\n"
            << "\t\t      Enqueue count: " << s.m_EnqueueCount.get() << "\n"
            << "\t\t       Enqueue race: " << s.m_EnqueueRace.get() << "\n"
            << "\t\t      Dequeue count: " << s.m_DequeueCount.get() << "\n"
            << "\t\t      Dequeue empty: " << s.m_EmptyDequeue.get() << "\n"
            << "\t\t       Dequeue race: " << s.m_DequeueRace.get() << "\n"
            << "\t\t Advance tail error: " << s.m_AdvanceTailError.get() << "\n"
            << "\t\t           Bad tail: " << s.m_BadTail.get() << "\n"
            << "\t\tAdd basket attempts: " << s.m_TryAddBasket.get() << "\n"
            << "\t\t Add basket success: " << s.m_AddBasketCount.get() << "\n";
    }
    static inline std::ostream& operator <<(std::ostream& o, cds::container::basket_queue::empty_stat const& /*s*/)
    {
        return o;
    }

    template <typename Counter>
    static inline std::ostream& operator <<( std::ostream& o, cds::container::msqueue::stat<Counter> const& s )
    {
        return o
            << "\tStatistics:\n"
            << "\t\t     Enqueue count: " << s.m_EnqueueCount.get() << "\n"
            << "\t\t      Enqueue race: " << s.m_EnqueueRace.get()  << "\n"
            << "\t\t     Dequeue count: " << s.m_DequeueCount.get() << "\n"
            << "\t\t     Dequeue empty: " << s.m_EmptyDequeue.get() << "\n"
            << "\t\t      Dequeue race: " << s.m_DequeueRace.get()  << "\n"
            << "\t\tAdvance tail error: " << s.m_AdvanceTailError.get() << "\n"
            << "\t\t          Bad tail: " << s.m_BadTail.get() << "\n";
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::container::msqueue::empty_stat const& /*s*/ )
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
            << "\tStatistics:\n"
            << "\t\t      Enqueue count: " << s.m_EnqueueCount.get() << "\n"
            << "\t\t       Enqueue race: " << s.m_EnqueueRace.get() << "\n"
            << "\t\t      Dequeue count: " << s.m_DequeueCount.get() << "\n"
            << "\t\t      Dequeue empty: " << s.m_EmptyDequeue.get() << "\n"
            << "\t\t       Dequeue race: " << s.m_DequeueRace.get() << "\n"
            << "\t\t Advance tail error: " << s.m_AdvanceTailError.get() << "\n"
            << "\t\t           Bad tail: " << s.m_BadTail.get() << "\n"
            << "\t\t      fix list call: " << s.m_FixListCount.get() << "\n";
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::optimistic_queue::empty_stat const& /*s*/ )
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

    static inline std::ostream& operator <<( std::ostream& o, cds::container::fcqueue::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::container::fcdeque::empty_stat const& /*s*/ )
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

#endif // #ifndef CDSUNIT_QUEUE_TYPES_H

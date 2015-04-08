//$$CDS-header$$

#ifndef CDSUNIT_INTRUSIVE_QUEUE_TYPES_H
#define CDSUNIT_INTRUSIVE_QUEUE_TYPES_H

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

#include "print_segmentedqueue_stat.h"

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
    }

    template <typename T>
    struct Types {

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
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                >::type
            >
        {
            typedef cds::intrusive::TsigasCycleQueue< T,
                typename cds::intrusive::tsigas_queue::make_traits<
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

        class TsigasCycleQueue_dyn_ic
            : public cds::intrusive::TsigasCycleQueue< T,
                typename cds::intrusive::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                    ,cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            >
        {
            typedef cds::intrusive::TsigasCycleQueue< T,
                typename cds::intrusive::tsigas_queue::make_traits<
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
        struct traits_VyukovMPMCCycleQueue_dyn : public cds::intrusive::vyukov_queue::traits
        {
            typedef cds::opt::v::dynamic_buffer< int > buffer;
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
}


// *********************************************
// Queue statistics
namespace std {
    /*
    // cds::intrusive::queue_stat
    template <typename Counter>
    static inline std::ostream& operator <<(std::ostream& o, cds::intrusive::queue_stat<Counter> const& s)
    {
        return o
            << "\tStatistics:\n"
            << "\t\t     Enqueue count: " << s.m_EnqueueCount.get() << "\n"
            << "\t\t      Enqueue race: " << s.m_EnqueueRace.get() << "\n"
            << "\t\t     Dequeue count: " << s.m_DequeueCount.get() << "\n"
            << "\t\t      Dequeue race: " << s.m_DequeueRace.get() << "\n"
            << "\t\tAdvance tail error: " << s.m_AdvanceTailError.get() << "\n"
            << "\t\t          Bad tail: " << s.m_BadTail.get() << "\n";
    }
    static inline std::ostream& operator <<(std::ostream& o, cds::intrusive::queue_dummy_stat const& s)
    {
        return o;
    }
    */


    template <typename Counter>
    static inline std::ostream& operator <<(std::ostream& o, cds::intrusive::basket_queue::stat<Counter> const& s)
    {
        return o
            << "\tStatistics:\n"
            << "\t\t      Enqueue count: " << s.m_EnqueueCount.get() << "\n"
            << "\t\t       Enqueue race: " << s.m_EnqueueRace.get() << "\n"
            << "\t\t      Dequeue count: " << s.m_DequeueCount.get() << "\n"
            << "\t\t       Dequeue race: " << s.m_DequeueRace.get() << "\n"
            << "\t\t Advance tail error: " << s.m_AdvanceTailError.get() << "\n"
            << "\t\t           Bad tail: " << s.m_BadTail.get() << "\n"
            << "\t\tAdd basket attempts: " << s.m_TryAddBasket.get() << "\n"
            << "\t\t Add basket success: " << s.m_AddBasketCount.get() << "\n";
    }
    static inline std::ostream& operator <<(std::ostream& o, cds::intrusive::basket_queue::empty_stat const& /*s*/)
    {
        return o;
    }

    template <typename Counter>
    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::msqueue::stat<Counter> const& s )
    {
        return o
            << "\tStatistics:\n"
            << "\t\t     Enqueue count: " << s.m_EnqueueCount.get() << "\n"
            << "\t\t      Enqueue race: " << s.m_EnqueueRace.get()  << "\n"
            << "\t\t     Dequeue count: " << s.m_DequeueCount.get() << "\n"
            << "\t\t      Dequeue race: " << s.m_DequeueRace.get()  << "\n"
            << "\t\tAdvance tail error: " << s.m_AdvanceTailError.get() << "\n"
            << "\t\t          Bad tail: " << s.m_BadTail.get() << "\n";
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::msqueue::empty_stat const& /*s*/ )
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
            << "\t\t     Enqueue count: " << s.m_EnqueueCount.get() << "\n"
            << "\t\t      Enqueue race: " << s.m_EnqueueRace.get() << "\n"
            << "\t\t     Dequeue count: " << s.m_DequeueCount.get() << "\n"
            << "\t\t      Dequeue race: " << s.m_DequeueRace.get() << "\n"
            << "\t\tAdvance tail error: " << s.m_AdvanceTailError.get() << "\n"
            << "\t\t          Bad tail: " << s.m_BadTail.get() << "\n"
            << "\t\t     fix list call: " << s.m_FixListCount.get() << "\n";
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::optimistic_queue::empty_stat const& /*s*/ )
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

    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::fcqueue::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline std::ostream& operator <<( std::ostream& o, queue::details::empty_stat const& /*s*/ )
    {
        return o;
    }

} // namespace std

#endif // #ifndef CDSUNIT_INTRUSIVE_QUEUE_TYPES_H

//$$CDS-header$$

#ifndef CDSUNIT_STACK_TYPES_H
#define CDSUNIT_STACK_TYPES_H

#include <cds/container/treiber_stack.h>
#include <cds/container/fcstack.h>
#include <cds/container/fcdeque.h>

#include <cds/gc/hp.h>
#include <cds/gc/dhp.h>

#include <mutex>
#include <cds/sync/spinlock.h>
#include <stack>
#include <list>
#include <vector>

namespace stack {

    namespace details {

        template <typename T, typename Traits=cds::container::fcdeque::traits>
        class FCDequeL: public cds::container::FCDeque<T, std::deque<T>, Traits >
        {
            typedef cds::container::FCDeque<T, std::deque<T>, Traits > base_class;
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

            bool pop( T& v )
            {
                return base_class::pop_front( v );
            }
        };

        template <typename T, typename Traits=cds::container::fcdeque::traits>
        class FCDequeR: public cds::container::FCDeque<T, std::deque<T>, Traits >
        {
            typedef cds::container::FCDeque<T, std::deque<T>, Traits > base_class;
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

            bool pop( T& v )
            {
                return base_class::pop_back( v );
            }
        };

        template < typename T, typename Stack, typename Lock>
        class StdStack
        {
            Stack   m_Impl;
            mutable Lock    m_Lock;
            cds::container::treiber_stack::empty_stat m_stat;

            typedef std::unique_lock<Lock>  unique_lock;

        public:
            bool push( T const& v )
            {
                unique_lock l( m_Lock );
                m_Impl.push( v );
                return true;
            }

            bool pop( T& v )
            {
                unique_lock l( m_Lock );
                if ( !m_Impl.empty() ) {
                    v = m_Impl.top();
                    m_Impl.pop();
                    return true;
                }
                return false;
            }

            bool empty() const
            {
                unique_lock l( m_Lock );
                return m_Impl.empty();
            }

            cds::container::treiber_stack::empty_stat const& statistics() const
            {
                return m_stat;
            }
        };
    }

    template <typename T>
    struct Types {

    // TreiberStack
        typedef cds::container::TreiberStack< cds::gc::HP,  T > Treiber_HP;
        typedef cds::container::TreiberStack< cds::gc::DHP, T > Treiber_DHP;

        struct traits_Treiber_seqcst: public
            cds::container::treiber_stack::make_traits<
                cds::opt::memory_model<cds::opt::v::sequential_consistent>
            >::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Treiber_seqcst > Treiber_HP_seqcst;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Treiber_seqcst > Treiber_DHP_seqcst;

        struct traits_Treiber_stat: public
            cds::container::treiber_stack::make_traits<
                cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            >::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP, T, traits_Treiber_stat > Treiber_HP_stat;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Treiber_stat > Treiber_DHP_stat;

        struct traits_Treiber_yield: public
            cds::container::treiber_stack::make_traits<
                cds::opt::back_off<cds::backoff::yield>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Treiber_yield > Treiber_HP_yield;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Treiber_yield > Treiber_DHP_yield;

        struct traits_Treiber_pause: public
            cds::container::treiber_stack::make_traits<
                cds::opt::back_off<cds::backoff::pause>
            >::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Treiber_pause > Treiber_HP_pause;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Treiber_pause > Treiber_DHP_pause;

        struct traits_Treiber_exp: public
            cds::container::treiber_stack::make_traits<
                cds::opt::back_off<
                    cds::backoff::exponential<
                        cds::backoff::pause,
                        cds::backoff::yield
                    >
                >
            >::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Treiber_exp > Treiber_HP_exp;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Treiber_exp > Treiber_DHP_exp;


    // Elimination stack
        struct traits_Elimination_on : public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_on > Elimination_HP;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_on > Elimination_DHP;

        struct traits_Elimination_stat : public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_stat > Elimination_HP_stat;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_stat > Elimination_DHP_stat;

        struct traits_Elimination_2ms: public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                ,cds::opt::elimination_backoff< cds::backoff::delay_of<2> >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_2ms >  Elimination_HP_2ms;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_2ms >  Elimination_DHP_2ms;

        struct traits_Elimination_2ms_stat : public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<2> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_2ms_stat > Elimination_HP_2ms_stat;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_2ms_stat > Elimination_DHP_2ms_stat;

        struct traits_Elimination_5ms : public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<5> >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_5ms > Elimination_HP_5ms;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_5ms > Elimination_DHP_5ms;

        struct traits_Elimination_5ms_stat : public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<5> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_5ms_stat > Elimination_HP_5ms_stat;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_5ms_stat > Elimination_DHP_5ms_stat;

        struct traits_Elimination_10ms : public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<10> >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_10ms > Elimination_HP_10ms;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_10ms > Elimination_DHP_10ms;

        struct traits_Elimination_10ms_stat : public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<10> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_10ms_stat > Elimination_HP_10ms_stat;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_10ms_stat > Elimination_DHP_10ms_stat;

        struct traits_Elimination_dyn: public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_dyn > Elimination_HP_dyn;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_dyn > Elimination_DHP_dyn;

        struct traits_Elimination_seqcst: public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::memory_model<cds::opt::v::sequential_consistent>
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_seqcst > Elimination_HP_seqcst;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_seqcst > Elimination_DHP_seqcst;

        struct traits_Elimination_dyn_stat: public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
                , cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_dyn_stat > Elimination_HP_dyn_stat;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_dyn_stat > Elimination_DHP_dyn_stat;

        struct traits_Elimination_yield: public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::back_off<cds::backoff::yield>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_yield > Elimination_HP_yield;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_yield > Elimination_DHP_yield;

        struct traits_Elimination_pause: public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                , cds::opt::back_off<cds::backoff::pause>
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_pause > Elimination_HP_pause;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_pause > Elimination_DHP_pause;

        struct traits_Elimination_exp: public
            cds::container::treiber_stack::make_traits <
                cds::opt::enable_elimination<true>
                ,cds::opt::back_off<
                    cds::backoff::exponential<
                        cds::backoff::pause,
                        cds::backoff::yield
                    >
                >
            > ::type
        {};
        typedef cds::container::TreiberStack< cds::gc::HP,  T, traits_Elimination_exp > Elimination_HP_exp;
        typedef cds::container::TreiberStack< cds::gc::DHP, T, traits_Elimination_exp > Elimination_DHP_exp;


    // FCStack
        typedef cds::container::FCStack< T > FCStack_deque;

        struct traits_FCStack_stat:
            public cds::container::fcstack::make_traits<
                cds::opt::stat< cds::container::fcstack::stat<> >
            >::type
        {};
        struct traits_FCStack_elimination:
            public cds::container::fcstack::make_traits<
            cds::opt::enable_elimination< true >
            >::type
        {};
        struct traits_FCStack_elimination_stat:
            public cds::container::fcstack::make_traits<
                cds::opt::stat< cds::container::fcstack::stat<> >,
                cds::opt::enable_elimination< true >
            >::type
        {};
        struct traits_FCStack_mutex:
            public cds::container::fcstack::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        {};

        typedef cds::container::FCStack< T, std::stack<T, std::deque<T> >, traits_FCStack_mutex > FCStack_deque_mutex;
        typedef cds::container::FCStack< T, std::stack<T, std::deque<T> >, traits_FCStack_stat > FCStack_deque_stat;
        typedef cds::container::FCStack< T, std::stack<T, std::deque<T> >, traits_FCStack_elimination > FCStack_deque_elimination;
        typedef cds::container::FCStack< T, std::stack<T, std::deque<T> >, traits_FCStack_elimination_stat > FCStack_deque_elimination_stat;
        typedef cds::container::FCStack< T, std::stack<T, std::vector<T> > > FCStack_vector;
        typedef cds::container::FCStack< T, std::stack<T, std::vector<T> >, traits_FCStack_mutex > FCStack_vector_mutex;
        typedef cds::container::FCStack< T, std::stack<T, std::vector<T> >, traits_FCStack_stat > FCStack_vector_stat;
        typedef cds::container::FCStack< T, std::stack<T, std::vector<T> >, traits_FCStack_elimination > FCStack_vector_elimination;
        typedef cds::container::FCStack< T, std::stack<T, std::vector<T> >, traits_FCStack_elimination_stat > FCStack_vector_elimination_stat;
        typedef cds::container::FCStack< T, std::stack<T, std::list<T> > > FCStack_list;
        typedef cds::container::FCStack< T, std::stack<T, std::list<T> >, traits_FCStack_mutex > FCStack_list_mutex;
        typedef cds::container::FCStack< T, std::stack<T, std::list<T> >, traits_FCStack_stat > FCStack_list_stat;
        typedef cds::container::FCStack< T, std::stack<T, std::list<T> >, traits_FCStack_elimination > FCStack_list_elimination;
        typedef cds::container::FCStack< T, std::stack<T, std::list<T> >, traits_FCStack_elimination_stat > FCStack_list_elimination_stat;

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


        typedef details::FCDequeL< T > FCDequeL_default;
        typedef details::FCDequeL< T, traits_FCDeque_mutex > FCDequeL_mutex;
        typedef details::FCDequeL< T, traits_FCDeque_stat > FCDequeL_stat;
        typedef details::FCDequeL< T, traits_FCDeque_elimination > FCDequeL_elimination;
        typedef details::FCDequeL< T, traits_FCDeque_elimination_stat > FCDequeL_elimination_stat;

        typedef details::FCDequeR< T > FCDequeR_default;
        typedef details::FCDequeR< T, traits_FCDeque_mutex > FCDequeR_mutex;
        typedef details::FCDequeR< T, traits_FCDeque_stat > FCDequeR_stat;
        typedef details::FCDequeR< T, traits_FCDeque_elimination > FCDequeR_elimination;
        typedef details::FCDequeR< T, traits_FCDeque_elimination_stat > FCDequeR_elimination_stat;


        // std::stack
        typedef details::StdStack< T, std::stack< T >, std::mutex >  StdStack_Deque_Mutex;
        typedef details::StdStack< T, std::stack< T >, cds::sync::spin > StdStack_Deque_Spin;
        typedef details::StdStack< T, std::stack< T, std::vector<T> >, std::mutex >  StdStack_Vector_Mutex;
        typedef details::StdStack< T, std::stack< T, std::vector<T> >, cds::sync::spin > StdStack_Vector_Spin;
        typedef details::StdStack< T, std::stack< T, std::list<T> >, std::mutex >  StdStack_List_Mutex;
        typedef details::StdStack< T, std::stack< T, std::list<T> >, cds::sync::spin > StdStack_List_Spin;

    };
} // namespace stack

namespace std {
    static inline ostream& operator <<( ostream& o, cds::container::treiber_stack::stat<> const& s )
    {
        return o << "\tStatistics:\n"
            << "\t                    Push: " << s.m_PushCount.get()              << "\n"
            << "\t                     Pop: " << s.m_PopCount.get()               << "\n"
            << "\t         Push contention: " << s.m_PushRace.get()               << "\n"
            << "\t          Pop contention: " << s.m_PopRace.get()                << "\n"
            << "\t   m_ActivePushCollision: " << s.m_ActivePushCollision.get()    << "\n"
            << "\t   m_PassivePopCollision: " << s.m_PassivePopCollision.get()    << "\n"
            << "\t    m_ActivePopCollision: " << s.m_ActivePopCollision.get()     << "\n"
            << "\t  m_PassivePushCollision: " << s.m_PassivePushCollision.get()   << "\n"
            << "\t     m_EliminationFailed: " << s.m_EliminationFailed.get()      << "\n";
    }

    static inline ostream& operator <<(ostream& o, cds::container::treiber_stack::empty_stat const& /*s*/)
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::container::fcstack::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::container::fcstack::stat<> const& s )
    {
        return o << "\tStatistics:\n"
            << "\t                    Push: " << s.m_nPush.get()              << "\n"
            << "\t                PushMove: " << s.m_nPushMove.get()          << "\n"
            << "\t                     Pop: " << s.m_nPop.get()               << "\n"
            << "\t               FailedPop: " << s.m_nFailedPop.get()         << "\n"
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

} // namespace std

#endif // #ifndef CDSUNIT_STACK_TYPES_H

//$$CDS-header$$

#ifndef CDSUNIT_INTRUSIVE_STACK_TYPES_H
#define CDSUNIT_INTRUSIVE_STACK_TYPES_H

#include <cds/intrusive/treiber_stack.h>
#include <cds/intrusive/fcstack.h>

#include <cds/gc/hp.h>
#include <cds/gc/dhp.h>

#include <mutex>
#include <cds/sync/spinlock.h>
#include <stack>
#include <list>
#include <vector>
#include <boost/intrusive/list.hpp>

namespace istack {

    namespace details {

        template < typename T, typename Stack, typename Lock>
        class StdStack
        {
            Stack   m_Impl;
            mutable Lock    m_Lock;
            cds::intrusive::treiber_stack::empty_stat m_stat;

            typedef std::unique_lock<Lock>  unique_lock;

        public:
            typedef T value_type;

            bool push( T& v )
            {
                unique_lock l( m_Lock );
                m_Impl.push( &v );
                return true;
            }

            T * pop()
            {
                unique_lock l( m_Lock );
                if ( !m_Impl.empty() ) {
                     T * v = m_Impl.top();
                    m_Impl.pop();
                    return v;
                }
                return nullptr;
            }

            bool empty() const
            {
                unique_lock l( m_Lock );
                return m_Impl.empty();
            }

            cds::intrusive::treiber_stack::empty_stat const& statistics() const
            {
                return m_stat;
            }
        };
    }

    template <typename T>
    struct Types {

        template <class GC>
        using base_hook = cds::intrusive::treiber_stack::base_hook < cds::opt::gc< GC > >;

    // TreiberStack
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T > Treiber_HP;
        struct traits_Treiber_DHP: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<cds::gc::DHP> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_DHP >Treiber_DHP;

        template <class GC> struct traits_Treiber_seqcst : public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::memory_model<cds::opt::v::sequential_consistent>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_seqcst<cds::gc::HP>  > Treiber_HP_seqcst;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_seqcst<cds::gc::DHP> > Treiber_DHP_seqcst;

        template <class GC> struct traits_Treiber_stat: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_stat<cds::gc::HP>  > Treiber_HP_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_stat<cds::gc::DHP> > Treiber_DHP_stat;

        template <class GC> struct traits_Treiber_yield: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::back_off<cds::backoff::yield>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_yield<cds::gc::HP>  > Treiber_HP_yield;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_yield<cds::gc::DHP> > Treiber_DHP_yield;

        template <class GC> struct traits_Treiber_pause: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::back_off<cds::backoff::pause>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_pause<cds::gc::HP>  > Treiber_HP_pause;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_pause<cds::gc::DHP> > Treiber_DHP_pause;

        template <class GC> struct traits_Treiber_exp: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                ,cds::opt::back_off<
                    cds::backoff::exponential<
                        cds::backoff::pause,
                        cds::backoff::yield
                    >
                >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_exp<cds::gc::HP>  > Treiber_HP_exp;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_exp<cds::gc::DHP> > Treiber_DHP_exp;


    // Elimination stack
        template <class GC> struct traits_Elimination_on : public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_on<cds::gc::HP>  > Elimination_HP;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_on<cds::gc::DHP> > Elimination_DHP;

        template <class GC> struct traits_Elimination_seqcst : public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::memory_model< cds::opt::v::sequential_consistent >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_seqcst<cds::gc::HP>  > Elimination_HP_seqcst;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_seqcst<cds::gc::DHP> > Elimination_DHP_seqcst;

        template <class GC> struct traits_Elimination_2ms: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<2> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_2ms<cds::gc::HP>  > Elimination_HP_2ms;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_2ms<cds::gc::DHP> > Elimination_DHP_2ms;

        template <class GC> struct traits_Elimination_2ms_stat: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<2> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_2ms_stat<cds::gc::HP>  > Elimination_HP_2ms_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_2ms_stat<cds::gc::DHP> > Elimination_DHP_2ms_stat;

        template <class GC> struct traits_Elimination_5ms: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<5> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_5ms<cds::gc::HP>  > Elimination_HP_5ms;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_5ms<cds::gc::DHP> > Elimination_DHP_5ms;

        template <class GC> struct traits_Elimination_5ms_stat: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<5> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_5ms_stat<cds::gc::HP>  > Elimination_HP_5ms_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_5ms_stat<cds::gc::DHP> > Elimination_DHP_5ms_stat;

        template <class GC> struct traits_Elimination_10ms: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<10> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_10ms<cds::gc::HP>  > Elimination_HP_10ms;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_10ms<cds::gc::DHP> > Elimination_DHP_10ms;

        template <class GC> struct traits_Elimination_10ms_stat: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<10> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_10ms_stat<cds::gc::HP>  > Elimination_HP_10ms_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_10ms_stat<cds::gc::DHP> > Elimination_DHP_10ms_stat;

        template <class GC> struct traits_Elimination_dyn: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_dyn<cds::gc::HP>  > Elimination_HP_dyn;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_dyn<cds::gc::DHP> > Elimination_DHP_dyn;

        template <class GC> struct traits_Elimination_stat: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_stat<cds::gc::HP>  > Elimination_HP_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_stat<cds::gc::DHP> > Elimination_DHP_stat;

        template <class GC> struct traits_Elimination_dyn_stat: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_dyn_stat<cds::gc::HP>  > Elimination_HP_dyn_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_dyn_stat<cds::gc::DHP> > Elimination_DHP_dyn_stat;

        template <class GC> struct traits_Elimination_yield: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::back_off<cds::backoff::yield>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_yield<cds::gc::HP>  > Elimination_HP_yield;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_yield<cds::gc::DHP> > Elimination_DHP_yield;

        template <class GC> struct traits_Elimination_pause: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::back_off<cds::backoff::pause>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_pause<cds::gc::HP>  > Elimination_HP_pause;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_pause<cds::gc::DHP> > Elimination_DHP_pause;

        template <class GC> struct traits_Elimination_exp: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                ,cds::opt::back_off<
                    cds::backoff::exponential<
                        cds::backoff::pause,
                        cds::backoff::yield
                    >
                >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_exp<cds::gc::HP>  > Elimination_HP_exp;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_exp<cds::gc::DHP> > Elimination_DHP_exp;

    // FCStack
        typedef cds::intrusive::FCStack< T > FCStack_slist;

        struct traits_FCStack_stat:
            public cds::intrusive::fcstack::make_traits<
                cds::opt::stat< cds::intrusive::fcstack::stat<> >
            >::type
        {};
        struct traits_FCStack_elimination:
            public cds::intrusive::fcstack::make_traits<
            cds::opt::enable_elimination< true >
            >::type
        {};
        struct traits_FCStack_elimination_stat:
            public cds::intrusive::fcstack::make_traits<
                cds::opt::stat< cds::intrusive::fcstack::stat<> >,
                cds::opt::enable_elimination< true >
            >::type
        {};

        struct traits_FCStack_mutex_stat:
            public cds::intrusive::fcstack::make_traits<
                cds::opt::stat< cds::intrusive::fcstack::stat<> >
                ,cds::opt::lock_type< std::mutex >
            >::type
        {};
        struct traits_FCStack_mutex_elimination:
            public cds::intrusive::fcstack::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::lock_type< std::mutex >
            >::type
        {};
        struct traits_FCStack_mutex_elimination_stat:
            public cds::intrusive::fcstack::make_traits<
                cds::opt::stat< cds::intrusive::fcstack::stat<> >
                ,cds::opt::enable_elimination< true >
                ,cds::opt::lock_type< std::mutex >
            >::type
        {};

        typedef cds::intrusive::FCStack< T, boost::intrusive::slist< T >, traits_FCStack_stat > FCStack_slist_stat;
        typedef cds::intrusive::FCStack< T, boost::intrusive::slist< T >, traits_FCStack_elimination > FCStack_slist_elimination;
        typedef cds::intrusive::FCStack< T, boost::intrusive::slist< T >, traits_FCStack_elimination_stat > FCStack_slist_elimination_stat;
        typedef cds::intrusive::FCStack< T, boost::intrusive::slist< T >, traits_FCStack_mutex_stat > FCStack_slist_mutex_stat;
        typedef cds::intrusive::FCStack< T, boost::intrusive::slist< T >, traits_FCStack_mutex_elimination > FCStack_slist_mutex_elimination;
        typedef cds::intrusive::FCStack< T, boost::intrusive::slist< T >, traits_FCStack_mutex_elimination_stat > FCStack_slist_mutex_elimination_stat;
        typedef cds::intrusive::FCStack< T, boost::intrusive::list< T > > FCStack_list;
        typedef cds::intrusive::FCStack< T, boost::intrusive::list< T >, traits_FCStack_stat > FCStack_list_stat;
        typedef cds::intrusive::FCStack< T, boost::intrusive::list< T >, traits_FCStack_elimination > FCStack_list_elimination;
        typedef cds::intrusive::FCStack< T, boost::intrusive::list< T >, traits_FCStack_elimination_stat > FCStack_list_elimination_stat;
        typedef cds::intrusive::FCStack< T, boost::intrusive::list< T >, traits_FCStack_mutex_stat > FCStack_list_mutex_stat;
        typedef cds::intrusive::FCStack< T, boost::intrusive::list< T >, traits_FCStack_mutex_elimination > FCStack_list_mutex_elimination;
        typedef cds::intrusive::FCStack< T, boost::intrusive::list< T >, traits_FCStack_mutex_elimination_stat > FCStack_list_mutex_elimination_stat;


        // std::stack
        typedef details::StdStack< T, std::stack< T* >, std::mutex >  StdStack_Deque_Mutex;
        typedef details::StdStack< T, std::stack< T* >, cds::sync::spin > StdStack_Deque_Spin;
        typedef details::StdStack< T, std::stack< T*, std::vector<T*> >, std::mutex >  StdStack_Vector_Mutex;
        typedef details::StdStack< T, std::stack< T*, std::vector<T*> >, cds::sync::spin > StdStack_Vector_Spin;
        typedef details::StdStack< T, std::stack< T*, std::list<T*> >, std::mutex >  StdStack_List_Mutex;
        typedef details::StdStack< T, std::stack< T*, std::list<T*> >, cds::sync::spin > StdStack_List_Spin;

    };
} // namespace istack

namespace std {
    static inline ostream& operator <<( ostream& o, cds::intrusive::treiber_stack::stat<> const& s )
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

    static inline ostream& operator <<( ostream& o, cds::intrusive::treiber_stack::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::fcstack::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::fcstack::stat<> const& s )
    {
        return o << "\tStatistics:\n"
            << "\t                    Push: " << s.m_nPush.get()              << "\n"
            << "\t                     Pop: " << s.m_nPop.get()               << "\n"
            << "\t               FailedPop: " << s.m_nFailedPop.get()         << "\n"
            << "\t  Collided push/pop pair: " << s.m_nCollided.get()          << "\n"
            << "\tFlat combining statistics:\n"
            << "\t        Combining factor: " << s.combining_factor()         << "\n"
            << "\t         Operation count: " << s.m_nOperationCount.get()    << "\n"
            << "\t      Combine call count: " << s.m_nCombiningCount.get()    << "\n"
            << "\t        Compact pub-list: " << s.m_nCompactPublicationList.get() << "\n"
            << "\t   Deactivate pub-record: " << s.m_nDeactivatePubRecord.get()    << "\n"
            << "\t     Activate pub-record: " << s.m_nActivatePubRecord.get()    << "\n"
            << "\t       Create pub-record: " << s.m_nPubRecordCreated.get()  << "\n"
            << "\t       Delete pub-record: " << s.m_nPubRecordDeteted.get()  << "\n"
            << "\t      Acquire pub-record: " << s.m_nAcquirePubRecCount.get()<< "\n"
            << "\t      Release pub-record: " << s.m_nReleasePubRecCount.get()<< "\n";
    }

} // namespace std

#endif // #ifndef CDSUNIT_INTRUSIVE_STACK_TYPES_H

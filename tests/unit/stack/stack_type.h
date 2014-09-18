//$$CDS-header$$

#ifndef __CDSUNIT_STACK_TYPES_H
#define __CDSUNIT_STACK_TYPES_H

#include <cds/container/treiber_stack.h>
#include <cds/container/michael_deque.h>
#include <cds/container/fcstack.h>
#include <cds/container/fcdeque.h>

#include <cds/gc/hp.h>
#include <cds/gc/ptb.h>
#include <cds/gc/hrc.h>

#include <cds/details/std/mutex.h>
#include <cds/lock/spinlock.h>
#include <stack>
#include <list>
#include <vector>

namespace stack {

    namespace details {
        template <typename GC, typename T, CDS_DECL_OPTIONS7>
        class MichaelDequeL: public cds::container::MichaelDeque< GC, T, CDS_OPTIONS7>
        {
            typedef cds::container::MichaelDeque< GC, T, CDS_OPTIONS7> base_class;
        public:
            MichaelDequeL( size_t nMaxItemCount )
                : base_class( (unsigned int) nMaxItemCount, 4 )
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

        template <typename GC, typename T, CDS_DECL_OPTIONS7>
        class MichaelDequeR: public cds::container::MichaelDeque< GC, T, CDS_OPTIONS7>
        {
            typedef cds::container::MichaelDeque< GC, T, CDS_OPTIONS7> base_class;
        public:
            MichaelDequeR( size_t nMaxItemCount )
                : base_class( (unsigned int) nMaxItemCount, 4 )
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

        template <typename T, typename Traits=cds::container::fcdeque::type_traits>
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

        template <typename T, typename Traits=cds::container::fcdeque::type_traits>
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

            typedef cds_std::unique_lock<Lock>  unique_lock;

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
        typedef cds::container::TreiberStack< cds::gc::HP, T
        >       Treiber_HP;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::memory_model<cds::opt::v::sequential_consistent>
        >       Treiber_HP_seqcst;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        >       Treiber_HP_stat;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
        >       Treiber_HRC;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        >       Treiber_HRC_stat;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
        > Treiber_PTB;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        >       Treiber_PTB_stat;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::back_off<cds::backoff::yield>
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
        > Treiber_HP_yield;
        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::back_off<cds::backoff::pause>
        > Treiber_HP_pause;
        typedef cds::container::TreiberStack< cds::gc::HP, T,
            cds::opt::back_off<
                cds::backoff::exponential<
                    cds::backoff::pause,
                    cds::backoff::yield
                >
            >
        > Treiber_HP_exp;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::back_off<cds::backoff::yield>
        >  Treiber_HRC_yield;
        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::back_off<cds::backoff::pause>
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
        > Treiber_HRC_pause;
        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::back_off<
                cds::backoff::exponential<
                    cds::backoff::pause,
                    cds::backoff::yield
                >
            >
        > Treiber_HRC_exp;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::back_off<cds::backoff::yield>
        >  Treiber_PTB_yield;
        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::back_off<cds::backoff::pause>
        > Treiber_PTB_pause;
        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::back_off<
                cds::backoff::exponential<
                    cds::backoff::pause,
                    cds::backoff::yield
                >
            >
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
        > Treiber_PTB_exp;


    // Elimination stack
        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
        >       Elimination_HP;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<2> >
        >       Elimination_HP_2ms;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<2> >
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        >       Elimination_HP_2ms_stat;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<5> >
        >       Elimination_HP_5ms;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<5> >
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        >       Elimination_HP_5ms_stat;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<10> >
        >       Elimination_HP_10ms;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<10> >
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        >       Elimination_HP_10ms_stat;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
        >       Elimination_HP_dyn;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::memory_model<cds::opt::v::sequential_consistent>
        >       Elimination_HP_seqcst;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        >       Elimination_HP_stat;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
        >       Elimination_HP_dyn_stat;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::enable_elimination<true>
        >       Elimination_HRC;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
        >       Elimination_HRC_dyn;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        >       Elimination_HRC_stat;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
        >       Elimination_HRC_dyn_stat;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<2> >
        > Elimination_PTB_2ms;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<2> >
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        > Elimination_PTB_2ms_stat;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<5> >
        > Elimination_PTB_5ms;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<5> >
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        > Elimination_PTB_5ms_stat;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<10> >
        > Elimination_PTB_10ms;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::elimination_backoff< cds::backoff::delay_of<10> >
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        > Elimination_PTB_10ms_stat;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
        > Elimination_PTB;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
        > Elimination_PTB_dyn;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
        >       Elimination_PTB_stat;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
        >       Elimination_PTB_dyn_stat;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::back_off<cds::backoff::yield>
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
        > Elimination_HP_yield;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::back_off<cds::backoff::pause>
        > Elimination_HP_pause;

        typedef cds::container::TreiberStack< cds::gc::HP, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::back_off<
                cds::backoff::exponential<
                    cds::backoff::pause,
                    cds::backoff::yield
                >
            >
        > Elimination_HP_exp;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::back_off<cds::backoff::yield>
        >  Elimination_HRC_yield;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::back_off<cds::backoff::pause>
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
        > Elimination_HRC_pause;

        typedef cds::container::TreiberStack< cds::gc::HRC, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::back_off<
                cds::backoff::exponential<
                    cds::backoff::pause,
                    cds::backoff::yield
                >
            >
        > Elimination_HRC_exp;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::back_off<cds::backoff::yield>
        >  Elimination_PTB_yield;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::back_off<cds::backoff::pause>
        > Elimination_PTB_pause;

        typedef cds::container::TreiberStack< cds::gc::PTB, T
            ,cds::opt::enable_elimination<true>
            ,cds::opt::back_off<
                cds::backoff::exponential<
                    cds::backoff::pause,
                    cds::backoff::yield
                >
            >
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
        > Elimination_PTB_exp;

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
                cds::opt::lock_type< cds_std::mutex >
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
                cds::opt::lock_type< cds_std::mutex >
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

    // MichaelDeque, left side
        typedef details::MichaelDequeL< cds::gc::HP, T> MichaelDequeL_HP;
        typedef details::MichaelDequeL< cds::gc::HP, T
            ,cds::opt::memory_model<cds::opt::v::sequential_consistent>
        > MichaelDequeL_HP_seqcst;

        typedef details::MichaelDequeL< cds::gc::HP, T
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        > MichaelDequeL_HP_ic;

        typedef details::MichaelDequeL< cds::gc::HP, T
            ,cds::opt::back_off<
                cds::backoff::exponential<
                    cds::backoff::pause,
                    cds::backoff::yield
                >
            >
        > MichaelDequeL_HP_exp;

        typedef details::MichaelDequeL< cds::gc::HP, T
            ,cds::opt::back_off< cds::backoff::yield >
        > MichaelDequeL_HP_yield;

        typedef details::MichaelDequeL< cds::gc::HP, T
            ,cds::opt::stat<cds::intrusive::deque_stat<> >
        > MichaelDequeL_HP_stat;


        typedef details::MichaelDequeL< cds::gc::PTB, T> MichaelDequeL_PTB;
        typedef details::MichaelDequeL< cds::gc::PTB, T
            ,cds::opt::memory_model<cds::opt::v::sequential_consistent>
        > MichaelDequeL_PTB_seqcst;

        typedef details::MichaelDequeL< cds::gc::PTB, T
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        > MichaelDequeL_PTB_ic;

        typedef details::MichaelDequeL< cds::gc::PTB, T
            ,cds::opt::back_off<
            cds::backoff::exponential<
            cds::backoff::pause,
            cds::backoff::yield
            >
            >
        > MichaelDequeL_PTB_exp;

        typedef details::MichaelDequeL< cds::gc::PTB, T
            ,cds::opt::back_off< cds::backoff::yield >
        > MichaelDequeL_PTB_yield;

        typedef details::MichaelDequeL< cds::gc::PTB, T
            ,cds::opt::stat<cds::intrusive::michael_deque::stat<> >
        > MichaelDequeL_PTB_stat;


    // MichaelDeque, right side
        typedef details::MichaelDequeR< cds::gc::HP, T> MichaelDequeR_HP;
        typedef details::MichaelDequeR< cds::gc::HP, T
            ,cds::opt::memory_model<cds::opt::v::sequential_consistent>
        > MichaelDequeR_HP_seqcst;

        typedef details::MichaelDequeR< cds::gc::HP, T
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        > MichaelDequeR_HP_ic;

        typedef details::MichaelDequeR< cds::gc::HP, T
            ,cds::opt::back_off<
            cds::backoff::exponential<
            cds::backoff::pause,
            cds::backoff::yield
            >
            >
        > MichaelDequeR_HP_exp;

        typedef details::MichaelDequeR< cds::gc::HP, T
            ,cds::opt::back_off< cds::backoff::yield >
        > MichaelDequeR_HP_yield;

        typedef details::MichaelDequeR< cds::gc::HP, T
            ,cds::opt::stat< cds::intrusive::michael_deque::stat<> >
        > MichaelDequeR_HP_stat;

        typedef details::MichaelDequeR< cds::gc::PTB, T> MichaelDequeR_PTB;
        typedef details::MichaelDequeR< cds::gc::PTB, T
            ,cds::opt::memory_model<cds::opt::v::sequential_consistent>
        > MichaelDequeR_PTB_seqcst;

        typedef details::MichaelDequeR< cds::gc::PTB, T
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        > MichaelDequeR_PTB_ic;

        typedef details::MichaelDequeR< cds::gc::PTB, T
            ,cds::opt::back_off<
                cds::backoff::exponential<
                    cds::backoff::pause,
                    cds::backoff::yield
                >
            >
        > MichaelDequeR_PTB_exp;

        typedef details::MichaelDequeR< cds::gc::PTB, T
            ,cds::opt::stat< cds::intrusive::deque_stat<> >
        > MichaelDequeR_PTB_stat;

        typedef details::MichaelDequeR< cds::gc::PTB, T
            ,cds::opt::back_off< cds::backoff::yield >
        > MichaelDequeR_PTB_yield;


        // std::stack
        typedef details::StdStack< T, std::stack< T >, cds_std::mutex >  StdStack_Deque_Mutex;
        typedef details::StdStack< T, std::stack< T >, cds::lock::Spin > StdStack_Deque_Spin;
        typedef details::StdStack< T, std::stack< T, std::vector<T> >, cds_std::mutex >  StdStack_Vector_Mutex;
        typedef details::StdStack< T, std::stack< T, std::vector<T> >, cds::lock::Spin > StdStack_Vector_Spin;
        typedef details::StdStack< T, std::stack< T, std::list<T> >, cds_std::mutex >  StdStack_List_Mutex;
        typedef details::StdStack< T, std::stack< T, std::list<T> >, cds::lock::Spin > StdStack_List_Spin;

    };
} // namespace stack

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

    static inline ostream& operator <<( ostream& o, cds::intrusive::treiber_stack::empty_stat const& s )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::container::fcstack::empty_stat const& s )
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

} // namespace std

#endif // #ifndef __CDSUNIT_STACK_TYPES_H

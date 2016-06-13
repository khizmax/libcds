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

#ifndef CDSSTRESS_STACK_TYPES_H
#define CDSSTRESS_STACK_TYPES_H

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

#include <cds_test/stress_test.h>
#include <cds_test/stat_flat_combining_out.h>

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
                , cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer<int> >
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
                , cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer<int> >
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

namespace cds_test {
    static inline property_stream& operator <<( property_stream& o, cds::container::treiber_stack::empty_stat const& )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::treiber_stack::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_PushCount )
            << CDSSTRESS_STAT_OUT( s, m_PopCount  )
            << CDSSTRESS_STAT_OUT( s, m_PushRace  )
            << CDSSTRESS_STAT_OUT( s, m_PopRace   )
            << CDSSTRESS_STAT_OUT( s, m_ActivePushCollision  )
            << CDSSTRESS_STAT_OUT( s, m_PassivePopCollision  )
            << CDSSTRESS_STAT_OUT( s, m_ActivePopCollision   )
            << CDSSTRESS_STAT_OUT( s, m_PassivePushCollision )
            << CDSSTRESS_STAT_OUT( s, m_EliminationFailed    );
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::fcstack::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::fcstack::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nPush )
            << CDSSTRESS_STAT_OUT( s, m_nPushMove )
            << CDSSTRESS_STAT_OUT( s, m_nPop )
            << CDSSTRESS_STAT_OUT( s, m_nFailedPop )
            << CDSSTRESS_STAT_OUT( s, m_nCollided )
            << static_cast<cds::algo::flat_combining::stat<> const&>( s );
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

#define CDSSTRESS_Stack_F( test_fixture, type_name ) \
    TEST_F( test_fixture, type_name ) \
    { \
        typedef stack::Types< value_type >::type_name stack_type; \
        stack_type stack; \
        test( stack ); \
    }

#define CDSSTRESS_EliminationStack_F( test_fixture, type_name ) \
    TEST_F( test_fixture, type_name ) \
    { \
        typedef stack::Types< value_type >::type_name stack_type; \
        stack_type stack( s_nEliminationSize ); \
        test_elimination( stack ); \
    }

#define CDSSTRESS_TreiberStack( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP )        \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP_seqcst ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP_pause )  \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP_exp )    \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP_stat   ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_DHP       ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_DHP_pause ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_DHP_exp   ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_DHP_stat  ) \

#define CDSSTRESS_EliminationStack( test_fixture ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP        ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_2ms    ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_2ms_stat) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_5ms    ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_5ms_stat) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_10ms    ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_10ms_stat) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_seqcst ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_pause  ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_exp    ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_stat   ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_dyn    ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_HP_dyn_stat) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP       ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_2ms    ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_2ms_stat) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_5ms    ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_5ms_stat) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_10ms    ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_10ms_stat) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_pause ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_exp   ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_stat  ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_dyn   ) \
    CDSSTRESS_EliminationStack_F( test_fixture, Elimination_DHP_dyn_stat)

#define CDSSTRESS_FCStack( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_deque ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_deque_mutex ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_deque_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_deque_elimination ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_deque_elimination_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_vector ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_vector_mutex ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_vector_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_vector_elimination ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_vector_elimination_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_mutex ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_elimination ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_elimination_stat )

#define CDSSTRESS_FCDeque( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeL_default ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeL_mutex ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeL_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeL_elimination ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeL_elimination_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeR_default ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeR_mutex ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeR_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeR_elimination ) \
    CDSSTRESS_Stack_F( test_fixture, FCDequeR_elimination_stat )

#define CDSSTRESS_StdStack( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_Deque_Mutex  ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_Deque_Spin   ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_Vector_Mutex ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_Vector_Spin  ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_List_Mutex   ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_List_Spin    )


#endif // #ifndef CDSSTRESS_STACK_TYPES_H

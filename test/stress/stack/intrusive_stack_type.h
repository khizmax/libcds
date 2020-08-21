// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSSTRESS_INTRUSIVE_STACK_TYPES_H
#define CDSSTRESS_INTRUSIVE_STACK_TYPES_H

#include <cds/intrusive/treiber_stack.h>
#include <cds/intrusive/fcstack.h>
#include <cds/intrusive/segmented_stack.h>

#include <cds/gc/hp.h>
#include <cds/gc/dhp.h>

#include <mutex>
#include <cds/sync/spinlock.h>
#include <stack>
#include <list>
#include <vector>
#include <boost/intrusive/list.hpp>

#include <cds_test/stress_test.h>
#include <cds_test/stat_flat_combining_out.h>


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
                if ( !m_Impl.empty()) {
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

    using custom_HP = cds::gc::custom_HP<cds::gc::hp::details::StrangeTLSManager>;

    template <typename T>
    struct Types {

        template <class GC>
        using base_hook = cds::intrusive::treiber_stack::base_hook < cds::opt::gc< GC > >;

    // TreiberStack
        typedef cds::intrusive::TreiberStack< cds::gc::HP, T > Treiber_HP;
        struct traits_Treiber_custom_HP: public
           cds::intrusive::treiber_stack::make_traits <
               cds::intrusive::opt::hook< base_hook<custom_HP> >
           > ::type
        {};
        typedef cds::intrusive::TreiberStack< custom_HP,   T, traits_Treiber_custom_HP > Treiber_custom_HP;
        struct traits_Treiber_DHP: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<cds::gc::DHP> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_DHP > Treiber_DHP;

        template <class GC> struct traits_Treiber_seqcst : public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::memory_model<cds::opt::v::sequential_consistent>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_seqcst<cds::gc::HP>  > Treiber_HP_seqcst;
        typedef cds::intrusive::TreiberStack<custom_HP,     T, traits_Treiber_seqcst<custom_HP>    > Treiber_custom_HP_seqcst;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_seqcst<cds::gc::DHP> > Treiber_DHP_seqcst;

        template <class GC> struct traits_Treiber_stat: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_stat<cds::gc::HP>  > Treiber_HP_stat;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Treiber_stat<custom_HP>    > Treiber_custom_HP_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_stat<cds::gc::DHP> > Treiber_DHP_stat;

        template <class GC> struct traits_Treiber_yield: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::back_off<cds::backoff::yield>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_yield<cds::gc::HP>  > Treiber_HP_yield;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Treiber_yield<custom_HP>    > Treiber_custom_HP_yield;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_yield<cds::gc::DHP> > Treiber_DHP_yield;

        template <class GC> struct traits_Treiber_pause: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::back_off<cds::backoff::pause>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_pause<cds::gc::HP>  > Treiber_HP_pause;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Treiber_pause<custom_HP>    > Treiber_custom_HP_pause;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_pause<cds::gc::DHP> > Treiber_DHP_pause;

        template <class GC> struct traits_Treiber_exp: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                ,cds::opt::back_off<
                    cds::backoff::make_exponential_t< cds::backoff::pause,cds::backoff::yield >
                >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Treiber_exp<cds::gc::HP>  > Treiber_HP_exp;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Treiber_exp<custom_HP>    > Treiber_custom_HP_exp;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Treiber_exp<cds::gc::DHP> > Treiber_DHP_exp;


    // Elimination stack
        template <class GC> struct traits_Elimination_on : public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_on<cds::gc::HP>  > Elimination_HP;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_on<custom_HP>    > Elimination_custom_HP;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_on<cds::gc::DHP> > Elimination_DHP;

        template <class GC> struct traits_Elimination_seqcst : public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::memory_model< cds::opt::v::sequential_consistent >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_seqcst<cds::gc::HP>  > Elimination_HP_seqcst;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_seqcst<custom_HP>    > Elimination_custom_HP_seqcst;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_seqcst<cds::gc::DHP> > Elimination_DHP_seqcst;

        template <class GC> struct traits_Elimination_2ms: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<2> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_2ms<cds::gc::HP>  > Elimination_HP_2ms;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_2ms<custom_HP>    > Elimination_custom_HP_2ms;
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
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_2ms_stat<custom_HP>    > Elimination_custom_HP_2ms_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_2ms_stat<cds::gc::DHP> > Elimination_DHP_2ms_stat;

        template <class GC> struct traits_Elimination_5ms: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<5> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_5ms<cds::gc::HP>  > Elimination_HP_5ms;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_5ms<custom_HP>    > Elimination_custom_HP_5ms;
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
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_5ms_stat<custom_HP>    > Elimination_custom_HP_5ms_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_5ms_stat<cds::gc::DHP> > Elimination_DHP_5ms_stat;

        template <class GC> struct traits_Elimination_10ms: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::elimination_backoff< cds::backoff::delay_of<10> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_10ms<cds::gc::HP>  > Elimination_HP_10ms;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_10ms<custom_HP>    > Elimination_custom_HP_10ms;
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
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_10ms_stat<custom_HP>    > Elimination_custom_HP_10ms_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_10ms_stat<cds::gc::DHP> > Elimination_DHP_10ms_stat;

        template <class GC> struct traits_Elimination_dyn: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer<int> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_dyn<cds::gc::HP>  > Elimination_HP_dyn;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_dyn<custom_HP>    > Elimination_custom_HP_dyn;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_dyn<cds::gc::DHP> > Elimination_DHP_dyn;

        template <class GC> struct traits_Elimination_stat: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_stat<cds::gc::HP>  > Elimination_HP_stat;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_stat<custom_HP>    > Elimination_custom_HP_stat;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_stat<cds::gc::DHP> > Elimination_DHP_stat;

        template <class GC> struct traits_Elimination_dyn_stat: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer<int> >
                , cds::opt::stat<cds::intrusive::treiber_stack::stat<> >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_dyn_stat<cds::gc::HP>  > Elimination_HP_dyn_stat;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_dyn_stat<custom_HP>    > Elimination_custom_HP_dyn_stat;
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
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_yield<custom_HP>    > Elimination_custom_HP_yield;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_yield<cds::gc::DHP> > Elimination_DHP_yield;

        template <class GC> struct traits_Elimination_pause: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                , cds::opt::back_off<cds::backoff::pause>
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_pause<cds::gc::HP>  > Elimination_HP_pause;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_pause<custom_HP>    > Elimination_custom_HP_pause;
        typedef cds::intrusive::TreiberStack< cds::gc::DHP, T, traits_Elimination_pause<cds::gc::DHP> > Elimination_DHP_pause;

        template <class GC> struct traits_Elimination_exp: public
            cds::intrusive::treiber_stack::make_traits <
                cds::intrusive::opt::hook< base_hook<GC> >
                , cds::opt::enable_elimination<true>
                ,cds::opt::back_off<
                    cds::backoff::make_exponential_t< cds::backoff::pause, cds::backoff::yield >
                >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,  T, traits_Elimination_exp<cds::gc::HP>  > Elimination_HP_exp;
        typedef cds::intrusive::TreiberStack< custom_HP,    T, traits_Elimination_exp<custom_HP>    > Elimination_custom_HP_exp;
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

		// SegmentedStack
		class traits_SegmentedStack_spin_stat :
			public cds::intrusive::segmented_stack::make_traits<
			cds::opt::stat< cds::intrusive::segmented_stack::stat<> >
			>::type
		{};
		class traits_SegmentedStack_spin_padding :
			public cds::intrusive::segmented_stack::make_traits<
			cds::opt::padding< cds::opt::cache_line_padding >
			>::type
		{};
		class traits_SegmentedStack_mutex_stat :
			public cds::intrusive::segmented_stack::make_traits<
			cds::opt::stat< cds::intrusive::segmented_stack::stat<> >
			, cds::opt::lock_type< std::mutex >
			>::type
		{};
		class traits_SegmentedStack_mutex :
			public cds::intrusive::segmented_stack::make_traits<
			cds::opt::lock_type< std::mutex >
			>::type
		{};
		class traits_SegmentedStack_mutex_padding :
			public cds::intrusive::segmented_stack::make_traits<
			cds::opt::lock_type< std::mutex >
			, cds::opt::padding< cds::opt::cache_line_padding >
			>::type
		{};

		typedef cds::intrusive::SegmentedStack< cds::gc::HP, T >  SegmentedStack_HP_spin;
		typedef cds::intrusive::SegmentedStack< cds::gc::HP, T, traits_SegmentedStack_spin_padding >  SegmentedStack_HP_spin_padding;
		typedef cds::intrusive::SegmentedStack< cds::gc::HP, T, traits_SegmentedStack_spin_stat >  SegmentedStack_HP_spin_stat;
		typedef cds::intrusive::SegmentedStack< cds::gc::HP, T, traits_SegmentedStack_mutex >  SegmentedStack_HP_mutex;
		typedef cds::intrusive::SegmentedStack< cds::gc::HP, T, traits_SegmentedStack_mutex_padding >  SegmentedStack_HP_mutex_padding;
		typedef cds::intrusive::SegmentedStack< cds::gc::HP, T, traits_SegmentedStack_mutex_stat >  SegmentedStack_HP_mutex_stat;

		typedef cds::intrusive::SegmentedStack< cds::gc::DHP, T >  SegmentedStack_DHP_spin;
		typedef cds::intrusive::SegmentedStack< cds::gc::DHP, T, traits_SegmentedStack_spin_padding >  SegmentedStack_DHP_spin_padding;
		typedef cds::intrusive::SegmentedStack< cds::gc::DHP, T, traits_SegmentedStack_spin_stat >  SegmentedStack_DHP_spin_stat;
		typedef cds::intrusive::SegmentedStack< cds::gc::DHP, T, traits_SegmentedStack_mutex >  SegmentedStack_DHP_mutex;
		typedef cds::intrusive::SegmentedStack< cds::gc::DHP, T, traits_SegmentedStack_mutex_padding >  SegmentedStack_DHP_mutex_padding;
		typedef cds::intrusive::SegmentedStack< cds::gc::DHP, T, traits_SegmentedStack_mutex_stat >  SegmentedStack_DHP_mutex_stat;

    };
} // namespace istack

namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::treiber_stack::empty_stat const& )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::treiber_stack::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_PushCount )
            << CDSSTRESS_STAT_OUT( s, m_PopCount )
            << CDSSTRESS_STAT_OUT( s, m_PushRace )
            << CDSSTRESS_STAT_OUT( s, m_PopRace )
            << CDSSTRESS_STAT_OUT( s, m_ActivePushCollision )
            << CDSSTRESS_STAT_OUT( s, m_PassivePopCollision )
            << CDSSTRESS_STAT_OUT( s, m_ActivePopCollision )
            << CDSSTRESS_STAT_OUT( s, m_PassivePushCollision )
            << CDSSTRESS_STAT_OUT( s, m_EliminationFailed );
    }


    static inline property_stream& operator <<( property_stream& o, cds::intrusive::fcstack::empty_stat const& )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::fcstack::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nPush )
            << CDSSTRESS_STAT_OUT( s, m_nPop )
            << CDSSTRESS_STAT_OUT( s, m_nFailedPop )
            << CDSSTRESS_STAT_OUT( s, m_nCollided )
            << static_cast< cds::algo::flat_combining::stat<> const&>( s );
    }
    

	static inline property_stream& operator <<(property_stream& o, cds::intrusive::segmented_stack::empty_stat const&)
	{
		return o;
	}

	static inline property_stream& operator <<(property_stream& o, cds::intrusive::segmented_stack::stat<> const& s)
	{
		return o
			<< CDSSTRESS_STAT_OUT(s, m_nPush)
			<< CDSSTRESS_STAT_OUT(s, m_nPushPopulated)
			<< CDSSTRESS_STAT_OUT(s, m_nPushContended)
			<< CDSSTRESS_STAT_OUT(s, m_nPop)
			<< CDSSTRESS_STAT_OUT(s, m_nPopEmpty)
			<< CDSSTRESS_STAT_OUT(s, m_nPopContended)
			<< CDSSTRESS_STAT_OUT(s, m_nCreateSegmentReq)
			<< CDSSTRESS_STAT_OUT(s, m_nDeleteSegmentReq)
			<< CDSSTRESS_STAT_OUT(s, m_nSegmentCreated)
			<< CDSSTRESS_STAT_OUT(s, m_nSegmentDeleted);
	}

} // namespace cds_test

#define CDSSTRESS_TreiberStack_HP( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP        ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP_seqcst ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP_pause  ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP_exp    ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_HP_stat   ) \

#define CDSSTRESS_TreiberStack_custom_HP( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_custom_HP        ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_custom_HP_pause  ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_custom_HP_exp    ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_custom_HP_stat    ) \

#define CDSSTRESS_TreiberStack_DHP( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_DHP       ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_DHP_pause ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_DHP_exp   ) \
    CDSSTRESS_Stack_F( test_fixture, Treiber_DHP_stat  )


#define CDSSTRESS_EliminationStack_HP( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_2ms ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_2ms_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_5ms ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_5ms_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_10ms ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_10ms_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_seqcst ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_pause ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_exp ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_dyn ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_HP_dyn_stat ) \

#define CDSSTRESS_EliminationStack_custom_HP( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_2ms ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_2ms_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_5ms ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_5ms_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_10ms ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_10ms_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_pause ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_exp ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_dyn ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_custom_HP_dyn_stat ) \

#define CDSSTRESS_EliminationStack_DHP( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_seqcst ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_2ms ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_2ms_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_5ms ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_5ms_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_10ms ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_10ms_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_pause ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_exp ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_stat ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_dyn ) \
    CDSSTRESS_Stack_F( test_fixture, Elimination_DHP_dyn_stat )

#define CDSSTRESS_FCStack_slist( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_slist ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_slist_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_slist_elimination ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_slist_elimination_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_slist_mutex_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_slist_mutex_elimination ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_slist_mutex_elimination_stat ) \

#define CDSSTRESS_SegmentedStack( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, SegmentedStack_HP_spin         ) \
    CDSSTRESS_Stack_F( test_fixture, SegmentedStack_HP_spin_padding ) \
    CDSSTRESS_Stack_F( test_fixture, SegmentedStack_HP_spin_stat    ) \
    CDSSTRESS_Stack_F( test_fixture, SegmentedStack_HP_mutex        ) \
    CDSSTRESS_Stack_F( test_fixture, SegmentedStack_HP_mutex_stat   ) \
    CDSSTRESS_Stack_F( test_fixture, SegmentedStack_DHP_mutex       ) \
    CDSSTRESS_Stack_F( test_fixture, SegmentedStack_DHP_spin        ) \
    CDSSTRESS_Stack_F( test_fixture, SegmentedStack_DHP_spin_stat   ) \
    CDSSTRESS_Stack_F( test_fixture, SegmentedStack_DHP_mutex_stat  ) \

#define CDSSTRESS_FCStack_list( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_elimination ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_elimination_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_mutex_stat ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_mutex_elimination ) \
    CDSSTRESS_Stack_F( test_fixture, FCStack_list_mutex_elimination_stat )

#define CDSSTRESS_StdStack( test_fixture ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_Deque_Mutex  ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_Deque_Spin   ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_Vector_Mutex ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_Vector_Spin  ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_List_Mutex   ) \
    CDSSTRESS_Stack_F( test_fixture, StdStack_List_Spin    )

#endif // #ifndef CDSSTRESS_INTRUSIVE_STACK_TYPES_H

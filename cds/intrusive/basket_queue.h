// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_BASKET_QUEUE_H
#define CDSLIB_INTRUSIVE_BASKET_QUEUE_H

#include <type_traits>
#include <cds/intrusive/details/single_link_struct.h>
#include <cds/details/marked_ptr.h>

namespace cds { namespace intrusive {

    /// BasketQueue -related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace basket_queue {
        /// BasketQueue node
        /**
            Template parameters:
            Template parameters:
            - GC - garbage collector used
            - Tag - a \ref cds_intrusive_hook_tag "tag"
            */
        template <class GC, typename Tag = opt::none>
        struct node
        {
            typedef GC      gc  ;   ///< Garbage collector
            typedef Tag     tag ;   ///< tag

            typedef cds::details::marked_ptr<node, 1>                    marked_ptr;        ///< marked pointer
            typedef typename gc::template atomic_marked_ptr< marked_ptr> atomic_marked_ptr; ///< atomic marked pointer specific for GC

            /// Rebind node for other template parameters
            template <class GC2, typename Tag2 = tag>
            struct rebind {
                typedef node<GC2, Tag2>  other ;    ///< Rebinding result
            };

            atomic_marked_ptr m_pNext ; ///< pointer to the next node in the container

            node()
            {
                m_pNext.store( marked_ptr(), atomics::memory_order_release );
            }
        };

        using cds::intrusive::single_link::default_hook;

        //@cond
        template < typename HookType, typename... Options>
        struct hook
        {
            typedef typename opt::make_options< default_hook, Options...>::type  options;
            typedef typename options::gc    gc;
            typedef typename options::tag   tag;
            typedef node<gc, tag> node_type;
            typedef HookType     hook_type;
        };
        //@endcond


        /// Base hook
        /**
            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < typename... Options >
        struct base_hook: public hook< opt::base_hook_tag, Options... >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < size_t MemberOffset, typename... Options >
        struct member_hook: public hook< opt::member_hook_tag, Options... >
        {
            //@cond
            static const size_t c_nMemberOffset = MemberOffset;
            //@endcond
        };

        /// Traits hook
        /**
            \p NodeTraits defines type traits for node.
            See \ref node_traits for \p NodeTraits interface description

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename NodeTraits, typename... Options >
        struct traits_hook: public hook< opt::traits_hook_tag, Options... >
        {
            //@cond
            typedef NodeTraits node_traits;
            //@endcond
        };

        /// BasketQueue internal statistics. May be used for debugging or profiling
        /**
            Template argument \p Counter defines type of counter.
            Default is \p cds::atomicity::event_counter, that is weak, i.e. it is not guaranteed
            strict event counting.
            You may use stronger type of counter like as \p cds::atomicity::item_counter,
            or even integral type, for example, \p int.
        */
        template <typename Counter = cds::atomicity::event_counter >
        struct stat
        {
            typedef Counter counter_type;   ///< Counter type

            counter_type m_EnqueueCount;    ///< Enqueue call count
            counter_type m_DequeueCount;    ///< Dequeue call count
            counter_type m_EnqueueRace;     ///< Count of enqueue race conditions encountered
            counter_type m_DequeueRace;     ///< Count of dequeue race conditions encountered
            counter_type m_AdvanceTailError;///< Count of "advance tail failed" events
            counter_type m_BadTail;         ///< Count of events "Tail is not pointed to the last item in the queue"
            counter_type m_TryAddBasket;    ///< Count of attemps adding new item to a basket (only or BasketQueue, for other queue this metric is not used)
            counter_type m_AddBasketCount;  ///< Count of events "Enqueue a new item into basket" (only or BasketQueue, for other queue this metric is not used)
            counter_type m_EmptyDequeue;    ///< Count of dequeue from empty queue

            /// Register enqueue call
            void onEnqueue()                { ++m_EnqueueCount; }
            /// Register dequeue call
            void onDequeue()                { ++m_DequeueCount; }
            /// Register enqueue race event
            void onEnqueueRace()            { ++m_EnqueueRace; }
            /// Register dequeue race event
            void onDequeueRace()            { ++m_DequeueRace; }
            /// Register "advance tail failed" event
            void onAdvanceTailFailed()      { ++m_AdvanceTailError; }
            /// Register event "Tail is not pointed to last item in the queue"
            void onBadTail()                { ++m_BadTail; }
            /// Register an attempt t add new item to basket
            void onTryAddBasket()           { ++m_TryAddBasket; }
            /// Register event "Enqueue a new item into basket" (only or BasketQueue, for other queue this metric is not used)
            void onAddBasket()              { ++m_AddBasketCount; }
            /// Register dequeuing from empty queue
            void onEmptyDequeue()           { ++m_EmptyDequeue; }


            //@cond
            void reset()
            {
                m_EnqueueCount.reset();
                m_DequeueCount.reset();
                m_EnqueueRace.reset();
                m_DequeueRace.reset();
                m_AdvanceTailError.reset();
                m_BadTail.reset();
                m_TryAddBasket.reset();
                m_AddBasketCount.reset();
                m_EmptyDequeue.reset();
            }

            stat& operator +=( stat const& s )
            {
                m_EnqueueCount  += s.m_EnqueueCount.get();
                m_DequeueCount  += s.m_DequeueCount.get();
                m_EnqueueRace   += s.m_EnqueueRace.get();
                m_DequeueRace   += s.m_DequeueRace.get();
                m_AdvanceTailError += s.m_AdvanceTailError.get();
                m_BadTail       += s.m_BadTail.get();
                m_TryAddBasket  += s.m_TryAddBasket.get();
                m_AddBasketCount += s.m_AddBasketCount.get();
                m_EmptyDequeue  += s.m_EmptyDequeue.get();
                return *this;
            }
            //@endcond
        };

        /// Dummy BasketQueue statistics - no counting is performed, no overhead. Support interface like \p basket_queue::stat
        struct empty_stat
        {
            //@cond
            void onEnqueue()            const {}
            void onDequeue()            const {}
            void onEnqueueRace()        const {}
            void onDequeueRace()        const {}
            void onAdvanceTailFailed()  const {}
            void onBadTail()            const {}
            void onTryAddBasket()       const {}
            void onAddBasket()          const {}
            void onEmptyDequeue()       const {}

            void reset() {}
            empty_stat& operator +=( empty_stat const& )
            {
                return *this;
            }
            //@endcond
        };

        /// BasketQueue default type traits
        struct traits
        {
            /// Back-off strategy
            typedef cds::backoff::empty             back_off;

            /// Hook, possible types are \p basket_queue::base_hook, \p basket_queue::member_hook, \p basket_queue::traits_hook
            typedef basket_queue::base_hook<>       hook;

            /// The functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used for dequeuing
            typedef opt::v::empty_disposer          disposer;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter   item_counter;

            /// Internal statistics (by default, disabled)
            /**
                Possible option value are: \p basket_queue::stat, \p basket_queue::empty_stat (the default),
                user-provided class that supports \p %basket_queue::stat interface.
            */
            typedef basket_queue::empty_stat        stat;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// Link checking, see \p cds::opt::link_checker
            static constexpr const opt::link_check_type link_checker = opt::debug_check_link;

            /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
            enum { padding = opt::cache_line_padding };
        };


        /// Metafunction converting option list to \p basket_queue::traits
        /**
            Supported \p Options are:
            - \p opt::hook - hook used. Possible hooks are: \p basket_queue::base_hook, \p basket_queue::member_hook, \p basket_queue::traits_hook.
                If the option is not specified, \p %basket_queue::base_hook<> is used.
            - \p opt::back_off - back-off strategy used, default is \p cds::backoff::empty.
            - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used
                when dequeuing.
            - \p opt::link_checker - the type of node's link fields checking. Default is \p opt::debug_check_link
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            - \p opt::stat - the type to gather internal statistics.
                Possible statistics types are: \p basket_queue::stat, \p basket_queue::empty_stat, user-provided class that supports \p %basket_queue::stat interface.
                Default is \p %basket_queue::empty_stat (internal statistics disabled).
            - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).

            Example: declare \p %BasketQueue with item counting and internal statistics
            \code
            typedef cds::intrusive::BasketQueue< cds::gc::HP, Foo,
                typename cds::intrusive::basket_queue::make_traits<
                    cds::intrusive::opt:hook< cds::intrusive::basket_queue::base_hook< cds::opt::gc<cds:gc::HP> >>,
                    cds::opt::item_counte< cds::atomicity::item_counter >,
                    cds::opt::stat< cds::intrusive::basket_queue::stat<> >
                >::type
            > myQueue;
            \endcode
        */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                , Options...
            >::type type;
#   endif
        };
    }   // namespace basket_queue

    /// Basket lock-free queue (intrusive variant)
    /** @ingroup cds_intrusive_queue
        Implementation of basket queue algorithm.

        \par Source:
            [2007] Moshe Hoffman, Ori Shalev, Nir Shavit "The Baskets Queue"

        <b>Key idea</b>

        In the 'basket' approach, instead of
        the traditional ordered list of nodes, the queue consists of an ordered list of groups
        of nodes (logical baskets). The order of nodes in each basket need not be specified, and in
        fact, it is easiest to maintain them in FIFO order. The baskets fulfill the following basic
        rules:
        - Each basket has a time interval in which all its nodes' enqueue operations overlap.
        - The baskets are ordered by the order of their respective time intervals.
        - For each basket, its nodes' dequeue operations occur after its time interval.
        - The dequeue operations are performed according to the order of baskets.

        Two properties define the FIFO order of nodes:
        - The order of nodes in a basket is not specified.
        - The order of nodes in different baskets is the FIFO-order of their respective baskets.

        In algorithms such as the MS-queue or optimistic
        queue, threads enqueue items by applying a Compare-and-swap (CAS) operation to the
        queue's tail pointer, and all the threads that fail on a particular CAS operation (and also
        the winner of that CAS) overlap in time. In particular, they share the time interval of
        the CAS operation itself. Hence, all the threads that fail to CAS on the tail-node of
        the queue may be inserted into the same basket. By integrating the basket-mechanism
        as the back-off mechanism, the time usually spent on backing-off before trying to link
        onto the new tail, can now be utilized to insert the failed operations into the basket,
        allowing enqueues to complete sooner. In the meantime, the next successful CAS operations
        by enqueues allow new baskets to be formed down the list, and these can be
        filled concurrently. Moreover, the failed operations don't retry their link attempt on the
        new tail, lowering the overall contention on it. This leads to a queue
        algorithm that unlike all former concurrent queue algorithms requires virtually no tuning
        of the backoff mechanisms to reduce contention, making the algorithm an attractive
        out-of-the-box queue.

        In order to enqueue, just as in \p MSQueue, a thread first tries to link the new node to
        the last node. If it failed to do so, then another thread has already succeeded. Thus it
        tries to insert the new node into the new basket that was created by the winner thread.
        To dequeue a node, a thread first reads the head of the queue to obtain the
        oldest basket. It may then dequeue any node in the oldest basket.

        <b>Template arguments:</b>
        - \p GC - garbage collector type: \p gc::HP, \p gc::DHP
        - \p T - type of value to be stored in the queue
        - \p Traits - queue traits, default is \p basket_queue::traits. You can use \p basket_queue::make_traits
            metafunction to make your traits or just derive your traits from \p %basket_queue::traits:
            \code
            struct myTraits: public cds::intrusive::basket_queue::traits {
                typedef cds::intrusive::basket_queue::stat<> stat;
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::intrusive::BasketQueue< cds::gc::HP, Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::intrusive::BasketQueue< cds::gc::HP, Foo,
                typename cds::intrusive::basket_queue::make_traits<
                    cds::opt::stat< cds::intrusive::basket_queue::stat<> >,
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode

        Garbage collecting schema \p GC must be consistent with the \p basket_queue::node GC.

        \par About item disposing
        Like \p MSQueue, the Baskets queue algo has a key feature: even if the queue is empty it contains one item that is "dummy" one from
        the standpoint of the algo. See \p dequeue() function doc for explanation.

        \par Examples
        \code
        #include <cds/intrusive/basket_queue.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::inrtusive;
        typedef cds::gc::HP hp_gc;

        // Basket queue with Hazard Pointer garbage collector, base hook + item disposer:
        struct Foo: public ci::basket_queue::node< hp_gc >
        {
            // Your data
            ...
        };

        // Disposer for Foo struct just deletes the object passed in
        struct fooDisposer {
            void operator()( Foo * p )
            {
                delete p;
            }
        };

        struct fooTraits: public ci::basket_queue::traits {
            typedef ci::basket_queue::base_hook< ci::opt::gc<hp_gc> > hook;
            typedef fooDisposer disposer;
        };
        typedef ci::BasketQueue< hp_gc, Foo, fooTraits > fooQueue;

        // BasketQueue with Hazard Pointer garbage collector,
        // member hook + item disposer + item counter,
        // without padding of internal queue data:
        struct Bar
        {
            // Your data
            ...
            ci::basket_queue::node< hp_gc > hMember;
        };

        struct barTraits: public
            ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(Bar, hMember)
                        ,ci::opt::gc<hp_gc>
                    >
                >
                ,ci::opt::disposer< fooDisposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::padding< cds::opt::no_special_padding >
            >::type
        {};
        typedef ci::BasketQueue< hp_gc, Bar, barTraits > barQueue;
        \endcode
    */
    template <typename GC, typename T, typename Traits = basket_queue::traits >
    class BasketQueue
    {
    public:
        typedef GC gc;          ///< Garbage collector
        typedef T  value_type;  ///< type of value stored in the queue
        typedef Traits traits;  ///< Queue traits
        typedef typename traits::hook       hook;       ///< hook type
        typedef typename hook::node_type    node_type;  ///< node type
        typedef typename traits::disposer   disposer;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits;   ///< node traits
        typedef typename single_link::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker

        typedef typename traits::back_off       back_off;     ///< back-off strategy
        typedef typename traits::item_counter   item_counter; ///< Item counting policy used
        typedef typename traits::stat           stat;         ///< Internal statistics policy used
        typedef typename traits::memory_model   memory_model; ///< Memory ordering. See cds::opt::memory_model option

        /// Rebind template arguments
        template <typename GC2, typename T2, typename Traits2>
        struct rebind {
            typedef BasketQueue< GC2, T2, Traits2> other   ;   ///< Rebinding result
        };

        static constexpr const size_t c_nHazardPtrCount = 6 ; ///< Count of hazard pointer required for the algorithm

    protected:
        //@cond
        typedef typename node_type::marked_ptr   marked_ptr;
        typedef typename node_type::atomic_marked_ptr atomic_marked_ptr;

        // GC and node_type::gc must be the same
        static_assert( std::is_same<gc, typename node_type::gc>::value, "GC and node_type::gc must be the same");
        //@endcond

        atomic_marked_ptr    m_pHead ;           ///< Queue's head pointer (aligned)
        //@cond
        typename opt::details::apply_padding< atomic_marked_ptr, traits::padding >::padding_type pad1_;
        //@endcond
        atomic_marked_ptr    m_pTail ;           ///< Queue's tail pointer (aligned)
        //@cond
        typename opt::details::apply_padding< atomic_marked_ptr, traits::padding >::padding_type pad2_;
        //@endcond
        node_type           m_Dummy ;           ///< dummy node
        //@cond
        typename opt::details::apply_padding< node_type, traits::padding >::padding_type pad3_;
        //@endcond
        item_counter        m_ItemCounter   ;   ///< Item counter
        stat                m_Stat  ;           ///< Internal statistics
        //@cond
        size_t const        m_nMaxHops;
        //@endcond

        //@cond

        struct dequeue_result {
            typename gc::template GuardArray<3>  guards;
            node_type * pNext;
        };

        bool do_dequeue( dequeue_result& res, bool bDeque )
        {
            // Note:
            // If bDeque == false then the function is called from empty method and no real dequeuing operation is performed

            back_off bkoff;

            marked_ptr h;
            marked_ptr t;
            marked_ptr pNext;

            while ( true ) {
                h = res.guards.protect( 0, m_pHead, []( marked_ptr p ) -> value_type * { return node_traits::to_value_ptr( p.ptr());});
                t = res.guards.protect( 1, m_pTail, []( marked_ptr p ) -> value_type * { return node_traits::to_value_ptr( p.ptr());});
                pNext = res.guards.protect( 2, h->m_pNext, []( marked_ptr p ) -> value_type * { return node_traits::to_value_ptr( p.ptr());});

                if ( h == m_pHead.load( memory_model::memory_order_acquire )) {
                    if ( h.ptr() == t.ptr()) {
                        if ( !pNext.ptr()) {
                            m_Stat.onEmptyDequeue();
                            return false;
                        }

                        {
                            typename gc::Guard g;
                            while ( pNext->m_pNext.load(memory_model::memory_order_relaxed).ptr() && m_pTail.load(memory_model::memory_order_relaxed) == t ) {
                                pNext = g.protect( pNext->m_pNext, []( marked_ptr p ) -> value_type * { return node_traits::to_value_ptr( p.ptr());});
                                res.guards.copy( 2, g );
                            }
                        }

                        m_pTail.compare_exchange_weak( t, marked_ptr(pNext.ptr()), memory_model::memory_order_acquire, atomics::memory_order_relaxed );
                    }
                    else {
                        marked_ptr iter( h );
                        size_t hops = 0;

                        typename gc::Guard g;

                        while ( pNext.ptr() && pNext.bits() && iter.ptr() != t.ptr() && m_pHead.load(memory_model::memory_order_relaxed) == h ) {
                            iter = pNext;
                            g.assign( res.guards.template get<value_type>(2));
                            pNext = res.guards.protect( 2, pNext->m_pNext, []( marked_ptr p ) -> value_type * { return node_traits::to_value_ptr( p.ptr());});
                            ++hops;
                        }

                        if ( m_pHead.load(memory_model::memory_order_relaxed) != h )
                            continue;

                        if ( iter.ptr() == t.ptr())
                            free_chain( h, iter );
                        else if ( bDeque ) {
                            res.pNext = pNext.ptr();

                            if ( iter->m_pNext.compare_exchange_weak( pNext, marked_ptr( pNext.ptr(), 1 ), memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                                if ( hops >= m_nMaxHops )
                                    free_chain( h, pNext );
                                break;
                            }
                        }
                        else
                            return true;
                    }
                }

                if ( bDeque )
                    m_Stat.onDequeueRace();
                bkoff();
            }

            if ( bDeque ) {
                --m_ItemCounter;
                m_Stat.onDequeue();
            }

            return true;
        }

        void free_chain( marked_ptr head, marked_ptr newHead )
        {
            // "head" and "newHead" are guarded

            if ( m_pHead.compare_exchange_strong( head, marked_ptr(newHead.ptr()), memory_model::memory_order_release, atomics::memory_order_relaxed ))
            {
                typename gc::template GuardArray<2> guards;
                guards.assign( 0, node_traits::to_value_ptr(head.ptr()));
                while ( head.ptr() != newHead.ptr()) {
                    marked_ptr pNext = guards.protect( 1, head->m_pNext, []( marked_ptr p ) -> value_type * { return node_traits::to_value_ptr( p.ptr());});
                    assert( pNext.bits() != 0 );
                    dispose_node( head.ptr());
                    guards.copy( 0, 1 );
                    head = pNext;
                }
            }
        }

        static void clear_links( node_type * pNode )
        {
            pNode->m_pNext.store( marked_ptr( nullptr ), memory_model::memory_order_release );
        }

        void dispose_node( node_type * p )
        {
            if ( p != &m_Dummy ) {
                struct internal_disposer
                {
                    void operator()( value_type * p )
                    {
                        assert( p != nullptr );
                        BasketQueue::clear_links( node_traits::to_node_ptr( p ));
                        disposer()(p);
                    }
                };
                gc::template retire<internal_disposer>( node_traits::to_value_ptr(p));
            }
        }
        //@endcond

    public:
        /// Initializes empty queue
        BasketQueue()
            : m_pHead( &m_Dummy )
            , m_pTail( &m_Dummy )
            , m_nMaxHops( 3 )
        {}

        /// Destructor clears the queue
        /**
            Since the baskets queue contains at least one item even
            if the queue is empty, the destructor may call item disposer.
        */
        ~BasketQueue()
        {
            clear();

            node_type * pHead = m_pHead.load(memory_model::memory_order_relaxed).ptr();
            assert( pHead != nullptr );

            {
                node_type * pNext = pHead->m_pNext.load( memory_model::memory_order_relaxed ).ptr();
                while ( pNext ) {
                    node_type * p = pNext;
                    pNext = pNext->m_pNext.load( memory_model::memory_order_relaxed ).ptr();
                    p->m_pNext.store( marked_ptr(), memory_model::memory_order_relaxed );
                    dispose_node( p );
                }
                pHead->m_pNext.store( marked_ptr(), memory_model::memory_order_relaxed );
                //m_pTail.store( marked_ptr( pHead ), memory_model::memory_order_relaxed );
            }

            m_pHead.store( marked_ptr( nullptr ), memory_model::memory_order_relaxed );
            m_pTail.store( marked_ptr( nullptr ), memory_model::memory_order_relaxed );

            dispose_node( pHead );
        }

        /// Enqueues \p val value into the queue.
        /** @anchor cds_intrusive_BasketQueue_enqueue
            The function always returns \p true.
        */
        bool enqueue( value_type& val )
        {
            node_type * pNew = node_traits::to_node_ptr( val );
            link_checker::is_empty( pNew );

            typename gc::Guard guard;
            typename gc::Guard gNext;
            back_off bkoff;

            marked_ptr t;
            while ( true ) {
                t = guard.protect( m_pTail, []( marked_ptr p ) -> value_type * { return node_traits::to_value_ptr( p.ptr());});

                marked_ptr pNext = t->m_pNext.load(memory_model::memory_order_relaxed );

                if ( pNext.ptr() == nullptr ) {
                    pNew->m_pNext.store( marked_ptr(), memory_model::memory_order_relaxed );
                    if ( t->m_pNext.compare_exchange_weak( pNext, marked_ptr(pNew), memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                        if ( !m_pTail.compare_exchange_strong( t, marked_ptr(pNew), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                            m_Stat.onAdvanceTailFailed();
                        break;
                    }

                    // Try adding to basket
                    m_Stat.onTryAddBasket();

                    // Reread tail next
                try_again:
                    pNext = gNext.protect( t->m_pNext, []( marked_ptr p ) -> value_type * { return node_traits::to_value_ptr( p.ptr());});

                    // add to the basket
                    if ( m_pTail.load( memory_model::memory_order_relaxed ) == t
                         && t->m_pNext.load( memory_model::memory_order_relaxed) == pNext
                         && !pNext.bits())
                    {
                        bkoff();
                        pNew->m_pNext.store( pNext, memory_model::memory_order_relaxed );
                        if ( t->m_pNext.compare_exchange_weak( pNext, marked_ptr( pNew ), memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                            m_Stat.onAddBasket();
                            break;
                        }
                        goto try_again;
                    }
                }
                else {
                    // Tail is misplaced, advance it

                    typename gc::template GuardArray<2> g;
                    g.assign( 0, node_traits::to_value_ptr( pNext.ptr()));
                    if ( m_pTail.load( memory_model::memory_order_acquire ) != t
                      || t->m_pNext.load( memory_model::memory_order_relaxed ) != pNext )
                    {
                        m_Stat.onEnqueueRace();
                        bkoff();
                        continue;
                    }

                    marked_ptr p;
                    bool bTailOk = true;
                    while ( (p = pNext->m_pNext.load( memory_model::memory_order_acquire )).ptr() != nullptr )
                    {
                        bTailOk = m_pTail.load( memory_model::memory_order_relaxed ) == t;
                        if ( !bTailOk )
                            break;

                        g.assign( 1, node_traits::to_value_ptr( p.ptr()));
                        if ( pNext->m_pNext.load( memory_model::memory_order_relaxed ) != p )
                            continue;
                        pNext = p;
                        g.assign( 0, g.template get<value_type>( 1 ));
                    }
                    if ( !bTailOk || !m_pTail.compare_exchange_weak( t, marked_ptr( pNext.ptr()), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                        m_Stat.onAdvanceTailFailed();

                    m_Stat.onBadTail();
                }

                m_Stat.onEnqueueRace();
            }

            ++m_ItemCounter;
            m_Stat.onEnqueue();

            return true;
        }

        /// Synonym for \p enqueue() function
        bool push( value_type& val )
        {
            return enqueue( val );
        }

        /// Dequeues a value from the queue
        /** @anchor cds_intrusive_BasketQueue_dequeue
            If the queue is empty the function returns \p nullptr.

            @note See \p MSQueue::dequeue() note about item disposing
        */
        value_type * dequeue()
        {
            dequeue_result res;

            if ( do_dequeue( res, true ))
                return node_traits::to_value_ptr( *res.pNext );
            return nullptr;
        }

        /// Synonym for \p dequeue() function
        value_type * pop()
        {
            return dequeue();
        }

        /// Checks if the queue is empty
        /**
            Note that this function is not \p const.
            The function is based on \p dequeue() algorithm
            but really it does not dequeue any item.
        */
        bool empty()
        {
            dequeue_result res;
            return !do_dequeue( res, false );
        }

        /// Clear the queue
        /**
            The function repeatedly calls \p dequeue() until it returns \p nullptr.
            The disposer defined in template \p Traits is called for each item
            that can be safely disposed.
        */
        void clear()
        {
            while ( dequeue());
        }

        /// Returns queue's item count
        /**
            The value returned depends on \p Traits (see basket_queue::traits::item_counter). For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the queue
            is empty. To check queue emptyness use \p empty() method.
        */
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns reference to internal statistics
        const stat& statistics() const
        {
            return m_Stat;
        }
    };

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_BASKET_QUEUE_H

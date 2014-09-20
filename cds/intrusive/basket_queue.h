//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_BASKET_QUEUE_H
#define __CDS_INTRUSIVE_BASKET_QUEUE_H

#include <type_traits>
#include <cds/intrusive/base.h>
#include <cds/details/marked_ptr.h>
#include <cds/intrusive/queue_stat.h>
#include <cds/intrusive/single_link_struct.h>
#include <cds/ref.h>
#include <cds/intrusive/details/dummy_node_holder.h>

namespace cds { namespace intrusive {

    /// BasketQueue -related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace basket_queue {
        /// BasketQueue node
        /**
            Template parameters:
            - GC - garbage collector used
            - Tag - a tag used to distinguish between different implementation
        */
        template <class GC, typename Tag = opt::none>
        struct node: public GC::container_node
        {
            typedef GC      gc  ;   ///< Garbage collector
            typedef Tag     tag ;   ///< tag

            typedef cds::details::marked_ptr<node, 1>   marked_ptr         ;   ///< marked pointer
            typedef typename gc::template atomic_marked_ptr< marked_ptr>     atomic_marked_ptr   ;   ///< atomic marked pointer specific for GC

            /// Rebind node for other template parameters
            template <class GC2, typename Tag2 = tag>
            struct rebind {
                typedef node<GC2, Tag2>  other ;    ///< Rebinding result
            };

            atomic_marked_ptr m_pNext ; ///< pointer to the next node in the container

            node()
                : m_pNext( nullptr )
            {}
        };

        //@cond
        // Specialization for HRC GC
        template <typename Tag>
        struct node< gc::HRC, Tag>: public gc::HRC::container_node
        {
            typedef gc::HRC gc  ;   ///< Garbage collector
            typedef Tag     tag ;   ///< tag

            typedef cds::details::marked_ptr<node, 1>   marked_ptr         ;   ///< marked pointer
            typedef typename gc::template atomic_marked_ptr< marked_ptr>     atomic_marked_ptr   ;   ///< atomic marked pointer specific for GC

            atomic_marked_ptr m_pNext ; ///< pointer to the next node in the container

            node()
                : m_pNext( nullptr )
            {}

        protected:
            virtual void cleanUp( cds::gc::hrc::ThreadGC * pGC )
            {
                assert( pGC != nullptr );
                typename gc::template GuardArray<2> aGuards( *pGC );

                while ( true ) {
                    marked_ptr pNext = aGuards.protect( 0, m_pNext );
                    if ( pNext.ptr() && pNext->m_bDeleted.load(CDS_ATOMIC::memory_order_acquire) ) {
                        marked_ptr p = aGuards.protect( 1, pNext->m_pNext );
                        m_pNext.compare_exchange_strong( pNext, p, CDS_ATOMIC::memory_order_acquire, CDS_ATOMIC::memory_order_relaxed );
                        continue;
                    }
                    else {
                        break;
                    }
                }
            }

            virtual void terminate( cds::gc::hrc::ThreadGC * pGC, bool bConcurrent )
            {
                if ( bConcurrent ) {
                    marked_ptr pNext = m_pNext.load(CDS_ATOMIC::memory_order_relaxed);
                    do {} while ( !m_pNext.compare_exchange_weak( pNext, marked_ptr(), CDS_ATOMIC::memory_order_release, CDS_ATOMIC::memory_order_relaxed ) );
                }
                else {
                    m_pNext.store( marked_ptr(), CDS_ATOMIC::memory_order_relaxed );
                }
            }
        };
        //@endcond

        using single_link::default_hook;

        //@cond
        template < typename HookType, CDS_DECL_OPTIONS2>
        struct hook
        {
            typedef typename opt::make_options< default_hook, CDS_OPTIONS2>::type  options;
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
            - opt::tag - tag
        */
        template < CDS_DECL_OPTIONS2 >
        struct base_hook: public hook< opt::base_hook_tag, CDS_OPTIONS2 >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - tag
        */
        template < size_t MemberOffset, CDS_DECL_OPTIONS2 >
        struct member_hook: public hook< opt::member_hook_tag, CDS_OPTIONS2 >
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
            - opt::tag - tag
        */
        template <typename NodeTraits, CDS_DECL_OPTIONS2 >
        struct traits_hook: public hook< opt::traits_hook_tag, CDS_OPTIONS2 >
        {
            //@cond
            typedef NodeTraits node_traits;
            //@endcond
        };


#if defined(CDS_CXX11_TEMPLATE_ALIAS_SUPPORT) && !defined(CDS_DOXYGEN_INVOKED)
        template < typename Node, opt::link_check_type LinkType > using get_link_checker = single_link::get_link_checker< Node, LinkType >;
#else
        /// Metafunction for selecting appropriate link checking policy
        template < typename Node, opt::link_check_type LinkType >
        struct get_link_checker: public single_link::get_link_checker< Node, LinkType >
        {};

#endif

        /// Basket queue internal statistics. May be used for debugging or profiling
        /**
            Basket queue statistics derives from cds::intrusive::queue_stat
            and extends it by two additional fields specific for the algorithm.
        */
        template <typename Counter = cds::atomicity::event_counter >
        struct stat: public cds::intrusive::queue_stat< Counter >
        {
            //@cond
            typedef cds::intrusive::queue_stat< Counter >   base_class;
            typedef typename base_class::counter_type       counter_type;
            //@endcond

            counter_type m_TryAddBasket      ;  ///< Count of attemps adding new item to a basket (only or BasketQueue, for other queue this metric is not used)
            counter_type m_AddBasketCount    ;  ///< Count of events "Enqueue a new item into basket" (only or BasketQueue, for other queue this metric is not used)

            /// Register an attempt t add new item to basket
            void onTryAddBasket()           { ++m_TryAddBasket; }
            /// Register event "Enqueue a new item into basket" (only or BasketQueue, for other queue this metric is not used)
            void onAddBasket()              { ++m_AddBasketCount; }

            //@cond
            void reset()
            {
                base_class::reset();
                m_TryAddBasket.reset();
                m_AddBasketCount.reset();
            }

            stat& operator +=( stat const& s )
            {
                base_class::operator +=( s );
                m_TryAddBasket += s.m_TryAddBasket.get();
                m_AddBasketCount += s.m_AddBasketCount.get();
                return *this;
            }
            //@endcond
        };

        /// Dummy basket queue statistics - no counting is performed. Support interface like \ref stat
        struct dummy_stat: public cds::intrusive::queue_dummy_stat
        {
            //@cond
            void onTryAddBasket()           {}
            void onAddBasket()              {}

            void reset() {}
            dummy_stat& operator +=( dummy_stat const& )
            {
                return *this;
            }
            //@endcond
        };

    }   // namespace basket_queue

    /// Basket lock-free queue (intrusive variant)
    /** @ingroup cds_intrusive_queue
        Implementation of basket queue algorithm.

        \par Source:
            [2007] Moshe Hoffman, Ori Shalev, Nir Shavit "The Baskets Queue"

        <b>Key idea</b>

        In the “basket” approach, instead of
        the traditional ordered list of nodes, the queue consists of an ordered list of groups
        of nodes (logical baskets). The order of nodes in each basket need not be specified, and in
        fact, it is easiest to maintain them in FIFO order. The baskets fulfill the following basic
        rules:
        - Each basket has a time interval in which all its nodes’ enqueue operations overlap.
        - The baskets are ordered by the order of their respective time intervals.
        - For each basket, its nodes’ dequeue operations occur after its time interval.
        - The dequeue operations are performed according to the order of baskets.

        Two properties define the FIFO order of nodes:
        - The order of nodes in a basket is not specified.
        - The order of nodes in different baskets is the FIFO-order of their respective baskets.

        In algorithms such as the MS-queue or optimistic
        queue, threads enqueue items by applying a Compare-and-swap (CAS) operation to the
        queue’s tail pointer, and all the threads that fail on a particular CAS operation (and also
        the winner of that CAS) overlap in time. In particular, they share the time interval of
        the CAS operation itself. Hence, all the threads that fail to CAS on the tail-node of
        the queue may be inserted into the same basket. By integrating the basket-mechanism
        as the back-off mechanism, the time usually spent on backing-off before trying to link
        onto the new tail, can now be utilized to insert the failed operations into the basket,
        allowing enqueues to complete sooner. In the meantime, the next successful CAS operations
        by enqueues allow new baskets to be formed down the list, and these can be
        filled concurrently. Moreover, the failed operations don’t retry their link attempt on the
        new tail, lowering the overall contention on it. This leads to a queue
        algorithm that unlike all former concurrent queue algorithms requires virtually no tuning
        of the backoff mechanisms to reduce contention, making the algorithm an attractive
        out-of-the-box queue.

        In order to enqueue, just as in MSQueue, a thread first tries to link the new node to
        the last node. If it failed to do so, then another thread has already succeeded. Thus it
        tries to insert the new node into the new basket that was created by the winner thread.
        To dequeue a node, a thread first reads the head of the queue to obtain the
        oldest basket. It may then dequeue any node in the oldest basket.

        <b>Template arguments:</b>
        - \p GC - garbage collector type: gc::HP, gc::HRC, gc::PTB
        - \p T - type to be stored in the queue, should be convertible to \ref single_link::node
        - \p Options - options

        <b>Type of node</b>: \ref single_link::node

        \p Options are:
        - opt::hook - hook used. Possible values are: basket_queue::base_hook, basket_queue::member_hook, basket_queue::traits_hook.
            If the option is not specified, <tt>basket_queue::base_hook<></tt> is used.
            For Gidenstam's gc::HRC, only basket_queue::base_hook is supported.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used.
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer. This option is used
            in \ref dequeue function.
        - opt::link_checker - the type of node's link fields checking. Default is \ref opt::debug_check_link
            Note: for gc::HRC garbage collector, link checking policy is always selected as \ref opt::always_check_link.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter (no item counting feature)
        - opt::stat - the type to gather internal statistics.
            Possible option value are: \ref basket_queue::stat, \ref basket_queue::dummy_stat,
            user-provided class that supports basket_queue::stat interface.
            Default is \ref basket_queue::dummy_stat.
            Generic option intrusive::queue_stat and intrusive::queue_dummy_stat are acceptable too, however,
            they will be automatically converted to basket_queue::stat and basket_queue::dummy_stat
            respectively.
        - opt::alignment - the alignment for internal queue data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).

        Garbage collecting schema \p GC must be consistent with the basket_queue::node GC.

        \par About item disposing
        Like MSQueue, the Baskets queue algo has a key feature: even if the queue is empty it contains one item that is "dummy" one from
        the standpoint of the algo. See \ref dequeue function doc for explanation.

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

        typedef ci::BasketQueue< hp_gc,
            Foo
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<hp_gc> >
            >
            ,ci::opt::disposer< fooDisposer >
        > fooQueue;

        // BasketQueue with Hazard Pointer garbage collector,
        // member hook + item disposer + item counter,
        // without alignment of internal queue data:
        struct Bar
        {
            // Your data
            ...
            ci::basket_queue::node< hp_gc > hMember;
        };

        typedef ci::BasketQueue< hp_gc,
            Foo
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(Bar, hMember)
                    ,ci::opt::gc<hp_gc>
                >
            >
            ,ci::opt::disposer< fooDisposer >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
            ,cds::opt::alignment< cds::opt::no_special_alignment >
        > barQueue;
        \endcode
    */
    template <typename GC, typename T, CDS_DECL_OPTIONS9>
    class BasketQueue
    {
        //@cond
        struct default_options
        {
            typedef cds::backoff::empty             back_off;
            typedef basket_queue::base_hook<>       hook;
            typedef opt::v::empty_disposer          disposer;
            typedef atomicity::empty_item_counter   item_counter;
            typedef basket_queue::dummy_stat        stat;
            typedef opt::v::relaxed_ordering        memory_model;
            static const opt::link_check_type link_checker = opt::debug_check_link;
            enum { alignment = opt::cache_line_alignment };
        };
        //@endcond

    public:
        //@cond
        typedef typename opt::make_options<
            typename cds::opt::find_type_traits< default_options, CDS_OPTIONS9 >::type
            ,CDS_OPTIONS9
        >::type   options;

        typedef typename std::conditional<
            std::is_same<typename options::stat, cds::intrusive::queue_stat<> >::value
            ,basket_queue::stat<>
            ,typename std::conditional<
                std::is_same<typename options::stat, cds::intrusive::queue_dummy_stat>::value
                ,basket_queue::dummy_stat
                ,typename options::stat
            >::type
        >::type stat_type_;

        //@endcond

    public:
        typedef T  value_type   ;   ///< type of value stored in the queue
        typedef typename options::hook      hook        ;   ///< hook type
        typedef typename hook::node_type    node_type   ;   ///< node type
        typedef typename options::disposer  disposer    ;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef typename basket_queue::get_link_checker< node_type, options::link_checker >::type link_checker   ;   ///< link checker

        typedef GC gc          ;   ///< Garbage collector
        typedef typename options::back_off  back_off    ;   ///< back-off strategy
        typedef typename options::item_counter item_counter ;   ///< Item counting policy used
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename options::stat      stat        ;   ///< Internal statistics policy used
#else
        typedef stat_type_  stat;
#endif
        typedef typename options::memory_model  memory_model ;   ///< Memory ordering. See cds::opt::memory_model option

        /// Rebind template arguments
        template <typename GC2, typename T2, CDS_DECL_OTHER_OPTIONS9>
        struct rebind {
            typedef BasketQueue< GC2, T2, CDS_OTHER_OPTIONS9> other   ;   ///< Rebinding result
        };

        static const size_t m_nHazardPtrCount = 6 ; ///< Count of hazard pointer required for the algorithm

    protected:
        //@cond

        struct internal_disposer
        {
            void operator()( value_type * p )
            {
                assert( p != nullptr );

                BasketQueue::clear_links( node_traits::to_node_ptr(p) );
                disposer()( p );
            }
        };

        typedef typename node_type::marked_ptr   marked_ptr;
        typedef typename node_type::atomic_marked_ptr atomic_marked_ptr;

        typedef intrusive::node_to_value<BasketQueue> node_to_value;
        typedef typename opt::details::alignment_setter< atomic_marked_ptr, options::alignment >::type aligned_node_ptr;
        typedef typename opt::details::alignment_setter<
            cds::intrusive::details::dummy_node< gc, node_type>,
            options::alignment
        >::type    dummy_node_type;

        //@endcond

        aligned_node_ptr    m_pHead ;           ///< Queue's head pointer (aligned)
        aligned_node_ptr    m_pTail ;           ///< Queue's tail pointer (aligned)

        dummy_node_type     m_Dummy ;           ///< dummy node
        item_counter        m_ItemCounter   ;   ///< Item counter
        stat                m_Stat  ;           ///< Internal statistics
        //@cond
        size_t const        m_nMaxHops;
        //@endcond

        //@cond

        template <size_t Count>
        static marked_ptr guard_node( typename gc::template GuardArray<Count>& g, size_t idx, atomic_marked_ptr const& p )
        {
            marked_ptr pg;
            while ( true ) {
                pg = p.load( memory_model::memory_order_relaxed );
                g.assign( idx, node_traits::to_value_ptr( pg.ptr() ) );
                if ( p.load( memory_model::memory_order_acquire) == pg ) {
                    return pg;
                }
            }
        }

        static marked_ptr guard_node( typename gc::Guard& g, atomic_marked_ptr const& p )
        {
            marked_ptr pg;
            while ( true ) {
                pg = p.load( memory_model::memory_order_relaxed );
                g.assign( node_traits::to_value_ptr( pg.ptr() ) );
                if ( p.load( memory_model::memory_order_acquire) == pg ) {
                    return pg;
                }
            }
        }

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
                h = guard_node( res.guards, 0, m_pHead );
                t = guard_node( res.guards, 1, m_pTail );
                pNext = guard_node( res.guards, 2, h->m_pNext );

                if ( h == m_pHead.load( memory_model::memory_order_acquire ) ) {
                    if ( h.ptr() == t.ptr() ) {
                        if ( !pNext.ptr() )
                            return false;

                        {
                            typename gc::Guard g;
                            while ( pNext->m_pNext.load(memory_model::memory_order_relaxed).ptr() && m_pTail.load(memory_model::memory_order_relaxed) == t ) {
                                pNext = guard_node( g, pNext->m_pNext );
                                res.guards.assign( 2, g.template get<value_type>() );
                            }
                        }

                        m_pTail.compare_exchange_weak( t, marked_ptr(pNext.ptr()), memory_model::memory_order_release, memory_model::memory_order_relaxed );
                    }
                    else {
                        marked_ptr iter( h );
                        size_t hops = 0;

                        typename gc::Guard g;

                        while ( pNext.ptr() && pNext.bits() && iter.ptr() != t.ptr() && m_pHead.load(memory_model::memory_order_relaxed) == h ) {
                            iter = pNext;
                            g.assign( res.guards.template get<value_type>(2) );
                            pNext = guard_node( res.guards, 2, pNext->m_pNext );
                            ++hops;
                        }

                        if ( m_pHead.load(memory_model::memory_order_relaxed) != h )
                            continue;

                        if ( iter.ptr() == t.ptr() )
                            free_chain( h, iter );
                        else if ( bDeque ) {
                            res.pNext = pNext.ptr();

                            if ( iter->m_pNext.compare_exchange_weak( pNext, marked_ptr( pNext.ptr(), 1 ), memory_model::memory_order_release, memory_model::memory_order_relaxed ) ) {
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

            if ( m_pHead.compare_exchange_strong( head, marked_ptr(newHead.ptr()), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
            {
                typename gc::template GuardArray<2> guards;
                guards.assign( 0, node_traits::to_value_ptr(head.ptr()) );
                while ( head.ptr() != newHead.ptr() ) {
                    marked_ptr pNext = guard_node( guards, 1, head->m_pNext );
                    assert( pNext.bits() != 0 );
                    dispose_node( head.ptr() );
                    guards.assign( 0, guards.template get<value_type>(1) );
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
            if ( p != m_Dummy.get() ) {
                gc::template retire<internal_disposer>( node_traits::to_value_ptr(p) );
            }
            else
                m_Dummy.retire();
        }
        //@endcond

    public:
        /// Initializes empty queue
        BasketQueue()
            : m_pHead( nullptr )
            , m_pTail( nullptr )
            , m_nMaxHops( 3 )
        {
            // GC and node_type::gc must be the same
            static_assert(( std::is_same<gc, typename node_type::gc>::value ), "GC and node_type::gc must be the same");

            // For cds::gc::HRC, only one base_hook is allowed
            static_assert((
                std::conditional<
                    std::is_same<gc, cds::gc::HRC>::value,
                    std::is_same< typename hook::hook_type, opt::base_hook_tag >,
                    boost::true_type
                >::type::value
            ), "For cds::gc::HRC, only base_hook is allowed");

            // Head/tail initialization should be made via store call
            // because of gc::HRC manages reference counting
            m_pHead.store( marked_ptr(m_Dummy.get()), memory_model::memory_order_relaxed );
            m_pTail.store( marked_ptr(m_Dummy.get()), memory_model::memory_order_relaxed );
        }

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

        /// Returns queue's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact is not mean that the queue
            is empty. To check queue emptyness use \ref empty() method.
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

        /// Enqueues \p val value into the queue.
        /** @anchor cds_intrusive_BasketQueue_enqueue
            The function always returns \p true.
        */
        bool enqueue( value_type& val )
        {
            node_type * pNew = node_traits::to_node_ptr( val );
            link_checker::is_empty( pNew );

            typename gc::Guard guard;
            back_off bkoff;

            marked_ptr t;
            while ( true ) {
                t = guard_node( guard, m_pTail );

                marked_ptr pNext = t->m_pNext.load(memory_model::memory_order_acquire );

                if ( pNext.ptr() == nullptr ) {
                    pNew->m_pNext.store( marked_ptr(), memory_model::memory_order_release );
                    if ( t->m_pNext.compare_exchange_weak( pNext, marked_ptr(pNew), memory_model::memory_order_release, memory_model::memory_order_relaxed ) ) {
                        if ( !m_pTail.compare_exchange_strong( t, marked_ptr(pNew), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                            m_Stat.onAdvanceTailFailed();
                        break;
                    }

                    // Try adding to basket
                    m_Stat.onTryAddBasket();

                    // Reread tail next
                    typename gc::Guard gNext;

                try_again:
                    pNext = guard_node( gNext, t->m_pNext );

                    // add to the basket
                    if ( m_pTail.load(memory_model::memory_order_relaxed) == t
                         && t->m_pNext.load( memory_model::memory_order_relaxed) == pNext
                         && !pNext.bits()  )
                    {
                        bkoff();
                        pNew->m_pNext.store( pNext, memory_model::memory_order_release );
                        if ( t->m_pNext.compare_exchange_weak( pNext, marked_ptr( pNew ), memory_model::memory_order_release, memory_model::memory_order_relaxed )) {
                            m_Stat.onAddBasket();
                            break;
                        }
                        goto try_again;
                    }
                }
                else {
                    // Tail is misplaced, advance it

                    typename gc::template GuardArray<2> g;
                    g.assign( 0, node_traits::to_value_ptr( pNext.ptr() ) );
                    if ( t->m_pNext.load( memory_model::memory_order_relaxed ) != pNext ) {
                        m_Stat.onEnqueueRace();
                        bkoff();
                        continue;
                    }

                    marked_ptr p;
                    bool bTailOk = true;
                    while ( (p = pNext->m_pNext.load( memory_model::memory_order_relaxed )).ptr() != nullptr )
                    {
                        bTailOk = m_pTail.load( memory_model::memory_order_relaxed ) == t;
                        if ( !bTailOk )
                            break;

                        g.assign( 1, node_traits::to_value_ptr( p.ptr() ));
                        if ( pNext->m_pNext.load(memory_model::memory_order_relaxed) != p )
                            continue;
                        pNext = p;
                        g.assign( 0, g.template get<value_type>( 1 ) );
                    }
                    if ( !bTailOk || !m_pTail.compare_exchange_weak( t, marked_ptr( pNext.ptr() ), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                        m_Stat.onAdvanceTailFailed();

                    m_Stat.onBadTail();
                }

                m_Stat.onEnqueueRace();
            }

            ++m_ItemCounter;
            m_Stat.onEnqueue();

            return true;
        }

        /// Dequeues a value from the queue
        /** @anchor cds_intrusive_BasketQueue_dequeue
            If the queue is empty the function returns \p nullptr.

            <b>Warning</b>: see MSQueue::deque note about item disposing
        */
        value_type * dequeue()
        {
            dequeue_result res;

            if ( do_dequeue( res, true ))
                return node_traits::to_value_ptr( *res.pNext );
            return nullptr;
        }

        /// Synonym for \ref cds_intrusive_BasketQueue_enqueue "enqueue" function
        bool push( value_type& val )
        {
            return enqueue( val );
        }

        /// Synonym for \ref cds_intrusive_BasketQueue_dequeue "dequeue" function
        value_type * pop()
        {
            return dequeue();
        }

        /// Checks if the queue is empty
        /**
            Note that this function is not \p const.
            The function is based on \ref dequeue algorithm
            but really does not dequeued any item.
        */
        bool empty()
        {
            dequeue_result res;
            return !do_dequeue( res, false );
        }

        /// Clear the queue
        /**
            The function repeatedly calls \ref dequeue until it returns \p nullptr.
            The disposer defined in template \p Options is called for each item
            that can be safely disposed.
        */
        void clear()
        {
            while ( dequeue() );
        }
    };

}} // namespace cds::intrusive

#endif // #ifndef __CDS_INTRUSIVE_BASKET_QUEUE_H

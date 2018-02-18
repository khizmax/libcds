// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_OPTIMISTIC_QUEUE_H
#define CDSLIB_INTRUSIVE_OPTIMISTIC_QUEUE_H

#include <type_traits>
#include <cds/intrusive/details/base.h>
#include <cds/algo/atomic.h>
#include <cds/gc/default_gc.h>

namespace cds { namespace intrusive {

    /// \p OptimisticQueue related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace optimistic_queue {

        /// Optimistic queue node
        /**
            Template parameters:
            - \p GC - garbage collector
            - \p Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <class GC, typename Tag = opt::none>
        struct node
        {
            typedef GC  gc  ;   ///< Garbage collector
            typedef Tag tag ;   ///< tag

            typedef typename gc::template atomic_ref<node>    atomic_node_ptr    ;    ///< atomic pointer

            atomic_node_ptr m_pNext ;   ///< Pointer to next node
            atomic_node_ptr m_pPrev ;   ///< Pointer to previous node

            node() noexcept
            {
                m_pNext.store( nullptr, atomics::memory_order_relaxed );
                m_pPrev.store( nullptr, atomics::memory_order_release );
            }
        };

        //@cond
        struct default_hook {
            typedef cds::gc::default_gc gc;
            typedef opt::none           tag;
        };
        //@endcond

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
            - \p opt::gc - garbage collector used.
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < typename... Options >
        struct base_hook: public hook< opt::base_hook_tag, Options... >
        {};

        /// Member hook
        /**
            \p MemberOffset specifies offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - \p opt::gc - garbage collector used.
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
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
            - \p opt::gc - garbage collector used.
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename NodeTraits, typename... Options >
        struct traits_hook: public hook< opt::traits_hook_tag, Options... >
        {
            //@cond
            typedef NodeTraits node_traits;
            //@endcond
        };

        /// Check link
        template <typename Node>
        struct link_checker {
            //@cond
            typedef Node node_type;
            //@endcond

            /// Checks if the link fields of node \p pNode is \p nullptr
            /**
                An asserting is generated if \p pNode link fields is not \p nullptr
            */
            static void is_empty( const node_type * pNode )
            {
                assert( pNode->m_pNext.load( atomics::memory_order_relaxed ) == nullptr );
                assert( pNode->m_pPrev.load( atomics::memory_order_relaxed ) == nullptr );
                CDS_UNUSED( pNode );
            }
        };

        /// Metafunction for selecting appropriate link checking policy
        template < typename Node, opt::link_check_type LinkType >
        struct get_link_checker
        {
            //@cond
            typedef intrusive::opt::v::empty_link_checker<Node>  type;
            //@endcond
        };

        //@cond
        template < typename Node >
        struct get_link_checker< Node, opt::always_check_link >
        {
            typedef link_checker<Node>  type;
        };
        template < typename Node >
        struct get_link_checker< Node, opt::debug_check_link >
        {
#       ifdef _DEBUG
            typedef link_checker<Node>  type;
#       else
            typedef intrusive::opt::v::empty_link_checker<Node>  type;
#       endif
        };
        //@endcond

        /// \p OptimisticQueue internal statistics. May be used for debugging or profiling
        /**
            Template argument \p Counter defines type of counter.
            Default is \p cds::atomicity::event_counter.
            You may use stronger type of counter like as \p cds::atomicity::item_counter,
            or even integral type, for example, \p int.
        */
        template <typename Counter = cds::atomicity::event_counter >
        struct stat
        {
            typedef Counter     counter_type;   ///< Counter type

            counter_type m_EnqueueCount;    ///< Enqueue call count
            counter_type m_DequeueCount;    ///< Dequeue call count
            counter_type m_EnqueueRace;     ///< Count of enqueue race conditions encountered
            counter_type m_DequeueRace;     ///< Count of dequeue race conditions encountered
            counter_type m_AdvanceTailError; ///< Count of "advance tail failed" events
            counter_type m_BadTail;         ///< Count of events "Tail is not pointed to the last item in the queue"
            counter_type m_FixListCount;    ///< Count of fix list event
            counter_type m_EmptyDequeue;    ///< Count of dequeue from empty queue

            /// Register enqueue call
            void onEnqueue() { ++m_EnqueueCount; }
            /// Register dequeue call
            void onDequeue() { ++m_DequeueCount; }
            /// Register enqueue race event
            void onEnqueueRace() { ++m_EnqueueRace; }
            /// Register dequeue race event
            void onDequeueRace() { ++m_DequeueRace; }
            /// Register "advance tail failed" event
            void onAdvanceTailFailed() { ++m_AdvanceTailError; }
            /// Register event "Tail is not pointed to last item in the queue"
            void onBadTail() { ++m_BadTail; }
            /// Register fix list event
            void onFixList()    { ++m_FixListCount; }
            /// Register dequeuing from empty queue
            void onEmptyDequeue() { ++m_EmptyDequeue; }

            //@cond
            void reset()
            {
                m_EnqueueCount.reset();
                m_DequeueCount.reset();
                m_EnqueueRace.reset();
                m_DequeueRace.reset();
                m_AdvanceTailError.reset();
                m_BadTail.reset();
                m_FixListCount.reset();
                m_EmptyDequeue.reset();
            }

            stat& operator +=( stat const& s )
            {
                m_EnqueueCount += s.m_EnqueueCount.get();
                m_DequeueCount += s.m_DequeueCount.get();
                m_EnqueueRace += s.m_EnqueueRace.get();
                m_DequeueRace += s.m_DequeueRace.get();
                m_AdvanceTailError += s.m_AdvanceTailError.get();
                m_BadTail += s.m_BadTail.get();
                m_FixListCount += s.m_FixListCount.get();
                m_EmptyDequeue += s.m_EmptyDequeue.get();

                return *this;
            }
            //@endcond
        };

        /// Dummy \p OptimisticQueue statistics - no counting is performed. Support interface like \p optimistic_queue::stat
        struct empty_stat
        {
            //@cond
            void onEnqueue()            const {}
            void onDequeue()            const {}
            void onEnqueueRace()        const {}
            void onDequeueRace()        const {}
            void onAdvanceTailFailed()  const {}
            void onBadTail()            const {}
            void onFixList()            const {}
            void onEmptyDequeue()       const {}

            void reset() {}
            empty_stat& operator +=( empty_stat const& )
            {
                return *this;
            }
            //@endcond
        };

        /// \p OptimisticQueue default type traits
        struct traits
        {
            /// Back-off strategy
            typedef cds::backoff::empty             back_off;

            /// Hook, possible types are \p optimistic_queue::base_hook, \p optimistic_queue::member_hook, \p optimistic_queue::traits_hook
            typedef optimistic_queue::base_hook<>   hook;

            /// The functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used for dequeuing
            typedef opt::v::empty_disposer          disposer;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef cds::atomicity::empty_item_counter   item_counter;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// Internal statistics (by default, disabled)
            /**
                Possible option value are: \p optimistic_queue::stat, \p optimistic_queue::empty_stat (the default),
                user-provided class that supports \p %optimistic_queue::stat interface.
            */
            typedef optimistic_queue::empty_stat    stat;

            /// Link checking, see \p cds::opt::link_checker
            static constexpr const opt::link_check_type link_checker = opt::debug_check_link;

            /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
            enum { padding = opt::cache_line_padding };
        };

        /// Metafunction converting option list to \p optimistic_queue::traits
        /**
            Supported \p Options are:

            - \p opt::hook - hook used. Possible hooks are: \p optimistic_queue::base_hook, \p optimistic_queue::member_hook, \p optimistic_queue::traits_hook.
                If the option is not specified, \p %optimistic_queue::base_hook<> is used.
            - \p opt::back_off - back-off strategy used, default is \p cds::backoff::empty.
            - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used
                when dequeuing.
            - \p opt::link_checker - the type of node's link fields checking. Default is \p opt::debug_check_link
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            - \p opt::stat - the type to gather internal statistics.
                Possible statistics types are: \p optimistic_queue::stat, \p optimistic_queue::empty_stat,
                user-provided class that supports \p %optimistic_queue::stat interface.
                Default is \p %optimistic_queue::empty_stat (internal statistics disabled).
            - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).

            Example: declare \p %OptimisticQueue with item counting and internal statistics
            \code
            typedef cds::intrusive::OptimisticQueue< cds::gc::HP, Foo,
                typename cds::intrusive::optimistic_queue::make_traits<
                    cds::intrusive::opt:hook< cds::intrusive::optimistic_queue::base_hook< cds::opt::gc<cds:gc::HP> >>,
                    cds::opt::item_counte< cds::atomicity::item_counter >,
                    cds::opt::stat< cds::intrusive::optimistic_queue::stat<> >
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
    }   // namespace optimistic_queue

    /// Optimistic intruive lock-free queue
    /** @ingroup cds_intrusive_queue
        Implementation of Ladan-Mozes & Shavit optimistic queue algorithm.
            [2008] Edya Ladan-Mozes, Nir Shavit "An Optimistic Approach to Lock-Free FIFO Queues"

        Template arguments:
        - \p GC - garbage collector type: \p gc::HP, \p gc::DHP
        - \p T - type of value to be stored in the queue. A value of type \p T must be derived from \p optimistic_queue::node for \p optimistic_queue::base_hook,
            or it should have a member of type \p %optimistic_queue::node for \p optimistic_queue::member_hook,
            or it should be convertible to \p %optimistic_queue::node for \p optimistic_queue::traits_hook.
        - \p Traits - queue traits, default is \p optimistic_queue::traits. You can use \p optimistic_queue::make_traits
            metafunction to make your traits or just derive your traits from \p %optimistic_queue::traits:
            \code
            struct myTraits: public cds::intrusive::optimistic_queue::traits {
                typedef cds::intrusive::optimistic_queue::stat<> stat;
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::intrusive::OptimisticQueue< cds::gc::HP, Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::intrusive::OptimisticQueue< cds::gc::HP, Foo,
                typename cds::intrusive::optimistic_queue::make_traits<
                    cds::opt::stat< cds::intrusive::optimistic_queue::stat<> >,
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode

        Garbage collecting schema \p GC must be consistent with the optimistic_queue::node GC.

        \par About item disposing
        The optimistic queue algo has a key feature: even if the queue is empty it contains one item that is "dummy" one from
        the standpoint of the algo. See \p dequeue() function for explanation.

        \par Examples
        \code
        #include <cds/gc/hp.h>
        #include <cds/intrusive/optimistic_queue.h>

        namespace ci = cds::inrtusive;
        typedef cds::gc::HP hp_gc;

        // Optimistic queue with Hazard Pointer garbage collector, base hook + item counter:
        struct Foo: public ci::optimistic_queue::node< hp_gc >
        {
            // Your data
            ...
        };

        typedef ci::OptimisticQueue< hp_gc,
            Foo,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc< hp_gc > >
                >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > FooQueue;

        // Optimistic queue with Hazard Pointer garbage collector, member hook, no item counter:
        struct Bar
        {
            // Your data
            ...
            ci::optimistic_queue::node< hp_gc > hMember;
        };

        typedef ci::OptimisticQueue< hp_gc,
            Bar,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(Bar, hMember)
                        ,ci::opt::gc< hp_gc >
                    >
                >
            >::type
        > BarQueue;
        \endcode
    */
    template <typename GC, typename T, typename Traits = optimistic_queue::traits >
    class OptimisticQueue
    {
    public:
        typedef GC gc;          ///< Garbage collector
        typedef T  value_type;  ///< type of value to be stored in the queue
        typedef Traits traits;  ///< Queue traits

        typedef typename traits::hook       hook;       ///< hook type
        typedef typename hook::node_type    node_type;  ///< node type
        typedef typename traits::disposer   disposer;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits;    ///< node traits
        typedef typename optimistic_queue::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker
        typedef typename traits::back_off  back_off;        ///< back-off strategy
        typedef typename traits::item_counter item_counter; ///< Item counting policy used
        typedef typename traits::memory_model  memory_model;///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::stat      stat;            ///< Internal statistics policy used

        /// Rebind template arguments
        template <typename GC2, typename T2, typename Traits2>
        struct rebind {
            typedef OptimisticQueue< GC2, T2, Traits2 > other   ;   ///< Rebinding result
        };

        static constexpr const size_t c_nHazardPtrCount = 5; ///< Count of hazard pointer required for the algorithm

    protected:
        //@cond
        typedef typename node_type::atomic_node_ptr atomic_node_ptr;

        // GC and node_type::gc must be the same
        static_assert((std::is_same<gc, typename node_type::gc>::value), "GC and node_type::gc must be the same");
        //@endcond

        atomic_node_ptr     m_pTail;   ///< Pointer to tail node
        //@cond
        typename opt::details::apply_padding< atomic_node_ptr, traits::padding >::padding_type pad1_;
        //@endcond
        atomic_node_ptr     m_pHead;   ///< Pointer to head node
        //@cond
        typename opt::details::apply_padding< atomic_node_ptr, traits::padding >::padding_type pad2_;
        //@endcond
        node_type           m_Dummy ;   ///< dummy node
        //@cond
        typename opt::details::apply_padding< atomic_node_ptr, traits::padding >::padding_type pad3_;
        //@endcond
        item_counter        m_ItemCounter   ;   ///< Item counter
        stat                m_Stat          ;   ///< Internal statistics

    protected:
        //@cond
        static void clear_links( node_type * pNode )
        {
            pNode->m_pNext.store( nullptr, memory_model::memory_order_release );
            pNode->m_pPrev.store( nullptr, memory_model::memory_order_release );
        }

        struct dequeue_result {
            typename gc::template GuardArray<3>  guards;

            node_type * pHead;
            node_type * pNext;
        };

        bool do_dequeue( dequeue_result& res )
        {
            node_type * pTail;
            node_type * pHead;
            node_type * pFirstNodePrev;
            back_off bkoff;

            while ( true ) { // Try till success or empty
                pHead = res.guards.protect( 0, m_pHead, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});
                pTail = res.guards.protect( 1, m_pTail, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});
                assert( pHead != nullptr );
                pFirstNodePrev = res.guards.protect( 2, pHead->m_pPrev, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});

                if ( pHead == m_pHead.load(memory_model::memory_order_acquire)) {
                    if ( pTail != pHead ) {
                        if ( pFirstNodePrev == nullptr
                          || pFirstNodePrev->m_pNext.load(memory_model::memory_order_acquire) != pHead )
                        {
                            fix_list( pTail, pHead );
                            continue;
                        }
                        if ( m_pHead.compare_exchange_weak( pHead, pFirstNodePrev, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                            // dequeue success
                            break;
                        }
                    }
                    else {
                        // the queue is empty
                        m_Stat.onEmptyDequeue();
                        return false;
                    }
                }

                m_Stat.onDequeueRace();
                bkoff();
            }

            --m_ItemCounter;
            m_Stat.onDequeue();

            res.pHead = pHead;
            res.pNext = pFirstNodePrev;
            return true;
        }


        /// Helper function for optimistic queue. Corrects \p prev pointer of queue's nodes if it is needed
        void fix_list( node_type * pTail, node_type * pHead )
        {
            // pTail and pHead are already guarded

            node_type * pCurNode;
            node_type * pCurNodeNext;

            typename gc::template GuardArray<2> guards;

            pCurNode = pTail;
            while ( pCurNode != pHead ) { // While not at head
                pCurNodeNext = guards.protect(0, pCurNode->m_pNext, [](node_type * p) -> value_type * { return node_traits::to_value_ptr(p);});
                if ( pHead != m_pHead.load(memory_model::memory_order_acquire))
                    break;
                pCurNodeNext->m_pPrev.store( pCurNode, memory_model::memory_order_release );
                guards.assign( 1, node_traits::to_value_ptr( pCurNode = pCurNodeNext ));
            }

            m_Stat.onFixList();
        }

        void dispose_result( dequeue_result& res )
        {
            dispose_node( res.pHead );
        }

        void dispose_node( node_type * p )
        {
            assert( p != nullptr );

            if ( p != &m_Dummy ) {
                struct internal_disposer
                {
                    void operator ()( value_type * p )
                    {
                        assert( p != nullptr );

                        OptimisticQueue::clear_links( node_traits::to_node_ptr( *p ));
                        disposer()(p);
                    }
                };
                gc::template retire<internal_disposer>( node_traits::to_value_ptr(p));
            }
        }

        //@endcond

    public:
        /// Constructor creates empty queue
        OptimisticQueue()
            : m_pTail( &m_Dummy )
            , m_pHead( &m_Dummy )
        {}

        ~OptimisticQueue()
        {
            clear();
            node_type * pHead = m_pHead.load(memory_model::memory_order_relaxed);
            CDS_DEBUG_ONLY( node_type * pTail = m_pTail.load(memory_model::memory_order_relaxed); )
            CDS_DEBUG_ONLY( assert( pHead == pTail ); )
            assert( pHead != nullptr );

            m_pHead.store( nullptr, memory_model::memory_order_relaxed );
            m_pTail.store( nullptr, memory_model::memory_order_relaxed );

            dispose_node( pHead );
        }

        /// @anchor cds_intrusive_OptimisticQueue_enqueue Enqueues \p data in lock-free manner. Always return \a true
        bool enqueue( value_type& val )
        {
            node_type * pNew = node_traits::to_node_ptr( val );
            link_checker::is_empty( pNew );

            typename gc::template GuardArray<2> guards;
            back_off bkoff;

            guards.assign( 1, &val );
            while( true ) {
                node_type * pTail = guards.protect( 0, m_pTail, []( node_type * p ) -> value_type * { return node_traits::to_value_ptr( p ); } );   // Read the tail
                pNew->m_pNext.store( pTail, memory_model::memory_order_relaxed );
                if ( m_pTail.compare_exchange_strong( pTail, pNew, memory_model::memory_order_release, atomics::memory_order_acquire )) { // Try to CAS the tail
                    pTail->m_pPrev.store( pNew, memory_model::memory_order_release ); // Success, write prev
                    ++m_ItemCounter;
                    m_Stat.onEnqueue();
                    break;     // Enqueue done!
                }
                m_Stat.onEnqueueRace();
                bkoff();
            }
            return true;
        }

        /// Dequeues a value from the queue
        /** @anchor cds_intrusive_OptimisticQueue_dequeue
            If the queue is empty the function returns \p nullptr

            \par Warning
            The queue algorithm has following feature: when \p dequeue is called,
            the item returning is still queue's top, and previous top is disposed:

            \code
            before dequeuing         Dequeue               after dequeuing
            +------------------+                           +------------------+
      Top ->|      Item 1      |  -> Dispose Item 1        |      Item 2      | <- Top
            +------------------+                           +------------------+
            |      Item 2      |  -> Return Item 2         |       ...        |
            +------------------+
            |       ...        |
            \endcode

            \p %dequeue() function returns Item 2, that becomes new top of queue, and calls
            the disposer for Item 1, that was queue's top on function entry.
            Thus, you cannot manually delete item returned because it is still included in
            the queue and it has valuable link field that must not be zeroed.
            The item may be deleted only in disposer call.
        */
        value_type * dequeue()
        {
            dequeue_result res;
            if ( do_dequeue( res )) {
                dispose_result( res );

                return node_traits::to_value_ptr( *res.pNext );
            }
            return nullptr;
        }

        /// Synonym for \p enqueue()
        bool push( value_type& val )
        {
            return enqueue( val );
        }

        /// Synonym for \p dequeue()
        value_type * pop()
        {
            return dequeue();
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            return m_pTail.load(memory_model::memory_order_relaxed) == m_pHead.load(memory_model::memory_order_relaxed);
        }

        /// Clear the stack
        /**
            The function repeatedly calls \ref dequeue until it returns \p nullptr.
            The disposer defined in template \p Traits is called for each item
            that can be safely disposed.
        */
        void clear()
        {
            value_type * pv;
            while ( (pv = dequeue()) != nullptr );
        }

        /// Returns queue's item count
        /**
            The value returned depends on \p optimistic_queue::traits::item_counter.
            For \p atomicity::empty_item_counter, this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the queue
            is empty. To check queue emptyness use \p empty() method.
        */
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns refernce to internal statistics
        const stat& statistics() const
        {
            return m_Stat;
        }
    };

}}  // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_OPTIMISTIC_QUEUE_H

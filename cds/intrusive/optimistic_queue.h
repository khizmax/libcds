//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_OPTIMISTIC_QUEUE_H
#define __CDS_INTRUSIVE_OPTIMISTIC_QUEUE_H

#include <type_traits>
#include <cds/intrusive/base.h>
#include <cds/cxx11_atomic.h>
#include <cds/gc/default_gc.h>
#include <cds/gc/hrc/gc_fwd.h>
#include <cds/intrusive/queue_stat.h>
#include <cds/ref.h>

namespace cds { namespace intrusive {

    /// Optimistic queue related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace optimistic_queue {

        /// Optimistic queue node
        /**
            Template parameters:
            - GC - garbage collector used. gc::HRC is not supported.
            - Tag - a tag used to distinguish between different implementation
        */
        template <class GC, typename Tag = opt::none>
        struct node: public GC::container_node
        {
            typedef GC  gc  ;   ///< Garbage collector
            typedef Tag tag ;   ///< tag

            typedef typename gc::template atomic_ref<node>    atomic_node_ptr    ;    ///< atomic pointer

            atomic_node_ptr m_pPrev ;   ///< Pointer to previous node
            atomic_node_ptr m_pNext ;   ///< Pointer to next node

            CDS_CONSTEXPR node() CDS_NOEXCEPT
                : m_pPrev( nullptr )
                , m_pNext( nullptr )
            {}
        };

        //@cond
        struct default_hook {
            typedef cds::gc::default_gc gc;
            typedef opt::none           tag;
        };
        //@endcond

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

        /// OptimisticQueue internal statistics. May be used for debugging or profiling
        /**
            Template argument \p Counter defines type of counter.
            Default is cds::atomicity::event_counter, that is weak, i.e. it is not guaranteed
            strict event counting.
            You may use stronger type of counter like as cds::atomicity::item_counter,
            or even integral type, for example, \p int.

            The class extends intrusive::queue_stat interface for OptimisticQueue.
        */
        template <typename Counter = cds::atomicity::event_counter >
        struct stat: public cds::intrusive::queue_stat<Counter>
        {
            //@cond
            typedef cds::intrusive::queue_stat<Counter> base_class;
            typedef typename base_class::counter_type    counter_type;
            //@endcond

            counter_type    m_FixListCount  ;   ///< Count of fix list event

            /// Register fix list event
            void onFixList()    { ++m_FixListCount; }

            //@cond
            void reset()
            {
                base_class::reset();
                m_FixListCount.reset();
            }

            stat& operator +=( stat const& s )
            {
                base_class::operator +=( s );
                m_FixListCount += s.m_FixListCount.get();
                return *this;
            }
            //@endcond
        };

        /// Dummy OptimisticQueue statistics - no counting is performed. Support interface like \ref optimistic_queue::stat
        struct dummy_stat: public cds::intrusive::queue_dummy_stat
        {
            //@cond
            void onFixList() {}

            void reset() {}
            dummy_stat& operator +=( dummy_stat const& )
            {
                return *this;
            }
            //@endcond
        };

    }   // namespace optimistic_queue

    /// Optimistic queue
    /** @ingroup cds_intrusive_queue
        Implementation of Ladan-Mozes & Shavit optimistic queue algorithm.

        \par Source:
            [2008] Edya Ladan-Mozes, Nir Shavit "An Optimistic Approach to Lock-Free FIFO Queues"

        Template arguments:
        - \p GC - garbage collector type: gc::HP, gc::PTB. Note that gc::HRC is <b>not</b> supported
        - \p T - type to be stored in the queue
        - \p Options - options

        Type of node: \ref optimistic_queue::node.

        \p Options are:
        - opt::hook - hook used. Possible values are: optimistic_queue::base_hook, optimistic_queue::member_hook, optimistic_queue::traits_hook.
            If the option is not specified, <tt>optimistic_queue::base_hook<></tt> is used.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used.
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer. This option is used
            in \ref dequeue function.
        - opt::link_checker - the type of node's link fields checking. Default is \ref opt::debug_check_link
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter
        - opt::stat - the type to gather internal statistics.
            Possible option value are: optimistic_queue::stat, optimistic_queue::dummy_stat,
            user-provided class that supports optimistic_queue::stat interface.
            Generic option intrusive::queue_stat and intrusive::queue_dummy_stat are acceptable too, however,
            they will be automatically converted to optimistic_queue::stat and optimistic_queue::dummy_stat
            respectively.
            Default is \ref optimistic_queue::dummy_stat.
        - opt::alignment - the alignment for internal queue data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).

        Garbage collecting schema \p GC must be consistent with the optimistic_queue::node GC.

        \par About item disposing
        The optimistic queue algo has a key feature: even if the queue is empty it contains one item that is "dummy" one from
        the standpoint of the algo. See \ref dequeue function for explanation.

        \par Examples
        \code
        #include <cds/intrusive/optimistic_queue.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::inrtusive;
        typedef cds::gc::HP hp_gc;

        // Optimistic queue with Hazard Pointer garbage collector, base hook + item counter:
        struct Foo: public ci::optimistic_queue::node< hp_gc >
        {
            // Your data
            ...
        };

        typedef ci::OptimisticQueue< hp_gc,
            Foo
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc< hp_gc > >
            >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
        > FooQueue;

        // Optimistic queue with Hazard Pointer garbage collector, member hook, no item counter:
        struct Bar
        {
            // Your data
            ...
            ci::optimistic_queue::node< hp_gc > hMember;
        };

        typedef ci::OptimisticQueue< hp_gc,
            Bar
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(Bar, hMember)
                    ,ci::opt::gc< hp_gc >
                >
            >
        > BarQueue;

        \endcode
    */
    template <typename GC, typename T, CDS_DECL_OPTIONS9>
    class OptimisticQueue
    {
        //@cond
        struct default_options
        {
            typedef cds::backoff::empty             back_off;
            typedef optimistic_queue::base_hook<>   hook;
            typedef opt::v::empty_disposer          disposer;
            typedef atomicity::empty_item_counter   item_counter;
            typedef opt::v::relaxed_ordering        memory_model;
            typedef optimistic_queue::dummy_stat    stat;
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
            ,optimistic_queue::stat<>
            ,typename std::conditional<
                std::is_same<typename options::stat, cds::intrusive::queue_dummy_stat>::value
                ,optimistic_queue::dummy_stat
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
        typedef typename optimistic_queue::get_link_checker< node_type, options::link_checker >::type link_checker   ;   ///< link checker

        typedef GC gc          ;   ///< Garbage collector
        typedef typename options::back_off  back_off    ;   ///< back-off strategy
        typedef typename options::item_counter item_counter ;   ///< Item counting policy used
        typedef typename options::memory_model  memory_model      ;   ///< Memory ordering. See cds::opt::memory_model option
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename options::stat      stat        ;   ///< Internal statistics policy used
#else
        typedef stat_type_  stat;
#endif

        /// Rebind template arguments
        template <typename GC2, typename T2, CDS_DECL_OTHER_OPTIONS9>
        struct rebind {
            typedef OptimisticQueue< GC2, T2, CDS_OTHER_OPTIONS9> other   ;   ///< Rebinding result
        };

    protected:
        //@cond

        struct internal_disposer
        {
            void operator ()( value_type * p )
            {
                assert( p != nullptr );

                OptimisticQueue::clear_links( node_traits::to_node_ptr(*p) );
                disposer()( p );
            }
        };

        typedef intrusive::node_to_value<OptimisticQueue> node_to_value;
        typedef typename opt::details::alignment_setter< typename node_type::atomic_node_ptr, options::alignment >::type aligned_node_ptr;
        //@endcond

        aligned_node_ptr m_pTail ;   ///< Pointer to tail node
        aligned_node_ptr m_pHead ;   ///< Pointer to head node
        node_type        m_Dummy ;           ///< dummy node

        item_counter        m_ItemCounter   ;   ///< Item counter
        stat                m_Stat          ;   ///< Internal statistics

        static CDS_CONSTEXPR_CONST size_t c_nHazardPtrCount = 5 ; ///< Count of hazard pointer required for the algorithm

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
                pHead = res.guards.protect( 0, m_pHead, node_to_value() );
                pTail = res.guards.protect( 1, m_pTail, node_to_value() );
                assert( pHead != nullptr );
                pFirstNodePrev = res.guards.protect( 2, pHead->m_pPrev, node_to_value() );

                if ( pHead == m_pHead.load(memory_model::memory_order_relaxed)) {
                    if ( pTail != pHead ) {
                        if ( pFirstNodePrev == nullptr
                          || pFirstNodePrev->m_pNext.load(memory_model::memory_order_relaxed) != pHead )
                        {
                            fix_list( pTail, pHead );
                            continue;
                        }
                        if ( m_pHead.compare_exchange_weak( pHead, pFirstNodePrev, memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                            // dequeue success
                            break;
                        }
                    }
                    else {
                        // the queue is empty
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
                pCurNodeNext = guards.protect(0, pCurNode->m_pNext, node_to_value() );
                if ( pHead != m_pHead.load(memory_model::memory_order_relaxed) )
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
                gc::template retire<internal_disposer>( node_traits::to_value_ptr(p) );
            }
        }

        //@endcond

    public:
        /// Constructor creates empty queue
        OptimisticQueue()
            : m_pTail( nullptr )
            , m_pHead( nullptr )
        {
            // GC and node_type::gc must be the same
            static_assert(( std::is_same<gc, typename node_type::gc>::value ), "GC and node_type::gc must be the same");

            // cds::gc::HRC is not allowed
            static_assert(( !std::is_same<gc, cds::gc::HRC>::value ), "cds::gc::HRC is not allowed here");

            m_pTail.store( &m_Dummy, memory_model::memory_order_relaxed );
            m_pHead.store( &m_Dummy, memory_model::memory_order_relaxed );
        }

        ~OptimisticQueue()
        {
            clear();
            node_type * pHead = m_pHead.load(memory_model::memory_order_relaxed);
            CDS_DEBUG_DO( node_type * pTail = m_pTail.load(memory_model::memory_order_relaxed); )
            CDS_DEBUG_DO( assert( pHead == pTail ); )
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
            node_type * pTail = guards.protect( 0, m_pTail, node_to_value() )  ;   // Read the tail
            while( true ) {
                pNew->m_pNext.store( pTail, memory_model::memory_order_release );
                if ( m_pTail.compare_exchange_strong( pTail, pNew, memory_model::memory_order_release, atomics::memory_order_relaxed ) ) {     // Try to CAS the tail
                    pTail->m_pPrev.store( pNew, memory_model::memory_order_release )     ;           // Success, write prev
                    ++m_ItemCounter;
                    m_Stat.onEnqueue();
                    break ;                             // Enqueue done!
                }
                guards.assign( 0, node_traits::to_value_ptr( pTail ) )   ;  // pTail has been changed by CAS above
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

            \p dequeue function returns Item 2, that becomes new top of queue, and calls
            the disposer for Item 1, that was queue's top on function entry.
            Thus, you cannot manually delete item returned because it is still included in
            item sequence and it has valuable link field that must not be zeroed.
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

        /// Synonym for @ref cds_intrusive_OptimisticQueue_enqueue "enqueue"
        bool push( value_type& val )
        {
            return enqueue( val );
        }

        /// Synonym for \ref cds_intrusive_OptimisticQueue_dequeue "dequeue"
        value_type * pop()
        {
            return dequeue();
        }

        /// Checks if queue is empty
        bool empty() const
        {
            return m_pTail.load(memory_model::memory_order_relaxed) == m_pHead.load(memory_model::memory_order_relaxed);
        }

        /// Clear the stack
        /**
            The function repeatedly calls \ref dequeue until it returns \p nullptr.
            The disposer defined in template \p Options is called for each item
            that can be safely disposed.
        */
        void clear()
        {
            value_type * pv;
            while ( (pv = dequeue()) != nullptr );
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

        /// Returns refernce to internal statistics
        const stat& statistics() const
        {
            return m_Stat;
        }
    };

}}  // namespace cds::intrusive

#endif // #ifndef __CDS_INTRUSIVE_OPTIMISTIC_QUEUE_H

//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_MSQUEUE_H
#define __CDS_INTRUSIVE_MSQUEUE_H

#include <type_traits>
#include <cds/intrusive/single_link_struct.h>
#include <cds/intrusive/queue_stat.h>
#include <cds/intrusive/details/dummy_node_holder.h>

namespace cds { namespace intrusive {

    /// Michael & Scott's lock-free queue (intrusive variant)
    /** @ingroup cds_intrusive_queue
        Implementation of well-known Michael & Scott's queue algorithm.

        \par Source:
            [1998] Maged Michael, Michael Scott "Simple, fast, and practical non-blocking and blocking
                   concurrent queue algorithms"

        Template arguments:
        - \p GC - garbage collector type: gc::HP, gc::HRC, gc::PTB
        - \p T - type to be stored in the queue, should be convertible to \ref single_link::node
        - \p Options - options

        Type of node: \ref single_link::node

        \p Options are:
        - opt::hook - hook used. Possible values are: single_link::base_hook, single_link::member_hook, single_link::traits_hook.
            If the option is not specified, <tt>single_link::base_hook<></tt> is used.
            For Gidenstam's gc::HRC, only single_link::base_hook is supported.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used.
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer. This option is used
            in \ref dequeue function.
        - opt::link_checker - the type of node's link fields checking. Default is \ref opt::debug_check_link
            Note: for gc::HRC garbage collector, link checking policy is always selected as \ref opt::always_check_link.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter (no item counting feature)
        - opt::stat - the type to gather internal statistics.
            Possible option value are: \ref queue_stat, \ref queue_dummy_stat, user-provided class that supports queue_stat interface.
            Default is \ref queue_dummy_stat.
        - opt::alignment - the alignment for internal queue data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).

        Garbage collecting schema \p GC must be consistent with the single_link::node GC.

        \par About item disposing
        The Michael & Scott's queue algo has a key feature: even if the queue is empty it contains one item that is "dummy" one from
        the standpoint of the algo. See \ref dequeue function doc for explanation.

        \par Examples
        \code
        #include <cds/intrusive/msqueue.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::inrtusive;
        typedef cds::gc::HP hp_gc;

        // MSQueue with Hazard Pointer garbage collector, base hook + item disposer:
        struct Foo: public ci::single_link::node< hp_gc >
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

        typedef ci::MSQueue< hp_gc,
            Foo
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<hp_gc> >
            >
            ,ci::opt::disposer< fooDisposer >
        > fooQueue;

        // MSQueue with Hazard Pointer garbage collector,
        // member hook + item disposer + item counter,
        // without alignment of internal queue data:
        struct Bar
        {
            // Your data
            ...
            ci::single_link::node< hp_gc > hMember;
        };

        typedef ci::MSQueue< hp_gc,
            Foo
            ,ci::opt::hook<
                ci::single_link::member_hook<
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
    class MSQueue
    {
        //@cond
        struct default_options
        {
            typedef cds::backoff::empty             back_off;
            typedef single_link::base_hook<>        hook;
            typedef opt::v::empty_disposer          disposer;
            typedef atomicity::empty_item_counter   item_counter;
            typedef queue_dummy_stat                stat;
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
        //@endcond

    public:
        typedef T  value_type   ;   ///< type of value stored in the queue
        typedef typename options::hook      hook        ;   ///< hook type
        typedef typename hook::node_type    node_type   ;   ///< node type
        typedef typename options::disposer  disposer    ;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef typename single_link::get_link_checker< node_type, options::link_checker >::type link_checker   ;   ///< link checker

        typedef GC gc          ;   ///< Garbage collector
        typedef typename options::back_off  back_off    ;   ///< back-off strategy
        typedef typename options::item_counter item_counter ;   ///< Item counting policy used
        typedef typename options::stat      stat        ;   ///< Internal statistics policy used
        typedef typename options::memory_model  memory_model ;   ///< Memory ordering. See cds::opt::memory_model option

        /// Rebind template arguments
        template <typename GC2, typename T2, CDS_DECL_OTHER_OPTIONS9>
        struct rebind {
            typedef MSQueue< GC2, T2, CDS_OTHER_OPTIONS9> other   ;   ///< Rebinding result
        };

    protected:
        //@cond
        struct internal_disposer
        {
            void operator()( value_type * p )
            {
                assert( p != nullptr );

                MSQueue::clear_links( node_traits::to_node_ptr(p) );
                disposer()( p );
            }
        };

        typedef intrusive::node_to_value<MSQueue> node_to_value;
        typedef typename opt::details::alignment_setter< typename node_type::atomic_node_ptr, options::alignment >::type aligned_node_ptr;

        typedef typename opt::details::alignment_setter<
            cds::intrusive::details::dummy_node< gc, node_type>,
            options::alignment
        >::type    dummy_node_type;

        aligned_node_ptr    m_pHead ;           ///< Queue's head pointer (cache-line aligned)
        aligned_node_ptr    m_pTail ;           ///< Queue's tail pointer (cache-line aligned)
        dummy_node_type     m_Dummy ;           ///< dummy node
        item_counter        m_ItemCounter   ;   ///< Item counter
        stat                m_Stat  ;           ///< Internal statistics
        //@endcond

        //@cond
        struct dequeue_result {
            typename gc::template GuardArray<2>  guards;

            node_type * pHead;
            node_type * pNext;
        };

        bool do_dequeue( dequeue_result& res )
        {
            node_type * pNext;
            back_off bkoff;

            node_type * h;
            while ( true ) {
                h = res.guards.protect( 0, m_pHead, node_to_value() );
                pNext = h->m_pNext.load( memory_model::memory_order_relaxed );
                res.guards.assign( 1, node_to_value()( pNext ));
                //pNext = res.guards.protect( 1, h->m_pNext, node_to_value() );
                if ( m_pHead.load(memory_model::memory_order_acquire) != h )
                    continue;

                if ( pNext == nullptr )
                    return false ;    // empty queue

                node_type * t = m_pTail.load(memory_model::memory_order_acquire);
                if ( h == t ) {
                    // It is needed to help enqueue
                    m_pTail.compare_exchange_strong( t, pNext, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    m_Stat.onBadTail();
                    continue;
                }

                if ( m_pHead.compare_exchange_strong( h, pNext, memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    break;

                m_Stat.onDequeueRace();
                bkoff();
            }

            --m_ItemCounter;
            m_Stat.onDequeue();

            res.pHead = h;
            res.pNext = pNext;
            return true;
        }

        static void clear_links( node_type * pNode )
        {
            pNode->m_pNext.store( nullptr, memory_model::memory_order_release );
        }

        void dispose_result( dequeue_result& res )
        {
            dispose_node( res.pHead );
        }

        void dispose_node( node_type * p )
        {
            if ( p != m_Dummy.get() ) {
                gc::template retire<internal_disposer>( node_traits::to_value_ptr(p) );
            }
            else {
                // We cannot clear m_Dummy here since it leads to ABA.
                // On the other hand, we cannot use deferred clear_links( &m_Dummy ) call via
                // HP retiring cycle since m_Dummy is member of MSQueue and may be destroyed
                // before HP retiring cycle invocation.
                // So, we will never clear m_Dummy for gc::HP and gc::PTB
                // However, gc::HRC nodes are managed by reference counting, so, we must
                // call HP retire cycle.
                m_Dummy.retire();
            }
        }
        //@endcond

    public:
        /// Initializes empty queue
        MSQueue()
            : m_pHead( nullptr )
            , m_pTail( nullptr )
        {
            // GC and node_type::gc must be the same
            static_assert(( std::is_same<gc, typename node_type::gc>::value ), "GC and node_type::gc must be the same");

            // For cds::gc::HRC, only base_hook is allowed
            static_assert((
                std::conditional<
                    std::is_same<gc, cds::gc::HRC>::value,
                    std::is_same< typename hook::hook_type, opt::base_hook_tag >,
                    boost::true_type
                >::type::value
            ), "For cds::gc::HRC, only base_hook is allowed");

            // Head/tail initialization should be made via store call
            // since gc::HRC manages reference counting
            m_pHead.store( m_Dummy.get(), memory_model::memory_order_relaxed );
            m_pTail.store( m_Dummy.get(), memory_model::memory_order_relaxed );
        }

        /// Destructor clears the queue
        /**
            Since the Michael & Scott queue contains at least one item even
            if the queue is empty, the destructor may call item disposer.
        */
        ~MSQueue()
        {
            clear();

            node_type * pHead = m_pHead.load(memory_model::memory_order_relaxed);

            assert( pHead != nullptr );
            assert( pHead == m_pTail.load(memory_model::memory_order_relaxed) );

            m_pHead.store( nullptr, memory_model::memory_order_relaxed );
            m_pTail.store( nullptr, memory_model::memory_order_relaxed );

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
        /** @anchor cds_intrusive_MSQueue_enqueue
            The function always returns \p true.
        */
        bool enqueue( value_type& val )
        {
            node_type * pNew = node_traits::to_node_ptr( val );
            link_checker::is_empty( pNew );

            typename gc::Guard guard;
            back_off bkoff;

            node_type * t;
            while ( true ) {
                t = guard.protect( m_pTail, node_to_value() );

                node_type * pNext = t->m_pNext.load(memory_model::memory_order_acquire);
                if ( pNext != nullptr ) {
                    // Tail is misplaced, advance it
                    m_pTail.compare_exchange_weak( t, pNext, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    m_Stat.onBadTail();
                    continue;
                }

                node_type * tmp = nullptr;
                if ( t->m_pNext.compare_exchange_strong( tmp, pNew, memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    break;

                m_Stat.onEnqueueRace();
                bkoff();
            }
            ++m_ItemCounter;
            m_Stat.onEnqueue();

            if ( !m_pTail.compare_exchange_strong( t, pNew, memory_model::memory_order_acq_rel, atomics::memory_order_relaxed ))
                m_Stat.onAdvanceTailFailed();
            return true;
        }

        /// Dequeues a value from the queue
        /** @anchor cds_intrusive_MSQueue_dequeue
            If the queue is empty the function returns \p nullptr.

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

        /// Synonym for \ref cds_intrusive_MSQueue_enqueue "enqueue" function
        bool push( value_type& val )
        {
            return enqueue( val );
        }

        /// Synonym for \ref cds_intrusive_MSQueue_dequeue "dequeue" function
        value_type * pop()
        {
            return dequeue();
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            typename gc::Guard guard;
            return guard.protect( m_pHead, node_to_value() )->m_pNext.load( memory_model::memory_order_relaxed ) == nullptr;
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

#endif // #ifndef __CDS_INTRUSIVE_MSQUEUE_H

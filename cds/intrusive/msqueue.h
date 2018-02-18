// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_MSQUEUE_H
#define CDSLIB_INTRUSIVE_MSQUEUE_H

#include <type_traits>
#include <cds/intrusive/details/single_link_struct.h>
#include <cds/algo/atomic.h>

namespace cds { namespace intrusive {

    /// MSQueue related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace msqueue {

        /// Queue node
        /**
            Template parameters:
            - GC - garbage collector used
            - Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <class GC, typename Tag = opt::none >
        using node = cds::intrusive::single_link::node< GC, Tag >;

        /// Base hook
        /**
            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < typename... Options >
        using base_hook = cds::intrusive::single_link::base_hook< Options...>;

        /// Member hook
        /**
            \p MemberOffset specifies offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < size_t MemberOffset, typename... Options >
        using member_hook = cds::intrusive::single_link::member_hook< MemberOffset, Options... >;

        /// Traits hook
        /**
            \p NodeTraits defines type traits for node.
            See \ref node_traits for \p NodeTraits interface description

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename NodeTraits, typename... Options >
        using traits_hook = cds::intrusive::single_link::traits_hook< NodeTraits, Options... >;

        /// Queue internal statistics. May be used for debugging or profiling
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
            typedef Counter     counter_type;   ///< Counter type

            counter_type m_EnqueueCount      ;  ///< Enqueue call count
            counter_type m_DequeueCount      ;  ///< Dequeue call count
            counter_type m_EnqueueRace       ;  ///< Count of enqueue race conditions encountered
            counter_type m_DequeueRace       ;  ///< Count of dequeue race conditions encountered
            counter_type m_AdvanceTailError  ;  ///< Count of "advance tail failed" events
            counter_type m_BadTail           ;  ///< Count of events "Tail is not pointed to the last item in the queue"
            counter_type m_EmptyDequeue      ;  ///< Count of dequeue from empty queue

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
                m_EmptyDequeue += s.m_EmptyDequeue.get();

                return *this;
            }
            //@endcond
        };

        /// Dummy queue statistics - no counting is performed, no overhead. Support interface like \p msqueue::stat
        struct empty_stat
        {
            //@cond
            void onEnqueue()                const {}
            void onDequeue()                const {}
            void onEnqueueRace()            const {}
            void onDequeueRace()            const {}
            void onAdvanceTailFailed()      const {}
            void onBadTail()                const {}
            void onEmptyDequeue()           const {}

            void reset() {}
            empty_stat& operator +=( empty_stat const& )
            {
                return *this;
            }
            //@endcond
        };

        /// MSQueue default traits
        struct traits
        {
            /// Back-off strategy
            typedef cds::backoff::empty         back_off;

            /// Hook, possible types are \p msqueue::base_hook, \p msqueue::member_hook, \p msqueue::traits_hook
            typedef msqueue::base_hook<>        hook;

            /// The functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used for dequeuing
            typedef opt::v::empty_disposer      disposer;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter   item_counter;

            /// Internal statistics (by default, disabled)
            /**
                Possible option value are: \p msqueue::stat, \p msqueue::empty_stat (the default),
                user-provided class that supports \p %msqueue::stat interface.
            */
            typedef msqueue::empty_stat         stat;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering    memory_model;

            /// Link checking, see \p cds::opt::link_checker
            static constexpr const opt::link_check_type link_checker = opt::debug_check_link;

            /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
            enum { padding = opt::cache_line_padding };
        };

        /// Metafunction converting option list to \p msqueue::traits
        /**
            Supported \p Options are:

            - \p opt::hook - hook used. Possible hooks are: \p msqueue::base_hook, \p msqueue::member_hook, \p msqueue::traits_hook.
                If the option is not specified, \p %msqueue::base_hook<> is used.
            - \p opt::back_off - back-off strategy used, default is \p cds::backoff::empty.
            - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used
                when dequeuing.
            - \p opt::link_checker - the type of node's link fields checking. Default is \p opt::debug_check_link
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            - \p opt::stat - the type to gather internal statistics.
                Possible statistics types are: \p msqueue::stat, \p msqueue::empty_stat, user-provided class that supports \p %msqueue::stat interface.
                Default is \p %msqueue::empty_stat (internal statistics disabled).
            - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).

            Example: declare \p %MSQueue with item counting and internal statistics
            \code
            typedef cds::intrusive::MSQueue< cds::gc::HP, Foo,
                typename cds::intrusive::msqueue::make_traits<
                    cds::intrusive::opt:hook< cds::intrusive::msqueue::base_hook< cds::opt::gc<cds:gc::HP> >>,
                    cds::opt::item_counte< cds::atomicity::item_counter >,
                    cds::opt::stat< cds::intrusive::msqueue::stat<> >
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
    } // namespace msqueue

    /// Michael & Scott's intrusive lock-free queue
    /** @ingroup cds_intrusive_queue
        Implementation of well-known Michael & Scott's queue algorithm:
        - [1998] Maged Michael, Michael Scott "Simple, fast, and practical non-blocking and blocking concurrent queue algorithms"

        Template arguments:
        - \p GC - garbage collector type: \p gc::HP, \p gc::DHP
        - \p T - type of value to be stored in the queue. A value of type \p T must be derived from \p msqueue::node for \p msqueue::base_hook,
            or it should have a member of type \p %msqueue::node for \p msqueue::member_hook,
            or it should be convertible to \p %msqueue::node for \p msqueue::traits_hook.
        - \p Traits - queue traits, default is \p msqueue::traits. You can use \p msqueue::make_traits
            metafunction to make your traits or just derive your traits from \p %msqueue::traits:
            \code
            struct myTraits: public cds::intrusive::msqueue::traits {
                typedef cds::intrusive::msqueue::stat<> stat;
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::intrusive::MSQueue< cds::gc::HP, Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::intrusive::MSQueue< cds::gc::HP, Foo,
                typename cds::intrusive::msqueue::make_traits<
                    cds::opt::stat< cds::intrusive::msqueue::stat<> >,
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode

        \par About item disposing
        The Michael & Scott's queue algo has a key feature: even if the queue is empty it contains one item that is "dummy" one from
        the standpoint of the algo. See \p dequeue() function for explanation.

        \par Examples
        \code
        #include <cds/intrusive/msqueue.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::inrtusive;
        typedef cds::gc::HP hp_gc;

        // MSQueue with Hazard Pointer garbage collector, base hook + item disposer:
        struct Foo: public ci::msqueue::node< hp_gc >
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

        // Declare traits for the queue
        struct myTraits: public ci::msqueue::traits {
            ,ci::opt::hook<
                ci::msqueue::base_hook< ci::opt::gc<hp_gc> >
            >
            ,ci::opt::disposer< fooDisposer >
        };

        // At least, declare the queue type
        typedef ci::MSQueue< hp_gc, Foo, myTraits > fooQueue;

        // Example 2:
        //  MSQueue with Hazard Pointer garbage collector,
        //  member hook + item disposer + item counter,
        //  without padding of internal queue data
        //  Use msqueue::make_traits
        struct Bar
        {
            // Your data
            ...
            ci::msqueue::node< hp_gc > hMember;
        };

        typedef ci::MSQueue< hp_gc,
            Foo,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::member_hook<
                        offsetof(Bar, hMember)
                        ,ci::opt::gc<hp_gc>
                    >
                >
                ,ci::opt::disposer< fooDisposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::padding< cds::opt::no_special_padding >
            >::type
        > barQueue;
        \endcode
    */
    template <typename GC, typename T, typename Traits = msqueue::traits>
    class MSQueue
    {
    public:
        typedef GC gc;          ///< Garbage collector
        typedef T  value_type;  ///< type of value to be stored in the queue
        typedef Traits traits;  ///< Queue traits

        typedef typename traits::hook       hook;       ///< hook type
        typedef typename hook::node_type    node_type;  ///< node type
        typedef typename traits::disposer   disposer;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits;   ///< node traits
        typedef typename single_link::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker

        typedef typename traits::back_off   back_off;       ///< back-off strategy
        typedef typename traits::item_counter item_counter; ///< Item counter class
        typedef typename traits::stat       stat;           ///< Internal statistics
        typedef typename traits::memory_model memory_model; ///< Memory ordering. See \p cds::opt::memory_model option

        /// Rebind template arguments
        template <typename GC2, typename T2, typename Traits2>
        struct rebind {
            typedef MSQueue< GC2, T2, Traits2 > other;   ///< Rebinding result
        };

        static constexpr const size_t c_nHazardPtrCount = 2; ///< Count of hazard pointer required for the algorithm

    protected:
        //@cond

        // GC and node_type::gc must be the same
        static_assert((std::is_same<gc, typename node_type::gc>::value), "GC and node_type::gc must be the same");

        typedef typename node_type::atomic_node_ptr atomic_node_ptr;

        atomic_node_ptr    m_pHead;        ///< Queue's head pointer
        typename opt::details::apply_padding< atomic_node_ptr, traits::padding >::padding_type pad1_;
        atomic_node_ptr    m_pTail;        ///< Queue's tail pointer
        typename opt::details::apply_padding< atomic_node_ptr, traits::padding >::padding_type pad2_;
        node_type          m_Dummy;        ///< dummy node
        typename opt::details::apply_padding< node_type, traits::padding >::padding_type pad3_;
        item_counter        m_ItemCounter; ///< Item counter
        stat                m_Stat;        ///< Internal statistics
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
                h = res.guards.protect( 0, m_pHead, []( node_type * p ) -> value_type * { return node_traits::to_value_ptr( p );});
                pNext = res.guards.protect( 1, h->m_pNext, []( node_type * p ) -> value_type * { return node_traits::to_value_ptr( p );});
                if ( m_pHead.load(memory_model::memory_order_acquire) != h )
                    continue;

                if ( pNext == nullptr ) {
                    m_Stat.onEmptyDequeue();
                    return false;    // empty queue
                }

                node_type * t = m_pTail.load(memory_model::memory_order_acquire);
                if ( h == t ) {
                    // It is needed to help enqueue
                    m_pTail.compare_exchange_strong( t, pNext, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    m_Stat.onBadTail();
                    continue;
                }

                if ( m_pHead.compare_exchange_strong( h, pNext, memory_model::memory_order_acquire, atomics::memory_order_relaxed ))
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
            // Note about the dummy node:
            // We cannot clear m_Dummy here since it leads to ABA.
            // On the other hand, we cannot use deferred clear_links( &m_Dummy ) call via
            // HP retiring cycle since m_Dummy is member of MSQueue and may be destroyed
            // before HP retiring cycle invocation.
            // So, we will never clear m_Dummy

            struct disposer_thunk {
                void operator()( value_type * p ) const
                {
                    assert( p != nullptr );
                    MSQueue::clear_links( node_traits::to_node_ptr( p ));
                    disposer()(p);
                }
            };

            if ( p != &m_Dummy )
                gc::template retire<disposer_thunk>( node_traits::to_value_ptr( p ));
        }
        //@endcond

    public:
        /// Initializes empty queue
        MSQueue()
            : m_pHead( &m_Dummy )
            , m_pTail( &m_Dummy )
        {}

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
            assert( pHead == m_pTail.load(memory_model::memory_order_relaxed));

            m_pHead.store( nullptr, memory_model::memory_order_relaxed );
            m_pTail.store( nullptr, memory_model::memory_order_relaxed );

            dispose_node( pHead );
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
                t = guard.protect( m_pTail, []( node_type * p ) -> value_type * { return node_traits::to_value_ptr( p );});

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

            if ( !m_pTail.compare_exchange_strong( t, pNew, memory_model::memory_order_release, atomics::memory_order_relaxed ))
                m_Stat.onAdvanceTailFailed();
            return true;
        }

        /// Dequeues a value from the queue
        /** @anchor cds_intrusive_MSQueue_dequeue
            If the queue is empty the function returns \p nullptr.

            \par Warning
            The queue algorithm has following feature: when \p %dequeue() is called,
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
            item sequence and it has valuable link field that must not be zeroed.
            The item should be deleted only in garbage collector retire cycle using the disposer.
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

        /// Synonym for \ref cds_intrusive_MSQueue_enqueue "enqueue()" function
        bool push( value_type& val )
        {
            return enqueue( val );
        }

        /// Synonym for \ref cds_intrusive_MSQueue_dequeue "dequeue()" function
        value_type * pop()
        {
            return dequeue();
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            typename gc::Guard guard;
            node_type * p = guard.protect( m_pHead, []( node_type * pNode ) -> value_type * { return node_traits::to_value_ptr( pNode );});
            return p->m_pNext.load( memory_model::memory_order_relaxed ) == nullptr;
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
            The value returned depends on \p msqueue::traits::item_counter. For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the queue
            is empty. To check queue emptyness use \p empty() method.
        */
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }
    };

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_MSQUEUE_H

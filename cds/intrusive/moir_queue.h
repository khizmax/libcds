// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_MOIR_QUEUE_H
#define CDSLIB_INTRUSIVE_MOIR_QUEUE_H

#include <cds/intrusive/msqueue.h>

namespace cds { namespace intrusive {

    /// A variation of Michael & Scott's lock-free queue (intrusive variant)
    /** @ingroup cds_intrusive_queue
        This is slightly optimized Michael & Scott's queue algorithm that overloads \ref dequeue function.

        Source:
            - [2000] Simon Doherty, Lindsay Groves, Victor Luchangco, Mark Moir
                "Formal Verification of a practical lock-free queue algorithm"

        Cite from this work about difference from Michael & Scott algo:
        "Our algorithm differs from Michael and Scott's [MS98] in that we test whether \p Tail points to the header
        node only <b>after</b> \p Head has been updated, so a dequeuing process reads \p Tail only once. The dequeue in
        [MS98] performs this test before checking whether the next pointer in the dummy node is null, which
        means that it reads \p Tail every time a dequeuing process loops. Under high load, when operations retry
        frequently, our modification will reduce the number of accesses to global memory. This modification, however,
        introduces the possibility of \p Head and \p Tail 'crossing'."

        Explanation of template arguments see \p intrusive::MSQueue.

        \par Examples
        \code
        #include <cds/intrusive/moir_queue.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::inrtusive;
        typedef cds::gc::HP hp_gc;

        // MoirQueue with Hazard Pointer garbage collector, base hook + item disposer:
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

        typedef ci::MoirQueue<
            hp_gc
            ,Foo
            typename ci::msqueue::make_traits<
                ,ci::opt::hook<
                    ci::msqueue::base_hook< ci::opt::gc<hp_gc> >
                >
                ,ci::opt::disposer< fooDisposer >
            >::type
        > fooQueue;

        // MoirQueue with Hazard Pointer garbage collector,
        // member hook + item disposer + item counter,
        // without padding of internal queue data:
        struct Bar
        {
            // Your data
            ...
            ci::msqueue::node< hp_gc > hMember;
        };

        struct barQueueTraits: public ci::msqueue::traits
        {
            typedef ci::msqueue::member_hook< offsetof(Bar, hMember), ,ci::opt::gc<hp_gc> > hook;
            typedef fooDisposer disposer;
            typedef cds::atomicity::item_counter item_counter;
            enum { padding = cds::opt::no_special_padding };
        };
        typedef ci::MoirQueue< hp_gc, Bar, barQueueTraits > barQueue;
        \endcode
    */
    template <typename GC, typename T, typename Traits = cds::intrusive::msqueue::traits>
    class MoirQueue: public MSQueue< GC, T, Traits >
    {
        //@cond
        typedef MSQueue< GC, T, Traits > base_class;
        typedef typename base_class::node_type node_type;
        //@endcond

    public:
        //@cond
        typedef typename base_class::value_type value_type;
        typedef typename base_class::back_off   back_off;
        typedef typename base_class::gc         gc;
        typedef typename base_class::node_traits node_traits;
        typedef typename base_class::memory_model   memory_model;
        //@endcond

        /// Rebind template arguments
        template < typename GC2, typename T2, typename Traits2 >
        struct rebind {
            typedef MoirQueue< GC2, T2, Traits2> other   ;   ///< Rebinding result
        };

    protected:
        //@cond
        typedef typename base_class::dequeue_result dequeue_result;

        bool do_dequeue( dequeue_result& res )
        {
            back_off bkoff;

            node_type * pNext;
            node_type * h;
            while ( true ) {
                h = res.guards.protect( 0, base_class::m_pHead, []( node_type * p ) -> value_type * { return node_traits::to_value_ptr( p );});
                pNext = res.guards.protect( 1, h->m_pNext, []( node_type * p ) -> value_type * { return node_traits::to_value_ptr( p );});

                if ( pNext == nullptr ) {
                    base_class::m_Stat.onEmptyDequeue();
                    return false;    // queue is empty
                }

                if ( base_class::m_pHead.compare_exchange_strong( h, pNext, memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                    node_type * t = base_class::m_pTail.load(memory_model::memory_order_acquire);
                    if ( h == t )
                        base_class::m_pTail.compare_exchange_strong( t, pNext, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    break;
                }

                base_class::m_Stat.onDequeueRace();
                bkoff();
            }

            --base_class::m_ItemCounter;
            base_class::m_Stat.onDequeue();

            res.pHead = h;
            res.pNext = pNext;
            return true;
        }
        //@endcond

    public:
        /// Dequeues a value from the queue
        /** @anchor cds_intrusive_MoirQueue_dequeue
            See warning about item disposing in \p MSQueue::dequeue.
        */
        value_type * dequeue()
        {
            dequeue_result res;
            if ( do_dequeue( res )) {
                base_class::dispose_result( res );
                return node_traits::to_value_ptr( *res.pNext );
            }
            return nullptr;
        }

        /// Synonym for \ref cds_intrusive_MoirQueue_dequeue "dequeue" function
        value_type * pop()
        {
            return dequeue();
        }
    };

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_MOIR_QUEUE_H

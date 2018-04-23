// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_BASKET_QUEUE_H
#define CDSLIB_CONTAINER_BASKET_QUEUE_H

#include <cds/intrusive/basket_queue.h>
#include <cds/container/details/base.h>
#include <memory>

namespace cds { namespace container {

    /// BasketQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace basket_queue {

        /// Internal statistics
        template <typename Counter = cds::intrusive::basket_queue::stat<>::counter_type >
        using stat = cds::intrusive::basket_queue::stat< Counter >;

        /// Dummy internal statistics
        typedef cds::intrusive::basket_queue::empty_stat empty_stat;

        /// BasketQueue default type traits
        struct traits
        {
            /// Node allocator
            typedef CDS_DEFAULT_ALLOCATOR       allocator;

            /// Back-off strategy
            typedef cds::backoff::empty         back_off;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter   item_counter;

            /// Internal statistics (by default, disabled)
            /**
                Possible option value are: \p basket_queue::stat, \p basket_queue::empty_stat (the default),
                user-provided class that supports \p %basket_queue::stat interface.
            */
            typedef basket_queue::empty_stat         stat;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering    memory_model;

            /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
            enum { padding = opt::cache_line_padding };
        };

        /// Metafunction converting option list to \p basket_queue::traits
        /**
            Supported \p Options are:
            - \p opt::allocator - allocator (like \p std::allocator) used for allocating queue nodes. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::back_off - back-off strategy used, default is \p cds::backoff::empty.
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            - \ opt::stat - the type to gather internal statistics.
                Possible statistics types are: \p basket_queue::stat, \p basket_queue::empty_stat, user-provided class that supports \p %basket_queue::stat interface.
                Default is \p %basket_queue::empty_stat.
            - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).

            Example: declare \p %BasketQueue with item counting and internal statistics
            \code
            typedef cds::container::BasketQueue< cds::gc::HP, Foo,
                typename cds::container::basket_queue::make_traits<
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
    } // namespace basket_queue

    //@cond
    namespace details {
        template <typename GC, typename T, typename Traits>
        struct make_basket_queue
        {
            typedef GC gc;
            typedef T value_type;
            typedef Traits traits;

            struct node_type: public intrusive::basket_queue::node< gc >
            {
                value_type  m_value;

                node_type( const value_type& val )
                    : m_value( val )
                {}
                template <typename... Args>
                node_type( Args&&... args )
                    : m_value( std::forward<Args>(args)...)
                {}
            };

            typedef typename std::allocator_traits< typename traits::allocator >::template rebind_alloc< node_type > allocator_type;
            //typedef typename traits::allocator::template rebind<node_type>::other allocator_type;
            typedef cds::details::Allocator< node_type, allocator_type >           cxx_allocator;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };

            struct intrusive_traits : public traits
            {
                typedef cds::intrusive::basket_queue::base_hook< opt::gc<gc> > hook;
                typedef node_deallocator disposer;
                static constexpr const cds::intrusive::opt::link_check_type link_checker = cds::intrusive::basket_queue::traits::link_checker;
            };

            typedef cds::intrusive::BasketQueue< gc, node_type, intrusive_traits > type;
        };
    }
    //@endcond

    /// Basket lock-free queue (non-intrusive variant)
    /** @ingroup cds_nonintrusive_queue
        It is non-intrusive version of basket queue algorithm based on intrusive::BasketQueue counterpart.

        \par Source:
            [2007] Moshe Hoffman, Ori Shalev, Nir Shavit "The Baskets Queue"

        <b>Key idea</b>

        In the 'basket' approach, instead of
        the traditional ordered list of nodes, the queue consists of an ordered list of groups
        of nodes (logical baskets). The order of nodes in each basket need not be specified, and in
        fact, it is easiest to maintain them in LIFO order. The baskets fulfill the following basic
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

        In order to enqueue, just as in MSQueue, a thread first tries to link the new node to
        the last node. If it failed to do so, then another thread has already succeeded. Thus it
        tries to insert the new node into the new basket that was created by the winner thread.
        To dequeue a node, a thread first reads the head of the queue to obtain the
        oldest basket. It may then dequeue any node in the oldest basket.


        Template arguments:
        - \p GC - garbage collector type: \p gc::HP, \p gc::DHP
        - \p T - type of value to be stored in the queue
        - \p Traits - queue traits, default is \p basket_queue::traits. You can use \p basket_queue::make_traits
            metafunction to make your traits or just derive your traits from \p %basket_queue::traits:
            \code
            struct myTraits: public cds::container::basket_queue::traits {
                typedef cds::intrusive::basket_queue::stat<> stat;
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::container::BasketQueue< cds::gc::HP, Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::container::BasketQueue< cds::gc::HP, Foo,
                typename cds::container::basket_queue::make_traits<
                    cds::opt::stat< cds::container::basket_queue::stat<> >,
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode
    */
    template <typename GC, typename T, typename Traits = basket_queue::traits >
    class BasketQueue:
#ifdef CDS_DOXYGEN_INVOKED
        private intrusive::BasketQueue< GC, intrusive::basket_queue::node< T >, Traits >
#else
        protected details::make_basket_queue< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_basket_queue< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename GC2, typename T2, typename Traits2>
        struct rebind {
            typedef BasketQueue< GC2, T2, Traits2> other   ;   ///< Rebinding result
        };

    public:
        typedef GC gc;          ///< Garbage collector
        typedef T  value_type;  ///< Type of value to be stored in the queue
        typedef Traits traits;  ///< Queue's traits

        typedef typename base_class::back_off       back_off;       ///< Back-off strategy used
        typedef typename maker::allocator_type      allocator_type; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter   item_counter;   ///< Item counting policy used
        typedef typename base_class::stat           stat;           ///< Internal statistics policy used
        typedef typename base_class::memory_model   memory_model;   ///< Memory ordering. See cds::opt::memory_model option

        static constexpr const size_t c_nHazardPtrCount = base_class::c_nHazardPtrCount; ///< Count of hazard pointer required for the algorithm

    protected:
        typedef typename maker::node_type node_type; ///< queue node type (derived from intrusive::basket_queue::node)

        //@cond
        typedef typename maker::cxx_allocator     cxx_allocator;
        typedef typename maker::node_deallocator  node_deallocator;   // deallocate node
        typedef typename base_class::node_traits  node_traits;
        //@endcond

    protected:
        ///@cond
        static node_type * alloc_node()
        {
            return cxx_allocator().New();
        }
        static node_type * alloc_node( const value_type& val )
        {
            return cxx_allocator().New( val );
        }
        template <typename... Args>
        static node_type * alloc_node_move( Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<Args>( args )... );
        }
        static void free_node( node_type * p )
        {
            node_deallocator()( p );
        }

        struct node_disposer {
            void operator()( node_type * pNode )
            {
                free_node( pNode );
            }
        };
        typedef std::unique_ptr< node_type, node_disposer > scoped_node_ptr;
        //@endcond

    public:
        /// Initializes empty queue
        BasketQueue()
        {}

        /// Destructor clears the queue
        ~BasketQueue()
        {}

        /// Enqueues \p val value into the queue.
        /**
            The function makes queue node in dynamic memory calling copy constructor for \p val
            and then it calls \p intrusive::BasketQueue::enqueue().
            Returns \p true if success, \p false otherwise.
        */
        bool enqueue( value_type const& val )
        {
            scoped_node_ptr p( alloc_node(val));
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Enqueues \p val value into the queue, move semantics
        bool enqueue( value_type&& val )
        {
            scoped_node_ptr p( alloc_node_move( std::move( val )));
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Enqueues \p data to queue using a functor
        /**
            \p Func is a functor called to create node.
            The functor \p f takes one argument - a reference to a new node of type \ref value_type :
            \code
            cds::container::BasketQueue< cds::gc::HP, Foo > myQueue;
            Bar bar;
            myQueue.enqueue_with( [&bar]( Foo& dest ) { dest = bar; } );
            \endcode
        */
        template <typename Func>
        bool enqueue_with( Func f )
        {
            scoped_node_ptr p( alloc_node());
            f( p->m_value );
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Synonym for \p enqueue() function
        bool push( value_type const& val )
        {
            return enqueue( val );
        }

        /// Synonym for \p enqueue() function, move semantics
        bool push( value_type&& val )
        {
            return enqueue( std::move( val ));
        }

        /// Synonym for \p enqueue_with() function
        template <typename Func>
        bool push_with( Func f )
        {
            return enqueue_with( f );
        }

        /// Enqueues data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr p( alloc_node_move( std::forward<Args>(args)...));
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Dequeues a value from the queue
        /**
            If queue is not empty, the function returns \p true, \p dest contains copy of
            dequeued value. The assignment operator for \p value_type is invoked.
            If queue is empty, the function returns \p false, \p dest is unchanged.
        */
        bool dequeue( value_type& dest )
        {
            return dequeue_with( [&dest]( value_type& src ) {
                // TSan finds a race between this read of \p src and node_type constructor
                // I think, it is wrong
                CDS_TSAN_ANNOTATE_IGNORE_READS_BEGIN;
                dest = std::move( src );
                CDS_TSAN_ANNOTATE_IGNORE_READS_END;
            });
        }

        /// Dequeues a value using a functor
        /**
            \p Func is a functor called to copy dequeued value.
            The functor takes one argument - a reference to removed node:
            \code
            cds:container::BasketQueue< cds::gc::HP, Foo > myQueue;
            Bar bar;
            myQueue.dequeue_with( [&bar]( Foo& src ) { bar = std::move( src );});
            \endcode
            The functor is called only if the queue is not empty.
        */
        template <typename Func>
        bool dequeue_with( Func f )
        {
            typename base_class::dequeue_result res;
            if ( base_class::do_dequeue( res, true )) {
                f( node_traits::to_value_ptr( *res.pNext )->m_value );
                return true;
            }
            return false;
        }

        /// Synonym for \p dequeue() function
        bool pop( value_type& dest )
        {
            return dequeue( dest );
        }

        /// Synonym for \p dequeue_with() function
        template <typename Func>
        bool pop_with( Func f )
        {
            return dequeue_with( f );
        }

        /// Checks if the queue is empty
        /**
            Note that this function is not \p const.
            The function is based on \p dequeue() algorithm.
        */
        bool empty()
        {
            return base_class::empty();
        }

        /// Clear the queue
        /**
            The function repeatedly calls \ref dequeue until it returns \p nullptr.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Returns queue's item count
        /** \copydetails cds::intrusive::BasketQueue::size()
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns reference to internal statistics
        const stat& statistics() const
        {
            return base_class::statistics();
        }

    };

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_BASKET_QUEUE_H

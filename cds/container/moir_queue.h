// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_MOIR_QUEUE_H
#define CDSLIB_CONTAINER_MOIR_QUEUE_H

#include <memory>
#include <cds/container/msqueue.h>
#include <cds/intrusive/moir_queue.h>

namespace cds { namespace container {

    //@cond
    namespace details {
        template <typename GC, typename T, typename Traits>
        struct make_moir_queue: public cds::container::details::make_msqueue< GC, T, Traits >
        {
            typedef cds::container::details::make_msqueue< GC, T, Traits > base_class;
            typedef cds::intrusive::MoirQueue< GC, typename base_class::node_type, typename base_class::intrusive_traits > type;
        };
    }
    //@endcond

    /// A variation of Michael & Scott's lock-free queue
    /** @ingroup cds_nonintrusive_queue
        It is non-intrusive version of \p cds::intrusive::MoirQueue.

        Template arguments:
        - \p GC - garbage collector type: \p gc::HP, \p gc::DHP
        - \p T - a type stored in the queue.
        - \p Traits - queue traits, default is \p msqueue::traits. You can use \p msqueue::make_traits
            metafunction to make your traits or just derive your traits from \p %msqueue::traits:
            \code
            struct myTraits: public cds::container::msqueue::traits {
                typedef cds::intrusive::msqueue::stat<> stat;
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::container::MoirQueue< cds::gc::HP, Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::container::MoirQueue< cds::gc::HP, Foo,
                typename cds::container::msqueue::make_traits<
                    cds::opt::stat< cds::container::msqueue::stat<> >,
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode
    */
    template <typename GC, typename T, typename Traits = cds::container::msqueue::traits >
    class MoirQueue:
#ifdef CDS_DOXYGEN_INVOKED
        private intrusive::MoirQueue< GC, intrusive::msqueue::node< T >, Traits >
#else
        private details::make_moir_queue< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_moir_queue< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename GC2, typename T2, typename Traits2>
        struct rebind {
            typedef MoirQueue< GC2, T2, Traits2 > other   ;   ///< Rebinding result
        };

        static constexpr const size_t c_nHazardPtrCount = base_class::c_nHazardPtrCount; ///< Count of hazard pointer required for the algorithm

    public:
        typedef T value_type ; ///< Value type stored in the queue
        typedef typename base_class::gc                 gc;             ///< Garbage collector
        typedef typename base_class::back_off           back_off;       ///< Back-off strategy
        typedef typename maker::allocator_type          allocator_type; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter       item_counter;   ///< Item counting policy used
        typedef typename base_class::stat               stat;           ///< Internal statistics policy used
        typedef typename base_class::memory_model       memory_model;   ///< Memory ordering. See cds::opt::memory_model option

    protected:
        //@cond
        typedef typename maker::node_type  node_type;   ///< queue node type (derived from intrusive::msqueue::node)

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
        typedef std::unique_ptr< node_type, node_disposer >     scoped_node_ptr;
        //@endcond

    public:
        /// Initializes empty queue
        MoirQueue()
        {}

        /// Destructor clears the queue
        ~MoirQueue()
        {}

        /// Enqueues \p val value into the queue.
        /**
            The function makes queue node in dynamic memory calling copy constructor for \p val
            and then it calls \p intrusive::MoirQueue::enqueue.
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
            \p Func is a functor calling to create a new node.
            The functor should initialize creating node
            and it takes one argument - a reference to a new node of type \ref value_type :
            \code
            cds:container::MoirQueue< cds::gc::HP, Foo > myQueue;
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

        /// Enqueues data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr p( alloc_node_move( std::forward<Args>( args )... ));
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

        /// Dequeues a value from the queue
        /**
            If queue is not empty, the function returns \p true, \p dest contains copy of
            dequeued value. The assignment operator for type \ref value_type is invoked.
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
            cds:container::MoirQueue< cds::gc::HP, Foo > myQueue;
            Bar bar;
            myQueue.dequeue_with( [&bar]( Foo& src ) { bar = std::move( src );});
            \endcode
            The functor is called only if the queue is not empty.
        */
        template <typename Func>
        bool dequeue_with( Func f )
        {
            typename base_class::dequeue_result res;
            if ( base_class::do_dequeue( res )) {
                f( node_traits::to_value_ptr( *res.pNext )->m_value );
                base_class::dispose_result( res );
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

        /// Clear the queue
        /**
            The function repeatedly calls \ref dequeue until it returns \p nullptr.
            The disposer defined in template \p Traits is called for each item
            that can be safely disposed.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns queue's item count (see \ref intrusive::MSQueue::size for explanation)
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns refernce to internal statistics
        const stat& statistics() const
        {
            return base_class::statistics();
        }

    };

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_MOIR_QUEUE_H



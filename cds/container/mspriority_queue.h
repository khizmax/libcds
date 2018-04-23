// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_MSPRIORITY_QUEUE_H
#define CDSLIB_CONTAINER_MSPRIORITY_QUEUE_H

#include <memory>
#include <cds/container/details/base.h>
#include <cds/intrusive/mspriority_queue.h>

namespace cds { namespace container {

    /// MSPriorityQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace mspriority_queue {

#ifdef CDS_DOXYGEN_INVOKED
        /// Synonym for \p cds::intrusive::mspriority_queue::stat
        typedef cds::intrusive::mspriority_queue::stat<> stat;

        /// Synonym for \p cds::intrusive::mspriority_queue::empty_stat
        typedef cds::intrusive::mspriority_queue::empty_stat empty_stat;
#else
        using cds::intrusive::mspriority_queue::stat;
        using cds::intrusive::mspriority_queue::empty_stat;
#endif

        /// MSPriorityQueue traits
        /**
            The traits for \p %cds::container::MSPriorityQueue is the same as for
            \p cds::intrusive::MSPriorityQueue (see \p cds::intrusive::mspriority_queue::traits)
            plus some additional properties.
        */
        struct traits: public cds::intrusive::mspriority_queue::traits
        {
            /// The allocator use to allocate memory for values
            typedef CDS_DEFAULT_ALLOCATOR   allocator;

            /// Move policy
            /**
                The move policy used in \p MSPriorityQueue::pop() function to move item's value.
                Default is \p opt::v::assignment_move_policy.
            */
            typedef cds::opt::v::assignment_move_policy  move_policy;
        };

        /// Metafunction converting option list to traits
        /**
            \p Options are:
            - \p opt::buffer - the buffer type for heap array. Possible type are: \p opt::v::initiaized_static_buffer, \p opt::v::initialized_dynamic_buffer.
                Default is \p %opt::v::initialized_dynamic_buffer.
                You may specify any type of values for the buffer since at instantiation time
                the \p buffer::rebind member metafunction is called to change the type of values stored in the buffer.
            - \p opt::compare - priority compare functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for priority compare. Default is \p std::less<T>.
            - \p opt::lock_type - lock type. Default is \p cds::sync::spin.
            - \p opt::back_off - back-off strategy. Default is \p cds::backoff::yield
            - \p opt::allocator - allocator (like \p std::allocator) for the values of queue's items.
                Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::move_policy - policy for moving item's value. Default is \p opt::v::assignment_move_policy.
                If the compiler supports move semantics it would be better to specify the move policy
                based on the move semantics for type \p T.
            - \p opt::stat - internal statistics. Available types: \p mspriority_queue::stat, \p mspriority_queue::empty_stat (the default, no overhead)
            */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#   endif
        };
    }   // namespace mspriority_queue

    /// Michael & Scott array-based lock-based concurrent priority queue heap
    /** @ingroup cds_nonintrusive_priority_queue
        Source:
            - [1996] G.Hunt, M.Michael, S. Parthasarathy, M.Scott
                "An efficient algorithm for concurrent priority queue heaps"

        \p %MSPriorityQueue augments the standard array-based heap data structure with
        a mutual-exclusion lock on the heap's size and locks on each node in the heap.
        Each node also has a tag that indicates whether
        it is empty, valid, or in a transient state due to an update to the heap
        by an inserting thread.
        The algorithm allows concurrent insertions and deletions in opposite directions,
        without risking deadlock and without the need for special server threads.
        It also uses a "bit-reversal" technique to scatter accesses across the fringe
        of the tree to reduce contention.
        On large heaps the algorithm achieves significant performance improvements
        over serialized single-lock algorithm, for various insertion/deletion
        workloads. For small heaps it still performs well, but not as well as
        single-lock algorithm.

        Template parameters:
        - \p T - type to be stored in the list. The priority is a part of \p T type.
        - \p Traits - the traits. See \p mspriority_queue::traits for explanation.
             It is possible to declare option-based queue with \p mspriority_queue::make_traits
             metafunction instead of \p Traits template argument.
    */
    template <typename T, class Traits = mspriority_queue::traits >
    class MSPriorityQueue: protected cds::intrusive::MSPriorityQueue< T, Traits >
    {
        //@cond
        typedef cds::intrusive::MSPriorityQueue< T, Traits > base_class;
        //@endcond
    public:
        typedef T           value_type  ;   ///< Value type stored in the queue
        typedef Traits      traits      ;   ///< Traits template parameter

        typedef typename base_class::key_comparator key_comparator; ///< priority comparing functor based on opt::compare and opt::less option setter.
        typedef typename base_class::lock_type lock_type;   ///< heap's size lock type
        typedef typename base_class::back_off  back_off ;   ///< Back-off strategy
        typedef typename traits::stat          stat;        ///< internal statistics type, see \p intrusive::mspriority_queue::traits::stat
        typedef typename base_class::item_counter  item_counter;///< Item counter type
        typedef typename std::allocator_traits<typename traits::allocator>::template rebind_alloc<value_type> allocator_type; ///< Value allocator
        typedef typename traits::move_policy   move_policy; ///< Move policy for type \p T

    protected:
        //@cond
        typedef cds::details::Allocator< value_type, allocator_type >  cxx_allocator;

        struct value_deleter {
            void operator()( value_type * p ) const
            {
                cxx_allocator().Delete( p );
            }
        };
        typedef std::unique_ptr<value_type, value_deleter> scoped_ptr;
        //@endcond

    public:
        /// Constructs empty priority queue
        /**
            For \p cds::opt::v::initialized_static_buffer the \p nCapacity parameter is ignored.
        */
        MSPriorityQueue( size_t nCapacity )
            : base_class( nCapacity )
        {}

        /// Clears priority queue and destructs the object
        ~MSPriorityQueue()
        {
            clear();
        }

        /// Inserts an item into priority queue
        /**
            If the priority queue is full, the function returns \p false,
            no item has been added.
            Otherwise, the function inserts the copy of \p val into the heap
            and returns \p true.

            The function use copy constructor to create new heap item from \p val.
        */
        bool push( value_type const& val )
        {
            scoped_ptr pVal( cxx_allocator().New( val ));
            if ( base_class::push( *(pVal.get()))) {
                pVal.release();
                return true;
            }
            return false;
        }

        /// Inserts an item into the queue using a functor
        /**
            \p Func is a functor called to create node.
            The functor \p f takes one argument - a reference to a new node of type \ref value_type :
            \code
            cds::container::MSPriorityQueue< Foo > myQueue;
            Bar bar;
            myQueue.push_with( [&bar]( Foo& dest ) { dest = bar; } );
            \endcode
        */
        template <typename Func>
        bool push_with( Func f )
        {
            scoped_ptr pVal( cxx_allocator().New());
            f( *pVal );
            if ( base_class::push( *pVal )) {
                pVal.release();
                return true;
            }
            return false;
        }

        /// Inserts a item into priority queue
        /**
            If the priority queue is full, the function returns \p false,
            no item has been added.
            Otherwise, the function inserts a new item created from \p args arguments
            into the heap and returns \p true.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_ptr pVal( cxx_allocator().MoveNew( std::forward<Args>(args)... ));
            if ( base_class::push( *(pVal.get()))) {
                pVal.release();
                return true;
            }
            return false;
        }

        /// Extracts item with high priority
        /**
            If the priority queue is empty, the function returns \p false.
            Otherwise, it returns \p true and \p dest contains the copy of extracted item.
            The item is deleted from the heap.

            The function uses \ref move_policy to move extracted value from the heap's top
            to \p dest.

            The function is equivalent of such call:
            \code
                pop_with( dest, [&dest]( value_type& src ) { move_policy()(dest, src); } );
            \endcode
        */
        bool pop( value_type& dest )
        {
            return pop_with( [&dest]( value_type& src ) { move_policy()(dest, std::move(src)); });
        }

        /// Extracts an item with high priority
        /**
            If the priority queue is empty, the function returns \p false.
            Otherwise, it returns \p true and \p dest contains the copy of extracted item.
            The item is deleted from the heap.

            \p Func is a functor called to copy popped value.
            The functor takes one argument - a reference to removed node:
            \code
            cds:container::MSPriorityQueue< Foo > myQueue;
            Bar bar;
            myQueue.pop_with( [&bar]( Foo& src ) { bar = std::move( src );});
            \endcode
        */
        template <typename Func>
        bool pop_with( Func f )
        {
            value_type * pVal = base_class::pop();
            if ( pVal ) {
                f( *pVal );
                cxx_allocator().Delete( pVal );
                return true;
            }
            return false;
        }

        /// Clears the queue (not atomic)
        /**
            This function is not atomic, but thread-safe
        */
        void clear()
        {
            base_class::clear_with( []( value_type& src ) { value_deleter()(&src); } );
        }

        /// Clears the queue (not atomic)
        /**
            This function is not atomic, but thread-safe.

            For each item removed the functor \p f is called.
            \p Func interface is:
            \code
                struct clear_functor
                {
                    void operator()( value_type& item );
                };
            \endcode
        */
        template <typename Func>
        void clear_with( Func f )
        {
            base_class::clear_with( [&f]( value_type& val ) { f(val); value_deleter()( &val ); } );
        }

        /// Checks is the priority queue is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Checks if the priority queue is full
        bool full() const
        {
            return base_class::full();
        }

        /// Returns current size of priority queue
        size_t size() const
        {
            return base_class::size();
        }

        /// Return capacity of the priority queue
        size_t capacity() const
        {
            return base_class::capacity();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_MSPRIORITY_QUEUE_H

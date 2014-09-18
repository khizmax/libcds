//$$CDS-header$$

#ifndef __CDS_CONTAINER_MSPRIORITY_QUEUE_H
#define __CDS_CONTAINER_MSPRIORITY_QUEUE_H

#include <cds/container/base.h>
#include <cds/intrusive/mspriority_queue.h>
#include <cds/details/std/memory.h>

namespace cds { namespace container {

    /// MSPriorityQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace mspriority_queue {

#ifdef CDS_DOXYGEN_INVOKED
        /// Synonym for cds::intrusive::mspriority_queue::stat
        typedef cds::intrusive::mspriority_queue::stat<> stat;

        /// Synonym for cds::intrusive::mspriority_queue::empty_stat
        typedef cds::intrusive::mspriority_queue::empty_stat empty_stat;
#else
        using cds::intrusive::mspriority_queue::stat;
        using cds::intrusive::mspriority_queue::empty_stat;
#endif

        /// Type traits for MSPriorityQueue
        /**
            The type traits for cds::container::MSPriorityQueue is the same as for
            cds::intrusive::MSPriorityQueue (see cds::intrusive::mspriority_queue::type_traits)
            plus some additional properties.
        */
        struct type_traits: public cds::intrusive::mspriority_queue::type_traits
        {
            /// The allocator use to allocate memory for values
            typedef CDS_DEFAULT_ALLOCATOR   allocator;

            /// Move policy
            /**
                The move policy used in MSPriorityQueue::pop functions
                to move item's value.
                Default is opt::v::assignment_move_policy.
            */
            typedef cds::opt::v::assignment_move_policy  move_policy;
        };

        /// Metafunction converting option list to traits
        /**
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>

            See \ref MSPriorityQueue, \ref type_traits, \ref cds::opt::make_options.
        */
        template <CDS_DECL_OPTIONS9>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< type_traits, CDS_OPTIONS9 >::type
                ,CDS_OPTIONS9
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
        - \p Traits - type traits. See mspriority_queue::type_traits for explanation.

        It is possible to declare option-based queue with cds::container::mspriority_queue::make_traits
        metafunction instead of \p Traits template argument.
        Template argument list \p Options of \p %cds::container::mspriority_queue::make_traits metafunction are:
        - opt::buffer - the buffer type for heap array. Possible type are: opt::v::static_buffer, opt::v::dynamic_buffer.
            Default is \p %opt::v::dynamic_buffer.
            You may specify any type of values for the buffer since at instantiation time
            the \p buffer::rebind member metafunction is called to change the type of values stored in the buffer.
        - opt::compare - priority compare functor. No default functor is provided.
            If the option is not specified, the opt::less is used.
        - opt::less - specifies binary predicate used for priority compare. Default is \p std::less<T>.
        - opt::lock_type - lock type. Default is cds::lock::Spin.
        - opt::back_off - back-off strategy. Default is cds::backoff::yield
        - opt::allocator - allocator (like \p std::allocator) for the values of queue's items.
            Default is \ref CDS_DEFAULT_ALLOCATOR
        - opt::move_policy - policy for moving item's value. Default is opt::v::assignment_move_policy.
            If the compiler supports move semantics it would be better to specify the move policy
            based on the move semantics for type \p T.
        - opt::stat - internal statistics. Available types: mspriority_queue::stat, mspriority_queue::empty_stat (the default)
    */
    template <typename T, class Traits>
    class MSPriorityQueue: protected cds::intrusive::MSPriorityQueue< T, Traits >
    {
        //@cond
        typedef cds::intrusive::MSPriorityQueue< T, Traits > base_class;
        //@endcond
    public:
        typedef T           value_type  ;   ///< Value type stored in the queue
        typedef Traits      traits      ;   ///< Traits template parameter

        typedef typename base_class::key_comparator key_comparator; ///< priority comparing functor based on opt::compare and opt::less option setter.
        typedef typename base_class::lock_type lock_type; ///< heap's size lock type
        typedef typename base_class::back_off  back_off ; ///< Back-off strategy
        typedef typename base_class::stat          stat ; ///< internal statistics type
        typedef typename traits::allocator::template rebind<value_type>::other allocator_type; ///< Value allocator
        typedef typename traits::move_policy move_policy; ///< Move policy for type \p T

    protected:
        //@cond
        typedef cds::details::Allocator< value_type, allocator_type >  cxx_allocator;

        struct value_deleter {
            void operator()( value_type * p ) const
            {
                cxx_allocator().Delete( p );
            }
#       ifndef CDS_CXX11_LAMBDA_SUPPORT
            void operator()( value_type& p ) const
            {
                cxx_allocator().Delete( &p );
            }
#       endif
        };
        typedef std::unique_ptr<value_type, value_deleter> scoped_ptr;

#   ifndef CDS_CXX11_LAMBDA_SUPPORT
        template <typename Func>
        struct clear_wrapper
        {
            Func& func;
            clear_wrapper( Func& f ): func(f) {}

            void operator()( value_type& src ) const
            {
                cds::unref(func)( src );
                value_deleter()( &src );
            }
        };
#   endif

        //@endcond

    public:
        /// Constructs empty priority queue
        /**
            For cds::opt::v::static_buffer the \p nCapacity parameter is ignored.
        */
        MSPriorityQueue( size_t nCapacity )
            : base_class( nCapacity )
        {}

        /// Clears priority queue and destructs the object
        ~MSPriorityQueue()
        {
            clear();
        }

        /// Inserts a item into priority queue
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
            if ( base_class::push( *(pVal.get()) )) {
                pVal.release();
                return true;
            }
            return false;
        }

#ifdef CDS_EMPLACE_SUPPORT
        /// Inserts a item into priority queue
        /**
            If the priority queue is full, the function returns \p false,
            no item has been added.
            Otherwise, the function inserts a new item created from \p args arguments
            into the heap and returns \p true.

            The function is available only for compilers supporting variable template
            and move semantics C++11 feature.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_ptr pVal( cxx_allocator().MoveNew( std::forward<Args>(args)... ));
            if ( base_class::push( *(pVal.get()) )) {
                pVal.release();
                return true;
            }
            return false;
        }
#endif

        /// Extracts item with high priority
        /**
            If the priority queue is empty, the function returns \p false.
            Otherwise, it returns \p true and \p dest contains the copy of extracted item.
            The item is deleted from the heap.

            The function uses \ref move_policy to move extracted value from the heap's top
            to \p dest.

            The function is equivalent of such call:
            \code
                pop_with( dest, move_policy() );
            \endcode
        */
        bool pop( value_type& dest )
        {
            return pop_with( dest, move_policy() );
        }

        /// Extracts item with high priority
        /**
            If the priority queue is empty, the function returns \p false.
            Otherwise, it returns \p true and \p dest contains the copy of extracted item.
            The item is deleted from the heap.

            The function uses \p MoveFunc \p f to move extracted value from the heap's top
            to \p dest. The interface of \p MoveFunc is:
            \code
            struct move_functor {
                void operator()( Q& dest, T& src );
            };
            \endcode
            In \p MoveFunc you may use move semantics for \p src argument
            since \p src will be destroyed.
        */
        template <typename Q, typename MoveFunc>
        bool pop_with( Q& dest, MoveFunc f )
        {
            value_type * pVal = base_class::pop();
            if ( pVal ) {
                cds::unref(f)( dest, *pVal );
                cxx_allocator().Delete( pVal );
                return true;
            }
            return false;
        }

        /// Clears the queue (not atomic)
        /**
            This function is no atomic, but thread-safe
        */
        void clear()
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            base_class::clear_with( []( value_type& src ) { cxx_allocator().Delete( &src ); });
#       else
            base_class::clear_with( value_deleter() );
#       endif
        }

        /// Clears the queue (not atomic)
        /**
            This function is no atomic, but thread-safe.

            For each item removed the functor \p f is called.
            \p Func interface is:
            \code
                struct clear_functor
                {
                    void operator()( value_type& item );
                };
            \endcode
            A lambda function or a function pointer can be used as \p f.
        */
        template <typename Func>
        void clear_with( Func f )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            base_class::clear_with( [&f]( value_type& val ) { cds::unref(f)(val); value_deleter()( &val ); } );
#       else
            clear_wrapper<Func> c(f);
            base_class::clear_with( cds::ref(c));
#       endif
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

#endif // #ifndef __CDS_CONTAINER_MSPRIORITY_QUEUE_H

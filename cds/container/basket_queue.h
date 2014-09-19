//$$CDS-header$$

#ifndef __CDS_CONTAINER_BASKET_QUEUE_H
#define __CDS_CONTAINER_BASKET_QUEUE_H

#include <memory>
#include <cds/intrusive/basket_queue.h>
#include <cds/container/base.h>
#include <cds/ref.h>
#include <cds/details/trivial_assign.h>


namespace cds { namespace container {

    //@cond
    namespace details {
        template <typename GC, typename T, CDS_DECL_OPTIONS7>
        struct make_basket_queue
        {
            typedef GC gc;
            typedef T value_type;

            struct default_options {
                typedef cds::backoff::empty     back_off;
                typedef CDS_DEFAULT_ALLOCATOR   allocator;
                typedef atomicity::empty_item_counter item_counter;
                typedef intrusive::basket_queue::dummy_stat stat;
                typedef opt::v::relaxed_ordering    memory_model;
                enum { alignment = opt::cache_line_alignment };
            };

            typedef typename opt::make_options<
                typename cds::opt::find_type_traits< default_options, CDS_OPTIONS7 >::type
                ,CDS_OPTIONS7
            >::type   options;

            struct node_type: public intrusive::basket_queue::node< gc >
            {
                value_type  m_value;

                node_type( const value_type& val )
                    : m_value( val )
                {}
#           ifdef CDS_EMPLACE_SUPPORT
                template <typename... Args>
                node_type( Args&&... args )
                    : m_value( std::forward<Args>(args)...)
                {}
#           else
                node_type()
                {}
#           endif
            };

            typedef typename options::allocator::template rebind<node_type>::other allocator_type;
            typedef cds::details::Allocator< node_type, allocator_type >           cxx_allocator;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };

            typedef intrusive::BasketQueue< gc,
                node_type
                ,intrusive::opt::hook<
                    intrusive::basket_queue::base_hook< opt::gc<gc> >
                >
                ,opt::back_off< typename options::back_off >
                ,intrusive::opt::disposer< node_deallocator >
                ,opt::item_counter< typename options::item_counter >
                ,opt::stat< typename options::stat >
                ,opt::alignment< options::alignment >
                ,opt::memory_model< typename options::memory_model >
            >   type;
        };
    }
    //@endcond

    /// Basket lock-free queue (non-intrusive variant)
    /** @ingroup cds_nonintrusive_queue
        It is non-intrusive version of basket queue algorithm based on intrusive::BasketQueue counterpart.

        \par Source:
            [2007] Moshe Hoffman, Ori Shalev, Nir Shavit "The Baskets Queue"

        <b>Key idea</b>

        In the “basket” approach, instead of
        the traditional ordered list of nodes, the queue consists of an ordered list of groups
        of nodes (logical baskets). The order of nodes in each basket need not be specified, and in
        fact, it is easiest to maintain them in LIFO order. The baskets fulfill the following basic
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


        Template arguments:
        - \p GC - garbage collector type: gc::HP, gc::HRC, gc::PTB
        - \p T is a type stored in the queue. It should be default-constructible, copy-constructible, assignable type.
        - \p Options - options

        Permissible \p Options:
        - opt::allocator - allocator (like \p std::allocator). Default is \ref CDS_DEFAULT_ALLOCATOR
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter
        - opt::stat - the type to gather internal statistics for debugging and profiling purposes.
            Possible option value are: intrusive::basket_queue::stat, intrusive::basket_queue::dummy_stat (the default),
            user-provided class that supports intrusive::basket_queue::stat interface.
            Generic option intrusive::queue_stat and intrusive::queue_dummy_stat are acceptable too, however,
            they will be automatically converted to intrusive::basket_queue::stat and intrusive::basket_queue::dummy_stat
            respectively.
        - opt::alignment - the alignment for internal queue data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).
    */
    template <typename GC, typename T, CDS_DECL_OPTIONS7>
    class BasketQueue:
#ifdef CDS_DOXYGEN_INVOKED
        intrusive::BasketQueue< GC, intrusive::basket_queue::node< T >, Options... >
#else
        details::make_basket_queue< GC, T, CDS_OPTIONS7 >::type
#endif
    {
        //@cond
        typedef details::make_basket_queue< GC, T, CDS_OPTIONS7 > options;
        typedef typename options::type base_class;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename GC2, typename T2, CDS_DECL_OTHER_OPTIONS7>
        struct rebind {
            typedef BasketQueue< GC2, T2, CDS_OTHER_OPTIONS7> other   ;   ///< Rebinding result
        };

    public:
        typedef T value_type ; ///< Value type stored in the queue

        typedef typename base_class::gc                 gc              ; ///< Garbage collector used
        typedef typename base_class::back_off           back_off        ; ///< Back-off strategy used
        typedef typename options::allocator_type        allocator_type  ; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter       item_counter    ; ///< Item counting policy used
        typedef typename base_class::stat               stat            ; ///< Internal statistics policy used
        typedef typename base_class::memory_model       memory_model    ; ///< Memory ordering. See cds::opt::memory_model option

    protected:
        typedef typename options::node_type  node_type   ;   ///< queue node type (derived from intrusive::single_link::node)

        //@cond
        typedef typename options::cxx_allocator     cxx_allocator;
        typedef typename options::node_deallocator  node_deallocator;   // deallocate node
        typedef typename base_class::node_traits    node_traits;
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
#   ifdef CDS_EMPLACE_SUPPORT
        template <typename... Args>
        static node_type * alloc_node_move( Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<Args>( args )... );
        }
#   endif
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
        BasketQueue()
        {}

        /// Destructor clears the queue
        ~BasketQueue()
        {}

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

        /// Enqueues \p val value into the queue.
        /**
            The function makes queue node in dynamic memory calling copy constructor for \p val
            and then it calls intrusive::BasketQueue::enqueue.
            Returns \p true if success, \p false otherwise.
        */
        bool enqueue( const value_type& val )
        {
            scoped_node_ptr p( alloc_node(val));
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Enqueues \p data to queue using copy functor
        /**
            \p Func is a functor called to copy value \p data of type \p Type
            which may be differ from type \p T stored in the queue.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(T& dest, Type const& data)
                {
                    // // Code to copy \p data to \p dest
                    dest = data;
                }
            };
            \endcode
            You may use \p boost:ref construction to pass functor \p f by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Type, typename Func>
        bool enqueue( const Type& data, Func f  )
        {
            scoped_node_ptr p( alloc_node());
            cds::unref(f)( p->m_value, data );
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

#   ifdef CDS_EMPLACE_SUPPORT
        /// Enqueues data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            This function is available only for compiler that supports
            variadic template and move semantics
        */
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
#   endif


        /// Dequeues a value using copy functor
        /**
            \p Func is a functor called to copy dequeued value to \p dest of type \p Type
            which may be differ from type \p T stored in the queue.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(Type& dest, T const& data)
                {
                    // Code to copy \p data to \p dest
                    dest = data;
                }
            };
            \endcode
            You may use \p boost:ref construction to pass functor \p f by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Type, typename Func>
        bool dequeue( Type& dest, Func f )
        {
            typename base_class::dequeue_result res;
            if ( base_class::do_dequeue( res, true )) {
                cds::unref(f)( dest, node_traits::to_value_ptr( *res.pNext )->m_value );
                return true;
            }
            return false;
        }

        /// Dequeues a value from the queue
        /**
            If queue is not empty, the function returns \p true, \p dest contains copy of
            dequeued value. The assignment operator for type \ref value_type is invoked.
            If queue is empty, the function returns \p false, \p dest is unchanged.
        */
        bool dequeue( value_type& dest )
        {
            typedef cds::details::trivial_assign<value_type, value_type> functor;
            return dequeue( dest, functor() );
        }

        /// Synonym for \ref enqueue function
        bool push( const value_type& val )
        {
            return enqueue( val );
        }

        /// Synonym for template version of \ref enqueue function
        template <typename Type, typename Func>
        bool push( const Type& data, Func f  )
        {
            return enqueue( data, f );
        }

        /// Synonym for \ref dequeue function
        bool pop( value_type& dest )
        {
            return dequeue( dest );
        }

        /// Synonym for template version of \ref dequeue function
        template <typename Type, typename Func>
        bool pop( Type& dest, Func f )
        {
            return dequeue( dest, f );
        }

        /// Checks if the queue is empty
        /**
            Note that this function is not \p const.
            The function is based on \ref dequeue algorithm.
        */
        bool empty()
        {
            return base_class::empty();
        }

        /// Clear the queue
        /**
            The function repeatedly calls \ref dequeue until it returns NULL.
        */
        void clear()
        {
            base_class::clear();
        }
    };

}}  // namespace cds::container

#endif  // #ifndef __CDS_CONTAINER_BASKET_QUEUE_H

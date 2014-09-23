//$$CDS-header$$

#ifndef __CDS_CONTAINER_TSIGAS_CYCLE_QUEUE_H
#define __CDS_CONTAINER_TSIGAS_CYCLE_QUEUE_H

#include <memory>
#include <cds/intrusive/tsigas_cycle_queue.h>
#include <cds/container/base.h>
#include <cds/details/trivial_assign.h>

namespace cds { namespace container {

    //@cond
    namespace details {
        template <typename T, CDS_DECL_OPTIONS7>
        struct make_tsigas_cycle_queue
        {
            typedef T value_type;

            struct default_options {
                typedef cds::backoff::empty     back_off;
                typedef CDS_DEFAULT_ALLOCATOR   allocator;
                typedef atomicity::empty_item_counter item_counter;
                typedef opt::v::relaxed_ordering    memory_model;
                enum { alignment = opt::cache_line_alignment };
            };

            typedef typename opt::make_options<
                typename cds::opt::find_type_traits< default_options, CDS_OPTIONS7 >::type
                ,CDS_OPTIONS7
            >::type   options;

            typedef typename options::allocator::template rebind<value_type>::other allocator_type;
            typedef cds::details::Allocator< value_type, allocator_type >           cxx_allocator;

            struct node_deallocator
            {
                void operator ()( value_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };
            typedef node_deallocator node_disposer;

            typedef intrusive::TsigasCycleQueue<
                value_type
                ,opt::buffer< typename options::buffer >
                ,opt::back_off< typename options::back_off >
                ,intrusive::opt::disposer< node_disposer >
                ,opt::item_counter< typename options::item_counter >
                ,opt::alignment< options::alignment >
                ,opt::memory_model< typename options::memory_model >
            >   type;
        };

    }
    //@endcond

    /// Non-blocking cyclic queue discovered by Philippas Tsigas and Yi Zhang
    /** @ingroup cds_nonintrusive_queue
        It is non-intrusive implementation of Tsigas & Zhang cyclic queue based on intrusive::TsigasCycleQueue.

        Source:
        \li [2000] Philippas Tsigas, Yi Zhang "A Simple, Fast and Scalable Non-Blocking Concurrent FIFO Queue
            for Shared Memory Multiprocessor Systems"

        \p T is a type stored in the queue. It should be default-constructible, copy-constructible, assignable type.

        Available \p Options:
        - opt::buffer - buffer to store items. Mandatory option, see option description for full list of possible types.
        - opt::allocator - allocator (like \p std::allocator). Default is \ref CDS_DEFAULT_ALLOCATOR
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used.
        - opt::alignment - the alignment for internal queue data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).

        \par Examples:
        \code
        #include <cds/container/tsigas_cycle_queue.h>

        struct Foo {
            ...
        };

        // Queue of Foo, capacity is 1024, statically allocated buffer:
        typedef cds::intrusive::TsigasCycleQueue<
            Foo
            ,cds::opt::buffer< cds::opt::v::static_buffer< Foo, 1024 > >
        > static_queue;
        static_queue    stQueue;

        // Queue of Foo, capacity is 1024, dynamically allocated buffer:
        typedef cds::intrusive::TsigasCycleQueue<
            Foo
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer< Foo > >
        > dynamic_queue;
        dynamic_queue    dynQueue( 1024 );
        \endcode
    */
    template <typename T, CDS_DECL_OPTIONS7>
    class TsigasCycleQueue:
#ifdef CDS_DOXYGEN_INVOKED
        intrusive::TsigasCycleQueue< T, Options... >
#else
        details::make_tsigas_cycle_queue< T, CDS_OPTIONS7 >::type
#endif
    {
        //@cond
        typedef details::make_tsigas_cycle_queue< T, CDS_OPTIONS7 > options;
        typedef typename options::type base_class;
        //@endcond
    public:
        typedef T value_type ; ///< Value type stored in the stack

        typedef typename base_class::back_off           back_off        ; ///< Back-off strategy used
        typedef typename options::allocator_type        allocator_type  ; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename options::options::item_counter item_counter    ; ///< Item counting policy used
        typedef typename base_class::memory_model       memory_model    ; ///< Memory ordering. See cds::opt::memory_model option

        /// Rebind template arguments
        template <typename T2, CDS_DECL_OTHER_OPTIONS7>
        struct rebind {
            typedef TsigasCycleQueue< T2, CDS_OTHER_OPTIONS7> other   ;   ///< Rebinding result
        };

    protected:
        //@cond
        typedef typename options::cxx_allocator     cxx_allocator;
        typedef typename options::node_deallocator  node_deallocator;   // deallocate node
        typedef typename options::node_disposer     node_disposer;
        //@endcond

    protected:
        ///@cond
        static value_type * alloc_node()
        {
            return cxx_allocator().New();
        }
        static value_type * alloc_node( const value_type& val )
        {
            return cxx_allocator().New( val );
        }
        template <typename... Args>
        static value_type * alloc_node_move( Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<Args>( args )... );
        }
        static void free_node( value_type * p )
        {
            node_deallocator()( p );
        }

        struct node_disposer2 {
            void operator()( value_type * pNode )
            {
                free_node( pNode );
            }
        };
        typedef std::unique_ptr< value_type, node_disposer2 >     scoped_node_ptr;
        //@endcond

    public:
        /// Initialize empty queue of capacity \p nCapacity
        /**
            For cds::opt::v::static_buffer the \p nCapacity parameter is ignored.

            Note that the real capacity of queue is \p nCapacity - 2.
        */
        TsigasCycleQueue( size_t nCapacity = 0 )
            : base_class( nCapacity )
        {}

        /// Returns queue's item count (see \ref intrusive::TsigasCycleQueue::size for explanation)
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns capacity of cyclic buffer
        /**
            Warning: real capacity of queue is two less than returned value of this function.
        */
        size_t capacity() const
        {
            return base_class::capacity();
        }

        /// Enqueues \p val value into the queue.
        /**
            The function makes queue node in dynamic memory calling copy constructor for \p val
            and then it calls intrusive::TsigasCycleQueue::enqueue.
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

        /// Enqueues \p data to queue using copy functor
        /**
            \p Func is a functor called to copy value \p data of type \p Type
            which may be differ from type \p T stored in the queue.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(T& dest, SOURCE const& data)
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
            unref(f)( *p, data );
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
            scoped_node_ptr p ( alloc_node_move( std::forward<Args>(args)...));
            if ( base_class::enqueue( *p)) {
                p.release();
                return true;
            }
            return false;
        }

        /// Dequeues a value using copy functor
        /**
            \p Func is a functor called to copy dequeued value to \p dest of type \p Type
            which may be differ from type \p T stored in the queue.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(Type& dest, T const& data)
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
        bool dequeue( Type& dest, Func f )
        {
            value_type * p = base_class::dequeue();
            if ( p ) {
                unref(f)( dest, *p );
                node_disposer()( p );
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
        bool empty() const
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
    };

}} // namespace cds::intrusive

#endif // #ifndef __CDS_CONTAINER_TSIGAS_CYCLE_QUEUE_H

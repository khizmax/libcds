/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#ifndef CDSLIB_CONTAINER_TSIGAS_CYCLE_QUEUE_H
#define CDSLIB_CONTAINER_TSIGAS_CYCLE_QUEUE_H

#include <memory>
#include <cds/intrusive/tsigas_cycle_queue.h>
#include <cds/container/details/base.h>

namespace cds { namespace container {

    /// TsigasCycleQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace tsigas_queue {

        /// TsigasCycleQueue default traits
        struct traits
        {
            /// Buffer type for cyclic array
            /*
                The type of element for the buffer is not important: the queue rebinds
                buffer for required type via \p rebind metafunction.

                For \p TsigasCycleQueue queue the buffer size should have power-of-2 size.

                You should use any initialized buffer type, see \p opt::buffer.
            */
            typedef cds::opt::v::initialized_dynamic_buffer< void * > buffer;

            /// Node allocator
            typedef CDS_DEFAULT_ALLOCATOR       allocator;

            /// Back-off strategy
            typedef cds::backoff::empty         back_off;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter item_counter;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering    memory_model;

            /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
            enum { padding = opt::cache_line_padding };
        };

        /// Metafunction converting option list to \p tsigas_queue::traits
        /**
            Supported \p Options are:
            - \p opt::buffer - the buffer type for internal cyclic array. Possible types are:
                \p opt::v::initialized_dynamic_buffer (the default), \p opt::v::initialized_static_buffer. The type of
                element in the buffer is not important: it will be changed via \p rebind metafunction.
            - \p opt::allocator - allocator (like \p std::allocator) used for allocating queue items. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::back_off - back-off strategy used, default is \p cds::backoff::empty.
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).

            Example: declare \p %TsigasCycleQueue with item counting and static iternal buffer of size 1024:
            \code
            typedef cds::container::TsigasCycleQueue< Foo,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::initialized_static_buffer< void *, 1024 >,
                    cds::opt::item_counter< cds::atomicity::item_counter >
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

    } // namespace tsigas_queue

    //@cond
    namespace details {
        template <typename T, typename Traits>
        struct make_tsigas_cycle_queue
        {
            typedef T value_type;
            typedef Traits traits;

            typedef typename traits::allocator::template rebind<value_type>::other allocator_type;
            typedef cds::details::Allocator< value_type, allocator_type >           cxx_allocator;

            struct node_deallocator
            {
                void operator ()( value_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };
            typedef node_deallocator node_disposer;

            struct intrusive_traits: public traits
            {
                typedef node_deallocator disposer;
            };

            typedef intrusive::TsigasCycleQueue< value_type, intrusive_traits > type;
        };
    } // namespace
    //@endcond

    /// Non-blocking cyclic bounded queue
    /** @ingroup cds_nonintrusive_queue
        It is non-intrusive implementation of Tsigas & Zhang cyclic queue based on \p intrusive::TsigasCycleQueue.

        Source:
        - [2000] Philippas Tsigas, Yi Zhang "A Simple, Fast and Scalable Non-Blocking Concurrent FIFO Queue
            for Shared Memory Multiprocessor Systems"

        Template arguments:
        - \p T is a type stored in the queue.
        - \p Traits - queue traits, default is \p tsigas_queue::traits. You can use \p tsigas_queue::make_traits
            metafunction to make your traits or just derive your traits from \p %tsigas_queue::traits:
            \code
            struct myTraits: public cds::container::tsigas_queue::traits {
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::container::TsigasCycleQueue< Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::container::TsigasCycleQueue< cds::gc::HP, Foo,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode

        \par Examples:
        \code
        #include <cds/container/tsigas_cycle_queue.h>

        struct Foo {
            ...
        };

        // Queue of Foo, capacity is 1024, statically allocated buffer:
        typedef cds::container::TsigasCycleQueue< Foo,
            typename cds::container::tsigas_queue::make_traits<
                cds::opt::buffer< cds::opt::v::initialized_static_buffer< Foo, 1024 > >
            >::type
        > static_queue;
        static_queue    stQueue;

        // Queue of Foo, capacity is 1024, dynamically allocated buffer:
        typedef cds::container::TsigasCycleQueue< Foo
            typename cds::container::tsigas_queue::make_traits<
                cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< Foo > >
            >::type
        > dynamic_queue;
        dynamic_queue    dynQueue( 1024 );
        \endcode
    */
    template <typename T, typename Traits = tsigas_queue::traits>
    class TsigasCycleQueue:
#ifdef CDS_DOXYGEN_INVOKED
        intrusive::TsigasCycleQueue< T, Traits >
#else
        details::make_tsigas_cycle_queue< T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_tsigas_cycle_queue< T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond
    public:
        typedef T value_type ;  ///< Value type stored in the stack
        typedef Traits traits;  ///< Queue traits

        typedef typename traits::back_off       back_off;       ///< Back-off strategy used
        typedef typename maker::allocator_type  allocator_type; ///< Allocator type used for allocate/deallocate the items
        typedef typename traits::item_counter   item_counter;   ///< Item counting policy used
        typedef typename traits::memory_model   memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option

        /// Rebind template arguments
        template <typename T2, typename Traits2>
        struct rebind {
            typedef TsigasCycleQueue< T2, Traits2> other   ;   ///< Rebinding result
        };

    protected:
        //@cond
        typedef typename maker::cxx_allocator     cxx_allocator;
        typedef typename maker::node_deallocator  node_deallocator;   // deallocate node
        typedef typename maker::node_disposer     node_disposer;
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
        typedef std::unique_ptr< value_type, node_disposer2 > scoped_node_ptr;
        //@endcond

    public:
        /// Initialize empty queue of capacity \p nCapacity
        /**
            If internal buffer type is \p cds::opt::v::initialized_static_buffer, the \p nCapacity parameter is ignored.

            Note, the real capacity of queue is \p nCapacity - 2.
        */
        TsigasCycleQueue( size_t nCapacity = 0 )
            : base_class( nCapacity )
        {}

        /// Enqueues \p val value into the queue.
        /**
            The function makes queue node in dynamic memory calling copy constructor for \p val
            and then it calls \p intrusive::TsigasCycleQueue::enqueue.

            Returns \p true if success, \p false if the queue is full.
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
            if ( base_class::enqueue( *p ) ) {
                p.release();
                return true;
            }
            return false;
        }

        /// Enqueues data to the queue using a functor
        /**
            \p Func is a functor called to create node.
            The functor \p f takes one argument - a reference to a new node of type \ref value_type :
            \code
            cds::container::TsigasCysleQueue< Foo > myQueue;
            Bar bar;
            myQueue.enqueue_with( [&bar]( Foo& dest ) { dest = bar; } );
            \endcode
        */
        template <typename Func>
        bool enqueue_with( Func f )
        {
            scoped_node_ptr p( alloc_node() );
            f( *p );
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

        /// Synonym for \p enqueue( value_type const& )
        bool push( value_type const& data )
        {
            return enqueue( data );
        }

        /// Synonym for \p enqueue( value_type&& )
        bool push( value_type&& data )
        {
            return enqueue( std::move( data ));
        }

        /// Synonym for \p enqueue_with() function
        template <typename Func>
        bool push_with( Func f )
        {
            return enqueue_with( f );
        }

        /// Dequeues a value using a functor
        /**
            \p Func is a functor called to copy dequeued value.
            The functor takes one argument - a reference to removed node:
            \code
            cds:container::TsigasCycleQueue< Foo > myQueue;
            Bar bar;
            myQueue.dequeue_with( [&bar]( Foo& src ) { bar = std::move( src );});
            \endcode
            The functor is called only if the queue is not empty.
        */
        template <typename Func>
        bool dequeue_with( Func f )
        {
            value_type * p = base_class::dequeue();
            if ( p ) {
                f( *p );
                free_node( p );
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
            return dequeue_with( [&dest]( value_type& src ) { dest = std::move( src );});
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
        bool empty() const
        {
            return base_class::empty();
        }

        /// Clear the queue
        /**
            The function repeatedly calls \p dequeue() until it returns \p nullptr.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Returns queue's item count
        /** \copydetails cds::intrusive::TsigasCycleQueue::size()
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns capacity of cyclic buffer
        size_t capacity() const
        {
            return base_class::capacity();
        }
    };

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_CONTAINER_TSIGAS_CYCLE_QUEUE_H

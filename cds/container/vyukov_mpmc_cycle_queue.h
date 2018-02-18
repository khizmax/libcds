// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_VYUKOV_MPMC_CYCLE_QUEUE_H
#define CDSLIB_CONTAINER_VYUKOV_MPMC_CYCLE_QUEUE_H

#include <cds/container/details/base.h>
#include <cds/opt/buffer.h>
#include <cds/opt/value_cleaner.h>
#include <cds/algo/atomic.h>
#include <cds/details/bounded_container.h>

namespace cds { namespace container {

    /// VyukovMPMCCycleQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace vyukov_queue {

        /// VyukovMPMCCycleQueue default traits
        struct traits {
            /// Buffer type for internal array
            /*
                The type of element for the buffer is not important: the queue rebinds
                the buffer for required type via \p rebind metafunction.

                For \p VyukovMPMCCycleQueue queue the buffer size should have power-of-2 size.

                You should use only uninitialized buffer for the queue -
                \p cds::opt::v::uninitialized_dynamic_buffer (the default),
                \p cds::opt::v::uninitialized_static_buffer.
            */
            typedef cds::opt::v::uninitialized_dynamic_buffer< void * > buffer;

            /// A functor to clean item dequeued.
            /**
                The functor calls the destructor for queue item.
                After an item is dequeued, \p value_cleaner cleans the cell that the item has been occupied.
                If \p T is a complex type, \p value_cleaner may be useful feature.

                Default value is \ref opt::v::auto_cleaner
            */
            typedef cds::opt::v::auto_cleaner value_cleaner;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef cds::atomicity::empty_item_counter item_counter;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consistent memory model).
            */
            typedef opt::v::relaxed_ordering    memory_model;

            /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
            enum { padding = opt::cache_line_padding };

            /// Back-off strategy
            typedef cds::backoff::Default           back_off;

            /// Single-consumer version
            /**
                For single-consumer version of algorithm some additional functions
                (\p front(), \p pop_front()) is available.

                Default is \p false
            */
            static constexpr bool const single_consumer = false;
        };

        /// Metafunction converting option list to \p vyukov_queue::traits
        /**
            Supported \p Options are:
            - \p opt::buffer - an uninitialized buffer type for internal cyclic array. Possible types are:
                \p opt::v::uninitialized_dynamic_buffer (the default), \p opt::v::uninitialized_static_buffer. The type of
                element in the buffer is not important: it will be changed via \p rebind metafunction.
            - \p opt::value_cleaner - a functor to clean item dequeued.
                The functor calls the destructor for queue item.
                After an item is dequeued, \p value_cleaner cleans the cell that the item has been occupied.
                If \p T is a complex type, \p value_cleaner can be an useful feature.
                Default value is \ref opt::v::auto_cleaner
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).

            Example: declare \p %VyukovMPMCCycleQueue with item counting and static iternal buffer of size 1024:
            \code
            typedef cds::container::VyukovMPMCCycleQueue< Foo,
                typename cds::container::vyukov_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::uninitialized_static_buffer< void *, 1024 >,
                    cds::opt::item_counte< cds::atomicity::item_counter >
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

    } //namespace vyukov_queue

    /// Vyukov's MPMC bounded queue
    /** @ingroup cds_nonintrusive_queue
        This algorithm is developed by Dmitry Vyukov (see http://www.1024cores.net)
        It's multi-producer multi-consumer (MPMC), array-based, fails on overflow, does not require GC, w/o priorities, causal FIFO,
        blocking producers and consumers queue. The algorithm is pretty simple and fast. It's not lock-free in the official meaning,
        just implemented by means of atomic RMW operations w/o mutexes.

        The cost of enqueue/dequeue is 1 CAS per operation.
        No dynamic memory allocation/management during operation. Producers and consumers are separated from each other (as in the two-lock queue),
        i.e. do not touch the same data while queue is not empty.

        There is multiple producer/single consumer version \p cds::container::VyukovMPSCCycleQueue
        that supports \p front() and \p pop_front() functions.

        Source:
            - http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue

        Template parameters
        - \p T - type stored in queue.
        - \p Traits - queue traits, default is \p vyukov_queue::traits. You can use \p vyukov_queue::make_traits
            metafunction to make your traits or just derive your traits from \p %vyukov_queue::traits:
            \code
            struct myTraits: public cds::container::vyukov_queue::traits {
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::container::VyukovMPMCCycleQueue< Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::container::VyukovMPMCCycleQueue< cds::gc::HP, Foo,
                typename cds::container::vyukov_queue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode

        \par License
            Simplified BSD license by Dmitry Vyukov (http://www.1024cores.net/site/1024cores/home/code-license)
    */
    template <typename T, typename Traits = vyukov_queue::traits >
    class VyukovMPMCCycleQueue : public cds::bounded_container
    {
    public:
        typedef T value_type;   ///< Value type to be stored in the queue
        typedef Traits traits;  ///< Queue traits
        typedef typename traits::item_counter  item_counter;  ///< Item counter type
        typedef typename traits::memory_model  memory_model;  ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename traits::value_cleaner value_cleaner; ///< Value cleaner, see \p vyukov_queue::traits::value_cleaner
        typedef typename traits::back_off  back_off;          ///< back-off strategy

        /// \p true for single-consumer version, \p false otherwise
        static constexpr bool const c_single_consumer = traits::single_consumer;

        /// Rebind template arguments
        template <typename T2, typename Traits2>
        struct rebind {
            typedef VyukovMPMCCycleQueue< T2, Traits2 > other   ;   ///< Rebinding result
        };

    protected:
        //@cond
        typedef atomics::atomic<size_t> sequence_type;
        struct cell_type
        {
            sequence_type   sequence;
            value_type      data;

            cell_type()
            {}
        };

        typedef typename traits::buffer::template rebind<cell_type>::other buffer;
        //@endcond

    protected:
        //@cond
        buffer          m_buffer;
        size_t const    m_nBufferMask;
        typename opt::details::apply_padding< size_t, traits::padding >::padding_type pad1_;
        sequence_type   m_posEnqueue;
        typename opt::details::apply_padding< sequence_type, traits::padding >::padding_type pad2_;
        sequence_type   m_posDequeue;
        typename opt::details::apply_padding< sequence_type, traits::padding >::padding_type pad3_;
        item_counter    m_ItemCounter;
        //@endcond

    public:
        /// Constructs the queue of capacity \p nCapacity
        /**
            For \p cds::opt::v::uninitialized_static_buffer the \p nCapacity parameter is ignored.

            The buffer capacity must be the power of two.
        */
        VyukovMPMCCycleQueue(
            size_t nCapacity = 0
            )
            : m_buffer( nCapacity )
            , m_nBufferMask( m_buffer.capacity() - 1 )
        {
            nCapacity = m_buffer.capacity();

            // Buffer capacity must be power of 2
            assert( nCapacity >= 2 && (nCapacity & (nCapacity - 1)) == 0 );

            for (size_t i = 0; i != nCapacity; ++i )
                m_buffer[i].sequence.store(i, memory_model::memory_order_relaxed);

            m_posEnqueue.store(0, memory_model::memory_order_relaxed);
            m_posDequeue.store(0, memory_model::memory_order_relaxed);
        }

        ~VyukovMPMCCycleQueue()
        {
            clear();
        }

        /// Enqueues data to the queue using a functor
        /**
            \p Func is a functor called to copy a value to the queue cell.
            The functor \p f takes one argument - a reference to a empty cell of type \ref value_type :
            \code
            cds::container::VyukovMPMCCycleQueue< Foo > myQueue;
            Bar bar;
            myQueue.enqueue_with( [&bar]( Foo& dest ) { dest = std::move(bar); } );
            \endcode
        */
        template <typename Func>
        bool enqueue_with(Func f)
        {
            cell_type* cell;
            back_off bkoff;

            size_t pos = m_posEnqueue.load(memory_model::memory_order_relaxed);
            for (;;)
            {
                cell = &m_buffer[pos & m_nBufferMask];
                size_t seq = cell->sequence.load(memory_model::memory_order_acquire);

                intptr_t dif = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

                if (dif == 0) {
                    if ( m_posEnqueue.compare_exchange_weak(pos, pos + 1, memory_model::memory_order_relaxed, atomics::memory_order_relaxed ))
                        break;
                }
                else if (dif < 0) {
                    // Queue full?
                    if ( pos - m_posDequeue.load( memory_model::memory_order_relaxed ) == capacity())
                        return false;   // queue full
                    bkoff();
                    pos = m_posEnqueue.load( memory_model::memory_order_relaxed );
                }
                else
                    pos = m_posEnqueue.load(memory_model::memory_order_relaxed);
            }

            f( cell->data );

            cell->sequence.store(pos + 1, memory_model::memory_order_release);
            ++m_ItemCounter;

            return true;
        }

        /// Enqueues \p val value into the queue.
        /**
            The new queue item is created by calling placement new in free cell.
            Returns \p true if success, \p false if the queue is full.
        */
        bool enqueue( value_type const& val )
        {
            return enqueue_with( [&val]( value_type& dest ){ new ( &dest ) value_type( val ); });
        }

        /// Enqueues \p val value into the queue, move semantics
        bool enqueue( value_type&& val )
        {
            return enqueue_with( [&val]( value_type& dest ) { new (&dest) value_type( std::move( val ));});
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

        /// Synonym for \p enqueue_with()
        template <typename Func>
        bool push_with( Func f )
        {
            return enqueue_with( f );
        }

        /// Enqueues data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        template <typename... Args>
        bool emplace( Args&&... args )
        {
#if (CDS_COMPILER == CDS_COMPILER_GCC) && (CDS_COMPILER_VERSION < 40900)
            //work around unsupported feature in g++ 4.8 for forwarding parameter packs to lambda.
            value_type val( std::forward<Args>(args)... );
            return enqueue_with( [&val]( value_type& dest ){ new ( &dest ) value_type( std::move( val )); });
#else
            return enqueue_with( [&args ...]( value_type& dest ){ new ( &dest ) value_type( std::forward<Args>( args )... ); });
#endif
        }

        /// Dequeues a value using a functor
        /**
            \p Func is a functor called to copy dequeued value.
            The functor takes one argument - a reference to removed node:
            \code
            cds:container::VyukovMPMCCycleQueue< Foo > myQueue;
            Bar bar;
            myQueue.dequeue_with( [&bar]( Foo& src ) { bar = std::move( src );});
            \endcode
            The functor is called only if the queue is not empty.
        */
        template <typename Func>
        bool dequeue_with( Func f )
        {
            cell_type * cell;
            back_off bkoff;

            size_t pos = m_posDequeue.load( memory_model::memory_order_relaxed );
            for (;;)
            {
                cell = &m_buffer[pos & m_nBufferMask];
                size_t seq = cell->sequence.load(memory_model::memory_order_acquire);
                intptr_t dif = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

                if (dif == 0) {
                    if ( m_posDequeue.compare_exchange_weak(pos, pos + 1, memory_model::memory_order_relaxed, atomics::memory_order_relaxed))
                        break;
                }
                else if (dif < 0) {
                    // Queue empty?
                    if ( pos - m_posEnqueue.load( memory_model::memory_order_relaxed ) == 0 )
                        return false;   // queue empty
                    bkoff();
                    pos = m_posDequeue.load( memory_model::memory_order_relaxed );
                }
                else
                    pos = m_posDequeue.load(memory_model::memory_order_relaxed);
            }

            f( cell->data );
            value_cleaner()( cell->data );
            cell->sequence.store( pos + m_nBufferMask + 1, memory_model::memory_order_release );
            --m_ItemCounter;

            return true;
        }

        /// Dequeues a value from the queue
        /**
            If queue is not empty, the function returns \p true, \p dest contains a copy of
            dequeued value. The assignment operator for type \ref value_type is invoked.
            If queue is empty, the function returns \p false, \p dest is unchanged.
        */
        bool dequeue(value_type& dest )
        {
            return dequeue_with( [&dest]( value_type& src ){ dest = std::move( src );});
        }

        /// Synonym for \p dequeue()
        bool pop(value_type& data)
        {
            return dequeue(data);
        }

        /// Synonym for \p dequeue_with()
        template <typename Func>
        bool pop_with( Func f )
        {
            return dequeue_with( f );
        }

        /// Returns a pointer to top element of the queue or \p nullptr if queue is empty (only for single-consumer version)
        template <bool SC = c_single_consumer >
        typename std::enable_if<SC, value_type *>::type front()
        {
            static_assert( c_single_consumer, "front() is enabled only if traits::single_consumer is true");

            cell_type * cell;
            back_off bkoff;

            size_t pos = m_posDequeue.load( memory_model::memory_order_relaxed );
            for ( ;;)
            {
                cell = &m_buffer[pos & m_nBufferMask];
                size_t seq = cell->sequence.load( memory_model::memory_order_acquire );
                intptr_t dif = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

                if ( dif == 0 )
                    return &cell->data;
                else if ( dif < 0 ) {
                    // Queue empty?
                    if ( pos - m_posEnqueue.load( memory_model::memory_order_relaxed ) == 0 )
                        return nullptr;   // queue empty
                    bkoff();
                    pos = m_posDequeue.load( memory_model::memory_order_relaxed );
                }
                else
                    pos = m_posDequeue.load( memory_model::memory_order_relaxed );
            }
        }

        /// Pops top element; returns \p true if queue is not empty, \p false otherwise (only for single-consumer version)
        template <bool SC = c_single_consumer >
        typename std::enable_if<SC, bool>::type pop_front()
        {
            return dequeue_with( []( value_type& ) {} );
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            const cell_type * cell;
            back_off bkoff;

            size_t pos = m_posDequeue.load(memory_model::memory_order_relaxed);
            for (;;)
            {
                cell = &m_buffer[pos & m_nBufferMask];
                size_t seq = cell->sequence.load(memory_model::memory_order_acquire);
                intptr_t dif = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

                if (dif == 0)
                    return false;
                else if (dif < 0) {
                    if ( pos - m_posEnqueue.load( memory_model::memory_order_relaxed ) == 0 )
                        return true;
                }
                bkoff();
                pos = m_posDequeue.load(memory_model::memory_order_relaxed);
            }
        }

        /// Clears the queue
        void clear()
        {
            value_type v;
            while ( pop(v));
        }

        /// Returns queue's item count
        /**
            The value returned depends on \p vyukov_queue::traits::item_counter option.
            For \p atomicity::empty_item_counter, the function always returns 0.
        */
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns capacity of the queue
        size_t capacity() const
        {
            return m_buffer.capacity();
        }
    };

    //@cond
    namespace vyukov_queue {

        template <typename Traits>
        struct single_consumer_traits : public Traits
        {
            static constexpr bool const single_consumer = true;
        };
    } // namespace vyukov_queue
    //@endcond

    /// Vyukov's queue multiple producer - single consumer version
    template <typename T, typename Traits = vyukov_queue::traits >
    using VyukovMPSCCycleQueue = VyukovMPMCCycleQueue< T, vyukov_queue::single_consumer_traits<Traits> >;

}}  // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_VYUKOV_MPMC_CYCLE_QUEUE_H

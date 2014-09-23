//$$CDS-header$$

#ifndef __CDS_CONTAINER_VYUKOV_MPMC_CYCLE_QUEUE_H
#define __CDS_CONTAINER_VYUKOV_MPMC_CYCLE_QUEUE_H

#include <cds/container/base.h>
#include <cds/opt/buffer.h>
#include <cds/opt/value_cleaner.h>
#include <cds/cxx11_atomic.h>
#include <cds/ref.h>
#include <cds/details/trivial_assign.h>
#include <cds/details/bounded_container.h>

namespace cds { namespace container {

    /// Vyukov's MPMC bounded queue
    /** @ingroup cds_nonintrusive_queue
        This algorithm is developed by Dmitry Vyukov (see http://www.1024cores.net)
        It's multi-producer multi-consumer (MPMC), array-based, fails on overflow, does not require GC, w/o priorities, causal FIFO,
        blocking producers and consumers queue. The algorithm is pretty simple and fast. It's not lock-free in the official meaning,
        just implemented by means of atomic RMW operations w/o mutexes.

        The cost of enqueue/dequeue is 1 CAS per operation.
        No dynamic memory allocation/management during operation. Producers and consumers are separated from each other (as in the two-lock queue),
        i.e. do not touch the same data while queue is not empty.

        \par Source:
            http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue

        \par Template parameters
            \li \p T - type stored in queue.
            \li \p Options - queue's options

        Options \p Options are:
        - opt::buffer - buffer to store items. Mandatory option, see option description for full list of possible types.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter
        - opt::value_cleaner - a functor to clean item dequeued. Default value is \ref opt::v::destruct_cleaner
            that calls the destructor of type \p T.
            After an item is dequeued, \p value_cleaner cleans the cell that the item has been occupied. If \p T
            is a complex type, \p value_cleaner may be the useful feature.
        - opt::alignment - the alignment for internal queue data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).

        \par License
            Simplified BSD license by Dmitry Vyukov (http://www.1024cores.net/site/1024cores/home/code-license)

        \par Example
        \code
        #include <cds/container/vyukov_mpmc_cycle_queue.h>

        // // Queue with 1024 item static buffer
        cds::container::vyukov_mpmc_bounded<
            int
            ,cds::opt::buffer< cds::opt::v::static_buffer<int, 1024> >
        > myQueue;
        \endcode
    */
    template <typename T, CDS_DECL_OPTIONS6>
    class VyukovMPMCCycleQueue
        : public cds::bounded_container
    {
    protected:
        //@cond
        struct default_options
        {
            typedef cds::opt::v::destruct_cleaner  value_cleaner;
            typedef atomicity::empty_item_counter item_counter;
            typedef opt::v::empty_disposer      disposer    ;   // for intrusive version only
            typedef opt::v::relaxed_ordering    memory_model;
            enum { alignment = opt::cache_line_alignment };
        };
        //@endcond

    public:
        //@cond
        typedef typename opt::make_options<
            typename cds::opt::find_type_traits< default_options, CDS_OPTIONS6 >::type
            ,CDS_OPTIONS6
        >::type   options;
        //@endcond

    protected:
        //@cond
        typedef typename options::value_cleaner  value_cleaner;
        //@endcond

    public:
        typedef T value_type    ;   ///< @anchor cds_container_VyukovMPMCCycleQueue_value_type type of value stored in the queue
        typedef typename options::item_counter  item_counter ;  ///< Item counter type
        typedef typename options::memory_model  memory_model ;  ///< Memory ordering. See cds::opt::memory_model option

        /// Rebind template arguments
        template <typename T2, CDS_DECL_OTHER_OPTIONS6>
        struct rebind {
            typedef VyukovMPMCCycleQueue< T2, CDS_OTHER_OPTIONS6> other   ;   ///< Rebinding result
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

#   ifndef CDS_CXX11_LAMBDA_SUPPORT
        struct copy_construct {
            void operator()( value_type& dest, value_type const& src )
            {
                new ( &dest ) value_type( src );
            }
        };
#   endif

        typedef cds::details::trivial_assign< value_type, value_type > copy_assign;

        typedef typename options::buffer::template rebind<cell_type>::other buffer;
        typedef typename opt::details::alignment_setter< sequence_type, options::alignment >::type aligned_sequence_type;
        typedef typename opt::details::alignment_setter< buffer, options::alignment >::type aligned_buffer;
        //@endcond

    protected:
        //@cond
        aligned_buffer  m_buffer;
        size_t const    m_nBufferMask;
        aligned_sequence_type m_posEnqueue;
        aligned_sequence_type m_posDequeue;
        item_counter    m_ItemCounter;
        //@endcond

    public:
        /// Constructs the queue of capacity \p nCapacity
        /**
            For cds::opt::v::static_buffer the \p nCapacity parameter is ignored.
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

            for (size_t i = 0; i != nCapacity; i += 1)
                m_buffer[i].sequence.store(i, memory_model::memory_order_relaxed);

            m_posEnqueue.store(0, memory_model::memory_order_relaxed);
            m_posDequeue.store(0, memory_model::memory_order_relaxed);
        }

        ~VyukovMPMCCycleQueue()
        {
            clear();
        }

        /// Enqueues \p data to queue using copy functor
        /** @anchor cds_container_VyukovMPMCCycleQueue_enqueue_func
            \p Func is a functor called to copy value \p data of type \p Source
            which may be differ from type \p T stored in the queue.
            The functor's interface is:
            \code
                struct myFunctor {
                    void operator()(T& dest, Source const& data)
                    {
                        // // Code to copy \p data to \p dest
                        dest = data;
                    }
                };
            \endcode
            You may use \p boost:ref construction to pass functor \p f by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Source, typename Func>
        bool enqueue(Source const& data, Func func)
        {
            cell_type* cell;
            size_t pos = m_posEnqueue.load(memory_model::memory_order_relaxed);

            for (;;)
            {
                cell = &m_buffer[pos & m_nBufferMask];
                size_t seq = cell->sequence.load(memory_model::memory_order_acquire);

                intptr_t dif = (intptr_t)seq - (intptr_t)pos;

                if (dif == 0)
                {
                    if ( m_posEnqueue.compare_exchange_weak(pos, pos + 1, memory_model::memory_order_relaxed) )
                        break;
                }
                else if (dif < 0)
                    return false;
                else
                    pos = m_posEnqueue.load(memory_model::memory_order_relaxed);
            }

            unref(func)( cell->data, data );

            cell->sequence.store(pos + 1, memory_model::memory_order_release);
            ++m_ItemCounter;

            return true;
        }

        /// @anchor cds_container_VyukovMPMCCycleQueue_enqueue Enqueues \p data to queue
        bool enqueue(value_type const& data )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return enqueue( data, [](value_type& dest, value_type const& src){ new ( &dest ) value_type( src ); });
#       else
            return enqueue( data, copy_construct() );
#       endif
        }

        /// Enqueues data of type \ref cds_container_VyukovMPMCCycleQueue_value_type "value_type" constructed with <tt>std::forward<Args>(args)...</tt>
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            cell_type* cell;
            size_t pos = m_posEnqueue.load(memory_model::memory_order_relaxed);

            for (;;)
            {
                cell = &m_buffer[pos & m_nBufferMask];
                size_t seq = cell->sequence.load(memory_model::memory_order_acquire);

                intptr_t dif = (intptr_t)seq - (intptr_t)pos;

                if (dif == 0)
                {
                    if ( m_posEnqueue.compare_exchange_weak(pos, pos + 1, memory_model::memory_order_relaxed) )
                        break;
                }
                else if (dif < 0)
                    return false;
                else
                    pos = m_posEnqueue.load(memory_model::memory_order_relaxed);
            }

            new ( &cell->data ) value_type( std::forward<Args>(args)... );

            cell->sequence.store(pos + 1, memory_model::memory_order_release);
            ++m_ItemCounter;

            return true;
        }

        /// Dequeues an item from queue
        /** @anchor cds_container_VyukovMPMCCycleQueue_dequeue_func
            \p Func is a functor called to copy dequeued value of type \p T to \p dest of type \p Dest.
            The functor's interface is:
            \code
            struct myFunctor {
            void operator()(Dest& dest, T const& data)
            {
                // // Code to copy \p data to \p dest
                dest = data;
            }
            };
            \endcode
            You may use \p boost:ref construction to pass functor \p func by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Dest, typename Func>
        bool dequeue( Dest& data, Func func )
        {
            cell_type * cell;
            size_t pos = m_posDequeue.load(memory_model::memory_order_relaxed);

            for (;;)
            {
                cell = &m_buffer[pos & m_nBufferMask];
                size_t seq = cell->sequence.load(memory_model::memory_order_acquire);
                intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);

                if (dif == 0) {
                    if ( m_posDequeue.compare_exchange_weak(pos, pos + 1, memory_model::memory_order_relaxed))
                        break;
                }
                else if (dif < 0)
                    return false;
                else
                    pos = m_posDequeue.load(memory_model::memory_order_relaxed);
            }

            unref(func)( data, cell->data );
            value_cleaner()( cell->data );
            --m_ItemCounter;
            cell->sequence.store( pos + m_nBufferMask + 1, memory_model::memory_order_release );

            return true;
        }

        /// Dequeues an item from queue to \p data
        /** @anchor cds_container_VyukovMPMCCycleQueue_dequeue
            If queue is empty, returns \p false, \p data is unchanged.
        */
        bool dequeue(value_type & data )
        {
            return dequeue( data, copy_assign() );
        }

        /// Synonym of \ref cds_container_VyukovMPMCCycleQueue_enqueue "enqueue"
        bool push(value_type const& data)
        {
            return enqueue(data);
        }

        /// Synonym for template version of \ref cds_container_VyukovMPMCCycleQueue_enqueue_func "enqueue" function
        template <typename Source, typename Func>
        bool push( const Source& data, Func f  )
        {
            return enqueue( data, f );
        }

        /// Synonym of \ref cds_container_VyukovMPMCCycleQueue_dequeue "dequeue"
        bool pop(value_type& data)
        {
            return dequeue(data);
        }

        /// Synonym for template version of \ref cds_container_VyukovMPMCCycleQueue_dequeue_func "dequeue" function
        template <typename Type, typename Func>
        bool pop( Type& dest, Func f )
        {
            return dequeue( dest, f );
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            const cell_type * cell;
            size_t pos = m_posDequeue.load(memory_model::memory_order_relaxed);

            for (;;)
            {
                cell = &m_buffer[pos & m_nBufferMask];
                size_t seq = cell->sequence.load(memory_model::memory_order_acquire);
                intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);

                if (dif == 0)
                    return false;
                else if (dif < 0)
                    return true;
                else
                    pos = m_posDequeue.load(memory_model::memory_order_relaxed);
            }
        }

        /// Clears the queue
        void clear()
        {
            value_type v;
            while ( pop(v) );
        }

        /// Returns queue's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.
        */
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns capacity of cyclic buffer
        size_t capacity() const
        {
            return m_buffer.capacity();
        }
    };
}}  // namespace cds::container

#endif // #ifndef __CDS_CONTAINER_VYUKOV_MPMC_CYCLE_QUEUE_H

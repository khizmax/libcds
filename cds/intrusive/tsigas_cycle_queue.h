//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_TSIGAS_CYCLE_QUEUE_H
#define __CDS_INTRUSIVE_TSIGAS_CYCLE_QUEUE_H

#include <functional>   // ref
#include <cds/intrusive/details/base.h>
#include <cds/cxx11_atomic.h>
#include <cds/details/bounded_container.h>
#include <cds/opt/buffer.h>

namespace cds { namespace intrusive {

    /// Non-blocking cyclic queue discovered by Philippas Tsigas and Yi Zhang
    /** @ingroup cds_intrusive_queue

        Source:
        \li [2000] Philippas Tsigas, Yi Zhang "A Simple, Fast and Scalable Non-Blocking Concurrent FIFO Queue
            for Shared Memory Multiprocessor Systems"

        Template arguments:
        - T - data stored in queue. The queue stores pointers to passed data of type \p T.
            <b>Restriction</b>: the queue can manage at least two-byte aligned data: the least significant bit (LSB)
            of any pointer stored in the queue must be zero since the algorithm may use LSB
            as a flag that marks the free cell.
        - Options - options

        \p Options are:
        - opt::buffer - buffer to store items. Mandatory option, see option description for full list of possible types.
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer. This option is used
            only in \ref clear function.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used.
        - opt::alignment - the alignment for internal queue data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).

        This queue algorithm does not require any garbage collector.

        \par Examples:
        \code
        #include <cds/intrusive/tsigas_cycle_queue.h>

        struct Foo {
            ...
        };

        // Queue of Foo pointers, capacity is 1024, statically allocated buffer:
        typedef cds::intrusive::TsigasCycleQueue<
            Foo
            ,cds::opt::buffer< cds::opt::v::static_buffer< Foo, 1024 > >
        > static_queue;
        static_queue    stQueue;

        // Queue of Foo pointers, capacity is 1024, dynamically allocated buffer:
        typedef cds::intrusive::TsigasCycleQueue<
            Foo
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer< Foo > >
        > dynamic_queue;
        dynamic_queue    dynQueue( 1024 );
        \endcode
    */
    template <typename T, typename... Options>
    class TsigasCycleQueue: public cds::bounded_container
    {
        //@cond
        struct default_options
        {
            typedef cds::backoff::empty         back_off;
            typedef opt::v::empty_disposer      disposer;
            typedef atomicity::empty_item_counter item_counter;
            typedef opt::v::relaxed_ordering    memory_model;
            enum { alignment = opt::cache_line_alignment };
        };
        //@endcond

    public:
        //@cond
        typedef typename opt::make_options<
            typename cds::opt::find_type_traits< default_options, Options...>::type
            ,Options...
        >::type   options;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename T2, typename... Options2>
        struct rebind {
            typedef TsigasCycleQueue< T2, Options2...> other   ;   ///< Rebinding result
        };

    public:
        typedef T value_type    ;   ///< type of value stored in the queue
        typedef typename options::item_counter  item_counter;   ///< Item counter type
        typedef typename options::disposer      disposer    ;   ///< Item disposer
        typedef typename options::back_off      back_off    ;   ///< back-off strategy used
        typedef typename options::memory_model  memory_model;   ///< Memory ordering. See cds::opt::memory_model option

    protected:
        //@cond
        typedef typename options::buffer::template rebind< atomics::atomic<value_type *> >::other buffer;
        typedef typename opt::details::alignment_setter< buffer, options::alignment >::type aligned_buffer;
        typedef size_t index_type;
        typedef typename opt::details::alignment_setter< atomics::atomic<index_type>, options::alignment >::type aligned_index;
        //@endcond

    protected:
        //@cond
        buffer          m_buffer    ;   ///< array of pointer T *, array size is equal to m_nCapacity+1
        aligned_index   m_nHead     ;   ///< index of queue's head
        aligned_index   m_nTail     ;   ///< index of queue's tail
        item_counter    m_ItemCounter   ;   ///< item counter
        //@endcond

    protected:
        //@cond
        static CDS_CONSTEXPR value_type * free0() CDS_NOEXCEPT
        {
            return nullptr;
        }
        static CDS_CONSTEXPR value_type * free1() CDS_NOEXCEPT
        {
            return (value_type*) 1;
        }
        static bool is_free( const value_type * p ) CDS_NOEXCEPT
        {
            return p == free0() || p == free1();
        }

        size_t buffer_capacity() const CDS_NOEXCEPT
        {
            return m_buffer.capacity();
        }

        index_type modulo() const CDS_NOEXCEPT
        {
            return buffer_capacity() - 1;
        }
        //@endcond

    public:
        /// Initialize empty queue of capacity \p nCapacity
        /**
            For cds::opt::v::static_buffer the \p nCapacity parameter is ignored.

            Note that the real capacity of queue is \p nCapacity - 2.
        */
        TsigasCycleQueue( size_t nCapacity = 0 )
            : m_buffer( nCapacity )
            , m_nHead(0)
            , m_nTail(1)
        {
            m_buffer.zeroize();
        }

        /// Clears the queue
        ~TsigasCycleQueue()
        {
            clear();
        }

        /// Returns queue's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.
        */
        size_t size() const CDS_NOEXCEPT
        {
            return m_ItemCounter.value();
        }

        /// Returns capacity of cyclic buffer
        size_t capacity() const CDS_NOEXCEPT
        {
            return buffer_capacity() - 2;
        }

        /// Enqueues item from the queue
        /** @anchor cds_intrusive_TsigasQueue_enqueue
            Returns \p true if success, \p false otherwise (for example, if queue is full)
        */
        bool enqueue( value_type& data )
        {
            value_type * pNewNode  = &data;
            assert( (reinterpret_cast<ptr_atomic_t>( pNewNode ) & 1) == 0 );
            back_off bkoff;

            const index_type nModulo = modulo();

            do {
                index_type te = m_nTail.load(memory_model::memory_order_acquire);
                index_type ate = te;
                value_type * tt = m_buffer[ ate ].load(memory_model::memory_order_relaxed);
                index_type temp = ( ate + 1 ) & nModulo ;    // next item after tail

                // Looking for actual tail
                while ( !is_free( tt ) ) {
                    if ( te != m_nTail.load(memory_model::memory_order_relaxed) )    // check the tail consistency
                        goto TryAgain;
                    if ( temp == m_nHead.load(memory_model::memory_order_acquire) )    // queue full?
                        break;
                    tt = m_buffer[ temp ].load(memory_model::memory_order_relaxed);
                    ate = temp;
                    temp = (temp + 1) & nModulo;
                }

                if ( te != m_nTail.load(memory_model::memory_order_relaxed) )
                    continue;

                // Check whether queue is full
                if ( temp == m_nHead.load(memory_model::memory_order_acquire) ) {
                    ate = ( temp + 1 ) & nModulo;
                    tt = m_buffer[ ate ].load(memory_model::memory_order_relaxed);
                    if ( !is_free( tt ) ) {
                        return false    ;    // Queue is full
                    }

                    // help the dequeue to update head
                    m_nHead.compare_exchange_strong( temp, ate, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    continue;
                }

                if ( tt == free1() )
                    pNewNode = reinterpret_cast<value_type *>(reinterpret_cast<intptr_t>( pNewNode ) | 1);
                if ( te != m_nTail.load(memory_model::memory_order_relaxed) )
                    continue;

                // get actual tail and try to enqueue new node
                if ( m_buffer[ate].compare_exchange_strong( tt, pNewNode, memory_model::memory_order_release, atomics::memory_order_relaxed ) ) {
                    if ( temp % 2 == 0 )
                        m_nTail.compare_exchange_strong( te, temp, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    ++m_ItemCounter;
                    return true;
                }
            TryAgain:;
            } while ( bkoff(), true );

            // No control path reaches this line!
            return false;
        }

        /// Dequeues item from the queue
        /** @anchor cds_intrusive_TsigasQueue_dequeue
            If the queue is empty the function returns \p nullptr

            Dequeue does not call value disposer. You can manually dispose returned value if it is needed.
        */
        value_type * dequeue()
        {
            back_off bkoff;

            const index_type nModulo = modulo();
            do {
                index_type th = m_nHead.load(memory_model::memory_order_acquire);
                index_type temp = ( th + 1 ) & nModulo;
                value_type * tt = m_buffer[ temp ].load(memory_model::memory_order_relaxed);
                value_type * pNull;

                // find the actual head after this loop
                while ( is_free( tt ) ) {
                    if ( th != m_nHead.load(memory_model::memory_order_relaxed) )
                        goto TryAgain;

                    // two consecutive nullptr means queue empty
                    if ( temp == m_nTail.load(memory_model::memory_order_acquire) )
                        return nullptr;

                    temp = ( temp + 1 ) & nModulo;
                    tt = m_buffer[ temp ].load(memory_model::memory_order_relaxed);
                }

                if ( th != m_nHead.load(memory_model::memory_order_relaxed) )
                    continue;

                // check whether the queue is empty
                if ( temp == m_nTail.load(memory_model::memory_order_acquire) ) {
                    // help the enqueue to update end
                    m_nTail.compare_exchange_strong( temp, (temp + 1) & nModulo, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    continue;
                }

                pNull = (reinterpret_cast<ptr_atomic_t>( tt ) & 1) ? free0() : free1();

                if ( th != m_nHead.load(memory_model::memory_order_relaxed) )
                    continue;

                // Get the actual head, null means empty
                if ( m_buffer[temp].compare_exchange_strong( tt, pNull, memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                    if ( temp % 2 == 0 )
                        m_nHead.compare_exchange_strong( th, temp, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    --m_ItemCounter;
                    return reinterpret_cast<value_type *>(reinterpret_cast<intptr_t>( tt ) & ~intptr_t(1));
                }

            TryAgain:;
            } while ( bkoff(), true );

            // No control path reaches this line!
            return nullptr;
        }

        /// Synonym of \ref cds_intrusive_TsigasQueue_enqueue "enqueue"
        bool push( value_type& data )
        {
            return enqueue( data );
        }

        /// Synonym of \ref cds_intrusive_TsigasQueue_dequeue "dequeue"
        value_type * pop()
        {
            return dequeue();
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            const index_type nModulo = modulo();

        TryAgain:
            index_type th = m_nHead.load(memory_model::memory_order_relaxed);
            index_type temp = ( th + 1 ) & nModulo;
            const value_type * tt = m_buffer[ temp ].load(memory_model::memory_order_relaxed);

            // find the actual head after this loop
            while ( is_free( tt ) ) {
                if ( th != m_nHead.load(memory_model::memory_order_relaxed) )
                    goto TryAgain;
                // two consecutive nullptr means queue empty
                if ( temp == m_nTail.load(memory_model::memory_order_relaxed) )
                    return true;
                temp = ( temp + 1 ) & nModulo;
                tt = m_buffer[ temp ].load(memory_model::memory_order_relaxed);
            }
            return false;
        }

        /// Clears queue in lock-free manner.
        /**
            \p f parameter is a functor to dispose removed items.
            The interface of \p DISPOSER is:
            \code
            struct myDisposer {
                void operator ()( T * val );
            };
            \endcode
            You can pass \p disposer by reference using \p std::ref.
            The disposer will be called immediately for each item.
        */
        template <typename Disposer>
        void clear( Disposer f )
        {
            value_type * pv;
            while ( (pv = pop()) != nullptr ) {
                f( pv );
            }
        }

        /// Clears the queue
        /**
            This function uses the disposer that is specified in \p Options.
        */
        void clear()
        {
            clear( disposer() );
        }
    };

}}  // namespace cds::intrusive

#endif  // #ifndef __CDS_INTRUSIVE_TSIGAS_CYCLE_QUEUE_H

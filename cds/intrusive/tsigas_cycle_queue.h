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

#ifndef CDSLIB_INTRUSIVE_TSIGAS_CYCLE_QUEUE_H
#define CDSLIB_INTRUSIVE_TSIGAS_CYCLE_QUEUE_H

#include <cds/intrusive/details/base.h>
#include <cds/algo/atomic.h>
#include <cds/details/bounded_container.h>
#include <cds/opt/buffer.h>

namespace cds { namespace intrusive {

    /// TsigasCycleQueue related definitions
    /** @ingroup cds_intrusive_helper
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

            /// Back-off strategy
            typedef cds::backoff::empty         back_off;

            /// The functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used for dequeuing
            typedef opt::v::empty_disposer      disposer;

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
            - \p opt::back_off - back-off strategy used, default is \p cds::backoff::empty.
            - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used
                when dequeuing.
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).

            Example: declare \p %TsigasCycleQueue with item counting and static iternal buffer of size 1024:
            \code
            typedef cds::intrusive::TsigasCycleQueue< Foo,
                typename cds::intrusive::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::initialized_static_buffer< void *, 1024 >,
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


    } //namespace tsigas_queue

    /// Non-blocking cyclic queue discovered by Philippas Tsigas and Yi Zhang
    /** @ingroup cds_intrusive_queue

        Source:
        \li [2000] Philippas Tsigas, Yi Zhang "A Simple, Fast and Scalable Non-Blocking Concurrent FIFO Queue
            for Shared Memory Multiprocessor Systems"

        Template arguments:
        - \p T - value type to be stored in queue. The queue stores pointers to passed data of type \p T.
            <b>Restriction</b>: the queue can manage at least two-byte aligned data: the least significant bit (LSB)
            of any pointer stored in the queue must be zero since the algorithm may use LSB
            as a flag that marks the free cell.
        - \p Traits - queue traits, default is \p tsigas_queue::traits. You can use \p tsigas_queue::make_traits
            metafunction to make your traits or just derive your traits from \p %tsigas_queue::traits:
            \code
            struct myTraits: public cds::intrusive::tsigas_queue::traits {
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::intrusive::TsigasCycleQueue< Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::intrusive::TsigasCycleQueue< Foo,
                typename cds::intrusive::tsigas_queue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode

        This queue algorithm does not require any garbage collector.

        \par Examples:
        \code
        #include <cds/intrusive/tsigas_cycle_queue.h>

        struct Foo {
            ...
        };

        // Queue of Foo pointers, capacity is 1024, statically allocated buffer:
        struct queue_traits: public cds::intrusive::tsigas_queue::traits
        {
            typedef cds::opt::v::initialized_static_buffer< Foo, 1024 > buffer;
        };
        typedef cds::intrusive::TsigasCycleQueue< Foo, queue_traits > static_queue;
        static_queue    stQueue;

        // Queue of Foo pointers, capacity is 1024, dynamically allocated buffer, with item counting:
        typedef cds::intrusive::TsigasCycleQueue< Foo,
            typename cds::intrusive::tsigas_queue::make_traits<
                cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer< Foo > >,
                cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > dynamic_queue;
        dynamic_queue    dynQueue( 1024 );
        \endcode
    */
    template <typename T, typename Traits = tsigas_queue::traits >
    class TsigasCycleQueue: public cds::bounded_container
    {
    public:
        /// Rebind template arguments
        template <typename T2, typename Traits2>
        struct rebind {
            typedef TsigasCycleQueue< T2, Traits2 > other   ;   ///< Rebinding result
        };

    public:
        typedef T value_type;   ///< type of value to be stored in the queue
        typedef Traits traits;  ///< Queue traits
        typedef typename traits::item_counter  item_counter;    ///< Item counter type
        typedef typename traits::disposer      disposer;        ///< Item disposer
        typedef typename traits::back_off      back_off;        ///< back-off strategy used
        typedef typename traits::memory_model  memory_model;    ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::buffer::template rebind< atomics::atomic<value_type *> >::other buffer; ///< Internal buffer

    protected:
        //@cond
        typedef size_t index_type;
        //@endcond

    protected:
        //@cond
        buffer       m_buffer    ;   ///< array of pointer T *, array size is equal to m_nCapacity+1
        typename opt::details::apply_padding< index_type, traits::padding >::padding_type pad1_;
        atomics::atomic<index_type>   m_nHead     ;   ///< index of queue's head
        typename opt::details::apply_padding< index_type, traits::padding >::padding_type pad2_;
        atomics::atomic<index_type>   m_nTail     ;   ///< index of queue's tail
        typename opt::details::apply_padding< index_type, traits::padding >::padding_type pad3_;
        item_counter m_ItemCounter;  ///< item counter
        //@endcond

    protected:
        //@cond
        static CDS_CONSTEXPR intptr_t const free0 = 0;
        static CDS_CONSTEXPR intptr_t const free1 = 1;

        static bool is_free( const value_type * p ) CDS_NOEXCEPT
        {
            return (reinterpret_cast<intptr_t>(p) & ~intptr_t(1)) == 0;
        }

        size_t CDS_CONSTEXPR buffer_capacity() const CDS_NOEXCEPT
        {
            return m_buffer.capacity();
        }

        index_type CDS_CONSTEXPR modulo() const CDS_NOEXCEPT
        {
            return buffer_capacity() - 1;
        }
        //@endcond

    public:
        /// Initialize empty queue of capacity \p nCapacity
        /**
            If internal buffer type is \p cds::opt::v::initialized_static_buffer, the \p nCapacity parameter is ignored.

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

        /// Enqueues an item to the queue
        /** @anchor cds_intrusive_TsigasQueue_enqueue
            Returns \p true if success, \p false if queue is full
        */
        bool enqueue( value_type& data )
        {
            value_type * pNewNode  = &data;
            assert( (reinterpret_cast<uintptr_t>(pNewNode) & 1) == 0 );
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

                if ( te != m_nTail.load(memory_model::memory_order_acquire) )
                    continue;

                // Check whether queue is full
                if ( temp == m_nHead.load(memory_model::memory_order_acquire) ) {
                    ate = ( temp + 1 ) & nModulo;
                    tt = m_buffer[ ate ].load(memory_model::memory_order_relaxed);
                    if ( !is_free( tt ) ) {
                        return false;   // Queue is full
                    }

                    // help the dequeue to update head
                    m_nHead.compare_exchange_strong( temp, ate, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    continue;
                }

                if ( tt == reinterpret_cast<value_type *>(free1) )
                    pNewNode = reinterpret_cast<value_type *>(reinterpret_cast<intptr_t>( pNewNode ) | 1);
                if ( te != m_nTail.load(memory_model::memory_order_acquire) )
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

            Dequeue does not call value disposer. You may manually dispose returned value if it is needed.
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

                    // two consecutive nullptr means the queue is empty
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
                    m_nTail.compare_exchange_weak( temp, (temp + 1) & nModulo, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    continue;
                }

                pNull = reinterpret_cast<value_type *>((reinterpret_cast<uintptr_t>(tt) & 1) ? free0 : free1);

                if ( th != m_nHead.load(memory_model::memory_order_relaxed) )
                    continue;

                // Get the actual head, null means empty
                if ( m_buffer[temp].compare_exchange_weak( tt, pNull, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                    if ( temp % 2 == 0 )
                        m_nHead.compare_exchange_weak( th, temp, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    --m_ItemCounter;
                    return reinterpret_cast<value_type *>(reinterpret_cast<intptr_t>( tt ) & ~intptr_t(1));
                }

            TryAgain:;
            } while ( bkoff(), true );

            // No control path reaches this line!
            return nullptr;
        }

        /// Synonym for \p enqueue()
        bool push( value_type& data )
        {
            return enqueue( data );
        }

        /// Synonym for \p dequeue()
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
            \p f parameter is a functor to dispose removed items:
            \code
            myQueue.clear( []( value_type * p ) { delete p; } );
            \endcode
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
            This function uses the disposer that is specified in \p Traits,
            see \p tsigas_queue::traits::disposer.
        */
        void clear()
        {
            clear( disposer() );
        }

        /// Returns queue's item count
        /**
            The value returned depends on \p tsigas_queue::traits::item_counter.
            For \p atomicity::empty_item_counter, the function always returns 0.
        */
        size_t size() const CDS_NOEXCEPT
        {
            return m_ItemCounter.value();
        }

        /// Returns capacity of internal cyclic buffer
        size_t CDS_CONSTEXPR capacity() const CDS_NOEXCEPT
        {
            return buffer_capacity() - 2;
        }
    };

}}  // namespace cds::intrusive

#endif  // #ifndef CDSLIB_INTRUSIVE_TSIGAS_CYCLE_QUEUE_H

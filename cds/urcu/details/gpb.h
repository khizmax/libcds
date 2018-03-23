// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_URCU_DETAILS_GPB_H
#define CDSLIB_URCU_DETAILS_GPB_H

#include <mutex>
#include <limits>
#include <cds/urcu/details/gp.h>
#include <cds/algo/backoff_strategy.h>
#include <cds/container/vyukov_mpmc_cycle_queue.h>

namespace cds { namespace urcu {

    /// User-space general-purpose RCU with deferred (buffered) reclamation
    /**
        @headerfile cds/urcu/general_buffered.h

        This URCU implementation contains an internal buffer where retired objects are
        accumulated. When the buffer becomes full, the RCU \p synchronize function is called
        that waits until all reader/updater threads end up their read-side critical sections,
        i.e. until the RCU quiescent state will come. After that the buffer and all retired objects are freed.
        This synchronization cycle may be called in any thread that calls \p retire_ptr function.

        The \p Buffer contains items of \ref cds_urcu_retired_ptr "epoch_retired_ptr" type and it should support a queue interface with
        three function:
        - <tt> bool push( retired_ptr& p ) </tt> - places the retired pointer \p p into queue. If the function
            returns \p false it means that the buffer is full and RCU synchronization cycle must be processed.
        - <tt>bool pop( retired_ptr& p ) </tt> - pops queue's head item into \p p parameter; if the queue is empty
            this function must return \p false
        - <tt>size_t size()</tt> - returns queue's item count.

        The buffer is considered as full if \p push() returns \p false or the buffer size reaches the RCU threshold.

        There is a wrapper \ref cds_urcu_general_buffered_gc "gc<general_buffered>" for \p %general_buffered class
        that provides unified RCU interface. You should use this wrapper class instead \p %general_buffered

        Template arguments:
        - \p Buffer - buffer type. Default is \p cds::container::VyukovMPMCCycleQueue
        - \p Lock - mutex type, default is \p std::mutex
        - \p Backoff - back-off schema, default is cds::backoff::Default
    */
    template <
        class Buffer = cds::container::VyukovMPMCCycleQueue< epoch_retired_ptr >
        ,class Lock = std::mutex
        ,class Backoff = cds::backoff::Default
    >
    class general_buffered: public details::gp_singleton< general_buffered_tag >
    {
        //@cond
        typedef details::gp_singleton< general_buffered_tag > base_class;
        //@endcond
    public:
        typedef general_buffered_tag rcu_tag ;  ///< RCU tag
        typedef Buffer  buffer_type ;   ///< Buffer type
        typedef Lock    lock_type   ;   ///< Lock type
        typedef Backoff back_off    ;   ///< Back-off type

        typedef base_class::thread_gc thread_gc ;   ///< Thread-side RCU part
        typedef typename thread_gc::scoped_lock scoped_lock ; ///< Access lock class

        //@cond
        static bool const c_bBuffered = true ; ///< Bufferized RCU
        //@endcond

    protected:
        //@cond
        typedef details::gp_singleton_instance< rcu_tag >    singleton_ptr;
        //@endcond

    protected:
        //@cond
        buffer_type                m_Buffer;
        atomics::atomic<uint64_t>  m_nCurEpoch;
        lock_type                  m_Lock;
        size_t const               m_nCapacity;
        //@endcond

    public:
        /// Returns singleton instance
        static general_buffered * instance()
        {
            return static_cast<general_buffered *>( base_class::instance());
        }
        /// Checks if the singleton is created and ready to use
        static bool isUsed()
        {
            return singleton_ptr::s_pRCU != nullptr;
        }

    protected:
        //@cond
        general_buffered( size_t nBufferCapacity )
            : m_Buffer( nBufferCapacity )
            , m_nCurEpoch(0)
            , m_nCapacity( nBufferCapacity )
        {}

        ~general_buffered()
        {
            clear_buffer( std::numeric_limits< uint64_t >::max());
        }

        void flip_and_wait()
        {
            back_off bkoff;
            base_class::flip_and_wait( bkoff );
        }

        void clear_buffer( uint64_t nEpoch )
        {
            epoch_retired_ptr p;
            while ( m_Buffer.pop( p )) {
                if ( p.m_nEpoch <= nEpoch ) {
                    p.free();
                }
                else {
                    push_buffer( std::move(p));
                    break;
                }
            }
        }

        // Return: \p true - synchronize has been called, \p false - otherwise
        bool push_buffer( epoch_retired_ptr&& ep )
        {
            bool bPushed = m_Buffer.push( ep );
            if ( !bPushed || m_Buffer.size() >= capacity()) {
                synchronize();
                if ( !bPushed ) {
                    ep.free();
                }
                return true;
            }
            return false;
        }
        //@endcond

    public:
        /// Creates singleton object
        /**
            The \p nBufferCapacity parameter defines RCU threshold.
        */
        static void Construct( size_t nBufferCapacity = 256 )
        {
            if ( !singleton_ptr::s_pRCU )
                singleton_ptr::s_pRCU = new general_buffered( nBufferCapacity );
        }

        /// Destroys singleton object
        static void Destruct( bool bDetachAll = false )
        {
            if ( isUsed()) {
                instance()->clear_buffer( std::numeric_limits< uint64_t >::max());
                if ( bDetachAll )
                    instance()->m_ThreadList.detach_all();
                delete instance();
                singleton_ptr::s_pRCU = nullptr;
            }
        }

    public:
        /// Retire \p p pointer
        /**
            The method pushes \p p pointer to internal buffer.
            When the buffer becomes full \ref synchronize function is called
            to wait for the end of grace period and then to free all pointers from the buffer.
        */
        virtual void retire_ptr( retired_ptr& p ) override
        {
            if ( p.m_p )
                push_buffer( epoch_retired_ptr( p, m_nCurEpoch.load( atomics::memory_order_relaxed )));
        }

        /// Retires the pointer chain [\p itFirst, \p itLast)
        template <typename ForwardIterator>
        void batch_retire( ForwardIterator itFirst, ForwardIterator itLast )
        {
            uint64_t nEpoch = m_nCurEpoch.load( atomics::memory_order_relaxed );
            while ( itFirst != itLast ) {
                epoch_retired_ptr ep( *itFirst, nEpoch );
                ++itFirst;
                push_buffer( std::move(ep));
            }
        }

        /// Retires the pointer chain until \p Func returns \p nullptr retired pointer
        template <typename Func>
        void batch_retire( Func e )
        {
            uint64_t nEpoch = m_nCurEpoch.load( atomics::memory_order_relaxed );
            for ( retired_ptr p{ e() }; p.m_p; ) {
                epoch_retired_ptr ep( p, nEpoch );
                p = e();
                push_buffer( std::move(ep));
            }
        }

        /// Wait to finish a grace period and then clear the buffer
        void synchronize()
        {
            epoch_retired_ptr ep( retired_ptr(), m_nCurEpoch.load( atomics::memory_order_relaxed ));
            synchronize( ep );
        }

        //@cond
        bool synchronize( epoch_retired_ptr& ep )
        {
            uint64_t nEpoch;
            {
                std::unique_lock<lock_type> sl( m_Lock );
                if ( ep.m_p && m_Buffer.push( ep ))
                    return false;
                nEpoch = m_nCurEpoch.fetch_add( 1, atomics::memory_order_relaxed );
                flip_and_wait();
                flip_and_wait();
            }
            clear_buffer( nEpoch );
            return true;
        }
        //@endcond

        /// Returns internal buffer capacity
        size_t capacity() const
        {
            return m_nCapacity;
        }
    };

    /// User-space general-purpose RCU with deferred (buffered) reclamation (stripped version)
    /**
        @headerfile cds/urcu/general_buffered.h

        This short version of \p general_buffered is intended for stripping debug info.
        If you use \p %general_buffered with default template arguments you may use
        this stripped version. All functionality of both classes are identical.
    */
    class general_buffered_stripped: public general_buffered<>
    {};

}} // namespace cds::urcu

#endif // #ifndef CDSLIB_URCU_DETAILS_GPB_H

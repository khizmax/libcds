// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_URCU_DETAILS_SIG_BUFFERED_H
#define CDSLIB_URCU_DETAILS_SIG_BUFFERED_H

#include <cds/urcu/details/sh.h>
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED

#include <mutex>
#include <limits>
#include <cds/algo/backoff_strategy.h>
#include <cds/container/vyukov_mpmc_cycle_queue.h>

namespace cds { namespace urcu {

    /// User-space signal-handled RCU with deferred (buffered) reclamation
    /**
        @headerfile cds/urcu/signal_buffered.h

        This URCU implementation contains an internal buffer where retired objects are
        accumulated. When the buffer becomes full, the RCU \p synchronize function is called
        that waits until all reader/updater threads end up their read-side critical sections,
        i.e. until the RCU quiescent state will come. After that the buffer and all retired objects are freed.
        This synchronization cycle may be called in any thread that calls \p retire_ptr function.

        The \p Buffer contains items of \ref cds_urcu_retired_ptr "retired_ptr" type and it should support a queue interface with
        three function:
        - <tt> bool push( retired_ptr& p ) </tt> - places the retired pointer \p p into queue. If the function
            returns \p false it means that the buffer is full and RCU synchronization cycle must be processed.
        - <tt>bool pop( retired_ptr& p ) </tt> - pops queue's head item into \p p parameter; if the queue is empty
            this function must return \p false
        - <tt>size_t size()</tt> - returns queue's item count.

        The buffer is considered as full if \p push returns \p false or the buffer size reaches the RCU threshold.

        There is a wrapper \ref cds_urcu_signal_buffered_gc "gc<signal_buffered>" for \p %signal_buffered class
        that provides unified RCU interface. You should use this wrapper class instead \p %signal_buffered

        Template arguments:
        - \p Buffer - buffer type. Default is cds::container::VyukovMPMCCycleQueue
        - \p Lock - mutex type, default is \p std::mutex
        - \p Backoff - back-off schema, default is cds::backoff::Default
    */
    template <
        class Buffer = cds::container::VyukovMPMCCycleQueue< epoch_retired_ptr >
        ,class Lock = std::mutex
        ,class Backoff = cds::backoff::Default
    >
    class signal_buffered: public details::sh_singleton< signal_buffered_tag >
    {
        //@cond
        typedef details::sh_singleton< signal_buffered_tag > base_class;
        //@endcond
    public:
        typedef signal_buffered_tag rcu_tag ;  ///< RCU tag
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
        typedef details::sh_singleton_instance< rcu_tag >    singleton_ptr;
        //@endcond

    protected:
        //@cond
        buffer_type               m_Buffer;
        atomics::atomic<uint64_t> m_nCurEpoch;
        lock_type                 m_Lock;
        size_t const              m_nCapacity;
        //@endcond

    public:
        /// Returns singleton instance
        static signal_buffered * instance()
        {
            return static_cast<signal_buffered *>( base_class::instance());
        }
        /// Checks if the singleton is created and ready to use
        static bool isUsed()
        {
            return singleton_ptr::s_pRCU != nullptr;
        }

    protected:
        //@cond
        signal_buffered( size_t nBufferCapacity, int nSignal = SIGUSR1 )
            : base_class( nSignal )
            , m_Buffer( nBufferCapacity )
            , m_nCurEpoch(0)
            , m_nCapacity( nBufferCapacity )
        {}

        ~signal_buffered()
        {
            clear_buffer( std::numeric_limits< uint64_t >::max());
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

            The \p nSignal parameter defines a signal number stated for RCU, default is \p SIGUSR1
        */
        static void Construct( size_t nBufferCapacity = 256, int nSignal = SIGUSR1 )
        {
            if ( !singleton_ptr::s_pRCU )
                singleton_ptr::s_pRCU = new signal_buffered( nBufferCapacity, nSignal );
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
                if ( ep.m_p && m_Buffer.push( ep ) && m_Buffer.size() < capacity())
                    return false;
                nEpoch = m_nCurEpoch.fetch_add( 1, atomics::memory_order_relaxed );

                back_off bkOff;
                base_class::force_membar_all_threads( bkOff );
                base_class::switch_next_epoch();
                bkOff.reset();
                base_class::wait_for_quiescent_state( bkOff );
                base_class::switch_next_epoch();
                bkOff.reset();
                base_class::wait_for_quiescent_state( bkOff );
                base_class::force_membar_all_threads( bkOff );
            }

            clear_buffer( nEpoch );
            return true;
        }
        //@endcond

        /// Returns the threshold of internal buffer
        size_t capacity() const
        {
            return m_nCapacity;
        }

        /// Returns the signal number stated for RCU
        int signal_no() const
        {
            return base_class::signal_no();
        }
    };


    /// User-space signal-handled RCU with deferred (buffered) reclamation (stripped version)
    /**
        @headerfile cds/urcu/signal_buffered.h

        This short version of \p signal_buffered is intended for stripping debug info.
        If you use \p %signal_buffered with default template arguments you may use
        this stripped version. All functionality of both classes are identical.
    */
    class signal_buffered_stripped: public signal_buffered<>
    {};

}} // namespace cds::urcu

#endif // #ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#endif // #ifndef CDSLIB_URCU_DETAILS_SIG_BUFFERED_H

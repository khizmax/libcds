//$$CDS-header$$1

#ifndef CDSLIB_URCU_DETAILS_SIG_THREADED_H
#define CDSLIB_URCU_DETAILS_SIG_THREADED_H

#include <mutex>    //unique_lock
#include <cds/urcu/details/sh.h>
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED

#include <cds/urcu/dispose_thread.h>
#include <cds/algo/backoff_strategy.h>
#include <cds/container/vyukov_mpmc_cycle_queue.h>

namespace cds { namespace urcu {

    /// User-space signal-handled RCU with deferred threaded reclamation
    /**
        @headerfile cds/urcu/signal_threaded.h

        This implementation is similar to \ref signal_buffered but separate thread is created
        for deleting the retired objects. Like \p %signal_buffered, the class contains an internal buffer
        where retired objects are accumulated. When the buffer becomes full,
        the RCU \p synchronize function is called that waits until all reader/updater threads end up their read-side critical sections,
        i.e. until the RCU quiescent state will come. After that the "work ready" message is sent to reclamation thread.
        The reclamation thread frees the buffer.
        This synchronization cycle may be called in any thread that calls \ref retire_ptr function.

        There is a wrapper \ref cds_urcu_signal_threaded_gc "gc<signal_threaded>" for \p %signal_threaded class
        that provides unified RCU interface. You should use this wrapper class instead \p %signal_threaded

        Template arguments:
        - \p Buffer - buffer type with FIFO semantics. Default is cds::container::VyukovMPMCCycleQueue. See \ref signal_buffered
            for description of buffer's interface. The buffer contains the objects of \ref epoch_retired_ptr
            type that contains additional \p m_nEpoch field. This field specifies an epoch when the object
            has been placed into the buffer. The \p %signal_threaded object has a global epoch counter
            that is incremented on each \p synchronize call. The epoch is used internally to prevent early deletion.
        - \p Lock - mutex type, default is \p std::mutex
        - \p DisposerThread - the reclamation thread class. Default is \ref cds::urcu::dispose_thread,
            see the description of this class for required interface.
        - \p Backoff - back-off schema, default is cds::backoff::Default
    */
    template <
        class Buffer = cds::container::VyukovMPMCCycleQueue< epoch_retired_ptr >
        ,class Lock = std::mutex
        ,class DisposerThread = dispose_thread<Buffer>
        ,class Backoff = cds::backoff::Default
    >
    class signal_threaded: public details::sh_singleton< signal_threaded_tag >
    {
        //@cond
        typedef details::sh_singleton< signal_threaded_tag > base_class;
        //@endcond
    public:
        typedef Buffer          buffer_type ;   ///< Buffer type
        typedef Lock            lock_type   ;   ///< Lock type
        typedef Backoff         back_off    ;   ///< Back-off scheme
        typedef DisposerThread  disposer_thread ;   ///< Disposer thread type

        typedef signal_threaded_tag     rcu_tag ;       ///< Thread-side RCU part
        typedef base_class::thread_gc   thread_gc ;     ///< Access lock class
        typedef typename thread_gc::scoped_lock scoped_lock ; ///< Access lock class

        static bool const c_bBuffered = true ; ///< This RCU buffers disposed elements

    protected:
        //@cond
        typedef details::sh_singleton_instance< rcu_tag >    singleton_ptr;

        struct scoped_disposer {
            void operator ()( signal_threaded * p )
            {
                delete p;
            }
        };
        //@endcond

    protected:
        //@cond
        buffer_type                     m_Buffer;
        atomics::atomic<uint64_t>    m_nCurEpoch;
        lock_type                       m_Lock;
        size_t const                    m_nCapacity;
        disposer_thread                 m_DisposerThread;
        //@endcond

    public:
        /// Returns singleton instance
        static signal_threaded * instance()
        {
            return static_cast<signal_threaded *>( base_class::instance() );
        }
        /// Checks if the singleton is created and ready to use
        static bool isUsed()
        {
            return singleton_ptr::s_pRCU != nullptr;
        }

    protected:
        //@cond
        signal_threaded( size_t nBufferCapacity, int nSignal = SIGUSR1 )
            : base_class( nSignal )
            , m_Buffer( nBufferCapacity )
            , m_nCurEpoch( 1 )
            , m_nCapacity( nBufferCapacity )
        {}

        // Return: true - synchronize has been called, false - otherwise
        bool push_buffer( epoch_retired_ptr& p )
        {
            bool bPushed = m_Buffer.push( p );
            if ( !bPushed || m_Buffer.size() >= capacity() ) {
                synchronize();
                if ( !bPushed )
                    p.free();
                return true;
            }
            return false;
        }

        //@endcond

    public:
        //@cond
        ~signal_threaded()
        {}
        //@endcond

        /// Creates singleton object and starts reclamation thread
        /**
            The \p nBufferCapacity parameter defines RCU threshold.

            The \p nSignal parameter defines a signal number stated for RCU, default is \p SIGUSR1
        */
        static void Construct( size_t nBufferCapacity = 256, int nSignal = SIGUSR1 )
        {
            if ( !singleton_ptr::s_pRCU ) {
                std::unique_ptr< signal_threaded, scoped_disposer > pRCU( new signal_threaded( nBufferCapacity, nSignal ) );
                pRCU->m_DisposerThread.start();

                singleton_ptr::s_pRCU = pRCU.release();
            }
        }

        /// Destroys singleton object and terminates internal reclamation thread
        static void Destruct( bool bDetachAll = false )
        {
            if ( isUsed() ) {
                signal_threaded * pThis = instance();
                if ( bDetachAll )
                    pThis->m_ThreadList.detach_all();

                pThis->m_DisposerThread.stop( pThis->m_Buffer, pThis->m_nCurEpoch.load( atomics::memory_order_acquire ));

                delete pThis;
                singleton_ptr::s_pRCU = nullptr;
            }
        }

    public:
        /// Retires \p p pointer
        /**
            The method pushes \p p pointer to internal buffer.
            When the buffer becomes full \ref synchronize function is called
            to wait for the end of grace period and then
            a message is sent to the reclamation thread.
        */
        virtual void retire_ptr( retired_ptr& p )
        {
            if ( p.m_p ) {
                epoch_retired_ptr ep( p, m_nCurEpoch.load( atomics::memory_order_acquire ) );
                push_buffer( ep );
            }
        }

        /// Retires the pointer chain [\p itFirst, \p itLast)
        template <typename ForwardIterator>
        void batch_retire( ForwardIterator itFirst, ForwardIterator itLast )
        {
            uint64_t nEpoch = m_nCurEpoch.load( atomics::memory_order_relaxed );
            while ( itFirst != itLast ) {
                epoch_retired_ptr p( *itFirst, nEpoch );
                ++itFirst;
                push_buffer( p );
            }
        }

        /// Waits to finish a grace period and calls disposing thread
        void synchronize()
        {
            synchronize( false );
        }

        //@cond
        void synchronize( bool bSync )
        {
            uint64_t nPrevEpoch = m_nCurEpoch.fetch_add( 1, atomics::memory_order_release );

            atomics::atomic_thread_fence( atomics::memory_order_acquire );
            {
                std::unique_lock<lock_type> sl( m_Lock );

                back_off bkOff;
                base_class::force_membar_all_threads( bkOff );
                base_class::switch_next_epoch();
                bkOff.reset();
                base_class::wait_for_quiescent_state( bkOff );
                base_class::switch_next_epoch();
                bkOff.reset();
                base_class::wait_for_quiescent_state( bkOff );
                base_class::force_membar_all_threads( bkOff );

                m_DisposerThread.dispose( m_Buffer, nPrevEpoch, bSync );
            }
        }
        void force_dispose()
        {
            synchronize( true );
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
}} // namespace cds::urcu

#endif // #ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#endif // #ifndef CDSLIB_URCU_DETAILS_SIG_THREADED_H

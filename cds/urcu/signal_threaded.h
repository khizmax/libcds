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

#ifndef CDSLIB_URCU_SIGNAL_THREADED_H
#define CDSLIB_URCU_SIGNAL_THREADED_H

#include <cds/urcu/details/sig_threaded.h>

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED

namespace cds { namespace urcu {

    /// User-space signal-handled RCU with special thread for deferred reclamation
    /** @anchor cds_urcu_signal_threaded_gc

        This is a wrapper around \p signal_threaded class.

        Template arguments:
        - \p Buffer - lock-free MPSC (muliple producer/single consumer) queue.
            Default is \p cds::container::VyukovMPSCCycleQueue< epoch_retired_ptr >
        - \p Lock - mutex type, default is \p std::mutex
        - \p DisposerThread - reclamation thread class, default is \p %general_threaded_dispose_thread
            See \ref cds::urcu::dispose_thread for class interface.
        - \p Backoff - back-off schema, default is \p cds::backoff::Default

    */
    template <
#ifdef CDS_DOXGEN_INVOKED
        class Buffer = cds::container::VyukovMPSCCycleQueue< epoch_retired_ptr >
        ,class Lock = std::mutex
        ,class DisposerThread = dispose_thread<Buffer>
        ,class Backoff = cds::backoff::Default
#else
        class Buffer
       ,class Lock
       ,class DisposerThread
       ,class Backoff
#endif
    >
    class gc< signal_threaded< Buffer, Lock, DisposerThread, Backoff > >: public details::gc_common
    {
    public:
        typedef signal_threaded< Buffer, Lock, DisposerThread, Backoff >  rcu_implementation   ;    ///< Wrapped URCU implementation

        typedef typename rcu_implementation::rcu_tag     rcu_tag     ;   ///< URCU tag
        typedef typename rcu_implementation::thread_gc   thread_gc   ;   ///< Thread-side RCU part
        typedef typename rcu_implementation::scoped_lock scoped_lock ;   ///< Access lock class

        using details::gc_common::atomic_marked_ptr;

    public:
        /// Creates URCU \p %general_threaded singleton.
        /**
            The \p nBufferCapacity parameter defines RCU threshold.

            The \p nSignal parameter defines a signal number stated for RCU, default is \p SIGUSR1
        */
        gc( size_t nBufferCapacity = 256, int nSignal = SIGUSR1 )
        {
            rcu_implementation::Construct( nBufferCapacity, nSignal );
        }

        /// Destroys URCU \p %signal_threaded singleton
        ~gc()
        {
            rcu_implementation::Destruct( true );
        }

    public:
        /// Waits to finish a grace period and calls disposing thread
        /**
            After grace period finished the function gives new task to disposing thread.
            Unlike \ref force_dispose the \p %synchronize function does not wait for
            task ending. Only a "task ready" message is sent to disposing thread.
        */
        static void synchronize()
        {
            rcu_implementation::instance()->synchronize();
        }

        /// Retires pointer \p p by the disposer \p pFunc
        /**
            If the buffer is full, \ref synchronize function is invoked.
        */
        template <typename T>
        static void retire_ptr( T * p, void (* pFunc)(T *) )
        {
            retired_ptr rp( reinterpret_cast<void *>( p ), reinterpret_cast<free_retired_ptr_func>( pFunc ) );
            retire_ptr( rp );
        }

        /// Retires pointer \p p using \p Disposer
        /**
            If the buffer is full, \ref synchronize function is invoked.
        */
        template <typename Disposer, typename T>
        static void retire_ptr( T * p )
        {
            retire_ptr( p, cds::details::static_functor<Disposer, T>::call );
        }

        /// Retires pointer \p p of type \ref cds_urcu_retired_ptr "retired_ptr"
        /**
            If the buffer is full, \ref synchronize function is invoked.
        */
        static void retire_ptr( retired_ptr& p )
        {
            rcu_implementation::instance()->retire_ptr(p);
        }

        /// Frees chain [ \p itFirst, \p itLast) in one synchronization cycle
        template <typename ForwardIterator>
        static void batch_retire( ForwardIterator itFirst, ForwardIterator itLast )
        {
            rcu_implementation::instance()->batch_retire( itFirst, itLast );
        }

        /// Retires the pointer chain until \p Func returns \p nullptr retired pointer
        template <typename Func>
        static void batch_retire( Func e )
        {
            rcu_implementation::instance()->batch_retire( e );
        }

         /// Acquires access lock (so called RCU reader-side lock)
        /**
            For safety reasons, it is better to use \ref scoped_lock class for locking/unlocking
        */
        static void access_lock()
        {
            thread_gc::access_lock();
        }

        /// Releases access lock (so called RCU reader-side lock)
        /**
            For safety reasons, it is better to use \ref scoped_lock class for locking/unlocking
        */
        static void access_unlock()
        {
            thread_gc::access_unlock();
        }

        /// Checks if the thread is inside read-side critical section (i.e. the lock is acquired)
        /**
            Usually, this function is used internally to be convinced
            that subsequent remove action is not lead to a deadlock.
        */
        static bool is_locked()
        {
            return thread_gc::is_locked();
        }

        /// Returns the threshold of internal buffer
        static size_t capacity()
        {
            return rcu_implementation::instance()->capacity();
        }

        /// Returns the signal number stated for RCU
        static int signal_no()
        {
            return rcu_implementation::instance()->signal_no();
        }

        /// Forces retired object removal (synchronous version of \ref synchronize)
        /**
            The function calls \ref synchronize and waits until reclamation thread
            frees retired objects.
        */
        static void force_dispose()
        {
            rcu_implementation::instance()->force_dispose();
        }
    };

    //@cond
    template<>
    class gc< signal_threaded_stripped >: public gc< signal_threaded<>>
    {};
    //@endcond

}} // namespace cds::urcu

#endif // #ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#endif // #ifndef CDSLIB_URCU_SIGNAL_THREADED_H

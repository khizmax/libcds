//$$CDS-header$$

#ifndef CDSLIB_URCU_SIGNAL_BUFFERED_H
#define CDSLIB_URCU_SIGNAL_BUFFERED_H

#include <cds/urcu/details/sig_buffered.h>
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED

namespace cds { namespace urcu {

    /// User-space signal-handled RCU with deferred buffered reclamation
    /** @anchor cds_urcu_signal_buffered_gc

        This is a wrapper around signal_buffered class used for metaprogramming.

        Template arguments:
        - \p Buffer - lock-free queue or lock-free bounded queue.
            Default is cds::container::VyukovMPMCCycleQueue< retired_ptr >
        - \p Lock - mutex type, default is \p std::mutex
        - \p Backoff - back-off schema, default is cds::backoff::Default
    */
    template <
#ifdef CDS_DOXGEN_INVOKED
        class Buffer = cds::container::VyukovMPMCCycleQueue< retired_ptr >
        ,class Lock = std::mutex
        ,class Backoff = cds::backoff::Default
#else
        class Buffer
       ,class Lock
       ,class Backoff
#endif
    >
    class gc< signal_buffered< Buffer, Lock, Backoff > >: public details::gc_common
    {
    public:
        typedef signal_buffered< Buffer, Lock, Backoff >  rcu_implementation   ;    ///< Wrapped URCU implementation

        typedef typename rcu_implementation::rcu_tag     rcu_tag     ;   ///< URCU tag
        typedef typename rcu_implementation::thread_gc   thread_gc   ;   ///< Thread-side RCU part
        typedef typename rcu_implementation::scoped_lock scoped_lock ;   ///< Access lock class

        using details::gc_common::atomic_marked_ptr;

    public:
        /// Creates URCU \p %signal_buffered singleton.
        /**
            The \p nBufferCapacity parameter defines RCU threshold.

            The \p nSignal parameter defines a signal number stated for RCU, default is \p SIGUSR1
        */
        gc( size_t nBufferCapacity = 256, int nSignal = SIGUSR1 )
        {
            rcu_implementation::Construct( nBufferCapacity, nSignal );
        }

        /// Destroys URCU \p %general_instant singleton
        ~gc()
        {
            rcu_implementation::Destruct( true );
        }

    public:
        /// Waits to finish a grace period and clears the buffer
        /**
            After grace period finished the function frees all retired pointer
            from internal buffer.
        */
        static void synchronize()
        {
            rcu_implementation::instance()->synchronize();
        }

        /// Places retired pointer <\p p, \p pFunc> to internal buffer
        /**
            If the buffer is full, \ref synchronize function is invoked.
        */
        template <typename T>
        static void retire_ptr( T * p, void (* pFunc)(T *) )
        {
            retired_ptr rp( reinterpret_cast<void *>( p ), reinterpret_cast<free_retired_ptr_func>( pFunc ) );
            retire_ptr( rp );
        }

        /// Places retired pointer \p p with \p Disposer to internal buffer
        /**
            If the buffer is full, \ref synchronize function is invoked.
        */
        template <typename Disposer, typename T>
        static void retire_ptr( T * p )
        {
            retire_ptr( p, cds::details::static_functor<Disposer, T>::call );
        }

        /// Places retired pointer \p p to internal buffer
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

        /// Checks if the thread is inside read-side critical section (i.e. the lock is acquired)
        /**
            Usually, this function is used internally to be convinced
            that subsequent remove action is not lead to a deadlock.
        */
        static bool is_locked()
        {
            return thread_gc::is_locked();
        }

        /// Forces retired object removal
        /**
            This function calls \ref synchronize
        */
        static void force_dispose()
        {
            synchronize();
        }
    };

}} // namespace cds::urcu

#endif // #ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#endif // #ifndef CDSLIB_URCU_SIGNAL_BUFFERED_H

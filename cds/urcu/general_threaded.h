//$$CDS-header$$

#ifndef CDSLIB_URCU_GENERAL_THREADED_H
#define CDSLIB_URCU_GENERAL_THREADED_H

#include <cds/urcu/details/gpt.h>

namespace cds { namespace urcu {

    /// User-space general-purpose RCU with special thread for deferred reclamation
    /** @anchor cds_urcu_general_threaded_gc

        This is a wrapper around general_threaded class used for metaprogramming.

        Template arguments:
        - \p Buffer - lock-free queue or lock-free bounded queue.
            Default is cds::container::VyukovMPMCCycleQueue< retired_ptr >
        - \p Lock - mutex type, default is \p std::mutex
        - \p DisposerThread - reclamation thread class, default is \p cds::urcu::dispose_thread
            See \ref cds::urcu::dispose_thread for class interface.
        - \p Backoff - back-off schema, default is cds::backoff::Default

    */
    template <
#ifdef CDS_DOXGEN_INVOKED
        class Buffer = cds::container::VyukovMPMCCycleQueue< epoch_retired_ptr >
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
    class gc< general_threaded< Buffer, Lock, DisposerThread, Backoff > >: public details::gc_common
    {
    public:
        typedef general_threaded< Buffer, Lock, DisposerThread, Backoff >  rcu_implementation   ;    ///< Wrapped URCU implementation

        typedef typename rcu_implementation::rcu_tag     rcu_tag     ;   ///< URCU tag
        typedef typename rcu_implementation::thread_gc   thread_gc   ;   ///< Thread-side RCU part
        typedef typename rcu_implementation::scoped_lock scoped_lock ;   ///< Access lock class

        using details::gc_common::atomic_marked_ptr;

    public:
        /// Creates URCU \p %general_threaded singleton.
        gc( size_t nBufferCapacity = 256 )
        {
            rcu_implementation::Construct( nBufferCapacity );
        }

        /// Destroys URCU \p %general_threaded singleton
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

}} // namespace cds::urcu

#endif // #ifndef CDSLIB_URCU_GENERAL_THREADED_H

//$$CDS-header$$

#ifndef CDSLIB_URCU_GENERAL_BUFFERED_H
#define CDSLIB_URCU_GENERAL_BUFFERED_H

#include <cds/urcu/details/gpb.h>

namespace cds { namespace urcu {

    /// User-space general-purpose RCU with deferred buffered reclamation
    /** @anchor cds_urcu_general_buffered_gc

        This is a wrapper around general_buffered class used for metaprogramming.

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
    class gc< general_buffered< Buffer, Lock, Backoff > >: public details::gc_common
    {
    public:
        typedef general_buffered< Buffer, Lock, Backoff >  rcu_implementation   ;    ///< Wrapped URCU implementation

        typedef typename rcu_implementation::rcu_tag     rcu_tag     ;   ///< URCU tag
        typedef typename rcu_implementation::thread_gc   thread_gc   ;   ///< Thread-side RCU part
        typedef typename rcu_implementation::scoped_lock scoped_lock ;   ///< Access lock class

        using details::gc_common::atomic_marked_ptr;

    public:
        /// Creates URCU \p %general_buffered singleton.
        gc( size_t nBufferCapacity = 256 )
        {
            rcu_implementation::Construct( nBufferCapacity );
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

#endif // #ifndef CDSLIB_URCU_GENERAL_BUFFERED_H

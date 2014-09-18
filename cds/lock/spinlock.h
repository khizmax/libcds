//$$CDS-header$$

#ifndef __CDS_LOCK_SPINLOCK_H
#define __CDS_LOCK_SPINLOCK_H

/*
    Defines spin-lock primitives
    Editions:
        2012.01.23  1.1.0 khizmax     Refactoring: use C++11 atomics
        2010.01.22  0.6.0 khizmax     Refactoring: use cds::atomic namespace
                                      Explicit memory ordering specification (atomic::memory_order_xxx)
        2006              khizmax     Created
*/

#include <cds/cxx11_atomic.h>
#include <cds/os/thread.h>
#include <cds/algo/backoff_strategy.h>
#include <cds/lock/scoped_lock.h>

#include <cds/details/noncopyable.h>

namespace cds {
    /// Synchronization primitives
    namespace lock {
        /// Spin lock.
        /**
            Simple and light-weight spin-lock critical section
            It is useful to gain access to small (short-timed) code

            Algorithm:

                TATAS (test-and-test-and-lock)
                [1984] L. Rudolph, Z. Segall. Dynamic Decentralized Cache Schemes for MIMD Parallel Processors.

            No serialization performed - any of waiting threads may owns the spin-lock.
            This spin-lock is NOT recursive: the thread owned the lock cannot call lock() method withod deadlock.
            The method unlock() can call any thread

            DEBUG version: The spinlock stores owner thead id. Assertion is raised when:
                - double lock attempt encountered by same thread (deadlock)
                - unlock by another thread

            If spin-lock is locked the Backoff algorithm is called. Predefined backoff::LockDefault class yields current
            thread and repeats lock attempts later

            Template parameters:
                - @p Backoff    backoff strategy. Used when spin lock is locked
        */
        template <class Backoff >
        class Spinlock
        {
        public:
            typedef        Backoff      backoff_strategy    ;        ///< back-off strategy type
        private:
            CDS_ATOMIC::atomic<bool>    m_spin  ;       ///< Spin
#    ifdef CDS_DEBUG
            typename OS::ThreadId       m_dbgOwnerId        ;       ///< Owner thread id (only for debug mode)
#    endif

        public:
            /// Construct free (unlocked) spin-lock
            Spinlock() CDS_NOEXCEPT
#    ifdef CDS_DEBUG
                :m_dbgOwnerId( OS::nullThreadId() )
#    endif
            {
                m_spin.store( false, CDS_ATOMIC::memory_order_relaxed );
            }

            /// Construct spin-lock in specified state
            /**
                In debug mode: if \p bLocked = true then spin-lock is made owned by current thread
            */
            Spinlock( bool bLocked ) CDS_NOEXCEPT
#    ifdef CDS_DEBUG
                :m_dbgOwnerId( bLocked ? OS::getCurrentThreadId() : OS::nullThreadId() )
#    endif
            {
                m_spin.store( bLocked, CDS_ATOMIC::memory_order_relaxed );
            }

            /// Dummy copy constructor
            /**
                In theory, spin-lock cannot be copied. However, it is not practical.
                Therefore, we provide dummy copy constructor that do no copy in fact. The ctor
                initializes the spin to free (unlocked) state like default ctor.
            */
            Spinlock(const Spinlock<Backoff>& ) CDS_NOEXCEPT
                : m_spin( false )
#   ifdef CDS_DEBUG
                , m_dbgOwnerId( OS::nullThreadId() )
#   endif
            {}

            /// Destructor. On debug time it checks whether spin-lock is free
            ~Spinlock()
            {
                assert( !m_spin.load( CDS_ATOMIC::memory_order_relaxed ) );
            }

            /// Check if the spin is locked
            bool is_locked() const CDS_NOEXCEPT
            {
                return m_spin.load( CDS_ATOMIC::memory_order_relaxed );
            }

            /// Try to lock the object
            /**
                Returns \p true if locking is succeeded
                otherwise (if the spin is already locked) returns \p false

                Debug version: deadlock can be detected
            */
            bool try_lock() CDS_NOEXCEPT
            {
                return tryLock();
            }

            /// Try to lock the object (synonym for \ref try_lock)
            bool tryLock() CDS_NOEXCEPT
            {
                bool bCurrent = false;
                m_spin.compare_exchange_strong( bCurrent, true, CDS_ATOMIC::memory_order_acquire, CDS_ATOMIC::memory_order_relaxed );

                CDS_DEBUG_DO(
                    if ( !bCurrent ) {
                        m_dbgOwnerId = OS::getCurrentThreadId();
                    }
                )
                return !bCurrent;
            }

            /// Try to lock the object, repeat @p nTryCount times if failed
            /**
                Returns \p true if locking is succeeded
                otherwise (if the spin is already locked) returns \p false
            */
            bool try_lock( unsigned int nTryCount ) CDS_NOEXCEPT
            {
                return tryLock( nTryCount );
            }

            /// Try to lock the object (synonym for \ref try_lock)
            bool tryLock( unsigned int nTryCount ) CDS_NOEXCEPT
            {
                Backoff backoff;
                while ( nTryCount-- ) {
                    if ( tryLock() )
                        return true;
                    backoff();
                }
                return false;
            }

            /// Lock the spin-lock. Waits infinitely while spin-lock is locked. Debug version: deadlock may be detected
            void lock() CDS_NOEXCEPT
            {
                Backoff backoff;

                // Deadlock detected
                assert( m_dbgOwnerId != OS::getCurrentThreadId() );

                // TATAS algorithm
                while ( !tryLock() ) {
                    while ( m_spin.load( CDS_ATOMIC::memory_order_relaxed ) ) {
                        backoff();
                    }
                }
                assert( m_dbgOwnerId == OS::getCurrentThreadId() );
            }

            /// Unlock the spin-lock. Debug version: deadlock may be detected
            void unlock() CDS_NOEXCEPT
            {
                assert( m_spin.load( CDS_ATOMIC::memory_order_relaxed ) );

                assert( m_dbgOwnerId == OS::getCurrentThreadId() );
                CDS_DEBUG_DO( m_dbgOwnerId = OS::nullThreadId() ;)

                m_spin.store( false, CDS_ATOMIC::memory_order_release );
            }
        };

        /// Spin-lock implementation default for the current platform
        typedef Spinlock<backoff::LockDefault >                 Spin;

        /// Recursive spin lock.
        /**
            Allows recursive calls: the owner thread may recursive enter to critical section guarded by the spin-lock.

            Template parameters:
                - @p Integral       one of integral atomic type: <tt>unsigned int</tt>, <tt>int</tt>, and others
                - @p Backoff        backoff strategy. Used when spin lock is locked
        */
        template <typename Integral, class Backoff>
        class ReentrantSpinT
        {
            typedef OS::ThreadId    thread_id    ;        ///< The type of thread id

        public:
            typedef Integral        integral_type       ; ///< The integral type
            typedef Backoff         backoff_strategy    ; ///< The backoff type

        private:
            CDS_ATOMIC::atomic<integral_type>   m_spin      ; ///< spin-lock atomic
            thread_id                           m_OwnerId   ; ///< Owner thread id. If spin-lock is not locked it usually equals to OS::nullThreadId()

        private:
            //@cond
            void beOwner( thread_id tid ) CDS_NOEXCEPT
            {
                m_OwnerId = tid;
            }

            void free() CDS_NOEXCEPT
            {
                m_OwnerId = OS::nullThreadId();
            }

            bool isOwned( thread_id tid ) const CDS_NOEXCEPT
            {
                return m_OwnerId == tid;
            }

            bool    tryLockOwned( thread_id tid ) CDS_NOEXCEPT
            {
                if ( isOwned( tid )) {
                    m_spin.fetch_add( 1, CDS_ATOMIC::memory_order_relaxed );
                    return true;
                }
                return false;
            }

            bool tryAcquireLock() CDS_NOEXCEPT
            {
                integral_type nCurrent = 0;
                return m_spin.compare_exchange_weak( nCurrent, 1, CDS_ATOMIC::memory_order_acquire, CDS_ATOMIC::memory_order_relaxed );
            }

            bool tryAcquireLock( unsigned int nTryCount ) CDS_NOEXCEPT_( noexcept( backoff_strategy()() ))
            {
                backoff_strategy bkoff;

                while ( nTryCount-- ) {
                    if ( tryAcquireLock() )
                        return true;
                    bkoff();
                }
                return false;
            }

            void acquireLock() CDS_NOEXCEPT_( noexcept( backoff_strategy()() ))
            {
                // TATAS algorithm
                backoff_strategy bkoff;
                while ( !tryAcquireLock() ) {
                    while ( m_spin.load( CDS_ATOMIC::memory_order_relaxed ) )
                        bkoff();
                }
            }
            //@endcond

        public:
            /// Default constructor initializes spin to free (unlocked) state
            ReentrantSpinT() CDS_NOEXCEPT
                : m_spin(0)
                , m_OwnerId( OS::nullThreadId() )
            {}

            /// Dummy copy constructor
            /**
                In theory, spin-lock cannot be copied. However, it is not practical.
                Therefore, we provide dummy copy constructor that do no copy in fact. The ctor
                initializes the spin to free (unlocked) state like default ctor.
            */
            ReentrantSpinT(const ReentrantSpinT<Integral, Backoff>& ) CDS_NOEXCEPT
                : m_spin(0)
                , m_OwnerId( OS::nullThreadId() )
            {}

            /// Construct object for specified state
            ReentrantSpinT(bool bLocked) CDS_NOEXCEPT
                : m_spin(0),
                m_OwnerId( OS::nullThreadId() )
            {
                if ( bLocked )
                    lock();
            }

            /// Checks if the spin is locked
            /**
                The spin is locked if lock count > 0 and the current thread is not an owner of the lock.
                Otherwise (i.e. lock count == 0 or the curren thread owns the spin) the spin is unlocked.
            */
            bool is_locked() const CDS_NOEXCEPT
            {
                return !( m_spin.load( CDS_ATOMIC::memory_order_relaxed ) == 0 || isOwned( cds::OS::getCurrentThreadId() ));
            }

            /// Try to lock the spin-lock (synonym for \ref try_lock)
            bool tryLock() CDS_NOEXCEPT
            {
                thread_id tid = OS::getCurrentThreadId();
                if ( tryLockOwned( tid ) )
                    return true;
                if ( tryAcquireLock()) {
                    beOwner( tid );
                    return true;
                }
                return false;
            }

            /// Try to lock the spin-lock. If spin-lock is free the current thread owns it. Return @p true if locking is success
            bool try_lock() CDS_NOEXCEPT
            {
                return tryLock();
            }

            /// Try to lock the object (synonym for \ref try_lock)
            bool tryLock( unsigned int nTryCount )
#       if !( (CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION >= 40600 && CDS_COMPILER_VERSION < 40700) || (CDS_COMPILER == CDS_COMPILER_CLANG && CDS_COMPILER_VERSION < 30100) )
                // GCC 4.6, clang 3.0 error in noexcept expression:
                // cannot call member function ‘bool cds::lock::ReentrantSpinT<Integral, Backoff>::tryAcquireLock(unsigned int) without object
                CDS_NOEXCEPT_( noexcept( tryAcquireLock(nTryCount) ))
#       endif
            {
                thread_id tid = OS::getCurrentThreadId();
                if ( tryLockOwned( tid ) )
                    return true;
                if ( tryAcquireLock( nTryCount )) {
                    beOwner( tid );
                    return true;
                }
                return false;
            }

            /// Try to lock the object.
            /**
                If the spin-lock is locked the method repeats attempts to own spin-lock up to @p nTryCount times.
                Between attempts @p backoff() is called.
                Return @p true if current thread owns the lock @p false otherwise
            */
            bool try_lock( unsigned int nTryCount )
#       if !( (CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION >= 40600 && CDS_COMPILER_VERSION < 40700) || (CDS_COMPILER == CDS_COMPILER_CLANG && CDS_COMPILER_VERSION < 30100) )
                // GCC 4.6, clang 3.0 error in noexcept expression:
                // cannot call member function ‘bool cds::lock::ReentrantSpinT<Integral, Backoff>::tryLock(unsigned int) without object
                CDS_NOEXCEPT_( noexcept( tryLock(nTryCount) ))
#       endif
            {
                return tryLock( nTryCount );
            }

            /// Lock the object waits if it is busy
            void lock() CDS_NOEXCEPT
            {
                thread_id tid = OS::getCurrentThreadId();
                if ( !tryLockOwned( tid ) ) {
                    acquireLock();
                    beOwner( tid );
                }
            }

            /// Unlock the spin-lock. Return @p true if the current thread is owner of spin-lock @p false otherwise
            bool unlock() CDS_NOEXCEPT
            {
                if ( isOwned( OS::getCurrentThreadId() ) ) {
                    integral_type n = m_spin.load( CDS_ATOMIC::memory_order_relaxed );
                    if ( n > 1 )
                        m_spin.store( n - 1, CDS_ATOMIC::memory_order_relaxed );
                    else {
                        free();
                        m_spin.store( 0, CDS_ATOMIC::memory_order_release );
                    }
                    return true;
                }
                return false;
            }

            /// Change the owner of locked spin-lock. May be called by thread that is owner of the spin-lock
            bool changeOwner( OS::ThreadId newOwnerId ) CDS_NOEXCEPT
            {
                if ( isOwned( OS::getCurrentThreadId() ) ) {
                    assert( newOwnerId != OS::nullThreadId() );
                    m_OwnerId = newOwnerId;
                    return true;
                }
                return false;
            }
        };

        /// Recursive spin-lock based on atomic32u_t
        typedef ReentrantSpinT<atomic32u_t, backoff::LockDefault>   ReentrantSpin32;

        /// Recursive spin-lock based on atomic64u_t type
        typedef ReentrantSpinT<atomic64u_t, backoff::LockDefault>   ReentrantSpin64;

        /// Recursive spin-lock based on atomic32_t type
        typedef ReentrantSpin32                                     ReentrantSpin;

        /// The best (for the current platform) auto spin-lock
        typedef scoped_lock<Spin>   AutoSpin;

    }    // namespace lock

    /// Standard (best for the current platform) spin-lock implementation
    typedef lock::Spin              SpinLock;

    /// Standard (best for the current platform) recursive spin-lock implementation
    typedef lock::ReentrantSpin     RecursiveSpinLock;

    /// 32bit recursive spin-lock shortcut
    typedef lock::ReentrantSpin32   RecursiveSpinLock32;

    /// 64bit recursive spin-lock shortcut
    typedef lock::ReentrantSpin64   RecursiveSpinLock64;

    /// Auto spin-lock shortcut
    typedef lock::AutoSpin          AutoSpinLock;

} // namespace cds

#endif  // #ifndef __CDS_LOCK_SPINLOCK_H

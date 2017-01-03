/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#ifndef CDSLIB_SYNC_SPINLOCK_H
#define CDSLIB_SYNC_SPINLOCK_H

#include <cds/algo/atomic.h>
#include <cds/os/thread.h>
#include <cds/algo/backoff_strategy.h>

namespace cds {
    /// Synchronization primitives
    namespace sync {
        /// Spin lock
        /**
            Simple and light-weight spin-lock critical section
            It is useful to gain access to small (short-timed) code

            Algorithm:

                TATAS (test-and-test-and-lock)
                [1984] L. Rudolph, Z. Segall. Dynamic Decentralized Cache Schemes for MIMD Parallel Processors.

            No serialization performed - any of waiting threads may owns the spin-lock.
            This spin-lock is NOT recursive: the thread owned the lock cannot call \p lock() method without deadlock.
            The method \p unlock() can call any thread

            DEBUG version: The spinlock stores owner thead id. Assertion is raised when:
                - double lock attempt encountered by same thread (deadlock)
                - unlock by another thread

            If spin-lock is locked the \p Backoff algorithm is called. Predefined \p backoff::LockDefault class yields current
            thread and repeats lock attempts later

            Template parameters:
                - \p Backoff - backoff strategy. Used when spin lock is locked
        */
        template <typename Backoff >
        class spin_lock
        {
        public:
            typedef Backoff backoff_strategy;   ///< back-off strategy type
        private:
            atomics::atomic<bool>    m_spin;    ///< Spin
#    ifdef CDS_DEBUG
            typename OS::ThreadId    m_dbgOwnerId; ///< Owner thread id (only for debug mode)
#    endif

        public:
            /// Construct free (unlocked) spin-lock
            spin_lock() CDS_NOEXCEPT
                : m_spin( false )
#    ifdef CDS_DEBUG
                , m_dbgOwnerId( OS::c_NullThreadId )
#    endif
            {}

            /// Construct spin-lock in specified state
            /**
                In debug mode: if \p bLocked = true then spin-lock is made owned by current thread
            */
            explicit spin_lock( bool bLocked ) CDS_NOEXCEPT
#    ifdef CDS_DEBUG
                : m_dbgOwnerId( bLocked ? cds::OS::get_current_thread_id() : cds::OS::c_NullThreadId )
#    endif
            {
                m_spin.store( bLocked, atomics::memory_order_relaxed );
            }

            /// Dummy copy constructor
            /**
                In theory, spin-lock cannot be copied. However, it is not practical.
                Therefore, we provide dummy copy constructor that do no copy in fact. The ctor
                initializes the spin to free (unlocked) state like default ctor.
            */
            spin_lock(const spin_lock<Backoff>& ) CDS_NOEXCEPT
                : m_spin( false )
#   ifdef CDS_DEBUG
                , m_dbgOwnerId( cds::OS::c_NullThreadId )
#   endif
            {}

            /// Destructor. On debug time it checks whether spin-lock is free
            ~spin_lock()
            {
                assert( !m_spin.load( atomics::memory_order_relaxed ));
            }

            /// Check if the spin is locked
            bool is_locked() const CDS_NOEXCEPT
            {
                return m_spin.load( atomics::memory_order_relaxed );
            }

            /// Try to lock the object
            /**
                Returns \p true if locking is succeeded
                otherwise (if the spin is already locked) returns \p false

                Debug version: deadlock can be detected
            */
            bool try_lock() CDS_NOEXCEPT
            {
                bool bCurrent = false;

                m_spin.compare_exchange_strong( bCurrent, true, atomics::memory_order_acquire, atomics::memory_order_acquire );

                CDS_DEBUG_ONLY(
                    if ( !bCurrent ) {
                        m_dbgOwnerId = OS::get_current_thread_id();
                    }
                )
                return !bCurrent;
            }

            /// Try to lock the object, repeat \p nTryCount times if failed
            /**
                Returns \p true if locking is succeeded
                otherwise (if the spin is already locked) returns \p false
            */
            bool try_lock( unsigned int nTryCount ) CDS_NOEXCEPT_( noexcept( backoff_strategy()()))
            {
                backoff_strategy backoff;
                while ( nTryCount-- ) {
                    if ( try_lock())
                        return true;
                    backoff();
                }
                return false;
            }

            /// Lock the spin-lock. Waits infinitely while spin-lock is locked. Debug version: deadlock may be detected
            void lock() CDS_NOEXCEPT_(noexcept( backoff_strategy()()))
            {
                backoff_strategy backoff;

                // Deadlock detected
                assert( m_dbgOwnerId != OS::get_current_thread_id());

                // TATAS algorithm
                while ( !try_lock()) {
                    while ( m_spin.load( atomics::memory_order_relaxed )) {
                        backoff();
                    }
                }
                assert( m_dbgOwnerId == OS::get_current_thread_id());
            }

            /// Unlock the spin-lock. Debug version: deadlock may be detected
            void unlock() CDS_NOEXCEPT
            {
                assert( m_spin.load( atomics::memory_order_relaxed ));

                assert( m_dbgOwnerId == OS::get_current_thread_id());
                CDS_DEBUG_ONLY( m_dbgOwnerId = OS::c_NullThreadId; )

                m_spin.store( false, atomics::memory_order_release );
            }
        };

        /// Spin-lock implementation default for the current platform
        typedef spin_lock<backoff::LockDefault > spin;

        /// Recursive spin lock.
        /**
            Allows recursive calls: the owner thread may recursive enter to critical section guarded by the spin-lock.

            Template parameters:
                - \p Integral       one of integral atomic type: <tt>unsigned int</tt>, \p int, and others
                - \p Backoff        backoff strategy. Used when spin lock is locked
        */
        template <typename Integral, class Backoff>
        class reentrant_spin_lock
        {
            typedef OS::ThreadId    thread_id    ;        ///< The type of thread id

        public:
            typedef Integral        integral_type       ; ///< The integral type
            typedef Backoff         backoff_strategy    ; ///< The backoff type

        private:
            atomics::atomic<integral_type>   m_spin      ; ///< spin-lock atomic
            thread_id                        m_OwnerId   ; ///< Owner thread id. If spin-lock is not locked it usually equals to \p OS::c_NullThreadId

        private:
            //@cond
            void take( thread_id tid ) CDS_NOEXCEPT
            {
                m_OwnerId = tid;
            }

            void free() CDS_NOEXCEPT
            {
                m_OwnerId = OS::c_NullThreadId;
            }

            bool is_taken( thread_id tid ) const CDS_NOEXCEPT
            {
                return m_OwnerId == tid;
            }

            bool try_taken_lock( thread_id tid ) CDS_NOEXCEPT
            {
                if ( is_taken( tid )) {
                    m_spin.fetch_add( 1, atomics::memory_order_relaxed );
                    return true;
                }
                return false;
            }

            bool try_acquire() CDS_NOEXCEPT
            {
                integral_type nCurrent = 0;
                return m_spin.compare_exchange_weak( nCurrent, 1, atomics::memory_order_acquire, atomics::memory_order_acquire );
            }

            bool try_acquire( unsigned int nTryCount ) CDS_NOEXCEPT_( noexcept( backoff_strategy()()))
            {
                backoff_strategy bkoff;

                while ( nTryCount-- ) {
                    if ( try_acquire())
                        return true;
                    bkoff();
                }
                return false;
            }

            void acquire() CDS_NOEXCEPT_( noexcept( backoff_strategy()()))
            {
                // TATAS algorithm
                backoff_strategy bkoff;
                while ( !try_acquire()) {
                    while ( m_spin.load( atomics::memory_order_relaxed ))
                        bkoff();
                }
            }
            //@endcond

        public:
            /// Default constructor initializes spin to free (unlocked) state
            reentrant_spin_lock() CDS_NOEXCEPT
                : m_spin(0)
                , m_OwnerId( OS::c_NullThreadId )
            {}

            /// Dummy copy constructor
            /**
                In theory, spin-lock cannot be copied. However, it is not practical.
                Therefore, we provide dummy copy constructor that do no copy in fact. The ctor
                initializes the spin to free (unlocked) state like default ctor.
            */
            reentrant_spin_lock( const reentrant_spin_lock<Integral, Backoff>& ) CDS_NOEXCEPT
                : m_spin(0)
                , m_OwnerId( OS::c_NullThreadId )
            {}

            /// Construct object in specified state
            explicit reentrant_spin_lock( bool bLocked )
                : m_spin(0)
                , m_OwnerId( OS::c_NullThreadId )
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
                return !( m_spin.load( atomics::memory_order_relaxed ) == 0 || is_taken( cds::OS::get_current_thread_id()));
            }

            /// Try to lock the spin-lock (synonym for \p try_lock())
            bool try_lock() CDS_NOEXCEPT
            {
                thread_id tid = OS::get_current_thread_id();
                if ( try_taken_lock( tid ))
                    return true;
                if ( try_acquire()) {
                    take( tid );
                    return true;
                }
                return false;
            }

            /// Try to lock the object
            bool try_lock( unsigned int nTryCount ) CDS_NOEXCEPT_( noexcept( std::declval<reentrant_spin_lock>().try_acquire( nTryCount )))
            {
                thread_id tid = OS::get_current_thread_id();
                if ( try_taken_lock( tid ))
                    return true;
                if ( try_acquire( nTryCount )) {
                    take( tid );
                    return true;
                }
                return false;
            }

            /// Lock the object waits if it is busy
            void lock() CDS_NOEXCEPT_( noexcept( std::declval<reentrant_spin_lock>().acquire()))
            {
                thread_id tid = OS::get_current_thread_id();
                if ( !try_taken_lock( tid )) {
                    acquire();
                    take( tid );
                }
            }

            /// Unlock the spin-lock. Return \p true if the current thread is owner of spin-lock \p false otherwise
            bool unlock() CDS_NOEXCEPT
            {
                if ( is_taken( OS::get_current_thread_id())) {
                    integral_type n = m_spin.load( atomics::memory_order_relaxed );
                    if ( n > 1 )
                        m_spin.store( n - 1, atomics::memory_order_relaxed );
                    else {
                        free();
                        m_spin.store( 0, atomics::memory_order_release );
                    }
                    return true;
                }
                return false;
            }

            /// Change the owner of locked spin-lock. May be called by thread that is owner of the spin-lock
            bool change_owner( OS::ThreadId newOwnerId ) CDS_NOEXCEPT
            {
                if ( is_taken( OS::get_current_thread_id())) {
                    assert( newOwnerId != OS::c_NullThreadId );
                    m_OwnerId = newOwnerId;
                    return true;
                }
                return false;
            }
        };

        /// Recursive 32bit spin-lock
        typedef reentrant_spin_lock<uint32_t, backoff::LockDefault> reentrant_spin32;

        /// Default recursive spin-lock
        typedef reentrant_spin32 reentrant_spin;

        /// Recursive 64bit spin-lock
        typedef reentrant_spin_lock<uint64_t, backoff::LockDefault> reentrant_spin64;
    }    // namespace sync
} // namespace cds

#endif  // #ifndef CDSLIB_SYNC_SPINLOCK_H

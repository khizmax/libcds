// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_SYNC_POOL_MONITOR_H
#define CDSLIB_SYNC_POOL_MONITOR_H

#include <cds/sync/monitor.h>
#include <cds/algo/atomic.h>
#include <cds/algo/backoff_strategy.h>
#include <cds/opt/options.h> // opt::none

namespace cds { namespace sync {

    /// \p pool_monitor traits
    struct pool_monitor_traits {

        /// Dummy internal statistics if \p Stat template parameter is \p false
        struct empty_stat
        {
            //@cond
            void onLock()              const {}
            void onUnlock()            const {}
            void onLockContention()    const {}
            void onUnlockContention()  const {}
            void onLockAllocation()    const {}
            void onLockDeallocation()  const {}
            //@endcond
        };

        /// Monitor's internal statistics, used if \p Stat template parameter is \p true
        template <typename Counter = cds::atomicity::event_counter >
        struct stat
        {
            typedef Counter event_counter; ///< measure type

            event_counter m_nLockCount;         ///< Number of monitor \p lock() call
            event_counter m_nUnlockCount;       ///< Number of monitor \p unlock() call
            event_counter m_nMaxLocked;         ///< Max number of simuntaneously locked mutexes
            event_counter m_nLockContention;    ///< Number of \p lock() contenton
            event_counter m_nUnlockContention;  ///< Number of \p unlock() contention
            event_counter m_nLockAllocation;    ///< Number of the lock allocation from the pool
            event_counter m_nLockDeallocation;  ///< Number of the lock deallocation
            event_counter m_nMaxAllocated;      ///< Max number of sumultaneously allocated mutexes

            //@cond
            void onLock()
            {
                ++m_nLockCount;
                int nDiff = static_cast<int>( m_nLockCount.get() - m_nUnlockCount.get());
                if ( nDiff > 0 && m_nMaxLocked.get() < static_cast<typename event_counter::value_type>( nDiff ))
                    m_nMaxLocked = static_cast<typename event_counter::value_type>( nDiff );
            }
            void onUnlock()             { ++m_nUnlockCount;     }
            void onLockContention()     { ++m_nLockContention;  }
            void onUnlockContention()   { ++m_nUnlockContention;}
            void onLockAllocation()
            {
                ++m_nLockAllocation;
                int nDiff = static_cast<int>( m_nLockAllocation.get() - m_nLockDeallocation.get());
                if ( nDiff > 0 && m_nMaxAllocated.get() < static_cast<typename event_counter::value_type>( nDiff ))
                    m_nMaxAllocated = static_cast<typename event_counter::value_type>( nDiff );
            }
            void onLockDeallocation()   { ++m_nLockDeallocation;}
            //@endcond
        };
    };


    /// @ref cds_sync_monitor "Monitor" that allocates node's lock when needed
    /**
        The monitor is intended for reducing the number of system mutexes for
        huge containers like a tree. The monitor allocates the mutex from the pool \p LockPool
        only when container's node should be locked. Lifetime of node's mutex is managed by
        reference counter. When the reference counter to node's mutex becomes zero,
        the mutex is given back to the pool.

        The monitor is blocked: the access to node's mutex is performed under the spin-lock.
        However, node locking/unlocking is performed beyond the spin-lock.

        Template arguments:
        - \p LockPool - the @ref cds_memory_pool "pool type". The pool must maintain
            the objects of type \p std::mutex or similar. The access to the pool is not synchronized.
        - \p BackOff - back-off strategy for spinning, default is \p cds::backoff::Default
        - \p Stat - enable (\p true) or disable (\p false, the default) monitor's internal statistics.

        <b>How to use</b>
        \code
        typedef cds::memory::vyukov_queue_pool< std::mutex > pool_type;
        typedef cds::sync::pool_monitor< pool_type > sync_monitor;
        \endcode
    */
    template <class LockPool, typename BackOff = cds::backoff::Default, bool Stat = false >
    class pool_monitor
    {
    public:
        typedef LockPool pool_type; ///< Pool type
        typedef typename pool_type::value_type lock_type; ///< node lock type
        typedef typename std::conditional<
            std::is_same< BackOff, cds::opt::none >::value,
            cds::backoff::yield,
            BackOff
        >::type  back_off;  ///< back-off strategy for spinning
        typedef uint32_t refspin_type;  ///< Reference counter + spin-lock bit

        /// Internal statistics
        typedef typename std::conditional<
            Stat,
            typename pool_monitor_traits::stat<>,
            typename pool_monitor_traits::empty_stat
        >::type internal_stat;

        /// Pool's default capacity
        static constexpr size_t const c_nDefaultCapacity = 256;

    private:
        //@cond
        static constexpr refspin_type const c_nSpinBit = 1;
        static constexpr refspin_type const c_nRefIncrement = 2;
        mutable pool_type      m_Pool;
        mutable internal_stat  m_Stat;
        //@endcond

    public:

        /// Node injection
        struct node_injection
        {
            mutable atomics::atomic<refspin_type>   m_RefSpin;  ///< Spin-lock for \p m_pLock (bit 0) + reference counter
            mutable lock_type *                     m_pLock;    ///< Node-level lock

            //@cond
            node_injection()
                : m_pLock( nullptr )
            {
                m_RefSpin.store( 0, atomics::memory_order_release );
            }

            ~node_injection()
            {
                assert( m_pLock == nullptr );
                assert( m_RefSpin.load( atomics::memory_order_relaxed ) == 0 );
            }

            bool check_free() const
            {
                return m_pLock == nullptr && m_RefSpin.load( atomics::memory_order_relaxed ) == 0;
            }
            //@endcond
        };

        /// Initializes the pool of 256 preallocated mutexes
        pool_monitor()
            : m_Pool( c_nDefaultCapacity )
        {}

        /// Initializes the pool of \p nPoolCapacity preallocated mutexes
        pool_monitor( size_t nPoolCapacity )
            : m_Pool( nPoolCapacity ? nPoolCapacity : c_nDefaultCapacity )
        {}

        /// Makes exclusive access to node \p p
        template <typename Node>
        void lock( Node const& p ) const
        {
            lock_type * pLock;

            m_Stat.onLock();

            // try lock spin and increment reference counter
            refspin_type cur = p.m_SyncMonitorInjection.m_RefSpin.load( atomics::memory_order_relaxed ) & ~c_nSpinBit;
            if ( !p.m_SyncMonitorInjection.m_RefSpin.compare_exchange_weak( cur, cur + c_nRefIncrement + c_nSpinBit,
                atomics::memory_order_acq_rel, atomics::memory_order_acquire ))
            {
                back_off bkoff;
                do {
                    m_Stat.onLockContention();
                    bkoff();
                    cur &= ~c_nSpinBit;
                } while ( !p.m_SyncMonitorInjection.m_RefSpin.compare_exchange_weak( cur, cur + c_nRefIncrement + c_nSpinBit,
                    atomics::memory_order_acq_rel, atomics::memory_order_acquire ));
            }

            // spin locked
            // If the node has no lock, allocate it from pool
            pLock = p.m_SyncMonitorInjection.m_pLock;
            if ( !pLock ) {
                assert( cur == 0 );
                pLock = p.m_SyncMonitorInjection.m_pLock = m_Pool.allocate( 1 );
                assert( pLock != nullptr );
                m_Stat.onLockAllocation();
            }

            // unlock spin
            p.m_SyncMonitorInjection.m_RefSpin.store( cur + c_nRefIncrement, atomics::memory_order_release );

            // lock the node
            pLock->lock();
        }

        /// Unlocks the node \p p
        template <typename Node>
        void unlock( Node const& p ) const
        {
            lock_type * pLock = nullptr;

            m_Stat.onUnlock();

            assert( p.m_SyncMonitorInjection.m_pLock != nullptr );
            p.m_SyncMonitorInjection.m_pLock->unlock();

            // try lock spin
            refspin_type cur = p.m_SyncMonitorInjection.m_RefSpin.load( atomics::memory_order_relaxed ) & ~c_nSpinBit;
            if ( !p.m_SyncMonitorInjection.m_RefSpin.compare_exchange_weak( cur, cur | c_nSpinBit,
                atomics::memory_order_acquire, atomics::memory_order_acquire ))
            {
                back_off bkoff;
                do {
                    m_Stat.onUnlockContention();
                    bkoff();
                    cur &= ~c_nSpinBit;
                } while ( !p.m_SyncMonitorInjection.m_RefSpin.compare_exchange_weak( cur, cur | c_nSpinBit,
                    atomics::memory_order_acquire, atomics::memory_order_acquire ));
            }

            // spin locked now

            // If we are the unique owner - deallocate lock
            if ( cur == c_nRefIncrement ) {
                pLock = p.m_SyncMonitorInjection.m_pLock;
                p.m_SyncMonitorInjection.m_pLock = nullptr;
            }

            // unlock spin
            p.m_SyncMonitorInjection.m_RefSpin.store( cur - c_nRefIncrement, atomics::memory_order_release );

            // free pLock
            if ( pLock ) {
                m_Pool.deallocate( pLock, 1 );
                m_Stat.onLockDeallocation();
            }
        }

        /// Scoped lock
        template <typename Node>
        using scoped_lock = monitor_scoped_lock< pool_monitor, Node >;

        /// Returns the reference to internal statistics
        /**
            If class' template argument \p Stat is \p false,
            the function returns \ref pool_monitor_traits::empty_stat "dummy statistics".
            Otherwise, it returns the reference to monitor's internal statistics
            of type \ref pool_monitor_traits::stat.
        */
        internal_stat const& statistics() const
        {
            return m_Stat;
        }
    };

}} // namespace cds::sync

#endif // #ifndef CDSLIB_SYNC_POOL_MONITOR_H


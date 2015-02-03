//$$CDS-header$$

#ifndef CDSLIB_SYNC_POOL_MONITOR_H
#define CDSLIB_SYNC_POOL_MONITOR_H

#include <cds/sync/monitor.h>
#include <cds/algo/atomic.h>
#include <cds/algo/backoff_strategy.h>

namespace cds { namespace sync {

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
        - \p BackOff - back-off strategy for spinning, default is \p cds::backoff::LockDefault

        <b>How to use</b>
        \code
        typedef cds::memory::vyukov_queue_pool< std::mutex > pool_type;
        typedef cds::sync::pool_monitor< pool_type > sync_monitor;
        \endcode
    */
    template <class LockPool, typename BackOff = cds::backoff::LockDefault >
    class pool_monitor
    {
    public:
        typedef LockPool pool_type; ///< Pool type
        typedef typename pool_type::value_type lock_type; ///< node lock type
        typedef BackOff  back_off;  ///< back-off strategy for spinning

    private:
        //@cond
        pool_type   m_Pool;
        //@endcond

    public:
        /// Monitor injection into \p Node
        template <typename Node>
        class node_injection : public Node
        {
            //@cond
            typedef unsigned int refspin_type;
            constexpr refspin_type const c_nSpinBit = 1;
            constexpr refspin_type const c_nRefIncrement = 2;

            struct injection
            {
                atomics::atomic<refspin_type>   m_RefSpin;  ///< Spin-lock for \p m_pLock (bit 0) + reference counter
                lock_type *                     m_pLock;    ///< Node-level lock

                injection()
                    : m_Access( 0 )
                    , m_pLock( nullptr )
                {}
            };
            //@endcond

        public:
            mutable injection   m_Access;   ///< injected data

#       ifdef CDS_CXX11_INHERITING_CTOR
            using Node::Node;
#       else
            // Inheriting ctor emulation
            template <typename... Args>
            node_injection( Args&&... args )
                : Node( std::forward<Args>( args )... )
            {}
#       endif
        };

        /// Initializes the pool of 256 preallocated mutexes
        pool_monitor()
            : m_Pool( 256 )
        {}

        /// Initializes the pool of \p nPoolCapacity preallocated mutexes
        pool_monitor( size_t nPoolCapacity )
            : m_Pool( nPoolCapacity)
        {}

        /// Makes exclusive access to node \p p
        template <typename Node>
        void lock( Node const& p ) const
        {
            lock_type * pLock;

            // try lock spin and increment reference counter
            refspin_type cur = p.m_Access.m_RefSpin.load( atomics::memory_order_relaxed ) & ~c_nSpinBit;
            if ( !p.m_Access.m_RefSpin.compare_exchange_weak( cur, cur + c_nRefIncrement + c_nSpinBit, 
                atomics::memory_order_acquire, atomics::memory_order_relaxed ) ) 
            {
                back_off bkoff;
                do {
                    bkoff();
                    cur &= ~c_nSpinBit;
                } while ( !p.m_Access.m_RefSpin.compare_exchange_weak( cur, cur + c_nRefIncrement + c_nSpinBit, 
                    atomics::memory_order_acquire, atomics::memory_order_relaxed );
            }

            // spin locked
            // If the node has no lock, allocate it from pool
            pLock = p.m_Access.m_pLock;
            if ( !pLock )
                pLock = p.m_Access.m_pLock = m_Pool.allocate( 1 );

            // unlock spin
            p.m_Access.m_RefSpin.store( cur + c_nRefIncrement, atomics::memory_order_release );

            // lock the node
            pLock->lock();
        }

        /// Unlocks the node \p p
        template <typename Node>
        void unlock( Node const& p ) const
        {
            lock_type * pLock = nullptr;

            assert( p.m_Access.m_pLock != nullptr );
            p.m_Access.m_pLock->unlock();

            // try lock spin
            refspin_type cur = p.m_Access.m_RefSpin.load( atomics::memory_order_relaxed ) & ~c_nSpinBit;
            if ( !p.m_Access.m_RefSpin.compare_exchange_weak( cur, cur + c_nSpinBit, 
                atomics::memory_order_acquire, atomics::memory_order_relaxed ) ) 
            {
                back_off bkoff;
                do {
                    bkoff();
                    cur &= ~c_nSpinBit;
                } while ( !p.m_Access.m_RefSpin.compare_exchange_weak( cur, cur + c_nSpinBit, 
                    atomics::memory_order_acquire, atomics::memory_order_relaxed );
            }

            // spin locked now

            // If we are the unique owner - deallocate lock
            if ( cur == c_nRefIncrement ) {
                pLock = p.m_Access.m_pLock;
                p.m_Access.m_pLock = nullptr;
            }

            // unlock spin
            p.m_Access.m_RefSpin.store( cur - c_nRefIncrement, atomics::memory_order_release );

            // free pLock
            if ( pLock )
                m_Pool.deallocate( pLock, 1 );
        }

        /// Scoped lock
        template <typename Node>
        using scoped_lock = monitor_scoped_lock< pool_monitor, Node >;
    };

}} // namespace cds::sync

#endif // #ifndef CDSLIB_SYNC_POOL_MONITOR_H


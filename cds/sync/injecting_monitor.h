// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_SYNC_INJECTING_MONITOR_H
#define CDSLIB_SYNC_INJECTING_MONITOR_H

#include <cds/sync/monitor.h>

namespace cds { namespace sync {

    //@cond
    struct injecting_monitor_traits {
        struct empty_stat
        {};
    };
    //@endcond

    /// @ref cds_sync_monitor "Monitor" that injects the lock into each node
    /**
        This simple monitor injects the lock object of type \p Lock into each node.
        The monitor is designed for user-space locking primitives like \ref sync::spin_lock "spin-lock".

        Template arguments:
        - Lock - lock type like \p std::mutex or \p cds::sync::spin
    */
    template <typename Lock>
    class injecting_monitor
    {
    public:
        typedef Lock lock_type; ///< Lock type

        /// Node injection
        struct node_injection {
            mutable lock_type m_Lock;   ///< Node spin-lock

            //@cond
            constexpr bool check_free() const
            {
                return true;
            }
            //@endcond
        };

        /// Makes exclusive access to node \p p
        template <typename Node>
        void lock( Node const& p ) const
        {
            p.m_SyncMonitorInjection.m_Lock.lock();
        }

        /// Unlocks the node \p p
        template <typename Node>
        void unlock( Node const& p ) const
        {
            p.m_SyncMonitorInjection.m_Lock.unlock();
        }

        //@cond
        injecting_monitor_traits::empty_stat statistics() const
        {
            return injecting_monitor_traits::empty_stat();
        }
        //@endcond

        /// Scoped lock
        template <typename Node>
        using scoped_lock = monitor_scoped_lock< injecting_monitor, Node >;
    };
}} // namespace cds::sync

#endif // #ifndef CDSLIB_SYNC_INJECTING_MONITOR_H

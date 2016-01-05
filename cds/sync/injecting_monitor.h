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

#ifndef CDSLIB_SYNC_INJECTING_MONITOR_H
#define CDSLIB_SYNC_INJECTING_MONITOR_H

#include <cds/sync/monitor.h>
#ifndef CDS_CXX11_INHERITING_CTOR
#   include <utility> // std::forward
#endif

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
            CDS_CONSTEXPR bool check_free() const
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

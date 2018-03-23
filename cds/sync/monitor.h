// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_SYNC_MONITOR_H
#define CDSLIB_SYNC_MONITOR_H

#include <cds/details/defs.h>

namespace cds { namespace sync {
    /**
        @page cds_sync_monitor Synchronization monitor
        A <a href="http://en.wikipedia.org/wiki/Monitor_%28synchronization%29">monitor</a> is synchronization construction
        that allows threads to have both mutual exclusion and the ability to wait (block) for a certain condition to become true.
        Some blocking data structure algoritms like the trees require per-node locking.
        For huge trees containing millions of nodes it can be very inefficient to inject
        the lock object into each node. Moreover, some operating systems may not support
        the millions of system objects like mutexes per user process.
        The monitor strategy is intended to solve that problem.
        When the node should be locked, the monitor is called to allocate appropriate
        lock object for the node if needed, and to lock the node.
        The monitor strategy can significantly reduce the number of system objects
        required for data structure.

        <b>Implemetations</b>

        \p libcds contains several monitor implementations:
        - \p sync::injecting_monitor injects the lock object into each node.
            That mock monitor is designed for user-space locking primitive like
            \ref sync::spin_lock "spin-lock".
        - \p sync::pool_monitor is the monitor that allocates a lock object
            for a node from the pool when needed. When the node is unlocked
            the lock assigned to it is given back to the pool if no thread
            references to that node.

        <b>How to use</b>

        If you use a container from \p libcds that requires a monitor, you should just
        specify required monitor type in container's traits. Usually, the monitor
        is specified by \p traits::sync_monitor typedef, or by \p cds::opt::sync_monitor
        option for container's \p make_traits metafunction.

        If you're developing a new container algorithm, you should know internal monitor
        interface:
        \code
        class Monitor {
        public:
            // Monitor's injection into the Node class
            template <typename Node>
            struct node_injection: public Node
            {
                // Monitor data injecting into container's node
                // ...
            };
            // Locks the node
            template <typename Node>
            void lock( Node& node );
            // Unlocks the node
            template <typename Node>
            void unlock( Node& node );
            // Scoped lock applyes RAII to Monitor
            template <typename Node>
            using scoped_lock = monitor_scoped_lock< pool_monitor, Node >;
        };
        \endcode
        Monitor's data must be injected into container's node as \p m_SyncMonitorInjection data member:
        \code
        template <typename SyncMonitor>
        struct my_node
        {
            // ...
            typename SyncMonitor::node_injection m_SyncMonitorInjection;
        };
        \endcode
        The monitor must be a member of your container:
        \code
        template <typename GC, typename T, typename Traits>
        class my_container {
            // ...
            typedef typename Traits::sync_monitor   sync_monitor;
            typedef my_node<sync_monitor> node_type;
            sync_monitor m_Monitor;
            //...
        };
        \endcode
    */
    /// Monitor scoped lock (RAII)
    /**
        Template arguments:
        - \p Monitor - monitor type
        - \p Node - node type
    */
    template <typename Monitor, typename Node>
    struct monitor_scoped_lock
    {
    public:
        typedef Monitor monitor_type;   ///< Monitor type
        typedef Node    node_type;      ///< Node type
    private:
        //@cond
        monitor_type&    m_Monitor; ///< Monitor
        node_type const& m_Node;    ///< Our locked node
        //@endcond
    public:
        /// Makes exclusive access to the node \p p by \p monitor
        monitor_scoped_lock( monitor_type& monitor, node_type const& p )
            : m_Monitor( monitor )
            , m_Node( p )
        {
            monitor.lock( p );
        }
        /// Unlocks the node
        ~monitor_scoped_lock()
        {
            m_Monitor.unlock( m_Node );
        }
    };
}} // namespace cds::sync
#endif // #ifndef CDSLIB_SYNC_MONITOR_H


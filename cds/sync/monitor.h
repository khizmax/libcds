//$$CDS-header$$

#ifndef CDSLIB_SYNC_MONITOR_H
#define CDSLIB_SYNC_MONITOR_H

#include <cds/details/defs.h>

namespace cds { namespace sync {

    /**
        @page cds_sync_monitor Synchronization monitor

        A <a href="http://en.wikipedia.org/wiki/Monitor_%28synchronization%29">monitor</a> is synchronization construct
        that allows threads to have both mutual exclusion and the ability to wait (block) for a certain condition to become true.

        Some blocking data structure algoritms like the trees require per-node locking.
        For huge trees containing millions of nodes it can be very inefficient to inject
        the lock object into each node. Moreover, some operating systems may not support
        the millions of system objects like mutexes per user process.

        The monitor strategy is intended to solve that problem.
        When the node should be locked, the monitor is called to allocate appropriate
        lock object for the node if it's needed, and to lock the node.
        The monitor strategy can significantly reduce the number of system objects
        required for the data structure.

        <b>Implemetatios</b>

        \p libcds contains several monitor implementations:
        - \p sync::injecting_monitor injects the lock object into each node.
            That mock monitor is designed for user-space locking primitive like
            \ref sync::spin_lock "spin-lock".

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
            struct node_wrapper;

            // Locks the node 
            template <typename Node>
            void lock( Node& node );

            // Unlocks the node
            template <typename Node>
            void unlock( Node& node );

            // Scoped lock applyes RAII to Monitor
            template <typename Node>
            class scoped_lock 
            {
            public:
                // Locks node by monitor mon
                scoped_lock( Monitor& mon, Node& node );

                // Unlocks the node locked by ctor
                ~scoped_lock();
            };
        };
        \endcode
        The monitor should be a member of your container:
        \code
        template <typename GC, typename T, typename Traits>
        class my_container {
            // ...
            typedef typename Traits::sync_monitor   sync_monitor;
            sync_monitor m_Monitor;
            //...
        };
        \endcode
    */

}} // namespace cds::sync

#endif // #ifndef CDSLIB_SYNC_MONITOR_H


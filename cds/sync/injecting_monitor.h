//$$CDS-header$$

#ifndef CDSLIB_SYNC_INJECTING_MONITOR_H
#define CDSLIB_SYNC_INJECTING_MONITOR_H

#include <cds/sync/monitor.h>
#ifndef CDS_CXX11_INHERITING_CTOR
#   include <utility> // std::forward
#endif

namespace cds { namespace sync {

    /// @ref cds_sync_monitor "Monitor" that injects the lock into each node
    /**
        This monitor injects the lock object of type \p Lock into each node. 
        The monitor is designed for user-space locking primitives like \ref sync::spin_lock "spin-lock".

        Template arguments:
        - Lock - lock type like \p std::mutex or \p cds::sync::spin
    */
    template <typename Lock>
    class injecting_monitor
    {
    public:
        typedef Lock lock_type; ///< Lock type

        /// Monitor injection into \p Node
        struct node_injection
        {
#       ifdef CDS_CXX11_INHERITING_CTOR
            using Node::Node;
#       else
            // Inheriting ctor emulation
            template <typename... Args>
            node_injection( Args&&... args )
                : Node( std::forward<Args>( args )... )
            {}
#       endif
            mutable lock_type m_Lock; ///< Node-level lock

            /// Makes exclusive access to the node
            void lock() const
            {
                m_Lock.lock;
            }

            /// Unlocks the node
            void unlock() const
            {
                m_Lock.unlock();
            }
        };

        /// Makes exclusive access to node \p p
        /**
            \p p must have method \p lock()
        */
        template <typename Node>
        void lock( Node const& p ) const
        {
            p.lock();
        }

        /// Unlocks the node \p p
        /**
            \p p must have method \p unlock()
        */
        template <typename Node>
        void unlock( Node const& p ) const
        {
            p.unlock();
        }

        /// Scoped lock
        template <typename Node>
        class scoped_lock
        {
            Node const& m_Locked;  ///< Our locked node

        public:
            /// Makes exclusive access to node \p p
            scoped_lock( injecting_monitor const&, Node const& p )
                : m_Locked( p )
            {
                p.lock();
            }

            /// Unlocks the node
            ~scoped_lock()
            {
                p.unlock();
            }
        };
    };
}} // namespace cds::sync

#endif // #ifndef CDSLIB_SYNC_INJECTING_MONITOR_H

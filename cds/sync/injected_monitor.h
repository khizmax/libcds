//$$CDS-header$$

#ifndef CDSLIB_SYNC_INJECTED_MONITOR_H
#define CDSLIB_SYNC_INJECTED_MONITOR_H

namespace cds { namespace sync {

    /// @ref cds_sync_monitor "Monitor" that injects the lock into each node
    /**
        This monitor injects the lock object of type \p Lock into each node. 
        The monitor is designed for user-space locking primitives like \ref sync::spin_lock "spin-lock".

        Template arguments:
        - Lock - lock type like \p std::mutex or \p cds::sync::spin


    */
    template <typename Lock>
    class injected_monitor
    {
    public:
        typedef Lock lock_type; ///< Lock type

        /// Monitor injection into \p T
        template <typename T>
        struct wrapper : public T
        {
            using T::T;
            mutable lock_type m_Lock; ///< Node-level lock

            /// Makes exclusive access to the object
            void lock() const
            {
                m_Lock.lock;
            }

            /// Unlocks the object
            void unlock() const
            {
                m_Lock.unlock();
            }
        };

        /// Makes exclusive access to node \p p of type \p T
        /**
            \p p must have method \p lock()
        */
        template <typename T>
        void lock( T const& p ) const
        {
            p.lock();
        }

        /// Unlocks the node \p p of type \p T
        /**
            \p p must have method \p unlock()
        */
        template <typename T>
        void unlock( T const& p ) const
        {
            p.unlock();
        }

        /// Scoped lock
        template <typename T>
        class scoped_lock
        {
            T const& m_Locked;  ///< Our locked node

        public:
            /// Makes exclusive access to object \p p of type T
            scoped_lock( injected_monitor const&, T const& p )
                : m_Locked( p )
            {
                p.lock();
            }

            /// Unlocks the object
            ~scoped_lock()
            {
                p.unlock();
            }
        };
    };
}} // namespace cds::sync

#endif // #ifndef CDSLIB_SYNC_INJECTED_MONITOR_H

//$$CDS-header$$

#ifndef CDSLIB_SYNC_INJECTED_MONITOR_H
#define CDSLIB_SYNC_INJECTED_MONITOR_H

namespace cds { namespace sync {

    /// Monitor that injects a lock as a member into a class
    /**
        Template arguments:
        - Lock - lock type like \p std::mutex or \p cds::sync::spin
    */
    template <typename Lock>
    class injected_monitor
    {
    public:
        typedef Lock lock_type;

        template <typename T>
        struct wrapper : public T
        {
            using T::T;
            mutable lock_type m_Lock;

            void lock() const
            {
                m_Lock.lock;
            }

            void unlock() const
            {
                m_Lock.unlock();
            }
        };

        template <typename T>
        void lock( T const& p ) const
        {
            p.lock();
        }

        template <typename T>
        void unlock( T const& p ) const
        {
            p.unlock();
        }

        template <typename T>
        class scoped_lock
        {
            T const& m_Locked;

        public:
            scoped_lock( injected_monitor const&, T const& p )
                : m_Locked( p )
            {
                p.lock();
            }

            ~scoped_lock()
            {
                p.unlock();
            }
        };
    };
}} // namespace cds::sync

#endif // #ifndef CDSLIB_SYNC_INJECTED_MONITOR_H

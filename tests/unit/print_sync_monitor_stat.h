//$$CDS-header$$

#if defined(CDSLIB_SYNC_INJECTING_MONITOR_H) && !defined(CDSUNIT_PRINT_INJECTING_MONITOR_STAT_H)
#define CDSUNIT_PRINT_INJECTING_MONITOR_STAT_H

namespace std {
    static inline ostream& operator <<( ostream& o, cds::sync::injecting_monitor_traits::empty_stat const& /*s*/ )
    {
        return o;
    }
}
#endif

#if defined(CDSLIB_SYNC_POOL_MONITOR_H) && !defined(CDSUNIT_PRINT_POOL_MONITOR_STAT_H)
#define CDSUNIT_PRINT_POOL_MONITOR_STAT_H

namespace std {
    static inline ostream& operator <<( ostream& o, cds::sync::pool_monitor_traits::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::sync::pool_monitor_traits::stat<> const& s )
    {
        return o << "cds::sync::pool_monitor statistics:\n"
            << "\t\t        m_nLockCount: " << s.m_nLockCount.get()        << "\n"
            << "\t\t      m_nUnlockCount: " << s.m_nUnlockCount.get()      << "\n"
            << "\t\t        m_nMaxLocked: " << s.m_nMaxLocked.get()        << "\n"
            << "\t\t   m_nLockContention: " << s.m_nLockContention.get()   << "\n"
            << "\t\t m_nUnlockContention: " << s.m_nUnlockContention.get() << "\n"
            << "\t\t   m_nLockAllocation: " << s.m_nLockAllocation.get()   << "\n"
            << "\t\t m_nLockDeallocation: " << s.m_nLockDeallocation.get() << "\n"
            << "\t\t     m_nMaxAllocated: " << s.m_nMaxAllocated.get()     << "\n";
    }
}
#endif

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

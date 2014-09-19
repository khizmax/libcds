//$$CDS-header$$

#ifndef __CDS_OS_WIN_THREAD_H
#define __CDS_OS_WIN_THREAD_H

#include <windows.h>

namespace cds { namespace OS {
    /// Windows-specific functions
    namespace Win32 {

        /// Tests whether the thread is alive
        static inline bool isThreadAlive( std::thread::id id )
        {
            HANDLE h = ::OpenThread( SYNCHRONIZE, FALSE, id );
            if ( h == NULL )
                return false;
            ::CloseHandle( h );
            return true;
        }

        /// Yield current thread, switch to another
        static inline void yield()
        {
    #    if _WIN32_WINNT >= 0x0400
            ::SwitchToThread();
    #   else
            ::Sleep(0);
    #   endif
        }

        /// Default backoff::yield implementation
        static inline void    backoff()
        {
            yield();
        }
    }    // namespace Win32

    using Win32::isThreadAlive;
    using Win32::yield;
    using Win32::backoff;

}} // namespace cds::OS

#endif // #ifndef __CDS_OS_WIN_THREAD_H

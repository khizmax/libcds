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

        /// Default backoff::yield implementation
        static inline void    backoff()
        {
            std::this_thread::yield();
        }
    }    // namespace Win32

    using Win32::isThreadAlive;
    using Win32::backoff;

}} // namespace cds::OS

#endif // #ifndef __CDS_OS_WIN_THREAD_H

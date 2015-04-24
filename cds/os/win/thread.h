//$$CDS-header$$

#ifndef CDSLIB_OS_WIN_THREAD_H
#define CDSLIB_OS_WIN_THREAD_H

#define NOMINMAX
#include <windows.h>

namespace cds { namespace OS {
    /// Windows-specific functions
    CDS_CXX11_INLINE_NAMESPACE namespace Win32 {

        /// OS-specific type of thread identifier
        typedef DWORD           ThreadId;

        /// Get current thread id
        static inline ThreadId get_current_thread_id()
        {
            return ::GetCurrentThreadId();
        }

        /// Tests whether the thread is alive
        static inline bool is_thread_alive( ThreadId id )
        {
            HANDLE h = ::OpenThread( SYNCHRONIZE, FALSE, id );
            if ( h == nullptr )
                return false;
            ::CloseHandle( h );
            return true;
        }
    }    // namespace Win32

    //@cond
    CDS_CONSTEXPR const Win32::ThreadId c_NullThreadId = 0;

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    using Win32::ThreadId;
    using Win32::get_current_thread_id;
    using Win32::is_thread_alive;
#endif
    //@endcond

}} // namespace cds::OS

#endif // #ifndef CDSLIB_OS_WIN_THREAD_H

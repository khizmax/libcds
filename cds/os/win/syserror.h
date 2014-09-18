//$$CDS-header$$

#ifndef __CDS_OS_WIN_SYSERROR_H
#define __CDS_OS_WIN_SYSERROR_H

#include <windows.h>
#include <string>

namespace cds { namespace OS {
    namespace Win32 {

        /// OS-specific type of error code
        typedef DWORD            error_code;

        /// Get system error code
        static inline error_code  getErrorCode()
        {
            return ::GetLastError();
        }

        /// Get system error text
        static inline std::string getSystemErrorText( error_code nCode )
        {
            char *ptmp = 0;
            if ( !FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                nCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &ptmp, 0, 0  )
                )
            {
                if ( ptmp )
                    LocalFree( ptmp );
                return std::string();
            }
            std::string str( ptmp );
            LocalFree( ptmp );
            return str;
        }
    }    // namespace Win32

    using Win32::error_code;
    using Win32::getErrorCode;
    using Win32::getSystemErrorText;
}} // namespace cds::OS

#endif // #ifndef __CDS_OS_WIN_SYSERROR_H


/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __UNIT_LOCK_WIN32_LOCK_H
#define __UNIT_LOCK_WIN32_LOCK_H

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

#define UNIT_LOCK_WIN_CS

namespace lock {
    namespace win {
        // Win32 critical section
        class CS {
            CRITICAL_SECTION    m_cs;

        public:
            CS()    { ::InitializeCriticalSection( &m_cs ) ; }
            ~CS()   { ::DeleteCriticalSection( &m_cs ) ; }

            void lock()     { ::EnterCriticalSection( &m_cs ) ; }
            void unlock()   { ::LeaveCriticalSection( &m_cs)  ; }
            bool try_lock()  { return ::TryEnterCriticalSection( &m_cs ) != 0 ; }
        };

        class Mutex {
            HANDLE  m_hMutex;
        public:

            Mutex()     { m_hMutex = ::CreateMutex( nullptr, false, nullptr ); }
            ~Mutex()    { ::CloseHandle( m_hMutex ) ; }

            void lock()     { ::WaitForSingleObject( m_hMutex, INFINITE ); }
            void unlock()   { ::ReleaseMutex( m_hMutex ); }
            bool try_lock()  { return ::WaitForSingleObject( m_hMutex, 0) == WAIT_OBJECT_0; }
        };

    } // namespace win
}   // namespace lock

#endif  // defined(_WIN32) || defined(_WIN64)
#endif  // #ifndef __UNIT_LOCK_WIN32_LOCK_H

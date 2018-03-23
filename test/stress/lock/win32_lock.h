// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_LOCK_WIN32_LOCK_H
#define CDSUNIT_LOCK_WIN32_LOCK_H

#if defined(_WIN32) || defined(_WIN64)
#define NOMINMAX
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
#endif  // #ifndef CDSUNIT_LOCK_WIN32_LOCK_H

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

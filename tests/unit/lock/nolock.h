/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __UNIT_LOCK_NOLOCK_H
#define __UNIT_LOCK_NOLOCK_H

namespace lock {
    // Win32 critical section
    class NoLock {
    public:
        void lock()     {}
        void unlock()   {}
        bool try_lock()  { return true; }
    };
}

#endif // #ifndef __UNIT_LOCK_NOLOCK_H

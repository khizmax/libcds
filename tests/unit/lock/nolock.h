//$$CDS-header$$

#ifndef __UNIT_LOCK_NOLOCK_H
#define __UNIT_LOCK_NOLOCK_H

namespace lock {
    // Win32 critical section
    class NoLock {
    public:
        void lock()     {}
        void unlock()   {}
        bool tryLock()  { return true; }
    };
}

#endif // #ifndef __UNIT_LOCK_NOLOCK_H

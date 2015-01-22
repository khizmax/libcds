//$$CDS-header$$

#ifndef CDSUNIT_LOCK_NOLOCK_H
#define CDSUNIT_LOCK_NOLOCK_H

namespace lock {
    // Win32 critical section
    class NoLock {
    public:
        void lock()     {}
        void unlock()   {}
        bool try_lock()  { return true; }
    };
}

#endif // #ifndef CDSUNIT_LOCK_NOLOCK_H

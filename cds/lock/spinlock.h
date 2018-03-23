// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_LOCK_SPINLOCK_H
#define CDSLIB_LOCK_SPINLOCK_H

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma message("cds/lock/spinlock.h is deprecated, use cds/sync/spinlock.h instead")
#else
#   warning "cds/lock/spinlock.h is deprecated, use cds/sync/spinlock.h instead"
#endif

#include <cds/sync/spinlock.h>

//@cond
namespace cds {
    /// Synchronization primitives (deprecated namespace, use \p cds::sync namespace instead)
    namespace lock {

        /// Alias for \p cds::sync::spin_lock for backward compatibility
        template <typename Backoff>
        using Spinlock = cds::sync::spin_lock< Backoff >;

        /// Spin-lock implementation default for the current platform
        typedef cds::sync::spin_lock< backoff::LockDefault> Spin;

        /// Alias for \p cds::sync::reentrant_spin_lock for backward compatibility
        template <typename Integral, class Backoff>
        using ReentrantSpinT = cds::sync::reentrant_spin_lock< Integral, Backoff >;

        /// Recursive 32bit spin-lock
        typedef cds::sync::reentrant_spin32 ReentrantSpin32;

        /// Recursive 64bit spin-lock
        typedef cds::sync::reentrant_spin64 ReentrantSpin64;

        /// Default recursive spin-lock type
        typedef ReentrantSpin32 ReentrantSpin;

    }    // namespace lock

    /// Standard (best for the current platform) spin-lock implementation
    typedef lock::Spin              SpinLock;

    /// Standard (best for the current platform) recursive spin-lock implementation
    typedef lock::ReentrantSpin     RecursiveSpinLock;

    /// 32bit recursive spin-lock shortcut
    typedef lock::ReentrantSpin32   RecursiveSpinLock32;

    /// 64bit recursive spin-lock shortcut
    typedef lock::ReentrantSpin64   RecursiveSpinLock64;

} // namespace cds
//@endcond

#endif  // #ifndef CDSLIB_LOCK_SPINLOCK_H

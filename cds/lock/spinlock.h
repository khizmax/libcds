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

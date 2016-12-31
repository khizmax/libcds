/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

// This header should be included AFTER <cds/algo/atomic.h> if needed

namespace {

    static inline atomics::memory_order convert_to_store_order( atomics::memory_order order )
    {
        switch ( order ) {
            case atomics::memory_order_acquire:
            case atomics::memory_order_consume:
                return atomics::memory_order_relaxed;
            case atomics::memory_order_acq_rel:
                return atomics::memory_order_release;
            default:
                return order;
        }
    }

    static inline atomics::memory_order convert_to_load_order( atomics::memory_order order )
    {
        switch ( order ) {
            case atomics::memory_order_release:
                return atomics::memory_order_relaxed;
            case atomics::memory_order_acq_rel:
                return atomics::memory_order_acquire;
            default:
                return order;
        }
    }

#if CDS_COMPILER == CDS_COMPILER_INTEL
    static inline atomics::memory_order convert_to_exchange_order( atomics::memory_order order )
    {
        return order == atomics::memory_order_consume ? atomics::memory_order_relaxed : order;
    }
#else
    static inline atomics::memory_order convert_to_exchange_order( atomics::memory_order order )
    {
        return order;
    }
#endif

    template <typename T, bool Volatile>
    struct add_volatile;

    template <typename T>
    struct add_volatile<T, false>
    {
        typedef T   type;
    };

    template <typename T>
    struct add_volatile<T, true>
    {
        typedef T volatile   type;
    };

} // namespace

//$$CDS-header$$

#ifndef __CDS_COMPILER_ICL_CXX11_ATOMIC_PATCHES_WIN_H
#define __CDS_COMPILER_ICL_CXX11_ATOMIC_PATCHES_WIN_H

//@cond
#if CDS_COMPILER == CDS_COMPILER_INTEL && CDS_CXX11_ATOMIC_SUPPORT == 1 && _MSC_VER < 1700

namespace std {
    typedef std::atomic<size_t> atomic_size_t;

    // Fences
    static inline void atomic_thread_fence(memory_order order) CDS_NOEXCEPT
    {
        CDS_COMPILER_RW_BARRIER;
    }
    static inline void atomic_signal_fence(memory_order order) CDS_NOEXCEPT
    {
        CDS_COMPILER_RW_BARRIER;
    }

}

#endif
//@endcond

#endif // #ifndef __CDS_COMPILER_ICL_CXX11_ATOMIC_PATCHES_WIN_H

//$$CDS-header$$

#ifndef __CDS_COMPILER_CLANG_CXX11_ATOMIC_PREPATCHES_H
#define __CDS_COMPILER_CLANG_CXX11_ATOMIC_PREPATCHES_H

#if CDS_CXX11_ATOMIC_SUPPORT == 1
#   if CDS_COMPILER_VERSION == 30100
        // Clang 3.1 workaround
        // Clang 3.1 does not support __atomic_is_lock_free intrinsic function from GCC
        template <typename T>
        static inline bool __atomic_is_lock_free( size_t, T* ) CDS_NOEXCEPT
        {
            return sizeof(T) <= 8;
        }

        // clang does not implement GCC 4.7 low-level __atomic_xxx intrinsics
        // See http://comments.gmane.org/gmane.comp.compilers.clang.devel/20093

        // In Clang 3.1 the following workaround results in
        //      Assertion `Proto && "Functions without a prototype cannot be overloaded"' failed
        // and clang crashed
        // So, we cannot use Clang with GCC 4.7 atomics
/*
        template <typename T>
        static inline void __atomic_load(T *ptr, T *ret, int memmodel)
        {
            *ret = __atomic_load( ptr, memmodel );
        }

        template <typename T>
        static inline  void __atomic_store(T *ptr, T *val, int memmodel)
        {
            __atomic_store( ptr, *val, memmodel );
        }

        template <typename T>
        static inline  void __atomic_exchange(T *ptr, T *val, T *ret, int memmodel)
        {
            *ret =  __atomic_exchange( ptr, *val, memmodel );
        }

        template <typename T>
        static inline  bool __atomic_compare_exchange(T *ptr, T *expected, T *desired, bool weak, int success_memmodel, int failure_memmodel)
        {
            if ( weak )
                return __atomic_compare_exchange_weak( ptr, expected, *desired, success_memmodel, failure_memmodel );
            else
                return __atomic_compare_exchange_strong( ptr, expected, *desired, success_memmodel, failure_memmodel );
        }
*/
#   endif
#endif

#endif // #ifndef __CDS_COMPILER_CLANG_CXX11_ATOMIC_PREPATCHES_H

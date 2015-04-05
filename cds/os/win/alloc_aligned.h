//$$CDS-header$$

#ifndef CDSLIB_OS_WIN_ALLOC_ALIGNED_H
#define CDSLIB_OS_WIN_ALLOC_ALIGNED_H

#include <malloc.h>

//@cond none
namespace cds { namespace OS {
    CDS_CXX11_INLINE_NAMESPACE namespace Win32 {
        /// Allocates memory on a specified alignment boundary
        static inline void * aligned_malloc(
            size_t nSize,       ///< Size of the requested memory allocation
            size_t nAlignment   ///< The alignment value, which must be an integer power of 2
            )
        {
            return ::_aligned_malloc( nSize, nAlignment );
        }

        /// Frees a block of memory that was allocated with aligned_malloc.
        static inline void aligned_free(
            void * pBlock   ///< A pointer to the memory block that was returned to the aligned_malloc function
            )
        {
            ::_aligned_free( pBlock );
        }
    }   // namespace Win32

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    using Win32::aligned_malloc;
    using Win32::aligned_free;
#endif

}} // namespace cds::OS
//@endcond

#endif // #ifndef CDSLIB_OS_WIN_ALLOC_ALIGNED_H


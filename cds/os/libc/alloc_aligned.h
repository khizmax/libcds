//$$CDS-header$$

#ifndef CDSLIB_OS_LIBC_ALLOC_ALIGNED_H
#define CDSLIB_OS_LIBC_ALLOC_ALIGNED_H

#include <stdlib.h>

//@cond none
namespace cds { namespace OS {
    CDS_CXX11_INLINE_NAMESPACE namespace libc {
        /// Allocates memory on a specified alignment boundary
        static inline void * aligned_malloc(
            size_t nSize,       ///< Size of the requested memory allocation
            size_t nAlignment   ///< The alignment value, which must be an integer power of 2
            )
        {
            return ::memalign( nAlignment, nSize );
        }

        /// Frees a block of memory that was allocated with aligned_malloc.
        static inline void aligned_free(
            void * pBlock   ///< A pointer to the memory block that was returned to the aligned_malloc function
            )
        {
            ::free( pBlock );
        }
    }   // namespace libc

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    using libc::aligned_malloc;
    using libc::aligned_free;
#endif

}} // namespace cds::OS
//@endcond

#endif // #ifndef CDSLIB_OS_LIBC_ALLOC_ALIGNED_H


//$$CDS-header$$

#ifndef CDSLIB_OS_POSIX_ALLOC_ALIGNED_H
#define CDSLIB_OS_POSIX_ALLOC_ALIGNED_H

#ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#endif
#ifndef _XOPEN_SOURCE
#   define _XOPEN_SOURCE 600
#endif

#include <stdlib.h>

//@cond none
namespace cds { namespace OS {
    CDS_CXX11_INLINE_NAMESPACE namespace posix {
        /// Allocates memory on a specified alignment boundary
        static inline void * aligned_malloc(
            size_t nSize,       ///< Size of the requested memory allocation
            size_t nAlignment   ///< The alignment value, which must be an integer power of 2
            )
        {
            void * pMem;
            return ::posix_memalign( &pMem, nAlignment, nSize ) == 0 ? pMem : nullptr;
        }

        /// Frees a block of memory that was allocated with aligned_malloc.
        static inline void aligned_free(
            void * pBlock   ///< A pointer to the memory block that was returned to the aligned_malloc function
            )
        {
            ::free( pBlock );
        }
    }   // namespace posix

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    using posix::aligned_malloc;
    using posix::aligned_free;
#endif

}} // namespace cds::OS
//@endcond


#endif // #ifndef CDSLIB_OS_POSIX_ALLOC_ALIGNED_H


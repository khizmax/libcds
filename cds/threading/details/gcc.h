//$$CDS-header$$

#ifndef CDSLIB_THREADING_DETAILS_GCC_H
#define CDSLIB_THREADING_DETAILS_GCC_H

#include <cds/threading/details/gcc_manager.h>

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
namespace cds { namespace threading {

    using gcc::Manager;

}} // namespace cds::threading
#endif

#endif // #ifndef CDSLIB_THREADING_DETAILS_GCC_H

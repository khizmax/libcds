//$$CDS-header$$

#ifndef __CDS_THREADING_DETAILS_GCC_H
#define __CDS_THREADING_DETAILS_GCC_H

#include <cds/threading/details/gcc_manager.h>

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
namespace cds { namespace threading {

    using gcc::Manager;

}} // namespace cds::threading
#endif

#endif // #ifndef __CDS_THREADING_DETAILS_GCC_H

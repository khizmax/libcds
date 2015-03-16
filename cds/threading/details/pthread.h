//$$CDS-header$$

#ifndef CDSLIB_THREADING_DETAILS_PTHREAD_H
#define CDSLIB_THREADING_DETAILS_PTHREAD_H

#include <cds/threading/details/pthread_manager.h>

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
namespace cds { namespace threading {

    using pthread::Manager;

}} // namespace cds::threading
#endif

#endif // #ifndef CDSLIB_THREADING_DETAILS_PTHREAD_H

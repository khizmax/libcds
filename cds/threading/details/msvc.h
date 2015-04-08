//$$CDS-header$$

#ifndef CDSLIB_THREADING_DETAILS_MSVC_H
#define CDSLIB_THREADING_DETAILS_MSVC_H

#include <cds/threading/details/msvc_manager.h>

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
namespace cds { namespace threading {

    using msvc::Manager;

}} // namespace cds::threading
#endif

#endif // #ifndef CDSLIB_THREADING_DETAILS_MSVC_H

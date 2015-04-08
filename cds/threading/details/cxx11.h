//$$CDS-header$$

#ifndef CDSLIB_THREADING_DETAILS_CXX11_H
#define CDSLIB_THREADING_DETAILS_CXX11_H

#include <cds/threading/details/cxx11_manager.h>

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
namespace cds { namespace threading {

    using cxx11::Manager;

}} // namespace cds::threading
#endif

#endif // #ifndef CDSLIB_THREADING_DETAILS_CXX11_H

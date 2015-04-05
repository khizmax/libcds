//$$CDS-header$$

#ifndef CDSLIB_THREADING_DETAILS_WINTLS_H
#define CDSLIB_THREADING_DETAILS_WINTLS_H

#include <stdio.h>
#include <cds/threading/details/wintls_manager.h>

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
namespace cds { namespace threading {

    using wintls::Manager;

}} // namespace cds::threading
#endif

#endif // #ifndef CDSLIB_THREADING_DETAILS_WINTLS_H

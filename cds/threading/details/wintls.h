//$$CDS-header$$

#ifndef __CDS_THREADING_DETAILS_WINTLS_H
#define __CDS_THREADING_DETAILS_WINTLS_H

#include <stdio.h>
#include <cds/threading/details/wintls_manager.h>

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
namespace cds { namespace threading {

    using wintls::Manager;

}} // namespace cds::threading
#endif

#endif // #ifndef __CDS_THREADING_DETAILS_WINTLS_H

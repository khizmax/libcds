//$$CDS-header$$

#ifndef __CDS_THREADING_DETAILS_MSVC_H
#define __CDS_THREADING_DETAILS_MSVC_H

#include <cds/threading/details/msvc_manager.h>

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
namespace cds { namespace threading {

    using msvc::Manager;

}} // namespace cds::threading
#endif

#endif // #ifndef __CDS_THREADING_DETAILS_MSVC_H

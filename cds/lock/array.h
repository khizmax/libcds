//$$CDS-header$$

#ifndef CDSLIB_LOCK_ARRAY_H
#define CDSLIB_LOCK_ARRAY_H

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma message("cds/lock/array.h is deprecated, use cds/sync/lock_array.h instead")
#else
#   warning "cds/lock/array.h is deprecated, use cds/sync/lock_array.h instead"
#endif

#include <cds/sync/lock_array.h>

//@cond
namespace cds { namespace lock {

    using cds::sync::trivial_select_policy;
    using cds::sync::mod_select_policy;
    using cds::sync::pow2_select_policy;

    template <typename Lock
        , typename SelectPolicy = mod_select_policy
        , class Alloc = CDS_DEFAULT_ALLOCATOR
    >
    using array = cds::sync::lock_array< Lock, SelectPolicy, Alloc >;

}} // namespace cds::lock
//@endcond

#endif // #ifndef CDSLIB_LOCK_ARRAY_H

//$$CDS-header$$

#ifndef __CDS_DETAILS_HASH_FUNCTOR_SELECTOR_H
#define __CDS_DETAILS_HASH_FUNCTOR_SELECTOR_H

//@cond

#if CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL
#   include <functional>
#elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG
    // GCC 4.3+
#   include <functional>
#else
    // Default, use boost implementation
#   include <boost/tr1/functional.hpp>
#endif

namespace cds { namespace details {

#if CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL
#   if _MSC_VER >= 1600
        // MSVC 2010 and above
        using std::hash;
#       define CDS_BEGIN_STD_HASH_NAMESPACE namespace std {
#       define CDS_END_STD_HASH_NAMESPACE   }
#       define CDS_STD_HASH_NAMESPACE std
#   else
        // MSVC 2008
        using std::tr1::hash;
#       define CDS_BEGIN_STD_HASH_NAMESPACE namespace std { namespace tr1 {
#       define CDS_END_STD_HASH_NAMESPACE   }}
#       define CDS_STD_HASH_NAMESPACE std::tr1
#   endif
#elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG
    // GCC 4.3+
    using std::hash;
#   define CDS_BEGIN_STD_HASH_NAMESPACE namespace std {
#   define CDS_END_STD_HASH_NAMESPACE   }
#   define CDS_STD_HASH_NAMESPACE std
#else
    // Default, use boost implementation
    using std::tr1::hash;
#   define CDS_BEGIN_STD_HASH_NAMESPACE namespace std { namespace tr1 {
#   define CDS_END_STD_HASH_NAMESPACE   }}
#   define CDS_STD_HASH_NAMESPACE std::tr1
#endif

}} // namespace cds::details
//@endcond

#endif // __CDS_DETAILS_HASH_FUNCTOR_SELECTOR_H

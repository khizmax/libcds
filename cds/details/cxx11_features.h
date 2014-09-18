//$$CDS-header$$

#ifndef __CDS_DETAILS_CXX11_FEATURES_H
#define __CDS_DETAILS_CXX11_FEATURES_H
//@cond

#ifndef __CDS_DEFS_H
#   error "<cds/details/cxx11_features.h> cannot be included directly, use <cds/details/defs.h> instead"
#endif

// =delete function specifier
#ifdef CDS_CXX11_DELETE_DEFINITION_SUPPORT
#   define CDS_DELETE_SPECIFIER     =delete
#else
#   define CDS_DELETE_SPECIFIER
#endif

// =default function specifier
#ifdef CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT
#   define CDS_DEFAULT_SPECIFIER    =default
#else
#   define CDS_DEFAULT_SPECIFIER
#endif

//@endcond
#endif // #ifndef __CDS_DETAILS_CXX11_FEATURES_H

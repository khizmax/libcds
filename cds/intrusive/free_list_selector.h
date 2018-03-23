// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_FREE_LIST_SELECTOR_H
#define CDSLIB_INTRUSIVE_FREE_LIST_SELECTOR_H

#include <cds/details/defs.h>

#ifdef CDS_DCAS_SUPPORT
#   include <cds/intrusive/free_list_tagged.h>
#else
#   include <cds/intrusive/free_list.h>
#endif

//@cond
namespace cds { namespace intrusive {

#ifdef CDS_DCAS_SUPPORT
    typedef TaggedFreeList  FreeListImpl;
#else
    typedef FreeList        FreeListImpl;
#endif

}} // namespace cds::intrusive
//@endcond

#endif // CDSLIB_INTRUSIVE_FREE_LIST_SELECTOR_H

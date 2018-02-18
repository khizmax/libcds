// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_USER_SETUP_ALLOCATOR_H
#define CDSLIB_USER_SETUP_ALLOCATOR_H

/** \file
    \brief Default implementation of allocator classes.

    If you want to use your allocator implementation you should define all or one of the following macro before
    including CDS library headers:
    \li CDS_DEFAULT_ALLOCATOR
    \li CDS_DEFAULT_ALIGNED_ALLOCATOR
*/

#ifndef CDS_DEFAULT_ALLOCATOR

    /** \def CDS_DEFAULT_ALLOCATOR
        The macro defines standard allocator implementation. Default is std::allocator.

        The CDS library uses \p std::allocator<int> to emulate template template parameter.
        The library uses the <tt> std::allocator<int>::rebind<Q>::other </tt> to redefine allocator's type to proper type \p Q.
    */
#define CDS_DEFAULT_ALLOCATOR        std::allocator<int>

#    include <memory>
#endif


#ifndef CDS_DEFAULT_ALIGNED_ALLOCATOR

    /** \def CDS_DEFAULT_ALIGNED_ALLOCATOR
        The macro defines aligned allocator implementation.

        The default is \ref cds::OS::aligned_allocator "cds::OS::aligned_allocator<int>".
        The library uses the <tt> cds::OS::aligned_allocator<int>::rebind<Q>::other </tt> to redefine allocator's type \p int to proper type \p Q.
    */
#define CDS_DEFAULT_ALIGNED_ALLOCATOR    cds::OS::aligned_allocator<int>

#   include <cds/os/alloc_aligned.h>
#endif

#endif // #ifndef CDSLIB_USER_SETUP_ALLOCATOR_H

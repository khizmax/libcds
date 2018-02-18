// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DETAILS_MAKE_CONST_TYPE_H
#define CDSLIB_DETAILS_MAKE_CONST_TYPE_H

#include <cds/details/defs.h>

namespace cds { namespace details {

    //@cond
    template <typename T, bool B>
    struct make_const_type
    {
        typedef T      type;
        typedef T *    pointer;
        typedef T &    reference;
    };
    template<typename T>
    struct make_const_type<T, true>
    {
        typedef T const      type;
        typedef T const *    pointer;
        typedef T const &    reference;
    };

    //@endcond

}}  // namespace cds::details

#endif  // #ifndef CDSLIB_DETAILS_MAKE_CONST_TYPE_H

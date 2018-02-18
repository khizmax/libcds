// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DETAILS_SIZE_T_CAST_H
#define CDSLIB_DETAILS_SIZE_T_CAST_H

#include <cds/details/defs.h>

//@cond
namespace cds { namespace details {

    template <size_t Size>
    struct size_t_unsigned;

    template <>
    struct size_t_unsigned<4>
    {
        typedef uint32_t type;
    };

    template <>
    struct size_t_unsigned<8>
    {
        typedef uint64_t type;
    };

    static inline size_t_unsigned<sizeof( size_t )>::type size_t_cast( size_t n )
    {
        return static_cast< size_t_unsigned<sizeof( size_t )>::type>( n );
    }

}} // namespace cds::details
//@endcond

#endif // #ifndef CDSLIB_DETAILS_SIZE_T_CAST_H


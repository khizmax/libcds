// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INT_ALGO_H
#define CDSLIB_INT_ALGO_H

#include <cds/algo/bitop.h>

namespace cds { namespace beans {
    /// Returns largest previous integer for <tt>log2( n )</tt>
    static inline size_t log2floor( size_t n )
    {
        return n ? cds::bitop::MSBnz( n ) : 0;
    }

    /// Returns smallest following integer for <tt>log2( n )</tt>
    static inline size_t log2ceil( size_t n )
    {
        size_t i = log2floor( n );
        return ( size_t( 1 ) << i ) < n ? i + 1 : i;
    }

    /// Returns largest previous power of 2 for \p n
    /**
        Examples:
        \code
        floor2(0) == 1   // !!!
        floor2(1) == 1
        floor2(2) == 2
        floor2(3) == 2
        floor2(4) == 4
        floor2(15) == 8
        floor2(16) == 16
        floor2(17) == 16
        \endcode
    */
    static inline size_t floor2( size_t n )
    {
        return size_t(1) << log2floor( n );
    }

    /// Returns smallest following power of 2 for \p n
    /**
        Examples:
        \code
        ceil2(0) == 1   // !!!
        ceil2(1) == 1
        ceil2(2) == 2
        ceil2(3) == 4
        ceil2(4) == 4
        ceil2(15) == 16
        ceil2(16) == 16
        ceil2(17) == 32
        \endcode
    */
    static inline size_t ceil2( size_t n )
    {
        return size_t(1) << log2ceil( n );
    }

    /// Checks if \p n is power of 2
    constexpr static inline bool is_power2( size_t n ) noexcept
    {
        return (n & (n - 1)) == 0 && n;
    }

    /// Returns binary logarithm of \p n if \p n is power of two, otherwise returns 0
    static inline size_t log2( size_t n )
    {
        return is_power2(n) ? log2floor(n) : 0;
    }

#if CDS_BUILD_BITS == 32
    //@cond
    // 64bit specializations

/// Returns largest previous integer for <tt>log2( n )</tt>
    static inline uint64_t log2floor( uint64_t n )
    {
        return n ? cds::bitop::MSBnz( n ) : 0;
    }

/// Returns smallest following integer for <tt>log2( n )</tt>
    static inline uint64_t log2ceil( uint64_t n )
    {
        uint64_t i = log2floor( n );
        return (uint64_t( 1 ) << i) < n ? i + 1 : i;
    }

/// Returns largest previous power of 2 for \p n
    /**
        Examples:
        \code
        floor2(0) == 1   // !!!
        floor2(1) == 1
        floor2(2) == 2
        floor2(3) == 2
        floor2(4) == 4
        floor2(15) == 8
        floor2(16) == 16
        floor2(17) == 16
        \endcode
    */
    static inline uint64_t floor2( uint64_t n )
    {
        return uint64_t( 1 ) << log2floor( n );
    }

/// Returns smallest following power of 2 for \p n
    /**
        Examples:
        \code
        ceil2(0) == 1   // !!!
        ceil2(1) == 1
        ceil2(2) == 2
        ceil2(3) == 4
        ceil2(4) == 4
        ceil2(15) == 16
        ceil2(16) == 16
        ceil2(17) == 32
        \endcode
    */
    static inline uint64_t ceil2( uint64_t n )
    {
        return uint64_t( 1 ) << log2ceil( n );
    }

/// Checks if \p n is power of 2
    constexpr static inline bool is_power2( uint64_t n ) noexcept
    {
        return (n & (n - 1)) == 0 && n;
    }

/// Returns binary logarithm of \p n if \p n is power of two, otherwise returns 0
    static inline uint64_t log2( uint64_t n )
    {
        return is_power2( n ) ? log2floor( n ) : 0;
    }

    //@endcond
#endif //#if CDS_BUILD_BITS == 32

}}   // namespace cds::beans

#endif  // #ifndef CDSLIB_INT_ALGO_H

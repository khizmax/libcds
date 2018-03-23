// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DETAILS_THROW_EXCEPTION_H
#define CDSLIB_DETAILS_THROW_EXCEPTION_H

#include <cds/details/defs.h>
#if !defined( CDS_EXCEPTION_ENABLED ) && !defined( CDS_USER_DEFINED_THROW_HANDLER )
#   include <stdio.h>
#endif

namespace cds {

#if !defined( CDS_USER_DEFINED_THROW_EXCEPTION )
#if defined( CDS_EXCEPTION_ENABLED )
    /// Function to throw an exception
    /**
        If you compile your code with exception enabled, \p %throw_exception() function
        throws the \p exception.

        If exception is disabled, \p %throw_exception() prints an exception message to
        standard output and call \p abort().

        You can supply your own \p %cds::throw_exception() function;
        for that you should specify \p -DCDS_USER_DEFINED_THROW_EXCEPTION
        in compiler command line.

        @note \p %throw_exception() never returns. If the user-defined \p %throw_exception() returns,
        the behavior is undefined.
    */
    template <typename E>
    CDS_NORETURN static inline void throw_exception(
        E&& exception,       ///< Exception to throw
        char const* file,   ///< Source filename
        int line            ///< File line
    )
    {
        CDS_UNUSED( file );
        CDS_UNUSED( line );

        throw exception;
    }
#else
    template <typename E>
    CDS_NORETURN static inline void throw_exception( E&& exception, char const* file, int line )
    {
        printf( "file %s, line %d: %s\n", file, line, exception.what());
        abort();
    }
#endif
//#else
    // User-provided cds::throw_exception()
#endif

#define CDS_THROW_EXCEPTION( exception ) ::cds::throw_exception( exception, __FILE__, __LINE__ )

} // namespace cds


#endif // #ifndef CDSLIB_DETAILS_THROW_EXCEPTION_H


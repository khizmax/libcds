/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
        printf( "file %s, line %d: %s\n", file, line, exception.what() );
        abort();
    }
#endif
//#else
    // User-provided cds::throw_exception()
#endif

#define CDS_THROW_EXCEPTION( exception ) ::cds::throw_exception( exception, __FILE__, __LINE__ )

} // namespace cds


#endif // #ifndef CDSLIB_DETAILS_THROW_EXCEPTION_H


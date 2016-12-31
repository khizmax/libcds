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

#ifndef CDSUNIT_HASH_FUNC_H
#define CDSUNIT_HASH_FUNC_H

#include <cds/details/defs.h>

#if CDS_BUILD_BITS == 64
#   include <cds_test/city.h>
#endif

namespace cds_test {

#if CDS_BUILD_BITS == 64
    class city32 {
    public:
        typedef uint32_t hash_type;
        typedef hash_type result_type;

        hash_type operator()( void const * pBuf, size_t len ) const
        {
            return CityHash32( reinterpret_cast<char const *>( pBuf ), len );
        }

        hash_type operator()( std::string const& s ) const
        {
            return CityHash32( s.c_str(), s.length());
        }

        template <typename T>
        hash_type operator()( T const& s ) const
        {
            return CityHash32( reinterpret_cast<char const *>( &s ), sizeof(s));
        }

        struct less
        {
            bool operator()( hash_type lhs, hash_type rhs ) const
            {
                return lhs < rhs;
            }
        };
    };

    class city64 {
    public:
        typedef uint64_t hash_type;
        typedef hash_type result_type;

        hash_type operator()( void const * pBuf, size_t len ) const
        {
            return CityHash64( reinterpret_cast<char const *>( pBuf ), len );
        }

        hash_type operator()( std::string const& s ) const
        {
            return CityHash64( s.c_str(), s.length());
        }

        template <typename T>
        hash_type operator()( T const& s ) const
        {
            return CityHash64( reinterpret_cast<char const *>( &s ), sizeof(s));
        }

        struct less
        {
            bool operator()( hash_type lhs, hash_type rhs ) const
            {
                return lhs < rhs;
            }
        };
    };

    class city128 {
    public:
        typedef uint128 hash_type;
        typedef hash_type result_type;

        hash_type operator()( void const * pBuf, size_t len ) const
        {
            return CityHash128( reinterpret_cast<char const *>( pBuf ), len );
        }

        hash_type operator()( std::string const& s ) const
        {
            return CityHash128( s.c_str(), s.length());
        }

        template <typename T>
        hash_type operator()( T const& s ) const
        {
            return CityHash128( reinterpret_cast<char const *>( &s ), sizeof(s));
        }

        struct less
        {
            bool operator()( hash_type const& lhs, hash_type const& rhs ) const
            {
                if ( lhs.first != rhs.first )
                    return lhs.second < rhs.second;
                return lhs.first < rhs.first;
            }
        };
    };
#endif // #if CDS_BUILD_BITS == 64


} // namespace cds_test

#endif // #ifndef CDSUNIT_HASH_FUNC_H

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

#ifndef CDSUNIT_STRIPED_MAP_TEST_MAP_DATA_H
#define CDSUNIT_STRIPED_MAP_TEST_MAP_DATA_H

#include <cds_test/check_size.h>
#include <cds_test/fixture.h>

#include <cds/opt/hash.h>

namespace cds_test {

    class striped_map_fixture: public fixture
    {
    public:
        struct key_type {
            int nKey;

            explicit key_type( int n )
                : nKey( n )
            {}

            explicit key_type( std::string const& str )
                : nKey( std::stoi( str ))
            {}

            key_type( key_type const& s )
                : nKey( s.nKey )
            {}
        };

        struct value_type {
            int nVal;
            std::string strVal;

            value_type()
                : nVal( 0 )
            {}

            explicit value_type( int n )
                : nVal( n )
                , strVal( std::to_string( n ))
            {}

            explicit value_type( std::string const& str )
                : nVal( std::stoi( str ))
                , strVal( str )
            {}

            explicit value_type( std::string&& str )
                : nVal( std::stoi( str ))
                , strVal( std::move( str ))
            {}

            value_type( int n, std::string const& str )
                : nVal( n )
                , strVal( str )
            {}

            value_type( int n, std::string&& str )
                : nVal( n )
                , strVal( std::move( str ))
            {}

            value_type( value_type&& v )
                : nVal( v.nVal )
                , strVal( std::move( v.strVal ))
            {}

            value_type( value_type const& v )
                : nVal( v.nVal )
                , strVal( v.strVal )
            {}

            value_type& operator=( value_type const& v )
            {
                if ( this != &v ) {
                    nVal = v.nVal;
                    strVal = v.strVal;
                }
                return *this;
            }

            value_type& operator=( value_type&& v )
            {
                if ( this != &v ) {
                    nVal = v.nVal;
                    strVal = std::move( v.strVal );
                }
                return *this;
            }

            value_type& operator=( int i )
            {
                nVal = i;
                strVal = std::to_string( i );
                return *this;
            }

            value_type& operator=( std::string const& s )
            {
                nVal = std::stoi( s );
                strVal = s;
                return *this;
            }
        };

        typedef std::pair<key_type const, value_type> pair_type;

        struct less
        {
            bool operator ()( key_type const& v1, key_type const& v2 ) const
            {
                return v1.nKey < v2.nKey;
            }

            bool operator ()( key_type const& v1, int v2 ) const
            {
                return v1.nKey < v2;
            }

            bool operator ()( int v1, key_type const& v2 ) const
            {
                return v1 < v2.nKey;
            }

            bool operator ()( key_type const& v1, std::string const& v2 ) const
            {
                return v1.nKey < std::stoi(v2 );
            }

            bool operator ()( std::string const& v1, key_type const& v2 ) const
            {
                return std::stoi( v1 ) < v2.nKey;
            }
        };

        struct cmp {
            int operator ()( key_type const& v1, key_type const& v2 ) const
            {
                if ( v1.nKey < v2.nKey )
                    return -1;
                return v1.nKey > v2.nKey ? 1 : 0;
            }

            int operator ()( key_type const& v1, int v2 ) const
            {
                if ( v1.nKey < v2 )
                    return -1;
                return v1.nKey > v2 ? 1 : 0;
            }

            int operator ()( int v1, key_type const& v2 ) const
            {
                if ( v1 < v2.nKey )
                    return -1;
                return v1 > v2.nKey ? 1 : 0;
            }

            int operator ()( key_type const& v1, std::string const& v2 ) const
            {
                int n2 = std::stoi( v2 );
                if ( v1.nKey < n2 )
                    return -1;
                return v1.nKey > n2 ? 1 : 0;
            }

            int operator ()( std::string const& v1, key_type const& v2 ) const
            {
                int n1 = std::stoi( v1 );
                if ( n1 < v2.nKey )
                    return -1;
                return n1 > v2.nKey ? 1 : 0;
            }
        };

        struct equal_to
        {
            bool operator ()( key_type const& v1, key_type const& v2 ) const
            {
                return v1.nKey == v2.nKey;
            }

            bool operator ()( key_type const& v1, int v2 ) const
            {
                return v1.nKey == v2;
            }

            bool operator ()( int v1, key_type const& v2 ) const
            {
                return v1 == v2.nKey;
            }

            bool operator ()( key_type const& v1, std::string const& v2 ) const
            {
                return v1.nKey == std::stoi( v2 );
            }

            bool operator ()( std::string const& v1, key_type const& v2 ) const
            {
                return std::stoi( v1 ) == v2.nKey;
            }
        };

        struct hash1 {
            size_t operator()( int i ) const
            {
                return cds::opt::v::hash<int>()( i );
            }

            size_t operator()( std::string const& str ) const
            {
                return cds::opt::v::hash<int>()( std::stoi( str ));
            }

            template <typename T>
            size_t operator()( T const& i ) const
            {
                return cds::opt::v::hash<int>()(i.nKey);
            }
        };

        struct hash2: private hash1
        {
            typedef hash1 base_class;

            size_t operator()( int i ) const
            {
                size_t h = ~(base_class::operator()( i ));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            template <typename Item>
            size_t operator()( const Item& i ) const
            {
                size_t h = ~(base_class::operator()( i ));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
        };

        struct other_item {
            int nKey;

            other_item( int key )
                : nKey( key )
            {}
        };

        struct other_less
        {
            bool operator ()( key_type const& v1, other_item const& v2 ) const
            {
                return v1.nKey < v2.nKey;
            }
            bool operator ()( other_item const& v1, key_type const& v2 ) const
            {
                return v1.nKey < v2.nKey;
            }
        };

        struct other_equal_to
        {
            bool operator ()( key_type const& v1, other_item const& v2 ) const
            {
                return v1.nKey == v2.nKey;
            }
            bool operator ()( other_item const& v1, key_type const& v2 ) const
            {
                return v1.nKey == v2.nKey;
            }
        };
    };
} // namespace cds_test

#endif // CDSUNIT_STRIPED_MAP_TEST_MAP_DATA_H

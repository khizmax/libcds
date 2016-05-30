//$$CDS-header$$

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
            return CityHash32( s.c_str(), s.length() );
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
            return CityHash64( s.c_str(), s.length() );
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
            return CityHash128( s.c_str(), s.length() );
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

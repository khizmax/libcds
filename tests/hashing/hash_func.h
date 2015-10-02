//$$CDS-header$$

#ifndef CDSUNIT_HASH_FUNC_H
#define CDSUNIT_HASH_FUNC_H

#include <cds/details/defs.h>

#include "hashing/sha256.h"
#include "hashing/md5.h"
#if CDS_BUILD_BITS == 64
#   include "hashing/city.h"
#endif

namespace hashing {

    template <class Hasher>
    class hasher {
        typedef Hasher hasher_type;
        hasher_type m_hasher;
    public:
        struct hash_type {
            uint8_t h[hasher_type::HashBytes];
        };

        hash_type operator()( void const * pBuf, size_t len )
        {
            m_hasher.reset();
            m_hasher.add(pBuf, len);
            hash_type result;
            m_hasher.getHash( result.h );
            return result;
        }

        hash_type operator()( std::string const& s )
        {
            return operator()( reinterpret_cast<void const *>(s.c_str()), s.length());
        }

        template <typename T>
        hash_type operator()( T const& s )
        {
            return operator()( reinterpret_cast<void const *>(&s), sizeof(s));
        }
    };

    typedef hasher<SHA256> sha256;
    typedef hasher<MD5> md5;

#if CDS_BUILD_BITS == 64
    class city32 {
    public:
        typedef uint32_t hash_type;

        hash_type operator()( void const * pBuf, size_t len )
        {
            return CityHash32( reinterpret_cast<char const *>( pBuf ), len );
        }

        hash_type operator()( std::string const& s )
        {
            return CityHash32( s.c_str(), s.length() );
        }

        template <typename T>
        hash_type operator()( T const& s )
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

        hash_type operator()( void const * pBuf, size_t len )
        {
            return CityHash64( reinterpret_cast<char const *>( pBuf ), len );
        }

        hash_type operator()( std::string const& s )
        {
            return CityHash64( s.c_str(), s.length() );
        }

        template <typename T>
        hash_type operator()( T const& s )
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

        hash_type operator()( void const * pBuf, size_t len )
        {
            return CityHash128( reinterpret_cast<char const *>( pBuf ), len );
        }

        hash_type operator()( std::string const& s )
        {
            return CityHash128( s.c_str(), s.length() );
        }

        template <typename T>
        hash_type operator()( T const& s )
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


} // namespace hashing

#endif // #ifndef CDSUNIT_HASH_FUNC_H

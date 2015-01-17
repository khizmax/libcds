/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

namespace set {
    namespace {

        typedef IntrusiveCuckooSetHdrTest::hash_int    hash1;
        struct hash2: private hash1
        {
            typedef hash1 base_class;

            size_t operator()( int i ) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            template <typename Item>
            size_t operator()( const Item& i ) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            size_t operator()( IntrusiveCuckooSetHdrTest::find_key const& i) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
        };

        template <typename T>
        struct equal_to
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() == v2.key();
            }

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() == v2;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 == v2.key();
            }
        };

    }
} // namespace set

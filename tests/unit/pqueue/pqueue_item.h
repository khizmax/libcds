/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __CDSUNIT_PQUEUE_ITEM_H
#define __CDSUNIT_PQUEUE_ITEM_H

namespace pqueue {
    struct SimpleValue {
        size_t      key;

        typedef size_t  key_type;

        struct key_extractor {
            void operator()( key_type& k, SimpleValue const& s ) const
            {
                k = s.key;
            }
        };

        SimpleValue(): key(0) {}
        SimpleValue( size_t n ): key(n) {}
    };
}

namespace std {
    template <>
    struct less<pqueue::SimpleValue>
    {
        bool operator()( pqueue::SimpleValue const& k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1.key < k2.key;
        }

        bool operator()( pqueue::SimpleValue const& k1, size_t k2 ) const
        {
            return k1.key < k2;
        }

        bool operator()( size_t k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1 < k2.key;
        }

        bool operator()( size_t k1, size_t k2 ) const
        {
            return k1 < k2;
        }
    };

    template <>
    struct greater<pqueue::SimpleValue>
    {
        bool operator()( pqueue::SimpleValue const& k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1.key > k2.key;
        }

        bool operator()( pqueue::SimpleValue const& k1, size_t k2 ) const
        {
            return k1.key > k2;
        }

        bool operator()( size_t k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1 > k2.key;
        }

        bool operator()( size_t k1, size_t k2 ) const
        {
            return k1 > k2;
        }
    };

} // namespace std

#endif // #ifndef __CDSUNIT_PQUEUE_ITEM_H

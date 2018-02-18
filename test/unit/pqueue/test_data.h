// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_PQUEUE_TEST_DATA_H
#define CDSUNIT_PQUEUE_TEST_DATA_H

#include <cds_test/fixture.h>
#include <memory>

namespace cds_test {

    class PQueueTest : public ::cds_test::fixture
    {
    public:
        typedef int     key_type;

        struct value_type {
            key_type    k;
            int         v;

            value_type()
            {}

            value_type( value_type const& kv )
                : k( kv.k )
                , v( kv.v )
            {}

            value_type( key_type key )
                : k( key )
                , v( key )
            {}

            value_type( key_type key, int val )
                : k( key )
                , v( val )
            {}

            value_type( std::pair<key_type, int> const& p )
                : k( p.first )
                , v( p.second )
            {}
        };

        struct compare {
            int operator()( value_type k1, value_type k2 ) const
            {
                return k1.k - k2.k;
            }
        };

        struct less {
            bool operator()( value_type k1, value_type k2 ) const
            {
                return k1.k < k2.k;
            }
        };

        enum {
            c_nMinValue = -123,
            c_nCapacity = 1024
        };

    protected:
        template <typename T>
        class data_array
        {
            std::unique_ptr<T[]> pFirst;
            T *     pLast;

        public:
            data_array( size_t nSize )
                : pFirst( new T[nSize] )
                , pLast( pFirst.get() + nSize )
            {
                key_type i = c_nMinValue;
                for ( T * p = pFirst.get(); p != pLast; ++p, ++i )
                    p->k = p->v = i;

                shuffle( pFirst.get(), pLast );
            }

            T * begin() { return pFirst.get(); }
            T * end()   { return pLast; }
            size_t size() const
            {
                return pLast - pFirst;
            }
        };
    };
} // namespace cds_test

namespace std {
    template<>
    struct less<cds_test::PQueueTest::value_type>
    {
        bool operator()( cds_test::PQueueTest::value_type const& v1, cds_test::PQueueTest::value_type const& v2 ) const
        {
            return cds_test::PQueueTest::less()(v1, v2);
        }
    };
}

#endif // CDSUNIT_PQUEUE_FCPQUEUE_H

/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

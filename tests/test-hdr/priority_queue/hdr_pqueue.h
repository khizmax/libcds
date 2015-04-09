//$$CDS-header$$

#ifndef CDSTEST_HDR_PQUEUE_H
#define CDSTEST_HDR_PQUEUE_H

#include "cppunit/cppunit_proxy.h"
#include "size_check.h"
#include <algorithm>
#include <functional>   // ref

namespace priority_queue {

    namespace pqueue {
        static size_t const c_nCapacity = 1024 * 16;

        struct disposer {
            size_t   m_nCallCount;

            disposer()
                : m_nCallCount(0)
            {}

            template <typename T>
            void operator()( T& )
            {
                ++m_nCallCount;
            }
        };

        template <typename PQueue>
        struct constants {
            static size_t const nCapacity = c_nCapacity;
        };
    } // namespace pqueue

    class PQueueHdrTest: public CppUnitMini::TestCase
    {
    public:
        static size_t const c_nCapacity = pqueue::c_nCapacity;

        typedef int     key_type;
        static key_type const c_nMinValue = -123;

        struct value_type {
            key_type    k;
            int         v;

            value_type()
            {}

            value_type( value_type const& kv )
                : k(kv.k)
                , v(kv.v)
            {}

            value_type( key_type key )
                : k(key)
                , v(key)
            {}

            value_type( key_type key, int val )
                : k(key)
                , v(val)
            {}

            value_type( std::pair<key_type, int> const& p )
                : k(p.first)
                , v(p.second)
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

        template <typename T>
        class data_array
        {
            T *     pFirst;
            T *     pLast;

        public:
            data_array( size_t nSize )
                : pFirst( new T[nSize] )
                , pLast( pFirst + nSize )
            {
                key_type i = c_nMinValue;
                for ( T * p = pFirst; p != pLast; ++p, ++i )
                    p->k = p->v = i;

                shuffle( pFirst, pLast );
            }

            ~data_array()
            {
                delete [] pFirst;
            }

            T * begin() { return pFirst; }
            T * end()   { return pLast ; }
            size_t size() const
            {
                return pLast - pFirst;
            }
        };

    protected:
        template <class PQueue>
        void test_bounded_with( PQueue& pq )
        {
            data_array<value_type> arr( pq.capacity() );
            value_type * pFirst = arr.begin();
            value_type * pLast  = pFirst + pq.capacity();

            CPPUNIT_ASSERT( pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 0 );
            CPPUNIT_ASSERT_EX( pq.capacity() == pqueue::constants<PQueue>::nCapacity,
                "pq.capacity() = " << pq.capacity() << ", pqueue::constants<PQueue>::nCapacity = " << pqueue::constants<PQueue>::nCapacity
                );

            size_t nSize = 0;

            // Push test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                switch ( pq.size() & 3 ) {
                    case 0:
                        CPPUNIT_ASSERT( pq.push_with( [p]( value_type& dest ) { dest = *p; } ));
                        break;
                    case 1:
                        CPPUNIT_ASSERT( pq.emplace( p->k, p->v ));
                        break;
                    case 2:
                        CPPUNIT_ASSERT( pq.emplace( std::make_pair( p->k, p->v ) ));
                        break;
                    default:
                        CPPUNIT_ASSERT( pq.push( *p ));
                }
                CPPUNIT_ASSERT( !pq.empty() );
                CPPUNIT_ASSERT( pq.size() == ++nSize );
            }

            CPPUNIT_ASSERT( pq.full() );
            CPPUNIT_ASSERT( pq.size() == pq.capacity() );

            // The queue is full
            key_type k = c_nMinValue + key_type(c_nCapacity);
            CPPUNIT_ASSERT( !pq.push( k ));
            CPPUNIT_ASSERT( pq.full() );
            CPPUNIT_ASSERT( pq.size() == pq.capacity() );

            // Pop test
            key_type nPrev = c_nMinValue + key_type(pq.capacity()) - 1;
            value_type kv(0);
            key_type   key;
            CPPUNIT_ASSERT( pq.pop(kv) );
            CPPUNIT_CHECK_EX( kv.k == nPrev, "Expected=" << nPrev << ", current=" << kv.k );

            CPPUNIT_ASSERT( pq.size() == pq.capacity() - 1 );
            CPPUNIT_ASSERT( !pq.full() );
            CPPUNIT_ASSERT( !pq.empty() );

            nSize = pq.size();
            while ( pq.size() > 1 ) {
                if ( pq.size() & 1 ) {
                    CPPUNIT_ASSERT( pq.pop(kv) );
                    CPPUNIT_CHECK_EX( kv.k == nPrev - 1, "Expected=" << nPrev - 1 << ", current=" << kv.k );
                    nPrev = kv.k;
                }
                else {
                    CPPUNIT_ASSERT( pq.pop_with( [&key]( value_type& src ) { key = src.k;  } ) );
                    CPPUNIT_CHECK_EX( key == nPrev - 1, "Expected=" << nPrev - 1 << ", current=" << key );
                    nPrev = key;
                }

                --nSize;
                CPPUNIT_ASSERT( pq.size() == nSize );
            }

            CPPUNIT_ASSERT( !pq.full() );
            CPPUNIT_ASSERT( !pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 1 );

            CPPUNIT_ASSERT( pq.pop(kv) );
            CPPUNIT_CHECK_EX( kv.k == c_nMinValue, "Expected=" << c_nMinValue << ", current=" << kv.k );

            CPPUNIT_ASSERT( !pq.full() );
            CPPUNIT_ASSERT( pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 0 );

            // Clear test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                CPPUNIT_ASSERT( pq.push( *p ));
            }
            CPPUNIT_ASSERT( !pq.empty() );
            CPPUNIT_ASSERT( pq.full() );
            CPPUNIT_ASSERT( pq.size() == pq.capacity() );
            pq.clear();
            CPPUNIT_ASSERT( pq.empty() );
            CPPUNIT_ASSERT( !pq.full() );
            CPPUNIT_ASSERT( pq.size() == 0 );

            // clear_with test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                CPPUNIT_ASSERT( pq.push( *p ));
            }
            CPPUNIT_ASSERT( !pq.empty() );
            CPPUNIT_ASSERT( pq.full() );
            CPPUNIT_ASSERT( pq.size() == pq.capacity() );

            {
                pqueue::disposer disp;
                pq.clear_with( std::ref(disp) );
                CPPUNIT_ASSERT( pq.empty() );
                CPPUNIT_ASSERT( !pq.full() );
                CPPUNIT_ASSERT( pq.size() == 0 );
                CPPUNIT_ASSERT( disp.m_nCallCount == pq.capacity() );
            }
        }

        template <class PQueue>
        void test_msq_stat()
        {
            PQueue pq( 0 );   // argument should be ignored for static buffer
            test_bounded_with( pq );
        }
        template <class PQueue>
        void test_msq_dyn()
        {
            PQueue pq( c_nCapacity );
            test_bounded_with( pq );
        }

        template <class PQueue>
        void test_fcpqueue()
        {
            PQueue pq;

            data_array<value_type> arr( c_nCapacity );
            value_type * pFirst = arr.begin();
            value_type * pLast  = pFirst + c_nCapacity;

            CPPUNIT_ASSERT( pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 0 );

            size_t nSize = 0;

            // Push test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                CPPUNIT_ASSERT( pq.push( *p ));
                CPPUNIT_ASSERT( !pq.empty() );
                CPPUNIT_ASSERT( pq.size() == ++nSize );
            }

            CPPUNIT_ASSERT( pq.size() == c_nCapacity );

            // Pop test
            key_type nPrev = c_nMinValue + key_type(c_nCapacity) - 1;
            value_type kv(0);
            //key_type   key;
            CPPUNIT_ASSERT( pq.pop(kv) );
            CPPUNIT_CHECK_EX( kv.k == nPrev, "Expected=" << nPrev << ", current=" << kv.k );

            CPPUNIT_ASSERT( pq.size() == c_nCapacity - 1 );
            CPPUNIT_ASSERT( !pq.empty() );

            nSize = pq.size();
            while ( pq.size() > 1 ) {
                CPPUNIT_ASSERT( pq.pop(kv) );
                CPPUNIT_CHECK_EX( kv.k == nPrev - 1, "Expected=" << nPrev - 1 << ", current=" << kv.k );
                nPrev = kv.k;

                --nSize;
                CPPUNIT_ASSERT( pq.size() == nSize );
            }

            CPPUNIT_ASSERT( !pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 1 );

            CPPUNIT_ASSERT( pq.pop(kv) );
            CPPUNIT_CHECK_EX( kv.k == c_nMinValue, "Expected=" << c_nMinValue << ", current=" << kv.k );

            CPPUNIT_ASSERT( pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 0 );

            // Clear test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                CPPUNIT_ASSERT( pq.push( *p ));
            }
            CPPUNIT_ASSERT( !pq.empty() );
            CPPUNIT_ASSERT( pq.size() == c_nCapacity );

            pq.clear();
            CPPUNIT_ASSERT( pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 0 );
        }

    public:
        void MSPQueue_st();
        void MSPQueue_st_cmp();
        void MSPQueue_st_less();
        void MSPQueue_st_cmpless();
        void MSPQueue_st_cmp_mtx();
        void MSPQueue_dyn();
        void MSPQueue_dyn_cmp();
        void MSPQueue_dyn_less();
        void MSPQueue_dyn_cmpless();
        void MSPQueue_dyn_cmp_mtx();

        void FCPQueue_vector();
        void FCPQueue_vector_stat();
        void FCPQueue_vector_mutex();
        void FCPQueue_deque();
        void FCPQueue_deque_stat();
        void FCPQueue_deque_mutex();
        void FCPQueue_boost_deque();
        void FCPQueue_boost_deque_stat();
        void FCPQueue_stablevector();
        void FCPQueue_stablevector_stat();

        CPPUNIT_TEST_SUITE(PQueueHdrTest)
            CPPUNIT_TEST(MSPQueue_st)
            CPPUNIT_TEST(MSPQueue_st_cmp)
            CPPUNIT_TEST(MSPQueue_st_less)
            CPPUNIT_TEST(MSPQueue_st_cmpless)
            CPPUNIT_TEST(MSPQueue_st_cmp_mtx)
            CPPUNIT_TEST(MSPQueue_dyn)
            CPPUNIT_TEST(MSPQueue_dyn_cmp)
            CPPUNIT_TEST(MSPQueue_dyn_less)
            CPPUNIT_TEST(MSPQueue_dyn_cmpless)
            CPPUNIT_TEST(MSPQueue_dyn_cmp_mtx)

            CPPUNIT_TEST(FCPQueue_vector)
            CPPUNIT_TEST(FCPQueue_vector_stat)
            CPPUNIT_TEST(FCPQueue_vector_mutex)
            CPPUNIT_TEST(FCPQueue_deque)
            CPPUNIT_TEST(FCPQueue_deque_stat)
            CPPUNIT_TEST(FCPQueue_deque_mutex)
            CPPUNIT_TEST(FCPQueue_boost_deque)
            CPPUNIT_TEST(FCPQueue_boost_deque_stat)
            CPPUNIT_TEST(FCPQueue_stablevector)
            CPPUNIT_TEST(FCPQueue_stablevector_stat)
        CPPUNIT_TEST_SUITE_END()
    };

} // namespace priority_queue

namespace std {
    template<>
    struct less<priority_queue::PQueueHdrTest::value_type>
    {
        bool operator()( priority_queue::PQueueHdrTest::value_type const& v1, priority_queue::PQueueHdrTest::value_type const& v2) const
        {
            return priority_queue::PQueueHdrTest::less()( v1, v2 );
        }
    };
}

#endif // #ifndef CDSTEST_HDR_PQUEUE_H

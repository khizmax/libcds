//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRUSIVE_PQUEUE_H
#define CDSTEST_HDR_INTRUSIVE_PQUEUE_H

#include "cppunit/cppunit_proxy.h"
#include "size_check.h"
#include <algorithm>
#include <functional>   // ref

namespace priority_queue {

    namespace intrusive_pqueue {
        static size_t const c_nCapacity = 1024 * 16;

        struct another_disposer {
            size_t   m_nCallCount;

            another_disposer()
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
    } // namespace intrusive_pqueue

    class IntrusivePQueueHdrTest: public CppUnitMini::TestCase
    {
    public:
        static size_t const c_nCapacity = intrusive_pqueue::c_nCapacity;

        typedef int     key_type;
        static key_type const c_nMinValue = -123;

        struct compare {
            int operator()( key_type k1, key_type k2 ) const
            {
                return k1 - k2;
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
                T i = c_nMinValue;
                for ( T * p = pFirst; p != pLast; ++p, ++i )
                    *p = i;

                CppUnitMini::TestCase::shuffle( pFirst, pLast );
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
            data_array<key_type> arr( pq.capacity() );
            key_type * pFirst = arr.begin();
            key_type * pLast  = pFirst + pq.capacity();

            CPPUNIT_ASSERT( pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 0 );
            CPPUNIT_ASSERT( pq.capacity() == intrusive_pqueue::constants<PQueue>::nCapacity );

            size_t nSize = 0;

            // Push test
            for ( key_type * p = pFirst; p < pLast; ++p ) {
                CPPUNIT_ASSERT( pq.push( *p ));
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
            key_type * p = pq.pop();
            CPPUNIT_ASSERT( p != nullptr );
            CPPUNIT_CHECK_EX( *p == nPrev, "Expected=" << nPrev << ", current=" << *p );

            CPPUNIT_ASSERT( pq.size() == pq.capacity() - 1 );
            CPPUNIT_ASSERT( !pq.full() );
            CPPUNIT_ASSERT( !pq.empty() );

            nSize = pq.size();
            while ( pq.size() > 1 ) {
                p = pq.pop();
                CPPUNIT_ASSERT( p != nullptr );
                CPPUNIT_CHECK_EX( *p == nPrev - 1, "Expected=" << nPrev - 1 << ", current=" << *p );
                nPrev = *p;
                --nSize;
                CPPUNIT_ASSERT( pq.size() == nSize );
            }

            CPPUNIT_ASSERT( !pq.full() );
            CPPUNIT_ASSERT( !pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 1 );

            p = pq.pop();
            CPPUNIT_ASSERT( p != nullptr );
            CPPUNIT_CHECK_EX( *p == c_nMinValue, "Expected=" << c_nMinValue << ", current=" << *p );

            CPPUNIT_ASSERT( !pq.full() );
            CPPUNIT_ASSERT( pq.empty() );
            CPPUNIT_ASSERT( pq.size() == 0 );

            // Clear test
            for ( key_type * p = pFirst; p < pLast; ++p ) {
                CPPUNIT_ASSERT( pq.push( *p ));
            }
            CPPUNIT_CHECK( !pq.empty() );
            CPPUNIT_CHECK( pq.full() );
            CPPUNIT_CHECK( pq.size() == pq.capacity() );
            pq.clear();
            CPPUNIT_CHECK( pq.empty() );
            CPPUNIT_CHECK( !pq.full() );
            CPPUNIT_CHECK( pq.size() == 0 );

            // clear_with test
            for ( key_type * p = pFirst; p < pLast; ++p ) {
                CPPUNIT_ASSERT( pq.push( *p ));
            }
            CPPUNIT_ASSERT( !pq.empty() );
            CPPUNIT_ASSERT( pq.full() );
            CPPUNIT_ASSERT( pq.size() == pq.capacity() );

            {
                intrusive_pqueue::another_disposer disp;
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

        CPPUNIT_TEST_SUITE(IntrusivePQueueHdrTest)
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
        CPPUNIT_TEST_SUITE_END()
    };

} // namespace priority_queue

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_PQUEUE_H

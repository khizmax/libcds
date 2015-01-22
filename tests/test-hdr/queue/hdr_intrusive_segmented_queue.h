//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRUSIVE_SEGMENTED_QUEUE_H
#define CDSTEST_HDR_INTRUSIVE_SEGMENTED_QUEUE_H

#include "cppunit/cppunit_proxy.h"
#include <cds/intrusive/details/base.h>
#include "size_check.h"

namespace queue {

    class HdrIntrusiveSegmentedQueue : public CppUnitMini::TestCase
    {
        struct item {
            int  nValue;

            size_t  nDisposeCount;
            size_t  nDispose2Count;

            item()
                : nValue( 0 )
                , nDisposeCount( 0 )
                , nDispose2Count( 0 )
            {}

            item( int nVal )
                : nValue( nVal )
                , nDisposeCount( 0 )
                , nDispose2Count( 0 )
            {}
        };

        struct big_item : public item
        {
            big_item()
            {}

            big_item( int nVal )
                : item( nVal )
            {}

            int arr[80];
        };

        struct Disposer
        {
            void operator()( item * p )
            {
                ++p->nDisposeCount;
            }
        };

        struct Disposer2
        {
            void operator()( item * p )
            {
                ++p->nDispose2Count;
            }
        };

        template <typename Queue>
        void test()
        {
            for ( size_t nQuasiFactor = 2; nQuasiFactor <= 256; ++nQuasiFactor ) {
                CPPUNIT_MSG( "QuasiFactor=" << nQuasiFactor << "..." );
                test_qf<Queue>( nQuasiFactor );
            }
        }

        template <typename Queue>
        void test_qf( size_t nQuasiFactor )
        {
            typedef typename Queue::value_type value_type;

            static size_t const c_nItemCount = 1000;
            value_type val[c_nItemCount];
            for ( int i = 0; i < static_cast<int>(sizeof(val)/sizeof(val[0])); ++i )
                val[i].nValue = i;

            {
                Queue q( nQuasiFactor );
                CPPUNIT_CHECK( q.quasi_factor() == cds::beans::ceil2(nQuasiFactor) );
                CPPUNIT_CHECK( misc::check_size( q, 0 ));
                CPPUNIT_CHECK( q.empty() );

                // push/enqueue
                for ( size_t i = 0; i < sizeof(val)/sizeof(val[0]); ++i ) {
                    if ( i & 1 ) {
                        CPPUNIT_ASSERT( q.push( val[i] ));
                    }
                    else {
                        CPPUNIT_ASSERT( q.enqueue( val[i] ));
                    }

                    CPPUNIT_CHECK( misc::check_size( q, i + 1 ));
                }
                CPPUNIT_CHECK( !q.empty() );

                // pop/dequeue
                size_t nCount = 0;
                while ( !q.empty() ) {
                    value_type * pVal;
                    if ( nCount & 1 )
                        pVal = q.pop();
                    else
                        pVal = q.dequeue();

                    CPPUNIT_ASSERT( pVal != nullptr );

                    int nSegment = int( nCount / q.quasi_factor() );
                    int nMin = nSegment * int(q.quasi_factor());
                    int nMax = nMin + int(q.quasi_factor()) - 1;
                    CPPUNIT_CHECK_EX( nMin <= pVal->nValue && pVal->nValue <= nMax, nMin << " <= " << pVal->nValue << " <= " << nMax );

                    ++nCount;
                    CPPUNIT_CHECK( misc::check_size( q, sizeof(val)/sizeof(val[0]) - nCount ));
                }
                CPPUNIT_CHECK( nCount == sizeof(val)/sizeof(val[0]) );
                CPPUNIT_CHECK( q.empty() );
                CPPUNIT_CHECK( misc::check_size( q, 0 ));

                // pop from empty queue
                CPPUNIT_ASSERT( q.pop() == nullptr );
                CPPUNIT_CHECK( q.empty() );
                CPPUNIT_CHECK( misc::check_size( q, 0 ));

                // check if Disposer has not been called
                Queue::gc::force_dispose();
                for ( int i = 0; i < static_cast<int>( sizeof(val)/sizeof(val[0]) ); ++i ) {
                    CPPUNIT_CHECK( val[i].nDisposeCount == 0 );
                    CPPUNIT_CHECK( val[i].nDispose2Count == 0 );
                }

                // Manually dispose the items
                for ( int i = 0; i < static_cast<int>( sizeof(val)/sizeof(val[0])); ++i )
                    Queue::gc::template retire<Disposer>( &(val[i]) );

                // check if Disposer has been called
                Queue::gc::force_dispose();
                for ( int i = 0; i < static_cast<int>( sizeof(val)/sizeof(val[0])); ++i ) {
                    CPPUNIT_CHECK( val[i].nDisposeCount == 1 );
                    CPPUNIT_CHECK( val[i].nDispose2Count == 0 );
                }


                // clear
                for ( int i = 0; i < static_cast<int>( sizeof(val)/sizeof(val[0])); ++i )
                    CPPUNIT_CHECK( q.push( val[i] ) );
                CPPUNIT_CHECK( misc::check_size( q, sizeof(val)/sizeof(val[0]) ));
                CPPUNIT_CHECK( !q.empty() );

                q.clear();
                CPPUNIT_CHECK( misc::check_size( q, 0));
                CPPUNIT_CHECK( q.empty() );

                // check if Disposer has been called
                Queue::gc::force_dispose();
                for ( int i = 0; i < static_cast<int>( sizeof(val)/sizeof(val[0])); ++i ) {
                    CPPUNIT_CHECK( val[i].nDisposeCount == 2 );
                    CPPUNIT_CHECK( val[i].nDispose2Count == 0 );
                }

                // clear_with
                for ( int i = 0; i < static_cast<int>( sizeof(val)/sizeof(val[0])); ++i )
                    CPPUNIT_CHECK( q.push( val[i] ) );
                CPPUNIT_CHECK( misc::check_size( q, sizeof(val)/sizeof(val[0]) ));
                CPPUNIT_CHECK( !q.empty() );

                q.clear_with( Disposer2() );
                CPPUNIT_CHECK( misc::check_size( q, 0));
                CPPUNIT_CHECK( q.empty() );

                // check if Disposer has been called
                Queue::gc::force_dispose();
                for ( int i = 0; i < static_cast<int>( sizeof(val)/sizeof(val[0])); ++i ) {
                    CPPUNIT_CHECK( val[i].nDisposeCount == 2 );
                    CPPUNIT_CHECK( val[i].nDispose2Count == 1 );
                }

                // check clear on destruct
                for ( int i = 0; i < static_cast<int>( sizeof(val)/sizeof(val[0])); ++i )
                    CPPUNIT_CHECK( q.push( val[i] ) );
                CPPUNIT_CHECK( misc::check_size( q, sizeof(val)/sizeof(val[0]) ));
                CPPUNIT_CHECK( !q.empty() );
            }

            // check if Disposer has been called
            Queue::gc::force_dispose();
            for ( int i = 0; i < static_cast<int>( sizeof(val)/sizeof(val[0])); ++i ) {
                CPPUNIT_CHECK( val[i].nDisposeCount == 3 );
                CPPUNIT_CHECK( val[i].nDispose2Count == 1 );
            }
        }

        void SegmQueue_HP();
        void SegmQueue_HP_mutex();
        void SegmQueue_HP_shuffle();
        void SegmQueue_HP_stat();
        void SegmQueue_HP_cacheline_padding();
        void SegmQueue_HP_mutex_cacheline_padding();
        void SegmQueue_HP_shuffle_cacheline_padding();
        void SegmQueue_HP_stat_cacheline_padding();
        void SegmQueue_HP_256_padding();
        void SegmQueue_HP_mutex_256_padding();
        void SegmQueue_HP_shuffle_256_padding();
        void SegmQueue_HP_stat_256_padding();
        void SegmQueue_HP_cacheline_padding_bigdata();
        void SegmQueue_HP_mutex_cacheline_padding_bigdata();
        void SegmQueue_HP_shuffle_cacheline_padding_bigdata();
        void SegmQueue_HP_stat_cacheline_padding_bigdata();

        void SegmQueue_DHP();
        void SegmQueue_DHP_mutex();
        void SegmQueue_DHP_shuffle();
        void SegmQueue_DHP_stat();
        void SegmQueue_DHP_cacheline_padding();
        void SegmQueue_DHP_mutex_cacheline_padding();
        void SegmQueue_DHP_shuffle_cacheline_padding();
        void SegmQueue_DHP_stat_cacheline_padding();
        void SegmQueue_DHP_256_padding();
        void SegmQueue_DHP_mutex_256_padding();
        void SegmQueue_DHP_shuffle_256_padding();
        void SegmQueue_DHP_stat_256_padding();
        void SegmQueue_DHP_cacheline_padding_bigdata();
        void SegmQueue_DHP_mutex_cacheline_padding_bigdata();
        void SegmQueue_DHP_shuffle_cacheline_padding_bigdata();
        void SegmQueue_DHP_stat_cacheline_padding_bigdata();

        CPPUNIT_TEST_SUITE(HdrIntrusiveSegmentedQueue)
            CPPUNIT_TEST( SegmQueue_HP )
            CPPUNIT_TEST( SegmQueue_HP_mutex )
            CPPUNIT_TEST( SegmQueue_HP_shuffle )
            CPPUNIT_TEST( SegmQueue_HP_stat )
            CPPUNIT_TEST( SegmQueue_HP_cacheline_padding )
            CPPUNIT_TEST( SegmQueue_HP_mutex_cacheline_padding )
            CPPUNIT_TEST( SegmQueue_HP_shuffle_cacheline_padding )
            CPPUNIT_TEST( SegmQueue_HP_stat_cacheline_padding )
            CPPUNIT_TEST( SegmQueue_HP_256_padding )
            CPPUNIT_TEST( SegmQueue_HP_mutex_256_padding )
            CPPUNIT_TEST( SegmQueue_HP_shuffle_256_padding )
            CPPUNIT_TEST( SegmQueue_HP_stat_256_padding )
            CPPUNIT_TEST( SegmQueue_HP_cacheline_padding_bigdata )
            CPPUNIT_TEST( SegmQueue_HP_mutex_cacheline_padding_bigdata )
            CPPUNIT_TEST( SegmQueue_HP_shuffle_cacheline_padding_bigdata )
            CPPUNIT_TEST( SegmQueue_HP_stat_cacheline_padding_bigdata )

            CPPUNIT_TEST( SegmQueue_DHP )
            CPPUNIT_TEST( SegmQueue_DHP_mutex )
            CPPUNIT_TEST( SegmQueue_DHP_shuffle )
            CPPUNIT_TEST( SegmQueue_DHP_stat )
            CPPUNIT_TEST( SegmQueue_DHP_cacheline_padding )
            CPPUNIT_TEST( SegmQueue_DHP_mutex_cacheline_padding )
            CPPUNIT_TEST( SegmQueue_DHP_shuffle_cacheline_padding )
            CPPUNIT_TEST( SegmQueue_DHP_stat_cacheline_padding )
            CPPUNIT_TEST( SegmQueue_DHP_256_padding )
            CPPUNIT_TEST( SegmQueue_DHP_mutex_256_padding )
            CPPUNIT_TEST( SegmQueue_DHP_shuffle_256_padding )
            CPPUNIT_TEST( SegmQueue_DHP_stat_256_padding )
            CPPUNIT_TEST( SegmQueue_DHP_cacheline_padding_bigdata )
            CPPUNIT_TEST( SegmQueue_DHP_mutex_cacheline_padding_bigdata )
            CPPUNIT_TEST( SegmQueue_DHP_shuffle_cacheline_padding_bigdata )
            CPPUNIT_TEST( SegmQueue_DHP_stat_cacheline_padding_bigdata )
        CPPUNIT_TEST_SUITE_END()
    };

} // namespace queue

#endif // CDSTEST_HDR_INTRUSIVE_SEGMENTED_QUEUE_H

#include <cds_test/ext_gtest.h>
#include <cds/container/ts_deque.h>
#include <cds/container/ts_timestamp.h>

namespace {

    class TSDeque: public ::testing::Test
    {
    protected:
        template <class Deque>
        void test( Deque& dq )
        {
            size_t const c_nSize = 100;

            // insert_right/remove_right
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i ) {
                EXPECT_TRUE( dq.insert_right( i ));
            }
            EXPECT_EQ( dq.size(), c_nSize );

            size_t nCount = 0;
            int val;
            while ( !dq.empty()) {
                EXPECT_TRUE( dq.remove_right( &val ));
                ++nCount;
                EXPECT_EQ( static_cast<int>(c_nSize - nCount), val );
            }
            EXPECT_EQ( nCount, c_nSize );

            // insert_left/remove_left
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                EXPECT_TRUE( dq.insert_left( i ));
            EXPECT_EQ( dq.size(), c_nSize );

            nCount = 0;
            while ( !dq.empty()) {
                EXPECT_TRUE( dq.remove_left( &val ));
                ++nCount;
                EXPECT_EQ( static_cast<int>(c_nSize - nCount), val );
            }
            EXPECT_EQ( nCount, c_nSize );

            // insert_left/remove_right
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                EXPECT_TRUE( dq.insert_left( i ));
            EXPECT_EQ( dq.size(), c_nSize );

            nCount = 0;
            while ( !dq.empty()) {
                EXPECT_TRUE( dq.remove_right( &val ));
                EXPECT_EQ( static_cast<int>( nCount ), val );
                ++nCount;
            }
            EXPECT_EQ( nCount, c_nSize );

            // insert_right/remove_left
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                EXPECT_TRUE( dq.insert_right( i ));
            EXPECT_EQ( dq.size(), c_nSize );

            nCount = 0;
            while ( !dq.empty()) {
                EXPECT_TRUE( dq.remove_left( &val ));
                EXPECT_EQ( static_cast<int>( nCount ), val );
                ++nCount;
            }
            EXPECT_EQ( nCount, c_nSize );

            // clear
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                EXPECT_TRUE( dq.insert_right( i ));
            EXPECT_EQ( dq.size(), c_nSize );

            EXPECT_FALSE( dq.empty());
            dq.clear();
            EXPECT_TRUE( dq.empty());
        }
    };

    TEST_F( TSDeque, hardware_timestamping )
    {
        typedef cds::container::TSDeque<int, cds::container::HardwareTimestamp,
            cds::container::tsdeque::make_traits<
                cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > deque_type;

        deque_type dq(1, 0);
        test( dq );
    }

    TEST_F( TSDeque, hardware_interval_timestamping )
    {
        typedef cds::container::TSDeque<int, cds::container::HardwareIntervalTimestamp,
            cds::container::tsdeque::make_traits<
                cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > deque_type;

        deque_type dq(1, 1000);
        test( dq );
    }

    TEST_F( TSDeque, atomic_counter_timestamping )
    {
        typedef cds::container::TSDeque<int, cds::container::AtomicCounterTimestamp,
            cds::container::tsdeque::make_traits<
                cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > deque_type;

        deque_type dq(1, 0);
        test( dq );
    }

} // namespace

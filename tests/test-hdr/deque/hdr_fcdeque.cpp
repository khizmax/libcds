//$$CDS-header$$

#include "cppunit/cppunit_proxy.h"
#include "cppunit/test_beans.h"
#include <cds/container/fcdeque.h>
#include <boost/container/deque.hpp>

namespace deque {

    class HdrFCDeque: public CppUnitMini::TestCase
    {
        template <class Deque>
        void test_with( Deque& dq )
        {
            size_t const c_nSize = 100;

            // push_front/pop_front
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_front( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            size_t nCount = 0;
            int val;
            while ( !dq.empty() ) {
                CPPUNIT_CHECK( dq.pop_front( val ) );
                ++nCount;
                CPPUNIT_CHECK( static_cast<int>(c_nSize - nCount) == val );
            }
            CPPUNIT_CHECK( nCount == c_nSize );

            // push_back/pop_back
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_back( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            nCount = 0;
            while ( !dq.empty() ) {
                CPPUNIT_CHECK( dq.pop_back( val ) );
                ++nCount;
                CPPUNIT_CHECK( static_cast<int>(c_nSize - nCount) == val );
            }
            CPPUNIT_CHECK( nCount == c_nSize );

            // push_back/pop_front
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_back( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            nCount = 0;
            while ( !dq.empty() ) {
                CPPUNIT_CHECK( dq.pop_front( val ) );
                CPPUNIT_CHECK( static_cast<int>( nCount ) == val );
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == c_nSize );

            // push_front/pop_back
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_front( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            nCount = 0;
            while ( !dq.empty() ) {
                CPPUNIT_CHECK( dq.pop_back( val ) );
                CPPUNIT_CHECK( static_cast<int>( nCount ) == val );
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == c_nSize );

            // clear
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_front( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            CPPUNIT_CHECK( !dq.empty() );
            dq.clear();
            CPPUNIT_CHECK( dq.empty() );
        }

        template <class Deque>
        void test()
        {
            Deque dq;
            test_with( dq );
        }

        void fcDeque()
        {
            typedef cds::container::FCDeque<int> deque_type;
            test<deque_type>();
        }

        void fcDeque_elimination()
        {
            typedef cds::container::FCDeque<int, std::deque<int>,
                cds::container::fcdeque::make_traits<
                    cds::opt::enable_elimination< true >
                >::type
            > deque_type;
            test<deque_type>();
        }

        void fcDeque_stat()
        {
            typedef cds::container::FCDeque<int, std::deque<int>,
                cds::container::fcdeque::make_traits<
                    cds::opt::stat< cds::container::fcdeque::stat<> >
                >::type
            > deque_type;
            test<deque_type>();
        }

        void fcDeque_mutex()
        {
            struct deque_traits : public
                cds::container::fcdeque::make_traits<
                    cds::opt::enable_elimination< true >
                >::type
            {
                typedef std::mutex lock_type;
            };
            typedef cds::container::FCDeque<int, std::deque<int>, deque_traits > deque_type;
            test<deque_type>();
        }

        void fcDeque_boost()
        {
            typedef cds::container::FCDeque<int, boost::container::deque<int> > deque_type;
            test<deque_type>();
        }

        void fcDeque_boost_elimination()
        {
            typedef cds::container::FCDeque<int, boost::container::deque<int>,
                cds::container::fcdeque::make_traits<
                    cds::opt::enable_elimination< true >
                >::type
            > deque_type;
            test<deque_type>();
        }

        void fcDeque_boost_stat()
        {
            typedef cds::container::FCDeque<int, boost::container::deque<int>,
                cds::container::fcdeque::make_traits<
                    cds::opt::stat< cds::container::fcdeque::stat<> >
                >::type
            > deque_type;
            test<deque_type>();
        }

        void fcDeque_boost_mutex()
        {
            typedef cds::container::FCDeque<int, boost::container::deque<int>,
                cds::container::fcdeque::make_traits<
                    cds::opt::enable_elimination< true >
                    ,cds::opt::lock_type< std::mutex >
                >::type
            > deque_type;
            test<deque_type>();
        }

        CPPUNIT_TEST_SUITE(HdrFCDeque)
            CPPUNIT_TEST(fcDeque)
            CPPUNIT_TEST(fcDeque_elimination)
            CPPUNIT_TEST(fcDeque_stat)
            CPPUNIT_TEST(fcDeque_mutex)
            CPPUNIT_TEST(fcDeque_boost)
            CPPUNIT_TEST(fcDeque_boost_elimination)
            CPPUNIT_TEST(fcDeque_boost_stat)
            CPPUNIT_TEST(fcDeque_boost_mutex)
        CPPUNIT_TEST_SUITE_END()
    };

} // namespace deque

CPPUNIT_TEST_SUITE_REGISTRATION(deque::HdrFCDeque);

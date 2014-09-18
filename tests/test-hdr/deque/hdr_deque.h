//$$CDS-header$$

#include "cppunit/cppunit_proxy.h"
#include "cppunit/test_beans.h"

namespace deque {

    class DequeHeaderTest: public CppUnitMini::TestCase
    {
    public:
        struct value_type {
            int     nVal;

            value_type()
            {}
            value_type( int i )
                : nVal(i)
                {}
        };

        struct assign_functor
        {
            template <typename T>
            void operator()( value_type& dest, T i ) const
            {
                dest.nVal = i;
            }
        };

        struct pop_functor
        {
            template <typename T>
            void operator()( T& dest, value_type const& v )
            {
                dest = v.nVal;
            }
        };

        static void assign_func( value_type& dest, int i )
        {
            dest.nVal = i;
        }
        static void pop_func( int& dest, value_type const& v )
        {
            dest = v.nVal;
        }

        template <class Deque>
        void test()
        {
            test_beans::check_item_counter<typename Deque::item_counter> check_ic;

            Deque q;
            value_type v;
            int i;

            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));

            // push_right/pop_right
            CPPUNIT_ASSERT( q.push_back( value_type(5) ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.push_back( 10, assign_functor() )) ; // functor
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.push_back( 20, assign_func )) ;     // function
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 3 ));

            CPPUNIT_ASSERT( q.pop_back(v) );
            CPPUNIT_ASSERT( v.nVal == 20 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_back( i, pop_functor()) );
            CPPUNIT_ASSERT( i == 10 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.pop_back(i, pop_func) );
            CPPUNIT_ASSERT( i == 5 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));

            v.nVal = -1;
            CPPUNIT_ASSERT( !q.pop_back(v) );
            CPPUNIT_ASSERT( v.nVal == -1 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));

#ifdef CDS_EMPLACE_SUPPORT
            CPPUNIT_ASSERT( q.emplace_back( 157 ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.emplace_back( 158 ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_back(i, pop_func) );
            CPPUNIT_ASSERT( i == 158 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.pop_back(i, pop_functor()) );
            CPPUNIT_ASSERT( i == 157 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
#endif

#ifdef CDS_CXX11_LAMBDA_SUPPORT
            CPPUNIT_ASSERT( q.push_back( value_type(511), [](value_type& dest, value_type const& i){ dest.nVal = i.nVal * 2; } ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.push_back( 512, [](value_type& dest, int i){ dest.nVal = i; } ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_back( i, []( int& dest, value_type const& v){ dest = v.nVal; } ) );
            CPPUNIT_ASSERT( i == 512 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            v.nVal = i = 0;
            CPPUNIT_ASSERT( q.pop_back( i, [&v]( int& dest, value_type const& val){ dest = -val.nVal; v.nVal = val.nVal; }) );
            CPPUNIT_ASSERT( i == -511 * 2 );
            CPPUNIT_ASSERT( v.nVal == 511 * 2);
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
#endif

            // push_right/pop_left
            CPPUNIT_ASSERT( q.push_back( value_type(5) ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.push_back( 10, assign_functor() )) ; // functor
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.push_back( 20, assign_func )) ;     // function
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 3 ));

            CPPUNIT_ASSERT( q.pop_front(v) );
            CPPUNIT_ASSERT( v.nVal == 5 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_front(i, pop_functor() ) );
            CPPUNIT_ASSERT( i == 10 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.pop_front( i, pop_func ) );
            CPPUNIT_ASSERT( i == 20 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));

            v.nVal = -1;
            CPPUNIT_ASSERT( !q.pop_back(v) );
            CPPUNIT_ASSERT( v.nVal == -1 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));

#ifdef CDS_EMPLACE_SUPPORT
            CPPUNIT_ASSERT( q.emplace_back( 157 ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.emplace_back( 158 ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_front(i, pop_func) );
            CPPUNIT_ASSERT( i == 157 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.pop_front(i, pop_functor()) );
            CPPUNIT_ASSERT( i == 158 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
#endif

#ifdef CDS_CXX11_LAMBDA_SUPPORT
            CPPUNIT_ASSERT( q.push_back( value_type(511), [](value_type& dest, value_type const& i){ dest.nVal = i.nVal * 2; } ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.push_back( 512, [](value_type& dest, int i){ dest.nVal = i; } ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_front( i, []( int& dest, value_type const& v){ dest = v.nVal; } ) );
            CPPUNIT_ASSERT( i == 511 * 2 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            v.nVal = i = 0;
            CPPUNIT_ASSERT( q.pop_front( i, [&v]( int& dest, value_type const& val){ dest = -val.nVal; v.nVal = val.nVal; }) );
            CPPUNIT_ASSERT( i == -512 );
            CPPUNIT_ASSERT( v.nVal == 512);
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
#endif


            // push_left/pop_left
            CPPUNIT_ASSERT( q.push_front( value_type(5) ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.push_front( 10, assign_functor() )) ; // functor
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.push_front( 20, assign_func )) ;     // function
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 3 ));

            CPPUNIT_ASSERT( q.pop_front(v) );
            CPPUNIT_ASSERT( v.nVal == 20 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_front(v) );
            CPPUNIT_ASSERT( v.nVal == 10 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.pop_front(v) );
            CPPUNIT_ASSERT( v.nVal == 5 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));

#ifdef CDS_EMPLACE_SUPPORT
            CPPUNIT_ASSERT( q.emplace_front( 157 ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.emplace_front( 158 ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_front(i, pop_func) );
            CPPUNIT_ASSERT( i == 158 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.pop_front(i, pop_functor()) );
            CPPUNIT_ASSERT( i == 157 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
#endif

#ifdef CDS_CXX11_LAMBDA_SUPPORT
            CPPUNIT_ASSERT( q.push_front( value_type(511), [](value_type& dest, value_type const& i){ dest.nVal = i.nVal * 2; } ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.push_front( 512, [](value_type& dest, int i){ dest.nVal = i; } ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_front( i, []( int& dest, value_type const& v){ dest = v.nVal; } ) );
            CPPUNIT_ASSERT( i == 512 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            v.nVal = i = 0;
            CPPUNIT_ASSERT( q.pop_front( i, [&v]( int& dest, value_type const& val){ dest = -val.nVal; v.nVal = val.nVal; }) );
            CPPUNIT_ASSERT( i == -511 * 2 );
            CPPUNIT_ASSERT( v.nVal == 511 * 2);
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
#endif

            // push_left/pop_right
            CPPUNIT_ASSERT( q.push_front( value_type(5) ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.push_front( 10, assign_functor() )) ; // functor
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.push_front( 20, assign_func )) ;     // function
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 3 ));

            CPPUNIT_ASSERT( q.pop_back(v) );
            CPPUNIT_ASSERT( v.nVal == 5 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_back(v) );
            CPPUNIT_ASSERT( v.nVal == 10 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.pop_back(v) );
            CPPUNIT_ASSERT( v.nVal == 20 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));

#ifdef CDS_EMPLACE_SUPPORT
            CPPUNIT_ASSERT( q.emplace_front( 157 ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.emplace_front( 158 ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_back(i, pop_func) );
            CPPUNIT_ASSERT( i == 157 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.pop_back(i, pop_functor()) );
            CPPUNIT_ASSERT( i == 158 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
#endif

#ifdef CDS_CXX11_LAMBDA_SUPPORT
            CPPUNIT_ASSERT( q.push_front( value_type(511), [](value_type& dest, value_type const& i){ dest.nVal = i.nVal * 2; } ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            CPPUNIT_ASSERT( q.push_front( 512, [](value_type& dest, int i){ dest.nVal = i * 3; } ));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));

            CPPUNIT_ASSERT( q.pop_back( i, []( int& dest, value_type const& v){ dest = v.nVal; } ) );
            CPPUNIT_ASSERT( i == 511 * 2 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));

            v.nVal = i = 0;
            CPPUNIT_ASSERT( q.pop_back( i, [&v]( int& dest, value_type const& val){ dest = -val.nVal; v.nVal = val.nVal; }) );
            CPPUNIT_ASSERT( i == -512 * 3 );
            CPPUNIT_ASSERT( v.nVal == 512 * 3);
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
#endif

            // clear test
            for ( int i = 0; i < 1000; i++ ) {
                CPPUNIT_ASSERT( q.push_back( value_type(i) ));
                CPPUNIT_ASSERT( q.push_front( value_type(i * 1024) ));
            }
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2000 ));
            q.clear();
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));

        }

        void test_MichaelDeque_HP();
        void test_MichaelDeque_HP_ic();
        void test_MichaelDeque_HP_stat();
        void test_MichaelDeque_HP_noalign();

        void test_MichaelDeque_PTB();
        void test_MichaelDeque_PTB_ic();
        void test_MichaelDeque_PTB_stat();
        void test_MichaelDeque_PTB_noalign();

        CPPUNIT_TEST_SUITE(DequeHeaderTest)
            CPPUNIT_TEST( test_MichaelDeque_HP);
            CPPUNIT_TEST( test_MichaelDeque_HP_ic);
            CPPUNIT_TEST( test_MichaelDeque_HP_stat);
            CPPUNIT_TEST( test_MichaelDeque_HP_noalign);

            CPPUNIT_TEST( test_MichaelDeque_PTB);
            CPPUNIT_TEST( test_MichaelDeque_PTB_ic);
            CPPUNIT_TEST( test_MichaelDeque_PTB_stat);
            CPPUNIT_TEST( test_MichaelDeque_PTB_noalign);

        CPPUNIT_TEST_SUITE_END()

    };
}   // namespace deque

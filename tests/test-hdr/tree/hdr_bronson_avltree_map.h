//$$CDS-header$$

#ifndef CDSTEST_HDR_BRONSON_AVLTREE_MAP_H
#define CDSTEST_HDR_BRONSON_AVLTREE_MAP_H

#include "cppunit/cppunit_proxy.h"
#include "size_check.h"
#include <functional>   // ref
#include <algorithm>

namespace tree {
    using misc::check_size;

    class BronsonAVLTreeHdrTest : public CppUnitMini::TestCase
    {
    public:
        typedef int     key_type;

        struct stat_data {
            size_t  nInsertFuncCall;
            size_t  nEnsureExistFuncCall;
            size_t  nEnsureNewFuncCall;
            size_t  nEraseFuncCall;
            size_t  nFindFuncCall;
            size_t  nFindConstFuncCall;

            stat_data()
                : nInsertFuncCall( 0 )
                , nEnsureExistFuncCall( 0 )
                , nEnsureNewFuncCall( 0 )
                , nEraseFuncCall( 0 )
                , nFindFuncCall( 0 )
                , nFindConstFuncCall( 0 )
            {}
        };

        struct value_type {
            int         nVal;
            stat_data   stat;

            value_type()
            {}

            value_type( int v )
                : nVal( v )
            {}
        };

        struct wrapped_int {
            int  nKey;

            wrapped_int( int n )
                : nKey( n )
            {}
        };

        struct wrapped_less
        {
            bool operator()( wrapped_int const& w, int n ) const
            {
                return w.nKey < n;
            }
            bool operator()( int n, wrapped_int const& w ) const
            {
                return n < w.nKey;
            }
            template <typename T>
            bool operator()( wrapped_int const& w, T const& v ) const
            {
                return w.nKey < v.nKey;
            }
            template <typename T>
            bool operator()( T const& v, wrapped_int const& w ) const
            {
                return v.nKey < w.nKey;
            }
        };

    protected:
        static const size_t c_nItemCount = 10000;

        class data_array
        {
            int *     pFirst;
            int *     pLast;

        public:
            data_array()
                : pFirst( new int[c_nItemCount] )
                , pLast( pFirst + c_nItemCount )
            {
                int i = 0;
                for ( int * p = pFirst; p != pLast; ++p, ++i )
                    *p = i;

                std::random_shuffle( pFirst, pLast );
            }

            ~data_array()
            {
                delete[] pFirst;
            }

            int operator[]( size_t i ) const
            {
                assert( i < size_t( pLast - pFirst ) );
                return pFirst[i];
            }
        };

        struct find_functor
        {
            template <typename T>
            void operator()( value_type& i, T& /*val*/ )
            {
                ++i.stat.nFindFuncCall;
            }
            template <typename T>
            void operator()( value_type& i, T const& /*val*/ )
            {
                ++i.stat.nFindConstFuncCall;
            }
        };

        template <typename Item>
        struct copy_found
        {
            Item    m_found;

            template <typename T>
            void operator()( Item& i, T& /*val*/ )
            {
                m_found = i;
            }

            void operator()( Item const& i )
            {
                m_found = i;
            }
        };

        struct insert_functor
        {
            template <typename Item>
            void operator()( Item& i )
            {
                i.nVal = i.nKey * 100;
                ++i.stat.nInsertFuncCall;
            }
        };

        template <typename Q>
        static void ensure_func( bool bNew, value_type& i, Q& /*val*/ )
        {
            if ( bNew )
                ++i.stat.nEnsureNewFuncCall;
            else
                ++i.stat.nEnsureExistFuncCall;
        }

        struct ensure_functor
        {
            template <typename Q>
            void operator()( bool bNew, value_type& i, Q& val )
            {
                ensure_func( bNew, i, val );
            }
        };

        struct extract_functor
        {
            int nKey;

            template <typename Q>
            void operator()( Q&, value_type& v )
            {
                nKey = v.nKey;
            }
        };

    protected:
        template <class Set>
        void test_with( Set& s )
        {
            value_type itm;
            int key;

            // insert/find test
            CPPUNIT_ASSERT( !s.find( 10 ) );
            CPPUNIT_ASSERT( s.insert( 10 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ) );
            CPPUNIT_ASSERT( s.find( 10 ) );

            CPPUNIT_ASSERT( !s.insert( 10 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ) );

            CPPUNIT_ASSERT( !s.find_with( 20, less() ) );
            CPPUNIT_ASSERT( s.insert( std::make_pair( 20, 25 ) ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ) );
            CPPUNIT_ASSERT( s.find_with( 10, less() ) );
            CPPUNIT_ASSERT( s.find( key = 20 ) );
            CPPUNIT_ASSERT( s.find_with( key, less(), find_functor() ) );
            {
                copy_found<value_type> f;
                f.m_found.nKey = 0;
                key = 20;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( f.m_found.stat.nFindConstFuncCall == 0 );
            }
            CPPUNIT_ASSERT( s.find( key, find_functor() ) );
            {
                copy_found<value_type> f;
                f.m_found.nKey = 0;
                key = 20;
                CPPUNIT_ASSERT( s.find_with( key, less(), std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.stat.nFindFuncCall == 2 );
                CPPUNIT_ASSERT( f.m_found.stat.nFindConstFuncCall == 0 );
            }
            CPPUNIT_ASSERT( s.find( 20, find_functor() ) );
            {
                copy_found<value_type> f;
                f.m_found.nKey = 0;
                CPPUNIT_ASSERT( s.find_with( 20, less(), std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.stat.nFindFuncCall == 2 );
                CPPUNIT_ASSERT( f.m_found.stat.nFindConstFuncCall == 1 );
            }

            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ) );

            CPPUNIT_ASSERT( !s.find( 25 ) );
            CPPUNIT_ASSERT( s.insert( std::make_pair( 25, -1 ), insert_functor() ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ) );
            {
                copy_found<value_type> f;
                f.m_found.nKey = 0;
                key = 25;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 25 );
                CPPUNIT_ASSERT( f.m_found.nVal == 2500 );
                CPPUNIT_ASSERT( f.m_found.stat.nInsertFuncCall == 1 );
            }

            // update test
            key = 10;
            {
                copy_found<value_type> f;
                f.m_found.nKey = 0;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 10 );
                CPPUNIT_ASSERT( f.m_found.nVal == 100 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureExistFuncCall == 0 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureNewFuncCall == 0 );
            }
            std::pair<bool, bool> ensureResult = s.update( key, ensure_functor() );
            CPPUNIT_ASSERT( ensureResult.first && !ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ) );
            {
                copy_found<value_type> f;
                f.m_found.nKey = 0;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 10 );
                CPPUNIT_ASSERT( f.m_found.nVal == 100 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureExistFuncCall == 1 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureNewFuncCall == 0 );
            }

            ensureResult = s.update( std::make_pair( 13, 1300 ), ensure_functor() );
            CPPUNIT_ASSERT( ensureResult.first && ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 4 ) );
            {
                copy_found<value_type> f;
                f.m_found.nKey = 0;
                key = 13;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 13 );
                CPPUNIT_ASSERT( f.m_found.nVal == 1300 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureExistFuncCall == 0 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureNewFuncCall == 1 );
            }

            // erase test
            CPPUNIT_ASSERT( s.erase( 13 ) );
            CPPUNIT_ASSERT( !s.find( 13 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ) );
            CPPUNIT_ASSERT( !s.erase( 13 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ) );

            CPPUNIT_ASSERT( s.find( 10 ) );
            CPPUNIT_ASSERT( s.erase_with( 10, less() ) );
            CPPUNIT_ASSERT( !s.find( 10 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ) );
            CPPUNIT_ASSERT( !s.erase_with( 10, less() ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ) );

            CPPUNIT_ASSERT( s.find( 20 ) );
            {
                copy_found<value_type> f;
                f.m_found.nKey = 0;
                CPPUNIT_ASSERT( s.erase( 20, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );

                CPPUNIT_ASSERT( s.insert( 235 ) )
                    CPPUNIT_ASSERT( s.erase_with( 235, less(), std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 235 );
                CPPUNIT_ASSERT( f.m_found.nVal == 2350 );
            }
            CPPUNIT_ASSERT( !s.find( 20 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ) );

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ) );

            // emplace test
            CPPUNIT_ASSERT( s.emplace( 151 ) );  // key = 151,  val = 1510
            CPPUNIT_ASSERT( s.emplace( 174, 471 ) );    // key = 174, val = 471
            CPPUNIT_ASSERT( s.emplace( std::make_pair( 190, 91 ) ) ); // key == 190, val = 91
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ) );

            CPPUNIT_ASSERT( s.find( 151 ) );
            CPPUNIT_ASSERT( s.find_with( 174, less() ) );
            CPPUNIT_ASSERT( s.find( 190 ) );

            {
                copy_found<value_type> f;
                f.m_found.nKey = 0;
                key = 151;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 151 );
                CPPUNIT_ASSERT( f.m_found.nVal == 1510 );

                key = 174;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 174 );
                CPPUNIT_ASSERT( f.m_found.nVal == 471 );

                key = 190;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 190 );
                CPPUNIT_ASSERT( f.m_found.nVal == 91 );
            }

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ) );
        }

        template <typename Set>
        void fill_set( Set& s, data_array& a )
        {
            CPPUNIT_ASSERT( s.empty() );
            for ( size_t i = 0; i < c_nItemCount; ++i ) {
                CPPUNIT_ASSERT( s.insert( a[i] ) );
            }
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, c_nItemCount ) );
        }

        template <class Set, class PrintStat>
        void test()
        {
            typedef Set set_type;

            set_type s;

            test_with( s );

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ) );



            PrintStat()(s);
        }


        void BronsonAVLTree_rcu_gpb_less();
        void BronsonAVLTree_rcu_gpb_cmp();
        void BronsonAVLTree_rcu_gpb_cmpless();
        void BronsonAVLTree_rcu_gpb_less_ic();
        void BronsonAVLTree_rcu_gpb_cmp_ic();
        void BronsonAVLTree_rcu_gpb_less_stat();
        void BronsonAVLTree_rcu_gpb_cmp_ic_stat();
        void BronsonAVLTree_rcu_gpb_cmp_ic_stat_yield();

        CPPUNIT_TEST_SUITE( BronsonAVLTreeHdrTest )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmp )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmpless )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_less_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmp_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmp_ic_stat_yield )
        CPPUNIT_TEST_SUITE_END()
    };
} // namespace tree

#endif // #ifndef CDSTEST_HDR_BRONSON_AVLTREE_MAP_H

//$$CDS-header$$

#ifndef CDSHDRTEST_ELLENBINTREE_MAP_H
#define CDSHDRTEST_ELLENBINTREE_MAP_H

#include "cppunit/cppunit_proxy.h"
#include "size_check.h"
#include <cds/ref.h>
#include <algorithm>

namespace tree {
    using misc::check_size;

    class EllenBinTreeMapHdrTest: public CppUnitMini::TestCase
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
                : nInsertFuncCall(0)
                , nEnsureExistFuncCall(0)
                , nEnsureNewFuncCall(0)
                , nEraseFuncCall(0)
                , nFindFuncCall(0)
                , nFindConstFuncCall(0)
            {}
        };

        struct value_type: public stat_data
        {
            int         nVal;

            value_type()
                : nVal(0)
            {}

            value_type( int v )
                : nVal( v )
            {}

            value_type( value_type const& v )
                : nVal( v.nVal )
            {}

            value_type( value_type&& v )
                : nVal( v.nVal )
            {}

            value_type& operator=( int n )
            {
                nVal = n;
                return *this;
            }

            value_type& operator=( value_type const& v )
            {
                nVal = v.nVal;
                return *this;
            }
        };

        typedef std::pair<key_type const, value_type> pair_type;

        struct less {
            bool operator()( int k1, int k2 ) const
            {
                return k1 < k2;
            }
        };

        struct compare {
            int cmp( int k1, int k2 ) const
            {
                return k1 < k2 ? -1 : (k1 > k2 ? 1 : 0);
            }
            int operator()( int k1, int k2 ) const
            {
                return cmp( k1, k2 );
            }
        };

        struct wrapped_int {
            int  nKey;

            wrapped_int( int n )
                : nKey(n)
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
            /*
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
            */
        };

    protected:
        template <typename Map>
        struct insert_functor
        {
            typedef typename Map::value_type pair_type;

            // insert ftor
            void operator()( pair_type& item )
            {
                item.second.nVal = item.first * 3;
            }

            // ensure ftor
            void operator()( bool bNew, pair_type& item )
            {
                if ( bNew )
                    item.second.nVal = item.first * 2;
                else
                    item.second.nVal = item.first * 5;
            }
        };

        struct check_value {
            int     m_nExpected;

            check_value( int nExpected )
                : m_nExpected( nExpected )
            {}

            template <typename T>
            void operator ()( T& pair )
            {
                CPPUNIT_ASSERT_CURRENT( pair.second.nVal == m_nExpected );
            }
            template <typename T, typename Q>
            void operator ()( T& pair, Q )
            {
                CPPUNIT_ASSERT_CURRENT( pair.second.nVal == m_nExpected );
            }
        };

        struct extract_functor
        {
            int *   m_pVal;
            void operator()( pair_type const& val )
            {
                *m_pVal = val.second.nVal;
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
                delete [] pFirst;
            }

            int operator[]( size_t i ) const
            {
                assert( i < size_t(pLast - pFirst) );
                return pFirst[i];
            }
        };

        struct extract_functor2
        {
            int nKey;

            template <typename Q>
            void operator()( Q&, pair_type& v )
            {
                nKey = v.first;
            }
        };


    protected:

        template <class Map>
        void test_with( Map& m )
        {
            std::pair<bool, bool> ensureResult;

            // insert
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));
            CPPUNIT_ASSERT( !m.find(25) );
            CPPUNIT_ASSERT( m.insert( 25 ) )    ;   // value = 0
            CPPUNIT_ASSERT( m.find(25) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 1 ));
            CPPUNIT_ASSERT( m.find(25) );

            CPPUNIT_ASSERT( !m.insert( 25 ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 1 ));

            CPPUNIT_ASSERT( !m.find_with(10, less()) );
            CPPUNIT_ASSERT( m.insert( 10, 10 ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));
            CPPUNIT_ASSERT( m.find_with(10, less()) );

            CPPUNIT_ASSERT( !m.insert( 10, 20 ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));

            CPPUNIT_ASSERT( !m.find(30) );
            CPPUNIT_ASSERT( m.insert_key( 30, insert_functor<Map>() ) )    ; // value = 90
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( m.find(30) );

            CPPUNIT_ASSERT( !m.insert_key( 10, insert_functor<Map>() ) );
            CPPUNIT_ASSERT( !m.insert_key( 25, insert_functor<Map>() ) );
            CPPUNIT_ASSERT( !m.insert_key( 30, insert_functor<Map>() ) );

            // ensure (new key)
            CPPUNIT_ASSERT( !m.find(27) );
            ensureResult = m.ensure( 27, insert_functor<Map>() ) ;   // value = 54
            CPPUNIT_ASSERT( ensureResult.first );
            CPPUNIT_ASSERT( ensureResult.second );
            CPPUNIT_ASSERT( m.find(27) );

            // find test
            check_value chk(10);
            CPPUNIT_ASSERT( m.find( 10, cds::ref(chk) ));
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find_with( 25, less(), boost::ref(chk) ));
            chk.m_nExpected = 90;
            CPPUNIT_ASSERT( m.find( 30, boost::ref(chk) ));
            chk.m_nExpected = 54;
            CPPUNIT_ASSERT( m.find( 27, boost::ref(chk) ));

            ensureResult = m.ensure( 10, insert_functor<Map>() ) ;   // value = 50
            CPPUNIT_ASSERT( ensureResult.first );
            CPPUNIT_ASSERT( !ensureResult.second );
            chk.m_nExpected = 50;
            CPPUNIT_ASSERT( m.find( 10, boost::ref(chk) ));

            // erase test
            CPPUNIT_ASSERT( !m.find(100) );
            CPPUNIT_ASSERT( !m.erase( 100 )) ;  // not found

            CPPUNIT_ASSERT( m.find(25) );
            CPPUNIT_ASSERT( check_size( m, 4 ));
            CPPUNIT_ASSERT( m.erase( 25 ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( !m.find(25) );
            CPPUNIT_ASSERT( !m.erase( 25 ));

            CPPUNIT_ASSERT( !m.find(258) );
            CPPUNIT_ASSERT( m.insert(258))
            CPPUNIT_ASSERT( check_size( m, 4 ));
            CPPUNIT_ASSERT( m.find_with(258, less()) );
            CPPUNIT_ASSERT( m.erase_with( 258, less() ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( !m.find(258) );
            CPPUNIT_ASSERT( !m.erase_with( 258, less() ));

            int nVal;
            extract_functor ext;
            ext.m_pVal = &nVal;

            CPPUNIT_ASSERT( !m.find(29) );
            CPPUNIT_ASSERT( m.insert(29, 290));
            CPPUNIT_ASSERT( check_size( m, 4 ));
            CPPUNIT_ASSERT( m.erase_with( 29, less(), boost::ref(ext)));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( nVal == 290 );
            nVal = -1;
            CPPUNIT_ASSERT( !m.erase_with( 29, less(), boost::ref(ext)));
            CPPUNIT_ASSERT( nVal == -1 );

            CPPUNIT_ASSERT( m.erase( 30, boost::ref(ext)));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));
            CPPUNIT_ASSERT( nVal == 90 );
            nVal = -1;
            CPPUNIT_ASSERT( !m.erase( 30, boost::ref(ext)));
            CPPUNIT_ASSERT( nVal == -1 );

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

#       ifdef CDS_EMPLACE_SUPPORT
            // emplace test
            CPPUNIT_ASSERT( m.emplace(126) ) ; // key = 126, val = 0
            CPPUNIT_ASSERT( m.emplace(137, 731))    ;   // key = 137, val = 731
            CPPUNIT_ASSERT( m.emplace( 149, value_type(941) ))   ;   // key = 149, val = 941

            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));

            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find( 126, cds::ref(chk) ));
            chk.m_nExpected = 731;
            CPPUNIT_ASSERT( m.find_with( 137, less(), cds::ref(chk) ));
            chk.m_nExpected = 941;
            CPPUNIT_ASSERT( m.find( 149, cds::ref(chk) ));

            CPPUNIT_ASSERT( !m.emplace(126, 621)) ; // already in map
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find( 126, cds::ref(chk) ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));
#       endif
        }

        template <typename Map>
        void fill_map( Map& s, data_array& a )
        {
            CPPUNIT_ASSERT( s.empty() );
            for ( size_t i = 0; i < c_nItemCount; ++i ) {
                CPPUNIT_ASSERT( s.insert( a[i] ));
            }
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, c_nItemCount ));
        }

        template <class Map, class PrintStat>
        void test()
        {
            typedef Map map_type;

            map_type m;

            test_with( m );

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

            // extract min/max
            {
                typename map_type::guarded_ptr gp;

                data_array arr;
                fill_map( m, arr );

                int i = 0;
                std::pair<key_type, value_type> v;
                while ( !m.empty() ) {
                    CPPUNIT_ASSERT( m.extract_min( gp ) );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_ASSERT( gp->first == i );
                    ++i;
                    gp.release();
                }
                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));


                fill_map( m, arr );
                i = (int) c_nItemCount - 1;
                while ( !m.empty() ) {
                    CPPUNIT_ASSERT( m.extract_max( gp ) );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_ASSERT( gp->first == i );
                    --i;
                    gp.release();
                }
                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));

                fill_map( m, arr );
                for ( int i = 0; i < static_cast<int>( c_nItemCount ); ++i ) {
                    int nKey = arr[i];
                    CPPUNIT_ASSERT( m.get( gp, nKey ));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );

                    gp.release();
                    CPPUNIT_ASSERT( m.extract( gp, nKey ));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );

                    gp.release();
                    CPPUNIT_CHECK( !m.get( gp, nKey ));
                    CPPUNIT_CHECK( gp.empty());
                    CPPUNIT_CHECK( !m.extract( gp, nKey ));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));

                fill_map( m, arr );
                for ( int i = 0; i < static_cast<int>( c_nItemCount ); ++i ) {
                    int nKey = arr[i];
                    CPPUNIT_ASSERT( m.get_with( gp, wrapped_int(nKey), wrapped_less() ));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );

                    gp.release();
                    CPPUNIT_ASSERT( m.extract_with( gp, wrapped_int(nKey), wrapped_less() ));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );

                    gp.release();
                    CPPUNIT_CHECK( !m.get_with( gp, wrapped_int(nKey), wrapped_less() ));
                    CPPUNIT_CHECK( gp.empty());
                    CPPUNIT_CHECK( !m.extract_with( gp, wrapped_int(nKey), wrapped_less() ));
                    CPPUNIT_CHECK( gp.empty());
                }

                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));
            }

            PrintStat()( m );
        }

        template <class Map, class PrintStat>
        void test_rcu()
        {
            typedef Map map_type;

            map_type m;

            test_with( m );

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

            // extract min/max
            {
                typename map_type::exempt_ptr ep;
                data_array arr;
                fill_map( m, arr );

                int i = 0;
                while ( !m.empty() ) {
                    CPPUNIT_ASSERT( m.extract_min( ep ) );
                    CPPUNIT_ASSERT( !ep.empty());
                    CPPUNIT_ASSERT(ep->first == i );
                    ++i;
                    ep.release();
                }
                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));
                CPPUNIT_ASSERT( !m.extract_min( ep ) );
                CPPUNIT_ASSERT( ep.empty());

                fill_map( m, arr );
                i = (int) c_nItemCount - 1;
                while ( !m.empty() ) {
                    CPPUNIT_ASSERT( m.extract_max( ep ) );
                    CPPUNIT_ASSERT( !ep.empty());
                    CPPUNIT_ASSERT( ep->first == i );
                    --i;
                    ep.release();
                }
                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));
                CPPUNIT_ASSERT( !m.extract_max( ep ) );
                CPPUNIT_ASSERT( ep.empty());

                fill_map( m, arr );
                for ( size_t i = 0; i < c_nItemCount; ++i ) {
                    int nKey = arr[i];
                    {
                        typename map_type::rcu_lock l;
                        typename map_type::value_type * pVal = m.get(nKey);
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->first == nKey);
                    }
                    CPPUNIT_ASSERT( m.extract( ep, nKey ));
                    CPPUNIT_ASSERT( !ep.empty());
                    CPPUNIT_CHECK( ep->first == nKey);
                    ep.release();

                    CPPUNIT_ASSERT( !m.extract( ep, nKey ));
                    CPPUNIT_ASSERT( ep.empty());
                    {
                        typename map_type::rcu_lock l;
                        CPPUNIT_CHECK( !m.get(nKey));
                    }
                }
                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));

                fill_map( m, arr );
                for ( size_t i = 0; i < c_nItemCount; ++i ) {
                    int nKey = arr[i];
                    {
                        typename map_type::rcu_lock l;
                        typename map_type::value_type * pVal = m.get_with(wrapped_int(nKey), wrapped_less());
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->first == nKey);
                    }
                    CPPUNIT_ASSERT( m.extract_with( ep, wrapped_int(nKey), wrapped_less() ));
                    CPPUNIT_ASSERT( !ep.empty());
                    CPPUNIT_CHECK( ep->first == nKey);
                    ep.release();

                    CPPUNIT_ASSERT( !m.extract_with( ep, wrapped_int(nKey), wrapped_less() ));
                    CPPUNIT_ASSERT( ep.empty());
                    {
                        typename map_type::rcu_lock l;
                        CPPUNIT_CHECK( !m.get_with(wrapped_int(nKey), wrapped_less()));
                    }
                }
                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));
            }

            PrintStat()( m );
        }

        void EllenBinTree_rcu_gpi_less();
        void EllenBinTree_rcu_gpi_cmp();
        void EllenBinTree_rcu_gpi_cmpless();
        void EllenBinTree_rcu_gpi_less_ic();
        void EllenBinTree_rcu_gpi_cmp_ic();
        void EllenBinTree_rcu_gpi_less_stat();
        void EllenBinTree_rcu_gpi_cmp_ic_stat();
        void EllenBinTree_rcu_gpi_less_pool();
        void EllenBinTree_rcu_gpi_less_pool_ic_stat();

        void EllenBinTree_hp_less();
        void EllenBinTree_hp_cmp();
        void EllenBinTree_hp_cmpless();
        void EllenBinTree_hp_less_ic();
        void EllenBinTree_hp_cmp_ic();
        void EllenBinTree_hp_less_stat();
        void EllenBinTree_hp_cmp_ic_stat();
        void EllenBinTree_hp_less_pool();
        void EllenBinTree_hp_less_pool_ic_stat();

        void EllenBinTree_ptb_less();
        void EllenBinTree_ptb_cmp();
        void EllenBinTree_ptb_cmpless();
        void EllenBinTree_ptb_less_ic();
        void EllenBinTree_ptb_cmp_ic();
        void EllenBinTree_ptb_less_stat();
        void EllenBinTree_ptb_cmp_ic_stat();
        void EllenBinTree_ptb_less_pool();
        void EllenBinTree_ptb_less_pool_ic_stat();

        void EllenBinTree_rcu_gpb_less();
        void EllenBinTree_rcu_gpb_cmp();
        void EllenBinTree_rcu_gpb_cmpless();
        void EllenBinTree_rcu_gpb_less_ic();
        void EllenBinTree_rcu_gpb_cmp_ic();
        void EllenBinTree_rcu_gpb_less_stat();
        void EllenBinTree_rcu_gpb_cmp_ic_stat();
        void EllenBinTree_rcu_gpb_less_pool();
        void EllenBinTree_rcu_gpb_less_pool_ic_stat();

        void EllenBinTree_rcu_gpt_less();
        void EllenBinTree_rcu_gpt_cmp();
        void EllenBinTree_rcu_gpt_cmpless();
        void EllenBinTree_rcu_gpt_less_ic();
        void EllenBinTree_rcu_gpt_cmp_ic();
        void EllenBinTree_rcu_gpt_less_stat();
        void EllenBinTree_rcu_gpt_cmp_ic_stat();
        void EllenBinTree_rcu_gpt_less_pool();
        void EllenBinTree_rcu_gpt_less_pool_ic_stat();

        void EllenBinTree_rcu_shb_less();
        void EllenBinTree_rcu_shb_cmp();
        void EllenBinTree_rcu_shb_cmpless();
        void EllenBinTree_rcu_shb_less_ic();
        void EllenBinTree_rcu_shb_cmp_ic();
        void EllenBinTree_rcu_shb_less_stat();
        void EllenBinTree_rcu_shb_cmp_ic_stat();
        void EllenBinTree_rcu_shb_less_pool();
        void EllenBinTree_rcu_shb_less_pool_ic_stat();

        void EllenBinTree_rcu_sht_less();
        void EllenBinTree_rcu_sht_cmp();
        void EllenBinTree_rcu_sht_cmpless();
        void EllenBinTree_rcu_sht_less_ic();
        void EllenBinTree_rcu_sht_cmp_ic();
        void EllenBinTree_rcu_sht_less_stat();
        void EllenBinTree_rcu_sht_cmp_ic_stat();
        void EllenBinTree_rcu_sht_less_pool();
        void EllenBinTree_rcu_sht_less_pool_ic_stat();

        CPPUNIT_TEST_SUITE(EllenBinTreeMapHdrTest)
            CPPUNIT_TEST(EllenBinTree_hp_less)
            CPPUNIT_TEST(EllenBinTree_hp_cmp)
            CPPUNIT_TEST(EllenBinTree_hp_less_stat)
            CPPUNIT_TEST(EllenBinTree_hp_cmpless)
            CPPUNIT_TEST(EllenBinTree_hp_less_ic)
            CPPUNIT_TEST(EllenBinTree_hp_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_hp_cmp_ic_stat)
            CPPUNIT_TEST(EllenBinTree_hp_less_pool)
            CPPUNIT_TEST(EllenBinTree_hp_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_ptb_less)
            CPPUNIT_TEST(EllenBinTree_ptb_cmp)
            CPPUNIT_TEST(EllenBinTree_ptb_less_stat)
            CPPUNIT_TEST(EllenBinTree_ptb_cmpless)
            CPPUNIT_TEST(EllenBinTree_ptb_less_ic)
            CPPUNIT_TEST(EllenBinTree_ptb_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_ptb_cmp_ic_stat)
            CPPUNIT_TEST(EllenBinTree_ptb_less_pool)
            CPPUNIT_TEST(EllenBinTree_ptb_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_gpi_less)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_cmp_ic_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_less_pool)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_gpb_less)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_cmp_ic_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_less_pool)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_gpt_less)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_cmp_ic_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_less_pool)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_shb_less)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_cmp_ic_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_less_pool)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_sht_less)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_cmp_ic_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_less_pool)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_less_pool_ic_stat)

            CPPUNIT_TEST_SUITE_END()

    };
} // namespace tree

#endif // #ifndef CDSHDRTEST_ELLENBINTREE_MAP_H

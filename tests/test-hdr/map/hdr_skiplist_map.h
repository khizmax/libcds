//$$CDS-header$$

#ifndef __CDSTEST_HDR_SKIPLIST_MAP_H
#define __CDSTEST_HDR_SKIPLIST_MAP_H

#include "map/hdr_map.h"

namespace map {

    class SkipListMapHdrTest: public HashMapHdrTest
    {
        typedef HashMapHdrTest base_class;
        typedef base_class::other_item  wrapped_item;
        typedef base_class::other_less  wrapped_less;

        template <class Map, typename PrintStat >
        void test()
        {
            Map m;
            test_int_with( m );

            static int const nLimit = 10000;
            typedef typename Map::iterator          set_iterator;
            typedef typename Map::const_iterator    const_set_iterator;

            int nCount = 0;
            int nPrevKey = 0;

            // Test iterator - ascending order
            m.clear();
            CPPUNIT_ASSERT( m.empty() );

            for ( int i = 0; i < nLimit; ++i ) {
                CPPUNIT_ASSERT( m.insert(i, i) );
            }
            CPPUNIT_MSG( PrintStat()(m, "Iterator test, ascending insert order") );

            nCount = 0;
            nPrevKey = 0;
            for ( set_iterator it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).first == it->second.m_val );
                CPPUNIT_ASSERT( m.find( it->first ));
                it->second.m_val = (*it).first * 2;
                ++nCount;
                if ( it != m.begin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->first );
                }
                nPrevKey = it->first;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            for ( const_set_iterator it = m.cbegin(), itEnd = m.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).first * 2 == it->second.m_val );
                ++nCount;
                if ( it != m.cbegin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->first );
                }
                nPrevKey = it->first;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            // Test iterator - descending order
            m.clear();
            CPPUNIT_ASSERT( m.empty() );

            for ( int i = nLimit; i > 0; --i ) {
                CPPUNIT_ASSERT( m.insert( i - 1, (i-1) * 2) );
            }
            CPPUNIT_MSG( PrintStat()(m, "Iterator test, descending insert order") );

            nCount = 0;
            nPrevKey = 0;
            for ( set_iterator it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).first * 2 == it->second.m_val );
                CPPUNIT_ASSERT( m.find( it->first ));
                it->second.m_val = (*it).first;
                ++nCount;
                if ( it != m.begin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->first );
                }
                nPrevKey = it->first;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            for ( const_set_iterator it = m.cbegin(), itEnd = m.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).first == it->second.m_val );
                ++nCount;
                if ( it != m.cbegin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->first );
                }
                nPrevKey = it->first;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            // Test iterator - random order
            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            {
                int nRand[nLimit];
                for ( int i = 0; i < nLimit; ++i ) {
                    nRand[i] = i;
                }
                std::random_shuffle( nRand, nRand + nLimit );

                for ( int i = 0; i < nLimit; ++i ) {
                    CPPUNIT_ASSERT( m.insert( nRand[i], nRand[i]) );
                }
                CPPUNIT_MSG( PrintStat()(m, "Iterator test, random insert order") );
            }

            nCount = 0;
            nPrevKey = 0;
            for ( set_iterator it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).first == it->second.m_val );
                CPPUNIT_ASSERT( m.find( it->first ));
                it->second.m_val = (*it).first * 2;
                ++nCount;
                if ( it != m.begin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->first );
                }
                nPrevKey = it->first;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            for ( const_set_iterator it = m.cbegin(), itEnd = m.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).first * 2 == it->second.m_val );
                ++nCount;
                if ( it != m.cbegin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->first );
                }
                nPrevKey = it->first;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            {
                typename Map::guarded_ptr gp;
                int arrItem[nLimit];
                for ( int i = 0; i < nLimit; ++i )
                    arrItem[i] = i;
                std::random_shuffle( arrItem, arrItem + nLimit );

                typedef base_class::less less;

                // extract/get
                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrItem[i];
                    CPPUNIT_ASSERT( m.get( gp, nKey ));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey * 2 );
                    gp.release();
                    CPPUNIT_ASSERT( m.extract(gp, nKey));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey * 2 );
                    gp.release();
                    CPPUNIT_CHECK( !m.get( gp, nKey ));
                    CPPUNIT_CHECK( !m.extract(gp, nKey));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( m.empty());

                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( m.insert(arrItem[i], arrItem[i]*2) );

                // extract_with/get_with
                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrItem[i];
                    CPPUNIT_ASSERT( m.get_with( gp, wrapped_item(nKey), wrapped_less() ));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey * 2 );
                    gp.release();
                    CPPUNIT_ASSERT( m.extract_with(gp, wrapped_item(nKey), wrapped_less()));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey * 2 );
                    gp.release();
                    CPPUNIT_CHECK( !m.get_with( gp, wrapped_item(nKey), wrapped_less()));
                    CPPUNIT_CHECK( !m.extract_with( gp, wrapped_item(nKey), wrapped_less()));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( m.empty());

                //extract_min
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( m.insert(arrItem[i], arrItem[i]*2) );

                for ( int i = 0; i < nLimit; ++i ) {
                    CPPUNIT_ASSERT( m.extract_min(gp));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == i );
                    CPPUNIT_CHECK( gp->second.m_val == i * 2 );
                    gp.release();
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_CHECK( !m.extract_min(gp));
                CPPUNIT_CHECK( gp.empty());
                CPPUNIT_ASSERT( m.empty());

                // extract_max
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( m.insert(arrItem[i], arrItem[i]*2) );

                for ( int i = nLimit - 1; i >= 0; --i ) {
                    CPPUNIT_ASSERT( m.extract_max(gp));
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == i );
                    CPPUNIT_CHECK( gp->second.m_val == i * 2 );
                    gp.release();
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_CHECK( !m.extract_max(gp));
                CPPUNIT_CHECK( gp.empty());
                CPPUNIT_ASSERT( m.empty());
            }

            CPPUNIT_MSG( PrintStat()(m, nullptr) );
        }

        template <class Map, typename PrintStat >
        void test_nogc()
        {
            typedef typename Map::iterator          iterator;
            typedef typename Map::const_iterator    const_iterator;

            Map m;

            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

            CPPUNIT_ASSERT( m.find(10) == m.end() );
            iterator it = m.insert( 10 );
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( it->first == 10 );
            CPPUNIT_ASSERT( it->second.m_val == 0 );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 1 ));
            CPPUNIT_ASSERT( m.find(10) == it );
            CPPUNIT_ASSERT( it->first == 10 );
            CPPUNIT_ASSERT( it->second.m_val == 0 );

            CPPUNIT_ASSERT( m.find(100) == m.end() );
            it = m.insert( 100, 200 );
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));
            CPPUNIT_ASSERT( m.find(100) == it );
            CPPUNIT_ASSERT( it->first == 100 );
            CPPUNIT_ASSERT( it->second.m_val == 200 );

            CPPUNIT_ASSERT( m.find(55) == m.end() );
            it = m.insert_key( 55, insert_functor<Map>() );
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( m.find(55) == it );
            CPPUNIT_ASSERT( it->first == 55 );
            CPPUNIT_ASSERT( it->second.m_val == 55 * 3 );

            CPPUNIT_ASSERT( m.insert( 55 ) == m.end() );
            CPPUNIT_ASSERT( m.insert( 55, 10 ) == m.end() );
            CPPUNIT_ASSERT( m.insert_key( 55, insert_functor<Map>()) == m.end() );

            CPPUNIT_ASSERT( m.find(10) != m.end() );
            std::pair<iterator, bool> ensureResult = m.ensure( 10 );
            CPPUNIT_ASSERT( ensureResult.first != m.end() );
            CPPUNIT_ASSERT( !ensureResult.second  );
            CPPUNIT_ASSERT( !m.empty() );
            ensureResult.first->second.m_val = ensureResult.first->first * 5;
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( m.find(10) == ensureResult.first );
            it = m.find_with( 10, typename base_class::less() );
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( it->second.m_val == 50 );

            CPPUNIT_ASSERT( m.find_with(120, base_class::less()) == m.end() );
            ensureResult = m.ensure( 120 );
            CPPUNIT_ASSERT( ensureResult.first != m.end() );
            CPPUNIT_ASSERT( ensureResult.second  );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 4 ));
            ensureResult.first->second.m_val = ensureResult.first->first * 5;
            CPPUNIT_ASSERT( m.find_with(120, base_class::less()) == ensureResult.first );
            it = m.find_with(120, base_class::less());
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( it->second.m_val == 120 * 5 );

#       ifdef CDS_EMPLACE_SUPPORT
            // emplace test
            it = m.emplace( 151 ) ;  // key = 151,  val = 0
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( it->first == 151 );
            CPPUNIT_ASSERT( it->second.m_val == 0 );

            it = m.emplace( 174, 471 ) ; // key == 174, val = 471
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( it->first == 174 );
            CPPUNIT_ASSERT( it->second.m_val == 471 );

            it = m.emplace( 190, value_type(91)) ; // key == 190, val = 19
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( it->first == 190 );
            CPPUNIT_ASSERT( it->second.m_val == 91 );

            it = m.emplace( 151, 1051 );
            CPPUNIT_ASSERT( it == m.end());

            it = m.find( 174 );
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( it->first == 174 );
            CPPUNIT_ASSERT( it->second.m_val == 471 );

            it = m.find( 190 );
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( it->first == 190 );
            CPPUNIT_ASSERT( it->second.m_val == 91 );

            it = m.find( 151 );
            CPPUNIT_ASSERT( it != m.end() );
            CPPUNIT_ASSERT( it->first == 151 );
            CPPUNIT_ASSERT( it->second.m_val == 0 );
#       endif

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

            // get_min test
            for ( int i = 500; i > 0; --i ) {
                CPPUNIT_ASSERT( m.insert( i, i * 2 ) != m.end() );

                typename Map::value_type * pVal = m.get_min();
                CPPUNIT_ASSERT( pVal != nullptr );
                CPPUNIT_CHECK( pVal->first == i );
                CPPUNIT_CHECK( pVal->second.m_val == i * 2 );
            }
            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));
            CPPUNIT_CHECK( m.get_min() == nullptr );
            CPPUNIT_CHECK( m.get_max() == nullptr );

            // iterator test

            for ( int i = 0; i < 500; ++i ) {
                CPPUNIT_ASSERT( m.insert( i, i * 2 ) != m.end() );

                typename Map::value_type * pVal = m.get_max();
                CPPUNIT_ASSERT( pVal != nullptr );
                CPPUNIT_CHECK( pVal->first == i );
                CPPUNIT_CHECK( pVal->second.m_val == i * 2 );
            }
            CPPUNIT_ASSERT( check_size( m, 500 ));

            for ( iterator it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first * 2 == (*it).second.m_val );
                it->second = it->first;
            }

            Map const& refMap = m;
            for ( const_iterator it = refMap.begin(), itEnd = refMap.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first == it->second.m_val );
                CPPUNIT_ASSERT( (*it).first == (*it).second.m_val );
            }

            CPPUNIT_MSG( PrintStat()(m, "SkipListMap statistics") );
        }

    public:
        void SkipList_HP_less();
        void SkipList_HP_cmp();
        void SkipList_HP_cmpless();
        void SkipList_HP_less_stat();
        void SkipList_HP_cmp_stat();
        void SkipList_HP_cmpless_stat();
        void SkipList_HP_xorshift_less();
        void SkipList_HP_xorshift_cmp();
        void SkipList_HP_xorshift_cmpless();
        void SkipList_HP_xorshift_less_stat();
        void SkipList_HP_xorshift_cmp_stat();
        void SkipList_HP_xorshift_cmpless_stat();
        void SkipList_HP_turbopas_less();
        void SkipList_HP_turbopas_cmp();
        void SkipList_HP_turbopas_cmpless();
        void SkipList_HP_turbopas_less_stat();
        void SkipList_HP_turbopas_cmp_stat();
        void SkipList_HP_turbopas_cmpless_stat();
        void SkipList_HP_michaelalloc_less();
        void SkipList_HP_michaelalloc_cmp();
        void SkipList_HP_michaelalloc_cmpless();
        void SkipList_HP_michaelalloc_less_stat();
        void SkipList_HP_michaelalloc_cmp_stat();
        void SkipList_HP_michaelalloc_cmpless_stat();

        void SkipList_HRC_less();
        void SkipList_HRC_cmp();
        void SkipList_HRC_cmpless();
        void SkipList_HRC_less_stat();
        void SkipList_HRC_cmp_stat();
        void SkipList_HRC_cmpless_stat();
        void SkipList_HRC_xorshift_less();
        void SkipList_HRC_xorshift_cmp();
        void SkipList_HRC_xorshift_cmpless();
        void SkipList_HRC_xorshift_less_stat();
        void SkipList_HRC_xorshift_cmp_stat();
        void SkipList_HRC_xorshift_cmpless_stat();
        void SkipList_HRC_turbopas_less();
        void SkipList_HRC_turbopas_cmp();
        void SkipList_HRC_turbopas_cmpless();
        void SkipList_HRC_turbopas_less_stat();
        void SkipList_HRC_turbopas_cmp_stat();
        void SkipList_HRC_turbopas_cmpless_stat();
        void SkipList_HRC_michaelalloc_less();
        void SkipList_HRC_michaelalloc_cmp();
        void SkipList_HRC_michaelalloc_cmpless();
        void SkipList_HRC_michaelalloc_less_stat();
        void SkipList_HRC_michaelalloc_cmp_stat();
        void SkipList_HRC_michaelalloc_cmpless_stat();

        void SkipList_PTB_less();
        void SkipList_PTB_cmp();
        void SkipList_PTB_cmpless();
        void SkipList_PTB_less_stat();
        void SkipList_PTB_cmp_stat();
        void SkipList_PTB_cmpless_stat();
        void SkipList_PTB_xorshift_less();
        void SkipList_PTB_xorshift_cmp();
        void SkipList_PTB_xorshift_cmpless();
        void SkipList_PTB_xorshift_less_stat();
        void SkipList_PTB_xorshift_cmp_stat();
        void SkipList_PTB_xorshift_cmpless_stat();
        void SkipList_PTB_turbopas_less();
        void SkipList_PTB_turbopas_cmp();
        void SkipList_PTB_turbopas_cmpless();
        void SkipList_PTB_turbopas_less_stat();
        void SkipList_PTB_turbopas_cmp_stat();
        void SkipList_PTB_turbopas_cmpless_stat();
        void SkipList_PTB_michaelalloc_less();
        void SkipList_PTB_michaelalloc_cmp();
        void SkipList_PTB_michaelalloc_cmpless();
        void SkipList_PTB_michaelalloc_less_stat();
        void SkipList_PTB_michaelalloc_cmp_stat();
        void SkipList_PTB_michaelalloc_cmpless_stat();

        void SkipList_NOGC_less();
        void SkipList_NOGC_cmp();
        void SkipList_NOGC_cmpless();
        void SkipList_NOGC_less_stat();
        void SkipList_NOGC_cmp_stat();
        void SkipList_NOGC_cmpless_stat();
        void SkipList_NOGC_xorshift_less();
        void SkipList_NOGC_xorshift_cmp();
        void SkipList_NOGC_xorshift_cmpless();
        void SkipList_NOGC_xorshift_less_stat();
        void SkipList_NOGC_xorshift_cmp_stat();
        void SkipList_NOGC_xorshift_cmpless_stat();
        void SkipList_NOGC_turbopas_less();
        void SkipList_NOGC_turbopas_cmp();
        void SkipList_NOGC_turbopas_cmpless();
        void SkipList_NOGC_turbopas_less_stat();
        void SkipList_NOGC_turbopas_cmp_stat();
        void SkipList_NOGC_turbopas_cmpless_stat();
        void SkipList_NOGC_michaelalloc_less();
        void SkipList_NOGC_michaelalloc_cmp();
        void SkipList_NOGC_michaelalloc_cmpless();
        void SkipList_NOGC_michaelalloc_less_stat();
        void SkipList_NOGC_michaelalloc_cmp_stat();
        void SkipList_NOGC_michaelalloc_cmpless_stat();

        CPPUNIT_TEST_SUITE(SkipListMapHdrTest)
            CPPUNIT_TEST(SkipList_HP_less)
            CPPUNIT_TEST(SkipList_HP_cmp)
            CPPUNIT_TEST(SkipList_HP_cmpless)
            CPPUNIT_TEST(SkipList_HP_less_stat)
            CPPUNIT_TEST(SkipList_HP_cmp_stat)
            CPPUNIT_TEST(SkipList_HP_cmpless_stat)
            CPPUNIT_TEST(SkipList_HP_xorshift_less)
            CPPUNIT_TEST(SkipList_HP_xorshift_cmp)
            CPPUNIT_TEST(SkipList_HP_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_HP_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_HP_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_HP_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_HP_turbopas_less)
            CPPUNIT_TEST(SkipList_HP_turbopas_cmp)
            CPPUNIT_TEST(SkipList_HP_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_HP_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_HP_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_HP_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_less)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_HRC_less)
            CPPUNIT_TEST(SkipList_HRC_cmp)
            CPPUNIT_TEST(SkipList_HRC_cmpless)
            CPPUNIT_TEST(SkipList_HRC_less_stat)
            CPPUNIT_TEST(SkipList_HRC_cmp_stat)
            CPPUNIT_TEST(SkipList_HRC_cmpless_stat)
            CPPUNIT_TEST(SkipList_HRC_xorshift_less)
            CPPUNIT_TEST(SkipList_HRC_xorshift_cmp)
            CPPUNIT_TEST(SkipList_HRC_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_HRC_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_HRC_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_HRC_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_HRC_turbopas_less)
            CPPUNIT_TEST(SkipList_HRC_turbopas_cmp)
            CPPUNIT_TEST(SkipList_HRC_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_HRC_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_HRC_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_HRC_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_HRC_michaelalloc_less)
            CPPUNIT_TEST(SkipList_HRC_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_HRC_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_HRC_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_HRC_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_HRC_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_PTB_less)
            CPPUNIT_TEST(SkipList_PTB_cmp)
            CPPUNIT_TEST(SkipList_PTB_cmpless)
            CPPUNIT_TEST(SkipList_PTB_less_stat)
            CPPUNIT_TEST(SkipList_PTB_cmp_stat)
            CPPUNIT_TEST(SkipList_PTB_cmpless_stat)
            CPPUNIT_TEST(SkipList_PTB_xorshift_less)
            CPPUNIT_TEST(SkipList_PTB_xorshift_cmp)
            CPPUNIT_TEST(SkipList_PTB_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_PTB_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_PTB_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_PTB_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_PTB_turbopas_less)
            CPPUNIT_TEST(SkipList_PTB_turbopas_cmp)
            CPPUNIT_TEST(SkipList_PTB_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_PTB_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_PTB_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_PTB_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_PTB_michaelalloc_less)
            CPPUNIT_TEST(SkipList_PTB_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_PTB_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_PTB_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_PTB_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_PTB_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_NOGC_less)
            CPPUNIT_TEST(SkipList_NOGC_cmp)
            CPPUNIT_TEST(SkipList_NOGC_cmpless)
            CPPUNIT_TEST(SkipList_NOGC_less_stat)
            CPPUNIT_TEST(SkipList_NOGC_cmp_stat)
            CPPUNIT_TEST(SkipList_NOGC_cmpless_stat)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_less)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_cmp)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_less)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_cmp)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_less)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_cmpless_stat)

        CPPUNIT_TEST_SUITE_END()

    };

} // namespace map

#endif // #ifndef __CDSTEST_HDR_SKIPLIST_MAP_H

//$$CDS-header$$

#ifndef CDSTEST_HDR_SKIPLIST_MAP_RCU_H
#define CDSTEST_HDR_SKIPLIST_MAP_RCU_H

#include "map/hdr_map.h"

namespace map {

    class SkipListMapRCUHdrTest: public HashMapHdrTest
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
            typedef typename Map::gc::scoped_lock   rcu_lock;

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
            {
                rcu_lock sl;
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
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            {
                rcu_lock sl;
                for ( const_set_iterator it = m.cbegin(), itEnd = m.cend(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).first * 2 == it->second.m_val );
                    ++nCount;
                    if ( it != m.cbegin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->first );
                    }
                    nPrevKey = it->first;
                }
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
            {
                rcu_lock sl;
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
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            {
                rcu_lock sl;
                for ( const_set_iterator it = m.cbegin(), itEnd = m.cend(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).first == it->second.m_val );
                    ++nCount;
                    if ( it != m.cbegin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->first );
                    }
                    nPrevKey = it->first;
                }
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
                shuffle( nRand, nRand + nLimit );

                for ( int i = 0; i < nLimit; ++i ) {
                    CPPUNIT_ASSERT( m.insert(i, i) );
                }
                CPPUNIT_MSG( PrintStat()(m, "Iterator test, random insert order") );
            }

            nCount = 0;
            nPrevKey = 0;
            {
                rcu_lock sl;
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
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            {
                rcu_lock sl;
                for ( const_set_iterator it = m.cbegin(), itEnd = m.cend(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).first * 2 == it->second.m_val );
                    ++nCount;
                    if ( it != m.cbegin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->first );
                    }
                    nPrevKey = it->first;
                }
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            {
                int arrItem[nLimit];
                for ( int i = 0; i < nLimit; ++i )
                    arrItem[i] = i;
                shuffle( arrItem, arrItem + nLimit );

                typedef typename Map::value_type value_type;
                typename Map::exempt_ptr ep;

                // extract/get
                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrItem[i];
                    {
                        rcu_lock l;
                        value_type * pVal = m.get( nKey );
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->first == nKey );
                        CPPUNIT_CHECK( pVal->second.m_val == nKey * 2 );
                    }

                    ep = m.extract( nKey );
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->first == nKey );
                    CPPUNIT_CHECK( ep->second.m_val == nKey * 2 );
                    ep.release();

                    {
                        rcu_lock l;
                        CPPUNIT_CHECK( m.get( nKey ) ==  nullptr );
                    }
                    ep = m.extract( nKey );
                    CPPUNIT_CHECK( !ep );
                }
                CPPUNIT_ASSERT( m.empty() );

                // extract_with/get_with
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( m.insert(arrItem[i], arrItem[i]*2) );

                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrItem[i];
                    {
                        rcu_lock l;
                        value_type * pVal = m.get_with( wrapped_item(nKey), wrapped_less() );
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->first == nKey );
                        CPPUNIT_CHECK( pVal->second.m_val == nKey * 2 );
                    }

                    ep = m.extract_with( wrapped_item( nKey ), wrapped_less() );
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->first == nKey );
                    CPPUNIT_CHECK( ep->second.m_val == nKey * 2 );
                    ep.release();

                    {
                        rcu_lock l;
                        CPPUNIT_CHECK( m.get_with( wrapped_item(nKey), wrapped_less() ) ==  nullptr );
                    }
                    ep = m.extract_with( wrapped_item( nKey ), wrapped_less() );
                    CPPUNIT_CHECK( !ep );
                }
                CPPUNIT_ASSERT( m.empty() );

                // extract_min
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( m.insert(arrItem[i], arrItem[i]*2) );
                for ( int i = 0; i < nLimit; ++i ) {
                    ep = m.extract_min();
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->first == i );
                    CPPUNIT_CHECK( ep->second.m_val == i * 2 );
                    ep.release();
                }
                CPPUNIT_ASSERT( m.empty() );
                ep = m.extract_min();
                CPPUNIT_CHECK( !ep );

                // extract_max
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( m.insert(arrItem[i], arrItem[i]*2) );
                for ( int i = nLimit-1; i >= 0; --i ) {
                    ep = m.extract_max();
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->first == i );
                    CPPUNIT_CHECK( ep->second.m_val == i * 2 );
                    ep.release();
                }
                CPPUNIT_ASSERT( m.empty() );
                ep = m.extract_max();
                CPPUNIT_CHECK( !ep );
            }

            CPPUNIT_MSG( PrintStat()(m, nullptr) );
        }

    public:
        void SkipList_RCU_GPI_less();
        void SkipList_RCU_GPI_cmp();
        void SkipList_RCU_GPI_cmpless();
        void SkipList_RCU_GPI_less_stat();
        void SkipList_RCU_GPI_cmp_stat();
        void SkipList_RCU_GPI_cmpless_stat();
        void SkipList_RCU_GPI_xorshift_less();
        void SkipList_RCU_GPI_xorshift_cmp();
        void SkipList_RCU_GPI_xorshift_cmpless();
        void SkipList_RCU_GPI_xorshift_less_stat();
        void SkipList_RCU_GPI_xorshift_cmp_stat();
        void SkipList_RCU_GPI_xorshift_cmpless_stat();
        void SkipList_RCU_GPI_turbopas_less();
        void SkipList_RCU_GPI_turbopas_cmp();
        void SkipList_RCU_GPI_turbopas_cmpless();
        void SkipList_RCU_GPI_turbopas_less_stat();
        void SkipList_RCU_GPI_turbopas_cmp_stat();
        void SkipList_RCU_GPI_turbopas_cmpless_stat();
        void SkipList_RCU_GPI_michaelalloc_less();
        void SkipList_RCU_GPI_michaelalloc_cmp();
        void SkipList_RCU_GPI_michaelalloc_cmpless();
        void SkipList_RCU_GPI_michaelalloc_less_stat();
        void SkipList_RCU_GPI_michaelalloc_cmp_stat();
        void SkipList_RCU_GPI_michaelalloc_cmpless_stat();

        void SkipList_RCU_GPB_less();
        void SkipList_RCU_GPB_cmp();
        void SkipList_RCU_GPB_cmpless();
        void SkipList_RCU_GPB_less_stat();
        void SkipList_RCU_GPB_cmp_stat();
        void SkipList_RCU_GPB_cmpless_stat();
        void SkipList_RCU_GPB_xorshift_less();
        void SkipList_RCU_GPB_xorshift_cmp();
        void SkipList_RCU_GPB_xorshift_cmpless();
        void SkipList_RCU_GPB_xorshift_less_stat();
        void SkipList_RCU_GPB_xorshift_cmp_stat();
        void SkipList_RCU_GPB_xorshift_cmpless_stat();
        void SkipList_RCU_GPB_turbopas_less();
        void SkipList_RCU_GPB_turbopas_cmp();
        void SkipList_RCU_GPB_turbopas_cmpless();
        void SkipList_RCU_GPB_turbopas_less_stat();
        void SkipList_RCU_GPB_turbopas_cmp_stat();
        void SkipList_RCU_GPB_turbopas_cmpless_stat();
        void SkipList_RCU_GPB_michaelalloc_less();
        void SkipList_RCU_GPB_michaelalloc_cmp();
        void SkipList_RCU_GPB_michaelalloc_cmpless();
        void SkipList_RCU_GPB_michaelalloc_less_stat();
        void SkipList_RCU_GPB_michaelalloc_cmp_stat();
        void SkipList_RCU_GPB_michaelalloc_cmpless_stat();

        void SkipList_RCU_GPT_less();
        void SkipList_RCU_GPT_cmp();
        void SkipList_RCU_GPT_cmpless();
        void SkipList_RCU_GPT_less_stat();
        void SkipList_RCU_GPT_cmp_stat();
        void SkipList_RCU_GPT_cmpless_stat();
        void SkipList_RCU_GPT_xorshift_less();
        void SkipList_RCU_GPT_xorshift_cmp();
        void SkipList_RCU_GPT_xorshift_cmpless();
        void SkipList_RCU_GPT_xorshift_less_stat();
        void SkipList_RCU_GPT_xorshift_cmp_stat();
        void SkipList_RCU_GPT_xorshift_cmpless_stat();
        void SkipList_RCU_GPT_turbopas_less();
        void SkipList_RCU_GPT_turbopas_cmp();
        void SkipList_RCU_GPT_turbopas_cmpless();
        void SkipList_RCU_GPT_turbopas_less_stat();
        void SkipList_RCU_GPT_turbopas_cmp_stat();
        void SkipList_RCU_GPT_turbopas_cmpless_stat();
        void SkipList_RCU_GPT_michaelalloc_less();
        void SkipList_RCU_GPT_michaelalloc_cmp();
        void SkipList_RCU_GPT_michaelalloc_cmpless();
        void SkipList_RCU_GPT_michaelalloc_less_stat();
        void SkipList_RCU_GPT_michaelalloc_cmp_stat();
        void SkipList_RCU_GPT_michaelalloc_cmpless_stat();

        void SkipList_RCU_SHB_less();
        void SkipList_RCU_SHB_cmp();
        void SkipList_RCU_SHB_cmpless();
        void SkipList_RCU_SHB_less_stat();
        void SkipList_RCU_SHB_cmp_stat();
        void SkipList_RCU_SHB_cmpless_stat();
        void SkipList_RCU_SHB_xorshift_less();
        void SkipList_RCU_SHB_xorshift_cmp();
        void SkipList_RCU_SHB_xorshift_cmpless();
        void SkipList_RCU_SHB_xorshift_less_stat();
        void SkipList_RCU_SHB_xorshift_cmp_stat();
        void SkipList_RCU_SHB_xorshift_cmpless_stat();
        void SkipList_RCU_SHB_turbopas_less();
        void SkipList_RCU_SHB_turbopas_cmp();
        void SkipList_RCU_SHB_turbopas_cmpless();
        void SkipList_RCU_SHB_turbopas_less_stat();
        void SkipList_RCU_SHB_turbopas_cmp_stat();
        void SkipList_RCU_SHB_turbopas_cmpless_stat();
        void SkipList_RCU_SHB_michaelalloc_less();
        void SkipList_RCU_SHB_michaelalloc_cmp();
        void SkipList_RCU_SHB_michaelalloc_cmpless();
        void SkipList_RCU_SHB_michaelalloc_less_stat();
        void SkipList_RCU_SHB_michaelalloc_cmp_stat();
        void SkipList_RCU_SHB_michaelalloc_cmpless_stat();

        void SkipList_RCU_SHT_less();
        void SkipList_RCU_SHT_cmp();
        void SkipList_RCU_SHT_cmpless();
        void SkipList_RCU_SHT_less_stat();
        void SkipList_RCU_SHT_cmp_stat();
        void SkipList_RCU_SHT_cmpless_stat();
        void SkipList_RCU_SHT_xorshift_less();
        void SkipList_RCU_SHT_xorshift_cmp();
        void SkipList_RCU_SHT_xorshift_cmpless();
        void SkipList_RCU_SHT_xorshift_less_stat();
        void SkipList_RCU_SHT_xorshift_cmp_stat();
        void SkipList_RCU_SHT_xorshift_cmpless_stat();
        void SkipList_RCU_SHT_turbopas_less();
        void SkipList_RCU_SHT_turbopas_cmp();
        void SkipList_RCU_SHT_turbopas_cmpless();
        void SkipList_RCU_SHT_turbopas_less_stat();
        void SkipList_RCU_SHT_turbopas_cmp_stat();
        void SkipList_RCU_SHT_turbopas_cmpless_stat();
        void SkipList_RCU_SHT_michaelalloc_less();
        void SkipList_RCU_SHT_michaelalloc_cmp();
        void SkipList_RCU_SHT_michaelalloc_cmpless();
        void SkipList_RCU_SHT_michaelalloc_less_stat();
        void SkipList_RCU_SHT_michaelalloc_cmp_stat();
        void SkipList_RCU_SHT_michaelalloc_cmpless_stat();

        CPPUNIT_TEST_SUITE(SkipListMapRCUHdrTest)
            CPPUNIT_TEST(SkipList_RCU_GPI_less)
            CPPUNIT_TEST(SkipList_RCU_GPI_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPI_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPI_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_RCU_GPB_less)
            CPPUNIT_TEST(SkipList_RCU_GPB_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPB_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPB_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_RCU_GPT_less)
            CPPUNIT_TEST(SkipList_RCU_GPT_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPT_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPT_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_RCU_SHB_less)
            CPPUNIT_TEST(SkipList_RCU_SHB_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHB_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHB_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_RCU_SHT_less)
            CPPUNIT_TEST(SkipList_RCU_SHT_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHT_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHT_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_cmpless_stat)

        CPPUNIT_TEST_SUITE_END()

    };

} // namespace map

#endif // #ifndef CDSTEST_HDR_SKIPLIST_MAP_RCU_H

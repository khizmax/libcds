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

            stat_data()
                : nInsertFuncCall( 0 )
                , nEnsureExistFuncCall( 0 )
                , nEnsureNewFuncCall( 0 )
                , nEraseFuncCall( 0 )
                , nFindFuncCall( 0 )
            {}
        };

        struct value_type {
            int         nVal;
            stat_data   stat;

            value_type()
                : nVal(0)
            {}

            value_type( int v )
                : nVal( v )
            {}
        };

        struct compare {
            int operator()( key_type k1, key_type k2 )
            {
                return k1 < k2 ? -1 : k1 > k2 ? 1 : 0;
            }
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

        struct find_functor
        {
            void operator()( key_type, value_type& v ) const
            {
                ++v.stat.nFindFuncCall;
            }
        };

        template <typename Item>
        struct copy_found
        {
            Item    m_found;

            void operator()( key_type const&, Item& v )
            {
                m_found = v;
            }

            void operator()( Item& v )
            {
                m_found = v;
            }
        };

        struct insert_functor
        {
            template <typename Item>
            void operator()( key_type key, Item& i )
            {
                i.nVal = key * 100;
                ++i.stat.nInsertFuncCall;
            }
        };

        template <typename Q>
        static void ensure_func( bool bNew, Q key, value_type& i )
        {
            i.nVal = key * 100;
            if ( bNew )
                ++i.stat.nEnsureNewFuncCall;
            else
                ++i.stat.nEnsureExistFuncCall;
        }

        struct ensure_functor
        {
            template <typename Q>
            void operator()( bool bNew, Q key, value_type& i )
            {
                ensure_func( bNew, key, i );
            }
        };

        struct check_functor
        {
            void operator()( size_t nLevel, size_t hLeft, size_t hRight )
            {
                CPPUNIT_MSG("Consistency violation: level=" << nLevel << ", hLeft=" << hLeft << ", hRight=" << hRight );
            }
        };

    protected:
        template <class Set>
        void test_with( Set& s )
        {
            value_type itm;
            int key;
            typedef typename Set::exempt_ptr exempt_ptr;

            // insert/find test
            CPPUNIT_ASSERT( !s.find( 10 ) );
            CPPUNIT_ASSERT( s.insert( 10 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ) );
            CPPUNIT_ASSERT( s.find( 10 ) );

            CPPUNIT_ASSERT( !s.insert( 10 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ) );

            CPPUNIT_ASSERT( !s.find_with( 20, std::less<key_type>() ) );
            CPPUNIT_ASSERT( s.insert( 20, 25 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ) );
            CPPUNIT_ASSERT( s.find_with( 10, std::less<key_type>() ) );
            CPPUNIT_ASSERT( s.find( key = 20 ) );
            CPPUNIT_ASSERT( s.find_with( key, std::less<key_type>(), find_functor() ) );
            {
                copy_found<value_type> f;
                key = 20;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.stat.nFindFuncCall == 1 );
            }
            CPPUNIT_ASSERT( s.find( key, find_functor() ) );
            {
                copy_found<value_type> f;
                key = 20;
                CPPUNIT_ASSERT( s.find_with( key, std::less<key_type>(), std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.stat.nFindFuncCall == 2 );
            }
            CPPUNIT_ASSERT( s.find( 20, find_functor() ) );
            {
                copy_found<value_type> f;
                CPPUNIT_ASSERT( s.find_with( 20, std::less<key_type>(), std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.stat.nFindFuncCall == 3 );
            }

            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ) );

            CPPUNIT_ASSERT( !s.find( 25 ) );
            CPPUNIT_ASSERT( s.insert_with( 25, insert_functor() ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ) );
            {
                copy_found<value_type> f;
                key = 25;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 2500 );
                CPPUNIT_ASSERT( f.m_found.stat.nInsertFuncCall == 1 );
            }

            // update test
            key = 10;
            {
                copy_found<value_type> f;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 0 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureExistFuncCall == 0 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureNewFuncCall == 0 );
            }
            std::pair<bool, bool> ensureResult = s.update( key, ensure_functor() );
            CPPUNIT_ASSERT( ensureResult.first && !ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ) );
            {
                copy_found<value_type> f;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 1000 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureExistFuncCall == 1 );
                CPPUNIT_ASSERT( f.m_found.stat.nEnsureNewFuncCall == 0 );
            }

            ensureResult = s.update( 13, []( bool /*bNew*/, key_type key, value_type& v ) 
                { 
                    v.nVal = key * 1000; 
                    ++v.stat.nEnsureNewFuncCall; 
                });
            CPPUNIT_ASSERT( ensureResult.first && ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 4 ) );
            {
                copy_found<value_type> f;
                key = 13;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 13000 );
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
            CPPUNIT_ASSERT( s.erase_with( 10, std::less<key_type>() ) );
            CPPUNIT_ASSERT( !s.find( 10 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ) );
            CPPUNIT_ASSERT( !s.erase_with( 10, std::less<key_type>() ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ) );

            CPPUNIT_ASSERT( s.find( 20 ) );
            {
                copy_found<value_type> f;
                CPPUNIT_ASSERT( s.erase( 20, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );

                CPPUNIT_ASSERT( s.insert( 235, 2350 ) );
                CPPUNIT_ASSERT( s.erase_with( 235, std::less<key_type>(), std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 2350 );
            }
            CPPUNIT_ASSERT( !s.find( 20 ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ) );

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ) );

            // emplace test
            CPPUNIT_ASSERT( s.emplace( 151 ) );  // key = 151, val=0
            CPPUNIT_ASSERT( s.emplace( 174, 471 ) );    // key = 174, val = 471
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ) );

            CPPUNIT_ASSERT( s.find( 151 ) );
            CPPUNIT_ASSERT( s.find_with( 174, std::less<key_type>() ) );
            CPPUNIT_ASSERT( !s.find( 190 ) );

            {
                copy_found<value_type> f;
                key = 151;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 0 );

                key = 174;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nVal == 471 );
            }

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ) );

            const int c_nStep = 10;
            int keys[1000];
            for ( key_type i = 0; i < static_cast<key_type>(sizeof(keys) / sizeof(keys[0])); ++i )
                keys[i] = i;
            shuffle( keys, keys + sizeof(keys) / sizeof(keys[0]));

            size_t nCount = 1;
            int nPrev;
            key_type keyPrev;
            exempt_ptr xp;

            // extract_min
            for ( int i = 0; i < static_cast<int>(sizeof(keys) / sizeof(keys[0])); ++i )
                CPPUNIT_ASSERT( s.emplace( keys[i], keys[i] * c_nStep ));
            CPPUNIT_CHECK( s.check_consistency( check_functor() ));

            xp = s.extract_min();
            CPPUNIT_ASSERT( xp );
            nPrev = xp->nVal;
            CPPUNIT_CHECK_EX( nPrev == 0, "Expected=0 real=" << nPrev );
            while ( !s.empty() ) {
                xp = s.extract_min();
                CPPUNIT_ASSERT( xp );
                CPPUNIT_CHECK_EX( nPrev + c_nStep == xp->nVal, "Expected=" << nPrev + c_nStep << " real=" << xp->nVal );
                nPrev = xp->nVal;
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == sizeof(keys) / sizeof(keys[0]));

            // extract_min<Func>
            for ( int i = 0; i < static_cast<int>(sizeof(keys) / sizeof(keys[0])); ++i )
                CPPUNIT_ASSERT( s.insert( keys[i], keys[i] * c_nStep ));
            CPPUNIT_CHECK( s.check_consistency( check_functor() ));

            nCount = 1;
            xp = s.extract_min( [&keyPrev]( key_type k ){ keyPrev = k; });
            CPPUNIT_ASSERT( xp );
            nPrev = xp->nVal;
            CPPUNIT_CHECK_EX( keyPrev == 0, "Expected=0 real=" << keyPrev );
            CPPUNIT_CHECK_EX( nPrev == 0, "Expected=0 real=" << nPrev );
            while ( !s.empty() ) {
                xp = s.extract_min( [&key](key_type k){ key = k; } );
                CPPUNIT_ASSERT( xp );
                CPPUNIT_CHECK_EX( key == keyPrev + 1, "Expected=" << keyPrev + 1 << " real=" << key );
                CPPUNIT_CHECK_EX( nPrev + c_nStep == xp->nVal, "Expected=" << nPrev + c_nStep << " real=" << xp->nVal );
                nPrev = xp->nVal;
                ++keyPrev;
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == sizeof(keys) / sizeof(keys[0]));

            // extract_min_key
            for ( int i = 0; i < static_cast<int>(sizeof(keys) / sizeof(keys[0])); ++i )
                CPPUNIT_ASSERT( s.insert( keys[i], keys[i] * c_nStep ));
            CPPUNIT_CHECK( s.check_consistency( check_functor() ));

            nCount = 1;
            xp = s.extract_min_key( keyPrev );
            CPPUNIT_ASSERT( xp );
            nPrev = xp->nVal;
            CPPUNIT_CHECK_EX( keyPrev == 0, "Expected=0 real=" << keyPrev );
            CPPUNIT_CHECK_EX( nPrev == 0, "Expected=0 real=" << nPrev );
            while ( !s.empty() ) {
                xp = s.extract_min_key( key );
                CPPUNIT_ASSERT( xp );
                CPPUNIT_CHECK_EX( key == keyPrev + 1, "Expected=" << keyPrev + 1 << " real=" << key );
                CPPUNIT_CHECK_EX( nPrev + c_nStep == xp->nVal, "Expected=" << nPrev + c_nStep << " real=" << xp->nVal );
                nPrev = xp->nVal;
                ++keyPrev;
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == sizeof(keys) / sizeof(keys[0]));

            // extract_max
            for ( int i = 0; i < static_cast<int>(sizeof(keys) / sizeof(keys[0])); ++i )
                CPPUNIT_ASSERT( s.emplace( keys[i], keys[i] * c_nStep ));
            CPPUNIT_CHECK( s.check_consistency( check_functor() ));

            nCount = 1;
            xp = s.extract_max();
            CPPUNIT_ASSERT( xp );
            nPrev = xp->nVal;
            CPPUNIT_CHECK_EX( nPrev == c_nStep * (sizeof(keys) / sizeof(keys[0]) - 1), 
                "Expected=" << c_nStep * (sizeof(keys) / sizeof(keys[0]) - 1) << " real=" << nPrev );
            while ( !s.empty() ) {
                xp = s.extract_max();
                CPPUNIT_ASSERT( xp );
                CPPUNIT_CHECK_EX( nPrev - c_nStep == xp->nVal, "Expected=" << nPrev - c_nStep << " real=" << xp->nVal );
                nPrev = xp->nVal;
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == sizeof(keys) / sizeof(keys[0]));

            // extract_max<Func>
            for ( int i = 0; i < static_cast<int>(sizeof(keys) / sizeof(keys[0])); ++i )
                CPPUNIT_ASSERT( s.emplace( keys[i], keys[i] * c_nStep ));
            CPPUNIT_CHECK( s.check_consistency( check_functor() ));

            nCount = 1;
            xp = s.extract_max( [&keyPrev]( key_type k ){ keyPrev = k; });
            CPPUNIT_ASSERT( xp );
            nPrev = xp->nVal;
            CPPUNIT_CHECK_EX( keyPrev == sizeof(keys) / sizeof(keys[0]) - 1, 
                "Expected=" << sizeof(keys) / sizeof(keys[0]) - 1 << " real=" << keyPrev );
            CPPUNIT_CHECK_EX( nPrev == c_nStep * (sizeof(keys) / sizeof(keys[0]) - 1), 
                "Expected=" << c_nStep * (sizeof(keys) / sizeof(keys[0]) - 1) << " real=" << nPrev );
            while ( !s.empty() ) {
                xp = s.extract_max( [&key](key_type k){ key = k; });
                CPPUNIT_ASSERT( xp );
                CPPUNIT_CHECK_EX( key == keyPrev - 1, "Expected=" << keyPrev - 1 << " real=" << key );
                CPPUNIT_CHECK_EX( nPrev - c_nStep == xp->nVal, "Expected=" << nPrev - c_nStep << " real=" << xp->nVal );
                nPrev = xp->nVal;
                --keyPrev;
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == sizeof(keys) / sizeof(keys[0]));

            // extract_max_key
            for ( int i = 0; i < static_cast<int>(sizeof(keys) / sizeof(keys[0])); ++i )
                CPPUNIT_ASSERT( s.emplace( keys[i], keys[i] * c_nStep ));
            CPPUNIT_CHECK( s.check_consistency( check_functor() ));

            nCount = 1;
            xp = s.extract_max_key( keyPrev );
            CPPUNIT_ASSERT( xp );
            nPrev = xp->nVal;
            CPPUNIT_CHECK_EX( keyPrev == sizeof(keys) / sizeof(keys[0]) - 1, 
                "Expected=" << sizeof(keys) / sizeof(keys[0]) - 1 << " real=" << keyPrev );
            CPPUNIT_CHECK_EX( nPrev == c_nStep * (sizeof(keys) / sizeof(keys[0]) - 1), 
                "Expected=" << c_nStep * (sizeof(keys) / sizeof(keys[0]) - 1) << " real=" << nPrev );
            while ( !s.empty() ) {
                xp = s.extract_max_key( key );
                CPPUNIT_ASSERT( xp );
                CPPUNIT_CHECK_EX( key == keyPrev - 1, "Expected=" << keyPrev - 1 << " real=" << key );
                CPPUNIT_CHECK_EX( nPrev - c_nStep == xp->nVal, "Expected=" << nPrev - c_nStep << " real=" << xp->nVal );
                nPrev = xp->nVal;
                --keyPrev;
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == sizeof(keys) / sizeof(keys[0]));

            // extract
            for ( int i = 0; i < static_cast<int>(sizeof(keys) / sizeof(keys[0])); ++i )
                CPPUNIT_ASSERT( s.emplace( keys[i], keys[i] * c_nStep ));
            CPPUNIT_CHECK( s.check_consistency( check_functor() ));

            for ( int i = 0; i < static_cast<int>(sizeof( keys ) / sizeof( keys[0] )); ++i ) {
                xp = s.extract(keys[i]);
                CPPUNIT_CHECK_EX( xp->nVal == keys[i] * c_nStep, "Expected value=" << keys[i] * c_nStep << " real=" << xp->nVal );
            }
            CPPUNIT_ASSERT(s.empty());


            // extract_with
            for ( int i = 0; i < static_cast<int>(sizeof(keys) / sizeof(keys[0])); ++i )
                CPPUNIT_ASSERT( s.emplace( keys[i], keys[i] * c_nStep ));
            CPPUNIT_CHECK( s.check_consistency( check_functor() ));

            for ( int i = 0; i < static_cast<int>(sizeof( keys ) / sizeof( keys[0] )); ++i ) {
                xp = s.extract_with( wrapped_int(keys[i]), wrapped_less());
                CPPUNIT_CHECK_EX( xp->nVal == keys[i] * c_nStep, "Expected value=" << keys[i] * c_nStep << " real=" << xp->nVal );
            }
            CPPUNIT_ASSERT(s.empty());
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

        void BronsonAVLTree_rcu_gpi_less();
        void BronsonAVLTree_rcu_gpi_less_stat();
        void BronsonAVLTree_rcu_gpi_cmp();
        void BronsonAVLTree_rcu_gpi_cmp_stat();
        void BronsonAVLTree_rcu_gpi_cmpless();
        void BronsonAVLTree_rcu_gpi_less_ic();
        void BronsonAVLTree_rcu_gpi_cmp_ic();
        void BronsonAVLTree_rcu_gpi_cmp_ic_stat();
        void BronsonAVLTree_rcu_gpi_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_gpi_less_relaxed_insert();
        void BronsonAVLTree_rcu_gpi_less_relaxed_insert_stat();
        void BronsonAVLTree_rcu_gpi_pool_monitor_less();
        void BronsonAVLTree_rcu_gpi_pool_monitor_less_stat();
        void BronsonAVLTree_rcu_gpi_pool_monitor_cmp_ic_stat();
        void BronsonAVLTree_rcu_gpi_pool_monitor_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_gpi_pool_monitor_less_relaxed_insert();
        void BronsonAVLTree_rcu_gpi_pool_monitor_less_relaxed_insert_stat();

        void BronsonAVLTree_rcu_gpb_less();
        void BronsonAVLTree_rcu_gpb_less_stat();
        void BronsonAVLTree_rcu_gpb_cmp();
        void BronsonAVLTree_rcu_gpb_cmp_stat();
        void BronsonAVLTree_rcu_gpb_cmpless();
        void BronsonAVLTree_rcu_gpb_less_ic();
        void BronsonAVLTree_rcu_gpb_cmp_ic();
        void BronsonAVLTree_rcu_gpb_cmp_ic_stat();
        void BronsonAVLTree_rcu_gpb_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_gpb_less_relaxed_insert();
        void BronsonAVLTree_rcu_gpb_less_relaxed_insert_stat();
        void BronsonAVLTree_rcu_gpb_pool_monitor_less();
        void BronsonAVLTree_rcu_gpb_pool_monitor_less_stat();
        void BronsonAVLTree_rcu_gpb_pool_monitor_cmp_ic_stat();
        void BronsonAVLTree_rcu_gpb_pool_monitor_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_gpb_pool_monitor_less_relaxed_insert();
        void BronsonAVLTree_rcu_gpb_pool_monitor_less_relaxed_insert_stat();

        void BronsonAVLTree_rcu_gpt_less();
        void BronsonAVLTree_rcu_gpt_less_stat();
        void BronsonAVLTree_rcu_gpt_cmp();
        void BronsonAVLTree_rcu_gpt_cmp_stat();
        void BronsonAVLTree_rcu_gpt_cmpless();
        void BronsonAVLTree_rcu_gpt_less_ic();
        void BronsonAVLTree_rcu_gpt_cmp_ic();
        void BronsonAVLTree_rcu_gpt_cmp_ic_stat();
        void BronsonAVLTree_rcu_gpt_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_gpt_less_relaxed_insert();
        void BronsonAVLTree_rcu_gpt_less_relaxed_insert_stat();
        void BronsonAVLTree_rcu_gpt_pool_monitor_less();
        void BronsonAVLTree_rcu_gpt_pool_monitor_less_stat();
        void BronsonAVLTree_rcu_gpt_pool_monitor_cmp_ic_stat();
        void BronsonAVLTree_rcu_gpt_pool_monitor_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_gpt_pool_monitor_less_relaxed_insert();
        void BronsonAVLTree_rcu_gpt_pool_monitor_less_relaxed_insert_stat();

        void BronsonAVLTree_rcu_shb_less();
        void BronsonAVLTree_rcu_shb_less_stat();
        void BronsonAVLTree_rcu_shb_cmp();
        void BronsonAVLTree_rcu_shb_cmp_stat();
        void BronsonAVLTree_rcu_shb_cmpless();
        void BronsonAVLTree_rcu_shb_less_ic();
        void BronsonAVLTree_rcu_shb_cmp_ic();
        void BronsonAVLTree_rcu_shb_cmp_ic_stat();
        void BronsonAVLTree_rcu_shb_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_shb_less_relaxed_insert();
        void BronsonAVLTree_rcu_shb_less_relaxed_insert_stat();
        void BronsonAVLTree_rcu_shb_pool_monitor_less();
        void BronsonAVLTree_rcu_shb_pool_monitor_less_stat();
        void BronsonAVLTree_rcu_shb_pool_monitor_cmp_ic_stat();
        void BronsonAVLTree_rcu_shb_pool_monitor_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_shb_pool_monitor_less_relaxed_insert();
        void BronsonAVLTree_rcu_shb_pool_monitor_less_relaxed_insert_stat();

        void BronsonAVLTree_rcu_sht_less();
        void BronsonAVLTree_rcu_sht_less_stat();
        void BronsonAVLTree_rcu_sht_cmp();
        void BronsonAVLTree_rcu_sht_cmp_stat();
        void BronsonAVLTree_rcu_sht_cmpless();
        void BronsonAVLTree_rcu_sht_less_ic();
        void BronsonAVLTree_rcu_sht_cmp_ic();
        void BronsonAVLTree_rcu_sht_cmp_ic_stat();
        void BronsonAVLTree_rcu_sht_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_sht_less_relaxed_insert();
        void BronsonAVLTree_rcu_sht_less_relaxed_insert_stat();
        void BronsonAVLTree_rcu_sht_pool_monitor_less();
        void BronsonAVLTree_rcu_sht_pool_monitor_less_stat();
        void BronsonAVLTree_rcu_sht_pool_monitor_cmp_ic_stat();
        void BronsonAVLTree_rcu_sht_pool_monitor_cmp_ic_stat_yield();
        void BronsonAVLTree_rcu_sht_pool_monitor_less_relaxed_insert();
        void BronsonAVLTree_rcu_sht_pool_monitor_less_relaxed_insert_stat();

        CPPUNIT_TEST_SUITE( BronsonAVLTreeHdrTest )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_cmp )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_cmp_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_cmpless )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_less_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_cmp_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_less_relaxed_insert_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_pool_monitor_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_pool_monitor_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_pool_monitor_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_pool_monitor_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_pool_monitor_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpi_pool_monitor_less_relaxed_insert_stat )

            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmp )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmp_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmpless )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_less_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmp_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_less_relaxed_insert_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_pool_monitor_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_pool_monitor_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_pool_monitor_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_pool_monitor_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_pool_monitor_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpb_pool_monitor_less_relaxed_insert_stat )

            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_cmp )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_cmp_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_cmpless )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_less_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_cmp_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_less_relaxed_insert_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_pool_monitor_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_pool_monitor_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_pool_monitor_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_pool_monitor_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_pool_monitor_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_gpt_pool_monitor_less_relaxed_insert_stat )

            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_cmp )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_cmp_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_cmpless )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_less_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_cmp_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_less_relaxed_insert_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_pool_monitor_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_pool_monitor_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_pool_monitor_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_pool_monitor_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_pool_monitor_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_shb_pool_monitor_less_relaxed_insert_stat )

            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_cmp )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_cmp_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_cmpless )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_less_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_cmp_ic )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_less_relaxed_insert_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_pool_monitor_less )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_pool_monitor_less_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_pool_monitor_cmp_ic_stat )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_pool_monitor_cmp_ic_stat_yield )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_pool_monitor_less_relaxed_insert )
            CPPUNIT_TEST( BronsonAVLTree_rcu_sht_pool_monitor_less_relaxed_insert_stat )

        CPPUNIT_TEST_SUITE_END()
    };
} // namespace tree

#endif // #ifndef CDSTEST_HDR_BRONSON_AVLTREE_MAP_H

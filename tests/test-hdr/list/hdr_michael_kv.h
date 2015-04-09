//$$CDS-header$$

#ifndef CDSTEST_HDR_MICHAEL_KV_H
#define CDSTEST_HDR_MICHAEL_KV_H

#include "cppunit/cppunit_proxy.h"
#include <cds/container/details/michael_list_base.h>

namespace ordlist {
    namespace cc = cds::container;
    namespace co = cds::container::opt;

    class MichaelKVListTestHeader: public CppUnitMini::TestCase
    {
    public:
        typedef int key_type;
        struct value_type {
            int m_val;

            value_type()
                : m_val(0)
            {}

            value_type( int n )
                : m_val( n )
            {}
        };

        template <typename T>
        struct lt
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1 < v2;
            }
        };

        template <typename T>
        struct cmp {
            int operator ()(const T& v1, const T& v2 ) const
            {
                if ( v1 < v2 )
                    return -1;
                return v1 > v2 ? 1 : 0;
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
                CPPUNIT_ASSERT_CURRENT( pair.second.m_val == m_nExpected );
            }
        };

        struct insert_functor {
            template <typename T>
            void operator()( T& pair )
            {
                pair.second.m_val = pair.first * 10;
            }
        };

        struct ensure_functor {
            template <typename T>
            void operator()( bool /*bNew*/, T& pair )
            {
                pair.second.m_val = pair.first * 50;
            }
        };

        struct erase_functor {
            int     nKey;
            int     nVal;

            erase_functor()
                : nKey(0)
                , nVal(0)
            {}

            template <typename T>
            void operator()( T& i )
            {
                nKey = i.first;
                nVal = i.second.m_val;
            }
        };

        typedef float other_key;
        struct other_less {
            bool operator()( float f, int i ) const
            {
                return int(f) < i;
            }
            bool operator()( int i, float f ) const
            {
                return i < int(f);
            }
        };

    protected:
        template <class OrdList>
        void test_with( OrdList& l)
        {
            typedef typename OrdList::value_type    value_type;

            typename OrdList::iterator itTest;
            typename OrdList::const_iterator citTest;

            CPPUNIT_ASSERT( l.empty() );

            // insert / find test
            CPPUNIT_ASSERT( !l.find( 100 ));
            CPPUNIT_ASSERT( l.insert( 100 ));
            CPPUNIT_ASSERT( !l.empty() );
            CPPUNIT_ASSERT( l.find( 100 ));

            check_value chk(0);
            CPPUNIT_ASSERT( l.find( 100, std::ref( chk ) ) );

            CPPUNIT_ASSERT( !l.find_with( 50, lt<key_type>() ));
            CPPUNIT_ASSERT( l.insert( 50, 500 ));
            CPPUNIT_ASSERT( l.find_with( 50, lt<key_type>() ));
            CPPUNIT_ASSERT( !l.insert( 50, 5 ));
            chk.m_nExpected = 500;
            CPPUNIT_ASSERT( l.find_with( 50, lt<key_type>(), std::ref( chk ) ) );
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( l.find_with( 100, lt<key_type>(), std::ref( chk ) ) );
            CPPUNIT_ASSERT( !l.empty() );

            CPPUNIT_ASSERT( !l.find( 150 ));
            CPPUNIT_ASSERT( l.insert_with( 150, insert_functor() ));
            CPPUNIT_ASSERT( l.find( 150 ));
            chk.m_nExpected = 1500;
            CPPUNIT_ASSERT( l.find( 150, std::ref( chk ) ) );
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( l.find( 100, std::ref( chk ) ) );
            chk.m_nExpected = 500;
            CPPUNIT_ASSERT( l.find( 50, std::ref( chk ) ) );
            CPPUNIT_ASSERT( !l.empty() );

            // erase test

            CPPUNIT_ASSERT( !l.erase( 500 ));
            CPPUNIT_ASSERT( !l.empty() );

            CPPUNIT_ASSERT( l.find( 50 ));
            {
                erase_functor ef;
                l.erase( 50, std::ref( ef ) );
                CPPUNIT_ASSERT( ef.nKey == 50 );
                CPPUNIT_ASSERT( ef.nVal == 500 );
            }
            CPPUNIT_ASSERT( !l.find( 50 ));

            // ensure test
            std::pair<bool, bool> bEnsureResult;
            bEnsureResult = l.ensure( 100, ensure_functor() );
            CPPUNIT_ASSERT( bEnsureResult.first );
            CPPUNIT_ASSERT( !bEnsureResult.second );
            chk.m_nExpected = 5000;
            CPPUNIT_ASSERT( l.find( 100, std::ref( chk ) ) );

            {
                ensure_functor ef;
                bEnsureResult = l.ensure( 50, std::ref( ef ) );
            }
            CPPUNIT_ASSERT( bEnsureResult.first );
            CPPUNIT_ASSERT( bEnsureResult.second );
            chk.m_nExpected = 2500;
            CPPUNIT_ASSERT( l.find( 50, std::ref( chk ) ) );

            // erase test
            CPPUNIT_ASSERT( !l.empty() );
            CPPUNIT_ASSERT( l.insert_with( 200, insert_functor() ));
            CPPUNIT_ASSERT( l.insert( 25 ));
            CPPUNIT_ASSERT( l.erase( 100 ));
            CPPUNIT_ASSERT( l.erase( 150 ));
            {
                erase_functor ef;
                CPPUNIT_ASSERT( l.erase_with( 200, lt<key_type>(), std::ref(ef)) );
                CPPUNIT_ASSERT( ef.nKey == 200 );
                CPPUNIT_ASSERT( ef.nVal == 2000 );
            }
            CPPUNIT_ASSERT( l.erase_with( 25, lt<key_type>()))
            CPPUNIT_ASSERT( l.erase( 50 ));
            CPPUNIT_ASSERT( l.empty() );

            // clear empty list
            l.clear();
            CPPUNIT_ASSERT( l.empty() );

            // insert test
            CPPUNIT_ASSERT( l.emplace( 501 ) );
            CPPUNIT_ASSERT( l.emplace( 251, 152 ));

            // insert failed - such key exists
            CPPUNIT_ASSERT( !l.emplace( 501, 2 ) );
            CPPUNIT_ASSERT( !l.emplace( 251, 10) );

            check_value cv(0);
            CPPUNIT_ASSERT( l.find( 501, std::ref(cv) ));
            cv.m_nExpected = 152;
            CPPUNIT_ASSERT( l.find( 251, std::ref(cv) ));

            l.clear();
            CPPUNIT_ASSERT( l.empty() );

            // Iterator test
            {
                int nCount = 100;
                for ( int i = 0; i < nCount; ++i )
                    CPPUNIT_ASSERT( l.insert(i, i * 2 ) );

                {
                    typename OrdList::iterator it( l.begin() );
                    typename OrdList::const_iterator cit( l.cbegin() );
                    CPPUNIT_CHECK( it == cit );
                    CPPUNIT_CHECK( it != l.end() );
                    CPPUNIT_CHECK( it != l.cend() );
                    CPPUNIT_CHECK( cit != l.end() );
                    CPPUNIT_CHECK( cit != l.cend() );
                    ++it;
                    CPPUNIT_CHECK( it != cit );
                    CPPUNIT_CHECK( it != l.end() );
                    CPPUNIT_CHECK( it != l.cend() );
                    CPPUNIT_CHECK( cit != l.end() );
                    CPPUNIT_CHECK( cit != l.cend() );
                    ++cit;
                    CPPUNIT_CHECK( it == cit );
                    CPPUNIT_CHECK( it != l.end() );
                    CPPUNIT_CHECK( it != l.cend() );
                    CPPUNIT_CHECK( cit != l.end() );
                    CPPUNIT_CHECK( cit != l.cend() );
                }

                int i = 0;
                for ( typename OrdList::iterator it = l.begin(), itEnd = l.end(); it != itEnd; ++it, ++i ) {
                    CPPUNIT_ASSERT( it.key() == i );
                    CPPUNIT_ASSERT( it->first == i );
                    CPPUNIT_ASSERT( (*it).first == i );

                    CPPUNIT_ASSERT( it.val().m_val == i * 2 );
                    CPPUNIT_ASSERT( it->second.m_val == i * 2 );
                    CPPUNIT_ASSERT( (*it).second.m_val == i * 2 );
                    it.val().m_val = i * 3;
                }

                // Check that we have visited all items
                for ( int i = 0; i < nCount; ++i ) {
                    chk.m_nExpected = i * 3;
                    CPPUNIT_ASSERT( l.find( i, std::ref( chk ) ) );
                }

                l.clear();
                CPPUNIT_ASSERT( l.empty() );

                // Const iterator
                for ( int i = 0; i < nCount; ++i )
                    CPPUNIT_ASSERT( l.insert(i, i * 7) );

                i = 0;
                const OrdList& rl = l;
                for ( typename OrdList::const_iterator it = rl.begin(), itEnd = rl.end(); it != itEnd; ++it, ++i ) {
                    CPPUNIT_ASSERT( it.key() == i );
                    CPPUNIT_ASSERT( it->first == i );
                    CPPUNIT_ASSERT( (*it).first == i );

                    CPPUNIT_ASSERT( it.val().m_val == i * 7 );
                    CPPUNIT_ASSERT( it->second.m_val == i * 7 );
                    CPPUNIT_ASSERT( (*it).second.m_val == i * 7 );
                }

                // Check that we have visited all items
                for ( int i = 0; i < nCount; ++i ) {
                    chk.m_nExpected = i * 7;
                    CPPUNIT_ASSERT( l.find_with( i, lt<key_type>(), std::ref( chk ) ) );
                }

                l.clear();
                CPPUNIT_ASSERT( l.empty() );
            }
        }

        template <class OrdList>
        void test()
        {
            OrdList l;
            test_with(l);

            typedef typename OrdList::guarded_ptr guarded_ptr;

            static int const nLimit = 20;
            int arr[nLimit];
            for ( int i = 0; i < nLimit; i++ )
                arr[i] = i;
            shuffle( arr, arr + nLimit );

            // extract/get
            for ( int i = 0; i < nLimit; ++i )
                l.insert( arr[i], arr[i] * 2 );
            {
                guarded_ptr gp;
                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arr[i];

                    gp = l.get( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey * 2 );
                    gp.release();
                    CPPUNIT_CHECK( gp.empty() );

                    gp = l.extract( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey*2 );
                    gp.release();

                    gp = l.get( nKey );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_CHECK( gp.empty());
                    CPPUNIT_CHECK( !l.extract( nKey));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( l.empty());
                CPPUNIT_CHECK( !l.get(arr[0]));
                CPPUNIT_CHECK( gp.empty());
                CPPUNIT_CHECK( !l.extract( arr[0]));
                CPPUNIT_CHECK( gp.empty());
            }

            // extract_with/get_with
            for ( int i = 0; i < nLimit; ++i )
                l.insert( arr[i], arr[i] * 2 );
            {
                guarded_ptr gp;
                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arr[i];
                    other_key key = float(nKey + 0.3);

                    gp = l.get_with( key, other_less() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey * 2 );
                    gp.release();

                    gp = l.extract_with( key, other_less() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey*2 );
                    gp.release();

                    gp = l.get_with( key, other_less() );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_CHECK( gp.empty());
                    CPPUNIT_CHECK( !l.extract_with( key, other_less()));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( l.empty());
                CPPUNIT_CHECK( !l.get_with( 3.4f, other_less()));
                CPPUNIT_CHECK( gp.empty());
                CPPUNIT_CHECK( !l.extract_with( 3.4f, other_less()));
                CPPUNIT_CHECK( gp.empty());
            }
        }

        template <class OrdList>
        void test_rcu()
        {
            OrdList l;
            test_with(l);

            static int const nLimit = 20;

            typedef typename OrdList::rcu_lock rcu_lock;
            typedef typename OrdList::value_type value_type;
            typedef typename OrdList::gc rcu_type;

            {
                int a[nLimit];
                for (int i = 0; i < nLimit; ++i)
                    a[i]=i;
                shuffle( a, a + nLimit );

                // extract/get
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( l.insert( a[i], a[i]*2 ) );

                typename OrdList::exempt_ptr ep;

                for ( int i = 0; i < nLimit; ++i ) {
                    {
                        rcu_lock lock;
                        value_type * pGet = l.get( a[i] );
                        CPPUNIT_ASSERT( pGet != nullptr );
                        CPPUNIT_CHECK( pGet->first == a[i] );
                        CPPUNIT_CHECK( pGet->second.m_val == a[i] * 2 );

                        ep = l.extract( a[i] );
                        CPPUNIT_ASSERT( ep );
                        CPPUNIT_ASSERT( !ep.empty() );
                        CPPUNIT_CHECK( ep->first == a[i] );
                        CPPUNIT_CHECK( (*ep).second.m_val == a[i] * 2 );
                    }
                    ep.release();
                    {
                        rcu_lock lock;
                        CPPUNIT_CHECK( l.get( a[i] ) == nullptr );
                        ep = l.extract( a[i] );
                        CPPUNIT_CHECK( !ep );
                        CPPUNIT_CHECK( ep.empty() );
                    }
                }
                CPPUNIT_ASSERT( l.empty() );

                {
                    rcu_lock lock;
                    CPPUNIT_CHECK( l.get( a[0] ) == nullptr );
                    CPPUNIT_CHECK( !l.extract( a[0] ) );
                    CPPUNIT_CHECK( ep.empty() );
                }

                // extract_with/get_with
                for ( int i = 0; i < nLimit; ++i ) {
                    CPPUNIT_ASSERT( l.insert( a[i], a[i]*2 ) );
                }

                for ( int i = 0; i < nLimit; ++i ) {
                    float itm = a[i] + 0.3f;
                    {
                        rcu_lock lock;
                        value_type * pGet = l.get_with( itm, other_less() );
                        CPPUNIT_ASSERT( pGet != nullptr );
                        CPPUNIT_CHECK( pGet->first == a[i] );
                        CPPUNIT_CHECK( pGet->second.m_val == a[i] * 2 );

                        ep = l.extract_with( itm, other_less() );
                        CPPUNIT_ASSERT( ep );
                        CPPUNIT_ASSERT( !ep.empty() );
                        CPPUNIT_CHECK( ep->first == a[i] );
                        CPPUNIT_CHECK( ep->second.m_val == a[i] * 2 );
                    }
                    ep.release();
                    {
                        rcu_lock lock;
                        CPPUNIT_CHECK( l.get_with( itm, other_less() ) == nullptr );
                        ep = l.extract_with( itm, other_less() );
                        CPPUNIT_CHECK( !ep );
                        CPPUNIT_CHECK( ep.empty() );
                    }
                }
                CPPUNIT_ASSERT( l.empty() );

                {
                    rcu_lock lock;
                    CPPUNIT_CHECK( l.get_with( 3.14f, other_less() ) == nullptr );
                    CPPUNIT_CHECK( !l.extract_with( 3.14f, other_less() ));
                    CPPUNIT_CHECK( ep.empty() );
                }
            }

        }

        template <class OrdList>
        void nogc_test()
        {
            typedef typename OrdList::value_type    value_type;
            typedef typename OrdList::iterator      iterator;

            {
                OrdList l;
                iterator it;

                CPPUNIT_ASSERT( l.empty() );

                // insert / find test
                CPPUNIT_ASSERT( l.find( 100 ) == l.end() );
                CPPUNIT_ASSERT( l.insert( 100 ) != l.end() );
                CPPUNIT_ASSERT( !l.empty() );
                it = l.find_with( 100, lt<key_type>() );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 100 );
                CPPUNIT_ASSERT( it.val().m_val == 0 );

                CPPUNIT_ASSERT( l.find_with( 50, lt<key_type>() ) == l.end() );
                CPPUNIT_ASSERT( l.insert( 50, 500 ) != l.end());
                it = l.find( 50 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 50 );
                CPPUNIT_ASSERT( it.val().m_val == 500 );

                CPPUNIT_ASSERT( l.insert( 50, 5 ) == l.end() );
                it = l.find( 50 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 50 );
                CPPUNIT_ASSERT( it.val().m_val == 500 );
                CPPUNIT_ASSERT( !l.empty() );

                CPPUNIT_ASSERT( l.find( 150 ) == l.end() );
                CPPUNIT_ASSERT( l.insert_with( 150, insert_functor() ) != l.end() );
                it = l.find( 150 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 150 );
                CPPUNIT_ASSERT( it.val().m_val == 1500 );
                it = l.find( 100 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 100 );
                CPPUNIT_ASSERT( it.val().m_val == 0 );
                it = l.find( 50 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 50 );
                CPPUNIT_ASSERT( it.val().m_val == 500 );
                it.val().m_val = 25;
                it = l.find( 50 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 50 );
                CPPUNIT_ASSERT( it.val().m_val == 25 );
                CPPUNIT_ASSERT( !l.empty() );

                // ensure existing item
                std::pair<iterator, bool> ensureResult;
                ensureResult = l.ensure( 100 );
                CPPUNIT_ASSERT( !ensureResult.second );
                CPPUNIT_ASSERT( ensureResult.first.key() == 100 );
                CPPUNIT_ASSERT( ensureResult.first.val().m_val == 0   );
                ensureResult.first.val().m_val = 5;
                it = l.find( 100 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 100 );
                CPPUNIT_ASSERT( it.val().m_val == 5 );

                CPPUNIT_ASSERT( !l.empty() );

                // ensure new item
                ensureResult = l.ensure( 1000 );
                CPPUNIT_ASSERT( ensureResult.second );
                CPPUNIT_ASSERT( ensureResult.first.key() == 1000 );
                CPPUNIT_ASSERT( ensureResult.first.val().m_val == 0   );
                ensureResult.first.val().m_val = 33;
                ensureResult = l.ensure( 1000 );
                CPPUNIT_ASSERT( !ensureResult.second );
                CPPUNIT_ASSERT( ensureResult.first.key() == 1000 );
                CPPUNIT_ASSERT( ensureResult.first.val().m_val == 33   );

                // clear test
                l.clear();
                CPPUNIT_ASSERT( l.empty() );

                // insert test
                CPPUNIT_ASSERT( l.emplace( 501 ) != l.end());
                CPPUNIT_ASSERT( l.emplace( 251, 152 ) != l.end());

                // insert failed - such key exists
                CPPUNIT_ASSERT( l.emplace( 501, 2 ) == l.end());
                CPPUNIT_ASSERT( l.emplace( 251, 10) == l.end());

                it = l.find( 501 );
                CPPUNIT_ASSERT( it != l.end());
                CPPUNIT_ASSERT( it.key() == 501 );
                CPPUNIT_ASSERT( it.val().m_val == 0 );

                it = l.find( 251 );
                CPPUNIT_ASSERT( it != l.end());
                CPPUNIT_ASSERT( it.key() == 251 );
                CPPUNIT_ASSERT( it.val().m_val == 152 );

                l.clear();
                CPPUNIT_ASSERT( l.empty() );

                // Iterator test
                {
                    int nCount = 100;
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i, i * 2 ) != l.end() );

                    {
                        typename OrdList::iterator it( l.begin() );
                        typename OrdList::const_iterator cit( l.cbegin() );
                        CPPUNIT_CHECK( it == cit );
                        CPPUNIT_CHECK( it != l.end() );
                        CPPUNIT_CHECK( it != l.cend() );
                        CPPUNIT_CHECK( cit != l.end() );
                        CPPUNIT_CHECK( cit != l.cend() );
                        ++it;
                        CPPUNIT_CHECK( it != cit );
                        CPPUNIT_CHECK( it != l.end() );
                        CPPUNIT_CHECK( it != l.cend() );
                        CPPUNIT_CHECK( cit != l.end() );
                        CPPUNIT_CHECK( cit != l.cend() );
                        ++cit;
                        CPPUNIT_CHECK( it == cit );
                        CPPUNIT_CHECK( it != l.end() );
                        CPPUNIT_CHECK( it != l.cend() );
                        CPPUNIT_CHECK( cit != l.end() );
                        CPPUNIT_CHECK( cit != l.cend() );
                    }

                    int i = 0;
                    for ( typename OrdList::iterator iter = l.begin(), itEnd = l.end(); iter != itEnd; ++iter, ++i ) {
                        CPPUNIT_ASSERT( iter.key() == i );
                        CPPUNIT_ASSERT( iter->first == i );
                        CPPUNIT_ASSERT( (*iter).first == i );

                        CPPUNIT_ASSERT( iter.val().m_val == i * 2 );
                        CPPUNIT_ASSERT( iter->second.m_val == i * 2 );
                        CPPUNIT_ASSERT( (*iter).second.m_val == i * 2 );

                        iter.val().m_val = i * 3;
                    }

                    // Check that we have visited all items
                    for ( int i = 0; i < nCount; ++i ) {
                        it = l.find( i );
                        CPPUNIT_ASSERT( it != l.end() );
                        CPPUNIT_ASSERT( it.key() == i );
                        CPPUNIT_ASSERT( it.val().m_val == i * 3 );
                    }

                    l.clear();
                    CPPUNIT_ASSERT( l.empty() );

                    // Const iterator
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i, i * 7) != l.end() );

                    i = 0;
                    const OrdList& rl = l;
                    for ( typename OrdList::const_iterator iter = rl.begin(), itEnd = rl.end(); iter != itEnd; ++iter, ++i ) {
                        CPPUNIT_ASSERT( iter.key() == i );
                        CPPUNIT_ASSERT( iter->first == i );
                        CPPUNIT_ASSERT( (*iter).first == i );

                        CPPUNIT_ASSERT( iter.val().m_val == i * 7 );
                        CPPUNIT_ASSERT( iter->second.m_val == i * 7 );
                        CPPUNIT_ASSERT( (*iter).second.m_val == i * 7 );

                        // it.val().m_val = i * 3    ; // error: const-iterator
                    }

                    l.clear();
                    CPPUNIT_ASSERT( l.empty() );
                }

            }
        }

        void HP_cmp();
        void HP_less();
        void HP_cmpmix();
        void HP_ic();

        void DHP_cmp();
        void DHP_less();
        void DHP_cmpmix();
        void DHP_ic();

        void RCU_GPI_cmp();
        void RCU_GPI_less();
        void RCU_GPI_cmpmix();
        void RCU_GPI_ic();

        void RCU_GPB_cmp();
        void RCU_GPB_less();
        void RCU_GPB_cmpmix();
        void RCU_GPB_ic();

        void RCU_GPT_cmp();
        void RCU_GPT_less();
        void RCU_GPT_cmpmix();
        void RCU_GPT_ic();

        void RCU_SHB_cmp();
        void RCU_SHB_less();
        void RCU_SHB_cmpmix();
        void RCU_SHB_ic();

        void RCU_SHT_cmp();
        void RCU_SHT_less();
        void RCU_SHT_cmpmix();
        void RCU_SHT_ic();

        void NOGC_cmp();
        void NOGC_less();
        void NOGC_cmpmix();
        void NOGC_ic();

        CPPUNIT_TEST_SUITE(MichaelKVListTestHeader)
            CPPUNIT_TEST(HP_cmp)
            CPPUNIT_TEST(HP_less)
            CPPUNIT_TEST(HP_cmpmix)
            CPPUNIT_TEST(HP_ic)

            CPPUNIT_TEST(DHP_cmp)
            CPPUNIT_TEST(DHP_less)
            CPPUNIT_TEST(DHP_cmpmix)
            CPPUNIT_TEST(DHP_ic)

            CPPUNIT_TEST(RCU_GPI_cmp)
            CPPUNIT_TEST(RCU_GPI_less)
            CPPUNIT_TEST(RCU_GPI_cmpmix)
            CPPUNIT_TEST(RCU_GPI_ic)

            CPPUNIT_TEST(RCU_GPB_cmp)
            CPPUNIT_TEST(RCU_GPB_less)
            CPPUNIT_TEST(RCU_GPB_cmpmix)
            CPPUNIT_TEST(RCU_GPB_ic)

            CPPUNIT_TEST(RCU_GPT_cmp)
            CPPUNIT_TEST(RCU_GPT_less)
            CPPUNIT_TEST(RCU_GPT_cmpmix)
            CPPUNIT_TEST(RCU_GPT_ic)

            CPPUNIT_TEST(RCU_SHB_cmp)
            CPPUNIT_TEST(RCU_SHB_less)
            CPPUNIT_TEST(RCU_SHB_cmpmix)
            CPPUNIT_TEST(RCU_SHB_ic)

            CPPUNIT_TEST(RCU_SHT_cmp)
            CPPUNIT_TEST(RCU_SHT_less)
            CPPUNIT_TEST(RCU_SHT_cmpmix)
            CPPUNIT_TEST(RCU_SHT_ic)

            CPPUNIT_TEST(NOGC_cmp)
            CPPUNIT_TEST(NOGC_less)
            CPPUNIT_TEST(NOGC_cmpmix)
            CPPUNIT_TEST(NOGC_ic)
        CPPUNIT_TEST_SUITE_END()
    };

}   // namespace ordlist

#endif // #ifndef CDSTEST_HDR_MICHAEL_KV_H

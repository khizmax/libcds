//$$CDS-header$$

#ifndef CDSTEST_HDR_MAP_H
#define CDSTEST_HDR_MAP_H
#include "size_check.h"

#include "cppunit/cppunit_proxy.h"
#include <cds/os/timer.h>
#include <cds/opt/hash.h>
#include <functional>   // ref

namespace cds { namespace container {}}

namespace map {
    using misc::check_size;

    namespace cc = cds::container;
    namespace co = cds::opt;

    // MichaelHashSet based on MichaelList
    class HashMapHdrTest: public CppUnitMini::TestCase
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

            value_type( value_type&& v )
                : m_val( v.m_val )
            {}

            value_type( value_type const& v )
                : m_val( v.m_val )
            {}

            value_type& operator=( value_type const& v )
            {
                m_val = v.m_val;
                return *this;
            }
        };

        typedef std::pair<key_type const, value_type> pair_type;

        struct less
        {
            bool operator ()(int v1, int v2 ) const
            {
                return v1 < v2;
            }
        };

        struct cmp {
            int operator ()(int v1, int v2 ) const
            {
                if ( v1 < v2 )
                    return -1;
                return v1 > v2 ? 1 : 0;
            }
        };

        struct equal {
            bool operator ()(int v1, int v2 ) const
            {
                return v1 == v2;
            }
        };

        struct hash_int {
            size_t operator()( int i ) const
            {
                return co::v::hash<int>()( i );
            }

            template <typename T>
            size_t operator()( T const& i ) const
            {
                return co::v::hash<int>()( i.nKey );
            }
        };

        struct simple_item_counter {
            size_t  m_nCount;

            simple_item_counter()
                : m_nCount(0)
            {}

            size_t operator ++()
            {
                return ++m_nCount;
            }

            size_t operator --()
            {
                return --m_nCount;
            }

            void reset()
            {
                m_nCount = 0;
            }

            operator size_t() const
            {
                return m_nCount;
            }
        };

        template <typename Map>
        struct insert_functor
        {
            typedef typename Map::value_type pair_type;

            // insert ftor
            void operator()( pair_type& item )
            {
                item.second.m_val = item.first * 3;
            }

            // ensure ftor
            void operator()( bool bNew, pair_type& item )
            {
                if ( bNew )
                    item.second.m_val = item.first * 2;
                else
                    item.second.m_val = item.first * 5;
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
            template <typename T, typename Q>
            void operator ()( T& pair, Q )
            {
                CPPUNIT_ASSERT_CURRENT( pair.second.m_val == m_nExpected );
            }
        };

        struct extract_functor
        {
            int *   m_pVal;
            void operator()( pair_type const& val )
            {
                *m_pVal = val.second.m_val;
            }
        };

        struct other_item {
            int nKey;

            other_item( int key )
                : nKey(key)
            {}
        };

        struct other_less
        {
            bool operator ()(int v1, other_item const& v2 ) const
            {
                return v1 < v2.nKey;
            }
            bool operator ()(other_item const& v1, int v2 ) const
            {
                return v1.nKey < v2;
            }
        };


        template <class Map>
        void test_int()
        {
            Map m( 100, 4 );

            test_int_with(m);

            // extract/get test
            CPPUNIT_ASSERT( m.empty() );
            {
                const int nLimit = 100;
                typename Map::guarded_ptr gp;
                int arrRandom[nLimit];
                for ( int i = 0; i < nLimit; ++i )
                    arrRandom[i] = i;
                shuffle( arrRandom, arrRandom + nLimit );

                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( m.insert( arrRandom[i], arrRandom[i] ));

                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrRandom[i];
                    gp = m.get( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey );
                    gp.release();

                    gp = m.extract( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey );
                    gp.release();

                    gp = m.get( nKey );
                    CPPUNIT_CHECK( !gp );

                    CPPUNIT_CHECK( !m.extract(nKey));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( m.empty() );

                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( m.insert( arrRandom[i], arrRandom[i] ));

                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrRandom[i];
                    gp = m.get_with( other_item( nKey ), other_less() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey );
                    gp.release();

                    gp = m.extract_with( other_item( nKey ), other_less() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->first == nKey );
                    CPPUNIT_CHECK( gp->second.m_val == nKey );
                    gp.release();

                    gp = m.get_with( other_item( nKey ), other_less() );
                    CPPUNIT_CHECK( !gp );

                    CPPUNIT_CHECK( !m.extract_with(other_item(nKey), other_less() ));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( m.empty() );
            }

            // iterator test
            test_iter<Map>();
        }

        template <class Map>
        void test_rcu()
        {
            Map m( 52, 4 );

            test_int_with(m);

            // extract/get test
            {
                typedef typename Map::gc    rcu;
                typedef typename Map::rcu_lock rcu_lock;
                typedef typename Map::value_type value_type;
                typename Map::exempt_ptr ep;

                static size_t const nLimit = 100;
                int arr[nLimit];
                for ( size_t i = 0; i < nLimit; ++i )
                    arr[i] = (int) i;
                shuffle( arr, arr + nLimit );

                for ( size_t i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( m.insert( arr[i], arr[i] ));

                for ( size_t i = 0; i < nLimit; i += 2 ) {
                    value_type * pVal;
                    int nKey = arr[i];
                    {
                        rcu_lock l;
                        pVal = m.get( nKey );
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->first == nKey );
                        CPPUNIT_CHECK( pVal->second.m_val == nKey );

                        ep = m.extract( nKey );
                        CPPUNIT_ASSERT( ep );
                        CPPUNIT_ASSERT( !ep.empty() );
                        CPPUNIT_CHECK( pVal->first == ep->first );
                        CPPUNIT_CHECK( pVal->second.m_val == ep->second.m_val );
                    }
                    ep.release();
                    {
                        rcu_lock l;
                        CPPUNIT_CHECK( m.get( nKey ) == nullptr );
                        ep = m.extract( nKey );
                        CPPUNIT_CHECK( !ep );
                        CPPUNIT_CHECK( ep.empty() );

                        nKey = arr[i+1];
                        pVal = m.get_with( other_item(nKey), other_less() );
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->first == nKey );
                        CPPUNIT_CHECK( pVal->second.m_val == nKey );

                        ep = m.extract_with( other_item( nKey ), other_less() );
                        CPPUNIT_ASSERT( ep );
                        CPPUNIT_ASSERT( !ep.empty() );
                        CPPUNIT_CHECK( pVal->first == ep->first );
                        CPPUNIT_CHECK( pVal->second.m_val == (*ep).second.m_val );
                    }
                    ep.release();
                    {
                        rcu_lock l;
                        CPPUNIT_CHECK( m.get_with( other_item(nKey), other_less() ) == nullptr );
                        CPPUNIT_CHECK( !m.extract_with( other_item(nKey), other_less() ));
                        CPPUNIT_CHECK( ep.empty() );
                    }
                }
                CPPUNIT_CHECK( m.empty() );
                CPPUNIT_CHECK( check_size( m, 0 ));
                {
                    rcu_lock l;
                    CPPUNIT_CHECK( m.get( int(nLimit / 2) ) == nullptr );
                    ep = m.extract( int( nLimit / 2 ) );
                    CPPUNIT_CHECK( !ep );
                    CPPUNIT_CHECK( ep.empty() );
                }
            }

            // iterator test
            test_iter<Map>();
        }

        template <class Map>
        void test_int_with( Map& m )
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
            CPPUNIT_ASSERT( m.insert_with( 30, insert_functor<Map>() ) )    ; // value = 90
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( m.find(30) );

            CPPUNIT_ASSERT( !m.insert_with( 10, insert_functor<Map>() ) );
            CPPUNIT_ASSERT( !m.insert_with( 25, insert_functor<Map>() ) );
            CPPUNIT_ASSERT( !m.insert_with( 30, insert_functor<Map>() ) );

            // ensure (new key)
            CPPUNIT_ASSERT( !m.find(27) );
            ensureResult = m.ensure( 27, insert_functor<Map>() ) ;   // value = 54
            CPPUNIT_ASSERT( ensureResult.first );
            CPPUNIT_ASSERT( ensureResult.second );
            CPPUNIT_ASSERT( m.find(27) );

            // find test
            check_value chk(10);
            CPPUNIT_ASSERT( m.find( 10, std::ref(chk) ));
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find_with( 25, less(), std::ref( chk ) ) );
            chk.m_nExpected = 90;
            CPPUNIT_ASSERT( m.find( 30, std::ref( chk ) ) );
            chk.m_nExpected = 54;
            CPPUNIT_ASSERT( m.find( 27, std::ref( chk ) ) );

            ensureResult = m.ensure( 10, insert_functor<Map>() ) ;   // value = 50
            CPPUNIT_ASSERT( ensureResult.first );
            CPPUNIT_ASSERT( !ensureResult.second );
            chk.m_nExpected = 50;
            CPPUNIT_ASSERT( m.find( 10, std::ref( chk ) ) );

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
            CPPUNIT_ASSERT( m.erase_with( 29, less(), std::ref( ext ) ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( nVal == 290 );
            nVal = -1;
            CPPUNIT_ASSERT( !m.erase_with( 29, less(), std::ref( ext ) ) );
            CPPUNIT_ASSERT( nVal == -1 );

            CPPUNIT_ASSERT( m.erase( 30, std::ref( ext ) ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));
            CPPUNIT_ASSERT( nVal == 90 );
            nVal = -1;
            CPPUNIT_ASSERT( !m.erase( 30, std::ref( ext ) ) );
            CPPUNIT_ASSERT( nVal == -1 );

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

            // emplace test
            CPPUNIT_ASSERT( m.emplace(126) ) ; // key = 126, val = 0
            CPPUNIT_ASSERT( m.emplace(137, 731))    ;   // key = 137, val = 731
            CPPUNIT_ASSERT( m.emplace( 149, value_type(941) ))   ;   // key = 149, val = 941

            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));

            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find( 126, std::ref(chk) ));
            chk.m_nExpected = 731;
            CPPUNIT_ASSERT( m.find_with( 137, less(), std::ref(chk) ));
            chk.m_nExpected = 941;
            CPPUNIT_ASSERT( m.find( 149, std::ref(chk) ));

            CPPUNIT_ASSERT( !m.emplace(126, 621)) ; // already in map
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find( 126, std::ref(chk) ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));
        }


        template <class Map>
        void test_int_nogc()
        {
            typedef typename Map::iterator          iterator;
            typedef typename Map::const_iterator    const_iterator;

            {
                Map m( 52, 4 );

                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));

                CPPUNIT_ASSERT( m.find(10) == m.end() );
                iterator it = m.insert( 10 );
                CPPUNIT_ASSERT( it != m.end() );
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
                CPPUNIT_ASSERT( m.find_with(100, less()) == it );
                CPPUNIT_ASSERT( it->first == 100 );
                CPPUNIT_ASSERT( it->second.m_val == 200 );

                CPPUNIT_ASSERT( m.find(55) == m.end() );
                it = m.insert_with( 55, insert_functor<Map>() );
                CPPUNIT_ASSERT( it != m.end() );
                CPPUNIT_ASSERT( !m.empty() );
                CPPUNIT_ASSERT( check_size( m, 3 ));
                CPPUNIT_ASSERT( m.find(55) == it );
                CPPUNIT_ASSERT( it->first == 55 );
                CPPUNIT_ASSERT( it->second.m_val == 55 * 3 );

                CPPUNIT_ASSERT( m.insert( 55 ) == m.end() );
                CPPUNIT_ASSERT( m.insert( 55, 10 ) == m.end() );
                CPPUNIT_ASSERT( m.insert_with( 55, insert_functor<Map>()) == m.end() );

                CPPUNIT_ASSERT( m.find(10) != m.end() );
                std::pair<iterator, bool> ensureResult = m.ensure( 10 );
                CPPUNIT_ASSERT( ensureResult.first != m.end() );
                CPPUNIT_ASSERT( !ensureResult.second  );
                CPPUNIT_ASSERT( !m.empty() );
                ensureResult.first->second.m_val = ensureResult.first->first * 5;
                CPPUNIT_ASSERT( check_size( m, 3 ));
                CPPUNIT_ASSERT( m.find(10) == ensureResult.first );
                it = m.find(10);
                CPPUNIT_ASSERT( it != m.end() );
                CPPUNIT_ASSERT( it->second.m_val == 50 );

                CPPUNIT_ASSERT( m.find(120) == m.end() );
                ensureResult = m.ensure( 120 );
                CPPUNIT_ASSERT( ensureResult.first != m.end() );
                CPPUNIT_ASSERT( ensureResult.second  );
                CPPUNIT_ASSERT( !m.empty() );
                CPPUNIT_ASSERT( check_size( m, 4 ));
                ensureResult.first->second.m_val = ensureResult.first->first * 5;
                CPPUNIT_ASSERT( m.find_with(120, less()) == ensureResult.first );
                it = m.find_with(120, less());
                CPPUNIT_ASSERT( it != m.end() );
                CPPUNIT_ASSERT( it->second.m_val == 120 * 5 );
                CPPUNIT_ASSERT( m.find_with(120, less()) == m.find(120) );

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
            }

            // iterator test

            {
                Map m( 52, 4 );

                for ( int i = 0; i < 500; ++i ) {
                    CPPUNIT_ASSERT( m.insert( i, i * 2 ) != m.end() );
                }
                CPPUNIT_ASSERT( check_size( m, 500 ));

                {
                    typename Map::iterator it( m.begin() );
                    typename Map::const_iterator cit( m.cbegin() );
                    CPPUNIT_CHECK( it == cit );
                    CPPUNIT_CHECK( it != m.end() );
                    CPPUNIT_CHECK( it != m.cend() );
                    CPPUNIT_CHECK( cit != m.end() );
                    CPPUNIT_CHECK( cit != m.cend() );
                    ++it;
                    CPPUNIT_CHECK( it != cit );
                    CPPUNIT_CHECK( it != m.end() );
                    CPPUNIT_CHECK( it != m.cend() );
                    CPPUNIT_CHECK( cit != m.end() );
                    CPPUNIT_CHECK( cit != m.cend() );
                    ++cit;
                    CPPUNIT_CHECK( it == cit );
                    CPPUNIT_CHECK( it != m.end() );
                    CPPUNIT_CHECK( it != m.cend() );
                    CPPUNIT_CHECK( cit != m.end() );
                    CPPUNIT_CHECK( cit != m.cend() );
                }


                for ( iterator it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                    iterator it2 = it;
                    CPPUNIT_CHECK( it2 == it );
                    CPPUNIT_CHECK( it2 != itEnd );
                    CPPUNIT_ASSERT( it->first * 2 == (*it).second.m_val );
                    it->second = it->first;
                }

                Map const& refMap = m;
                for ( const_iterator it = refMap.begin(), itEnd = refMap.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( it->first == it->second.m_val );
                    CPPUNIT_ASSERT( (*it).first == (*it).second.m_val );
                }
            }
        }

        template <class Map>
        void test_int_nogc_unordered()
        {
            typedef typename Map::iterator          iterator;
            typedef typename Map::const_iterator    const_iterator;

            {
                Map m( 52, 4 );

                CPPUNIT_ASSERT( m.empty() );
                CPPUNIT_ASSERT( check_size( m, 0 ));

                CPPUNIT_ASSERT( m.find(10) == m.end() );
                iterator it = m.insert( 10 );
                CPPUNIT_ASSERT( it != m.end() );
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
                CPPUNIT_ASSERT( m.find_with(100, equal()) == it );
                CPPUNIT_ASSERT( it->first == 100 );
                CPPUNIT_ASSERT( it->second.m_val == 200 );

                CPPUNIT_ASSERT( m.find(55) == m.end() );
                it = m.insert_with( 55, insert_functor<Map>() );
                CPPUNIT_ASSERT( it != m.end() );
                CPPUNIT_ASSERT( !m.empty() );
                CPPUNIT_ASSERT( check_size( m, 3 ));
                CPPUNIT_ASSERT( m.find(55) == it );
                CPPUNIT_ASSERT( it->first == 55 );
                CPPUNIT_ASSERT( it->second.m_val == 55 * 3 );

                CPPUNIT_ASSERT( m.insert( 55 ) == m.end() );
                CPPUNIT_ASSERT( m.insert( 55, 10 ) == m.end() );
                CPPUNIT_ASSERT( m.insert_with( 55, insert_functor<Map>()) == m.end() );

                CPPUNIT_ASSERT( m.find(10) != m.end() );
                std::pair<iterator, bool> ensureResult = m.ensure( 10 );
                CPPUNIT_ASSERT( ensureResult.first != m.end() );
                CPPUNIT_ASSERT( !ensureResult.second  );
                CPPUNIT_ASSERT( !m.empty() );
                ensureResult.first->second.m_val = ensureResult.first->first * 5;
                CPPUNIT_ASSERT( check_size( m, 3 ));
                CPPUNIT_ASSERT( m.find(10) == ensureResult.first );
                it = m.find(10);
                CPPUNIT_ASSERT( it != m.end() );
                CPPUNIT_ASSERT( it->second.m_val == 50 );

                CPPUNIT_ASSERT( m.find(120) == m.end() );
                ensureResult = m.ensure( 120 );
                CPPUNIT_ASSERT( ensureResult.first != m.end() );
                CPPUNIT_ASSERT( ensureResult.second  );
                CPPUNIT_ASSERT( !m.empty() );
                CPPUNIT_ASSERT( check_size( m, 4 ));
                ensureResult.first->second.m_val = ensureResult.first->first * 5;
                CPPUNIT_ASSERT( m.find_with(120, equal()) == ensureResult.first );
                it = m.find_with(120, equal());
                CPPUNIT_ASSERT( it != m.end() );
                CPPUNIT_ASSERT( it->second.m_val == 120 * 5 );
                CPPUNIT_ASSERT( m.find_with(120, equal()) == m.find(120) );

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
            }

            // iterator test

            {
                Map m( 52, 4 );

                for ( int i = 0; i < 500; ++i ) {
                    CPPUNIT_ASSERT( m.insert( i, i * 2 ) != m.end() );
                }
                CPPUNIT_ASSERT( check_size( m, 500 ));

                {
                    typename Map::iterator it( m.begin() );
                    typename Map::const_iterator cit( m.cbegin() );
                    CPPUNIT_CHECK( it == cit );
                    CPPUNIT_CHECK( it != m.end() );
                    CPPUNIT_CHECK( it != m.cend() );
                    CPPUNIT_CHECK( cit != m.end() );
                    CPPUNIT_CHECK( cit != m.cend() );
                    ++it;
                    CPPUNIT_CHECK( it != cit );
                    CPPUNIT_CHECK( it != m.end() );
                    CPPUNIT_CHECK( it != m.cend() );
                    CPPUNIT_CHECK( cit != m.end() );
                    CPPUNIT_CHECK( cit != m.cend() );
                    ++cit;
                    CPPUNIT_CHECK( it == cit );
                    CPPUNIT_CHECK( it != m.end() );
                    CPPUNIT_CHECK( it != m.cend() );
                    CPPUNIT_CHECK( cit != m.end() );
                    CPPUNIT_CHECK( cit != m.cend() );
                }


                for ( iterator it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                    iterator it2 = it;
                    CPPUNIT_CHECK( it2 == it );
                    CPPUNIT_CHECK( it2 != itEnd );
                    CPPUNIT_ASSERT( it->first * 2 == (*it).second.m_val );
                    it->second = it->first;
                }

                Map const& refMap = m;
                for ( const_iterator it = refMap.begin(), itEnd = refMap.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( it->first == it->second.m_val );
                    CPPUNIT_ASSERT( (*it).first == (*it).second.m_val );
                }
            }
        }

        template <class Map>
        void test_iter()
        {
            typedef typename Map::iterator          iterator;
            typedef typename Map::const_iterator    const_iterator;

            Map s( 100, 4 );

            const int nMaxCount = 500;
            for ( int i = 0; i < nMaxCount; ++i ) {
                CPPUNIT_ASSERT( s.insert( i, i * 2 ));
            }

            {
                typename Map::iterator it( s.begin() );
                typename Map::const_iterator cit( s.cbegin() );
                CPPUNIT_CHECK( it == cit );
                CPPUNIT_CHECK( it != s.end() );
                CPPUNIT_CHECK( it != s.cend() );
                CPPUNIT_CHECK( cit != s.end() );
                CPPUNIT_CHECK( cit != s.cend() );
                ++it;
                CPPUNIT_CHECK( it != cit );
                CPPUNIT_CHECK( it != s.end() );
                CPPUNIT_CHECK( it != s.cend() );
                CPPUNIT_CHECK( cit != s.end() );
                CPPUNIT_CHECK( cit != s.cend() );
                ++cit;
                CPPUNIT_CHECK( it == cit );
                CPPUNIT_CHECK( it != s.end() );
                CPPUNIT_CHECK( it != s.cend() );
                CPPUNIT_CHECK( cit != s.end() );
                CPPUNIT_CHECK( cit != s.cend() );
            }

            int nCount = 0;
            for ( iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first * 2 == it->second.m_val );
                CPPUNIT_ASSERT( (*it).first * 2 == (*it).second.m_val );
                it->second.m_val = it->first;
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == nMaxCount );

            Map const& refSet = s;
            nCount = 0;
            for ( const_iterator it = refSet.begin(), itEnd = refSet.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first == it->second.m_val );
                CPPUNIT_ASSERT( (*it).first == (*it).second.m_val );
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == nMaxCount );
        }


        void Michael_HP_cmp();
        void Michael_HP_less();
        void Michael_HP_cmpmix();

        void Michael_DHP_cmp();
        void Michael_DHP_less();
        void Michael_DHP_cmpmix();

        void Michael_RCU_GPI_cmp();
        void Michael_RCU_GPI_less();
        void Michael_RCU_GPI_cmpmix();

        void Michael_RCU_GPB_cmp();
        void Michael_RCU_GPB_less();
        void Michael_RCU_GPB_cmpmix();

        void Michael_RCU_GPT_cmp();
        void Michael_RCU_GPT_less();
        void Michael_RCU_GPT_cmpmix();

        void Michael_RCU_SHB_cmp();
        void Michael_RCU_SHB_less();
        void Michael_RCU_SHB_cmpmix();

        void Michael_RCU_SHT_cmp();
        void Michael_RCU_SHT_less();
        void Michael_RCU_SHT_cmpmix();

        void Michael_nogc_cmp();
        void Michael_nogc_less();
        void Michael_nogc_cmpmix();

        void Lazy_HP_cmp();
        void Lazy_HP_less();
        void Lazy_HP_cmpmix();

        void Lazy_DHP_cmp();
        void Lazy_DHP_less();
        void Lazy_DHP_cmpmix();

        void Lazy_RCU_GPI_cmp();
        void Lazy_RCU_GPI_less();
        void Lazy_RCU_GPI_cmpmix();

        void Lazy_RCU_GPB_cmp();
        void Lazy_RCU_GPB_less();
        void Lazy_RCU_GPB_cmpmix();

        void Lazy_RCU_GPT_cmp();
        void Lazy_RCU_GPT_less();
        void Lazy_RCU_GPT_cmpmix();

        void Lazy_RCU_SHB_cmp();
        void Lazy_RCU_SHB_less();
        void Lazy_RCU_SHB_cmpmix();

        void Lazy_RCU_SHT_cmp();
        void Lazy_RCU_SHT_less();
        void Lazy_RCU_SHT_cmpmix();

        void Lazy_nogc_cmp();
        void Lazy_nogc_less();
        void Lazy_nogc_equal();
        void Lazy_nogc_cmpmix();

        void Split_HP_cmp();
        void Split_HP_less();
        void Split_HP_cmpmix();
        void Split_HP_cmpmix_stat();

        void Split_DHP_cmp();
        void Split_DHP_less();
        void Split_DHP_cmpmix();
        void Split_DHP_cmpmix_stat();

        void Split_RCU_GPI_cmp();
        void Split_RCU_GPI_less();
        void Split_RCU_GPI_cmpmix();
        void Split_RCU_GPI_cmpmix_stat();

        void Split_RCU_GPB_cmp();
        void Split_RCU_GPB_less();
        void Split_RCU_GPB_cmpmix();
        void Split_RCU_GPB_cmpmix_stat();

        void Split_RCU_GPT_cmp();
        void Split_RCU_GPT_less();
        void Split_RCU_GPT_cmpmix();
        void Split_RCU_GPT_cmpmix_stat();

        void Split_RCU_SHB_cmp();
        void Split_RCU_SHB_less();
        void Split_RCU_SHB_cmpmix();
        void Split_RCU_SHB_cmpmix_stat();

        void Split_RCU_SHT_cmp();
        void Split_RCU_SHT_less();
        void Split_RCU_SHT_cmpmix();
        void Split_RCU_SHT_cmpmix_stat();

        void Split_nogc_cmp();
        void Split_nogc_less();
        void Split_nogc_cmpmix();
        void Split_nogc_cmpmix_stat();

        void Split_Lazy_HP_cmp();
        void Split_Lazy_HP_less();
        void Split_Lazy_HP_cmpmix();
        void Split_Lazy_HP_cmpmix_stat();

        void Split_Lazy_DHP_cmp();
        void Split_Lazy_DHP_less();
        void Split_Lazy_DHP_cmpmix();
        void Split_Lazy_DHP_cmpmix_stat();

        void Split_Lazy_RCU_GPI_cmp();
        void Split_Lazy_RCU_GPI_less();
        void Split_Lazy_RCU_GPI_cmpmix();
        void Split_Lazy_RCU_GPI_cmpmix_stat();

        void Split_Lazy_RCU_GPB_cmp();
        void Split_Lazy_RCU_GPB_less();
        void Split_Lazy_RCU_GPB_cmpmix();
        void Split_Lazy_RCU_GPB_cmpmix_stat();

        void Split_Lazy_RCU_GPT_cmp();
        void Split_Lazy_RCU_GPT_less();
        void Split_Lazy_RCU_GPT_cmpmix();
        void Split_Lazy_RCU_GPT_cmpmix_stat();

        void Split_Lazy_RCU_SHB_cmp();
        void Split_Lazy_RCU_SHB_less();
        void Split_Lazy_RCU_SHB_cmpmix();
        void Split_Lazy_RCU_SHB_cmpmix_stat();

        void Split_Lazy_RCU_SHT_cmp();
        void Split_Lazy_RCU_SHT_less();
        void Split_Lazy_RCU_SHT_cmpmix();
        void Split_Lazy_RCU_SHT_cmpmix_stat();

        void Split_Lazy_nogc_cmp();
        void Split_Lazy_nogc_less();
        void Split_Lazy_nogc_cmpmix();
        void Split_Lazy_nogc_cmpmix_stat();

        CPPUNIT_TEST_SUITE(HashMapHdrTest)
            CPPUNIT_TEST(Michael_HP_cmp)
            CPPUNIT_TEST(Michael_HP_less)
            CPPUNIT_TEST(Michael_HP_cmpmix)

            CPPUNIT_TEST(Michael_DHP_cmp)
            CPPUNIT_TEST(Michael_DHP_less)
            CPPUNIT_TEST(Michael_DHP_cmpmix)

            CPPUNIT_TEST(Michael_RCU_GPI_cmp)
            CPPUNIT_TEST(Michael_RCU_GPI_less)
            CPPUNIT_TEST(Michael_RCU_GPI_cmpmix)

            CPPUNIT_TEST(Michael_RCU_GPB_cmp)
            CPPUNIT_TEST(Michael_RCU_GPB_less)
            CPPUNIT_TEST(Michael_RCU_GPB_cmpmix)

            CPPUNIT_TEST(Michael_RCU_SHT_cmp)
            CPPUNIT_TEST(Michael_RCU_SHT_less)
            CPPUNIT_TEST(Michael_RCU_SHT_cmpmix)

            CPPUNIT_TEST(Michael_RCU_SHB_cmp)
            CPPUNIT_TEST(Michael_RCU_SHB_less)
            CPPUNIT_TEST(Michael_RCU_SHB_cmpmix)

            CPPUNIT_TEST(Michael_RCU_GPT_cmp)
            CPPUNIT_TEST(Michael_RCU_GPT_less)
            CPPUNIT_TEST(Michael_RCU_GPT_cmpmix)

            CPPUNIT_TEST(Michael_nogc_cmp)
            CPPUNIT_TEST(Michael_nogc_less)
            CPPUNIT_TEST(Michael_nogc_cmpmix)

            CPPUNIT_TEST(Lazy_HP_cmp)
            CPPUNIT_TEST(Lazy_HP_less)
            CPPUNIT_TEST(Lazy_HP_cmpmix)

            CPPUNIT_TEST(Lazy_DHP_cmp)
            CPPUNIT_TEST(Lazy_DHP_less)
            CPPUNIT_TEST(Lazy_DHP_cmpmix)

            CPPUNIT_TEST(Lazy_RCU_GPI_cmp)
            CPPUNIT_TEST(Lazy_RCU_GPI_less)
            CPPUNIT_TEST(Lazy_RCU_GPI_cmpmix)

            CPPUNIT_TEST(Lazy_RCU_GPB_cmp)
            CPPUNIT_TEST(Lazy_RCU_GPB_less)
            CPPUNIT_TEST(Lazy_RCU_GPB_cmpmix)

            CPPUNIT_TEST(Lazy_RCU_GPT_cmp)
            CPPUNIT_TEST(Lazy_RCU_GPT_less)
            CPPUNIT_TEST(Lazy_RCU_GPT_cmpmix)

            CPPUNIT_TEST(Lazy_RCU_SHB_cmp)
            CPPUNIT_TEST(Lazy_RCU_SHB_less)
            CPPUNIT_TEST(Lazy_RCU_SHB_cmpmix)

            CPPUNIT_TEST(Lazy_RCU_SHT_cmp)
            CPPUNIT_TEST(Lazy_RCU_SHT_less)
            CPPUNIT_TEST(Lazy_RCU_SHT_cmpmix)

            CPPUNIT_TEST(Lazy_nogc_cmp)
            CPPUNIT_TEST(Lazy_nogc_less)
            CPPUNIT_TEST(Lazy_nogc_equal)
            CPPUNIT_TEST(Lazy_nogc_cmpmix)

            CPPUNIT_TEST(Split_HP_cmp)
            CPPUNIT_TEST(Split_HP_less)
            CPPUNIT_TEST(Split_HP_cmpmix)
            CPPUNIT_TEST( Split_HP_cmpmix_stat )

            CPPUNIT_TEST(Split_DHP_cmp)
            CPPUNIT_TEST(Split_DHP_less)
            CPPUNIT_TEST(Split_DHP_cmpmix)
            CPPUNIT_TEST( Split_DHP_cmpmix_stat )

            CPPUNIT_TEST(Split_RCU_GPI_cmp)
            CPPUNIT_TEST(Split_RCU_GPI_less)
            CPPUNIT_TEST(Split_RCU_GPI_cmpmix)
            CPPUNIT_TEST( Split_RCU_GPI_cmpmix_stat )

            CPPUNIT_TEST(Split_RCU_GPB_cmp)
            CPPUNIT_TEST(Split_RCU_GPB_less)
            CPPUNIT_TEST(Split_RCU_GPB_cmpmix)
            CPPUNIT_TEST( Split_RCU_GPB_cmpmix_stat )

            CPPUNIT_TEST(Split_RCU_GPT_cmp)
            CPPUNIT_TEST(Split_RCU_GPT_less)
            CPPUNIT_TEST(Split_RCU_GPT_cmpmix)
            CPPUNIT_TEST( Split_RCU_GPT_cmpmix_stat )

            CPPUNIT_TEST(Split_RCU_SHB_cmp)
            CPPUNIT_TEST(Split_RCU_SHB_less)
            CPPUNIT_TEST(Split_RCU_SHB_cmpmix)
            CPPUNIT_TEST( Split_RCU_SHB_cmpmix_stat )

            CPPUNIT_TEST(Split_RCU_SHT_cmp)
            CPPUNIT_TEST(Split_RCU_SHT_less)
            CPPUNIT_TEST(Split_RCU_SHT_cmpmix)
            CPPUNIT_TEST( Split_RCU_SHT_cmpmix_stat )

            CPPUNIT_TEST(Split_nogc_cmp)
            CPPUNIT_TEST(Split_nogc_less)
            CPPUNIT_TEST(Split_nogc_cmpmix)
            CPPUNIT_TEST( Split_nogc_cmpmix_stat )

            CPPUNIT_TEST(Split_Lazy_HP_cmp)
            CPPUNIT_TEST(Split_Lazy_HP_less)
            CPPUNIT_TEST(Split_Lazy_HP_cmpmix)
            CPPUNIT_TEST( Split_Lazy_HP_cmpmix_stat )

            CPPUNIT_TEST(Split_Lazy_DHP_cmp)
            CPPUNIT_TEST(Split_Lazy_DHP_less)
            CPPUNIT_TEST(Split_Lazy_DHP_cmpmix)
            CPPUNIT_TEST( Split_Lazy_DHP_cmpmix_stat )

            CPPUNIT_TEST(Split_Lazy_RCU_GPI_cmp)
            CPPUNIT_TEST(Split_Lazy_RCU_GPI_less)
            CPPUNIT_TEST(Split_Lazy_RCU_GPI_cmpmix)
            CPPUNIT_TEST( Split_Lazy_RCU_GPI_cmpmix_stat )

            CPPUNIT_TEST(Split_Lazy_RCU_GPB_cmp)
            CPPUNIT_TEST(Split_Lazy_RCU_GPB_less)
            CPPUNIT_TEST(Split_Lazy_RCU_GPB_cmpmix)
            CPPUNIT_TEST( Split_Lazy_RCU_GPB_cmpmix_stat )

            CPPUNIT_TEST(Split_Lazy_RCU_GPT_cmp)
            CPPUNIT_TEST(Split_Lazy_RCU_GPT_less)
            CPPUNIT_TEST(Split_Lazy_RCU_GPT_cmpmix)
            CPPUNIT_TEST( Split_Lazy_RCU_GPT_cmpmix_stat )

            CPPUNIT_TEST(Split_Lazy_RCU_SHB_cmp)
            CPPUNIT_TEST(Split_Lazy_RCU_SHB_less)
            CPPUNIT_TEST(Split_Lazy_RCU_SHB_cmpmix)
            CPPUNIT_TEST( Split_Lazy_RCU_SHB_cmpmix_stat )

            CPPUNIT_TEST(Split_Lazy_RCU_SHT_cmp)
            CPPUNIT_TEST(Split_Lazy_RCU_SHT_less)
            CPPUNIT_TEST(Split_Lazy_RCU_SHT_cmpmix)
            CPPUNIT_TEST( Split_Lazy_RCU_SHT_cmpmix_stat )

            CPPUNIT_TEST(Split_Lazy_nogc_cmp)
            CPPUNIT_TEST(Split_Lazy_nogc_less)
            CPPUNIT_TEST(Split_Lazy_nogc_cmpmix)
            CPPUNIT_TEST( Split_Lazy_nogc_cmpmix_stat )
            CPPUNIT_TEST_SUITE_END()

    };
}   // namespace map

#endif // #ifndef CDSTEST_HDR_MAP_H

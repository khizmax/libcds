//$$CDS-header$$

#ifndef CDSTEST_HDR_SET_H
#define CDSTEST_HDR_SET_H

#include "cppunit/cppunit_proxy.h"
#include "size_check.h"

#include <cds/opt/hash.h>
#include <cds/os/timer.h>
#include <functional>   // ref

// forward namespace declaration
namespace cds {
    namespace container {}
    namespace opt {}
}

namespace set {
    using misc::check_size;

    namespace cc = cds::container;
    namespace co = cds::opt;


    class HashSetHdrTest: public CppUnitMini::TestCase
    {
    public:
        struct stat
        {
            unsigned int nFindCount     ;   // count of find-functor calling
            unsigned int nEnsureNewCount;
            unsigned int nEnsureCount;

            stat()
            {
                memset( this, 0, sizeof(*this));
            }

            void copy( stat const& s )
            {
                nFindCount = s.nFindCount;
                nEnsureCount = s.nEnsureCount;
                nEnsureNewCount = s.nEnsureNewCount;
            }
        };

        struct item: public stat
        {
            int nKey;
            int nVal;

            item()
            {}

            item( int key )
                : nKey( key )
                , nVal( key )
            {}

            item (int key, int val )
                : nKey(key)
                , nVal( val )
            {}

            item( std::pair<int,int> const& p )
                : nKey( p.first )
                , nVal( p.second )
            {}

            item( item const& i )
                : nKey( i.nKey )
                , nVal( i.nVal )
            {}

            item& operator=(item const& i)
            {
                nKey = i.nKey;
                nVal = i.nVal;
                stat::copy(i);

                return *this;
            }

            item( item&& i )
                : nKey( i.nKey )
                , nVal( i.nVal )
            {}

            //item& operator=(item&& i)
            //{
            //    nKey = i.nKey;
            //    nVal = i.nVal;
            //    return *this;
            //}

            int key() const
            {
                return nKey;
            }

            int val() const
            {
                return nVal;
            }
        };

        struct hash_int {
            size_t operator()( int i ) const
            {
                return co::v::hash<int>()( i );
            }

            size_t operator()( std::pair<int,int> const& i ) const
            {
                return co::v::hash<int>()( i.first );
            }

            template <typename Item>
            size_t operator()( Item const& i ) const
            {
                return (*this)( i.key() );
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

        template <typename T>
        struct less
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() < v2.key();
            }

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() < v2;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 < v2.key();
            }

            bool operator ()( std::pair<int, int> const& v1, const T& v2 ) const
            {
                return v1.first < v2.key();
            }

            bool operator ()(const T& v1, std::pair<int, int> const& v2 ) const
            {
                return v1.key() < v2.first;
            }
        };

        struct other_item {
            int nKey;

            other_item( int key )
                : nKey(key)
            {}

            int key() const
            {
                return nKey;
            }
        };

        struct other_less
        {
            template <typename T>
            bool operator ()(T const& v1, other_item const& v2 ) const
            {
                return v1.key() < v2.nKey;
            }
            template <typename T>
            bool operator ()(other_item const& v1, T const& v2 ) const
            {
                return v1.nKey < v2.key();
            }
        };

        template <typename T>
        struct cmp {
            int operator ()(const T& v1, const T& v2 ) const
            {
                if ( v1.key() < v2.key() )
                    return -1;
                return v1.key() > v2.key() ? 1 : 0;
            }

            template <typename Q>
            int operator ()(const T& v1, const Q& v2 ) const
            {
                if ( v1.key() < v2 )
                    return -1;
                return v1.key() > v2 ? 1 : 0;
            }

            template <typename Q>
            int operator ()(const Q& v1, const T& v2 ) const
            {
                if ( v1 < v2.key() )
                    return -1;
                return v1 > v2.key() ? 1 : 0;
            }

            int operator()( std::pair<int,int> const& v1, T const& v2 ) const
            {
                if ( v1.first < v2.key() )
                    return -1;
                return v1.first > v2.key() ? 1 : 0;
            }

            int operator()( T const& v1, std::pair<int,int> const& v2 ) const
            {
                if ( v1.key() < v2.first )
                    return -1;
                return v1.key() > v2.first ? 1 : 0;
            }
        };

        template <typename T>
        struct equal
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() == v2.key();
            }

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() == v2;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 == v2.key();
            }

            bool operator ()( std::pair<int, int> const& v1, const T& v2 ) const
            {
                return v1.first == v2.key();
            }

            bool operator ()(const T& v1, std::pair<int, int> const& v2 ) const
            {
                return v1.key() == v2.first;
            }
        };

        struct find_functor
        {
            template <typename Item, typename T>
            void operator()( Item& i, T& /*val*/ ) const
            {
                ++i.nFindCount;
            }
            template <typename Item, typename T>
            void operator()( Item& i, T const& /*val*/ ) const
            {
                ++i.nFindCount;
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
            void operator()(Item& i )
            {
                i.nVal = i.nKey * 100;
            }
        };

        template <typename Item, typename Q>
        static void ensure_func( bool bNew, Item& i, Q& /*val*/ )
        {
            if ( bNew )
                ++i.nEnsureNewCount;
            else
                ++i.nEnsureCount;
        }

        struct ensure_functor
        {
            template <typename Item, typename Q>
            void operator()( bool bNew, Item& i, Q& val )
            {
                ensure_func( bNew, i, val );
            }
        };

        template <class Set>
        void test_int()
        {
            Set s( 100, 4 );
            test_int_with( s );

            // extract/get test
            CPPUNIT_ASSERT( s.empty() );
            {
                const int nLimit = 100;
                typename Set::guarded_ptr gp;
                int arrRandom[nLimit];
                for ( int i = 0; i < nLimit; ++i )
                    arrRandom[i] = i;
                shuffle( arrRandom, arrRandom + nLimit );

                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrRandom[i] ));

                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrRandom[i];
                    gp = s.get( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey );
                    gp.release();

                    gp = s.extract( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey );
                    gp.release();
                    CPPUNIT_CHECK( !s.get( nKey ) );

                    gp = s.extract( nKey );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( s.empty() );


                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrRandom[i] ));

                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrRandom[i];
                    gp = s.get_with( other_item( nKey ), other_less() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey );
                    gp.release();

                    gp = s.extract_with( other_item( nKey ), other_less() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey );
                    gp.release();

                    gp = s.get_with( other_item( nKey ), other_less() );
                    CPPUNIT_CHECK( !gp );

                    CPPUNIT_CHECK( !s.extract_with(other_item(nKey), other_less() ));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( s.empty() );
            }

            // iterator test
            test_iter<Set>();
        }

        template <class Set>
        void test_int_rcu()
        {
            Set s( 100, 4 );
            test_int_with( s );

            // extract/get test
            {
                typedef typename Set::gc    rcu;
                typedef typename Set::rcu_lock rcu_lock;
                typedef typename Set::value_type value_type;
                typename Set::exempt_ptr ep;

                static size_t const nLimit = 100;
                int arr[nLimit];
                for ( size_t i = 0; i < nLimit; ++i )
                    arr[i] = (int) i;
                shuffle( arr, arr + nLimit );

                for ( size_t i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arr[i] ));

                for ( size_t i = 0; i < nLimit; i += 2 ) {
                    value_type * pVal;
                    int nKey = arr[i];
                    {
                        rcu_lock l;
                        pVal = s.get( nKey );
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->nKey == nKey );
                        CPPUNIT_CHECK( pVal->nVal == nKey );

                        ep = s.extract( nKey );
                        CPPUNIT_ASSERT( ep );
                        CPPUNIT_ASSERT( !ep.empty() );
                        CPPUNIT_CHECK( pVal->nKey == ep->nKey );
                        CPPUNIT_CHECK( pVal->nVal == (*ep).nVal );
                    }
                    ep.release();
                    {
                        rcu_lock l;
                        CPPUNIT_CHECK( s.get( nKey ) == nullptr );
                        ep = s.extract( nKey );
                        CPPUNIT_CHECK( !ep );
                        CPPUNIT_CHECK( ep.empty() );

                        nKey = arr[i+1];
                        pVal = s.get_with( other_item(nKey), other_less() );
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->nKey == nKey );
                        CPPUNIT_CHECK( pVal->nVal == nKey );

                        ep = s.extract_with( other_item( nKey ), other_less() );
                        CPPUNIT_ASSERT( ep );
                        CPPUNIT_ASSERT( !ep.empty() );
                        CPPUNIT_CHECK( pVal->nKey == ep->nKey );
                        CPPUNIT_CHECK( pVal->nVal == (*ep).nVal );
                    }
                    ep.release();
                    {
                        rcu_lock l;
                        CPPUNIT_CHECK( s.get_with( other_item( nKey ), other_less() ) == nullptr );
                        CPPUNIT_CHECK( !s.extract_with( other_item(nKey), other_less() ));
                        CPPUNIT_CHECK( ep.empty() );
                    }
                }
                CPPUNIT_CHECK( s.empty() );
                CPPUNIT_CHECK( check_size( s, 0 ));
                {
                    rcu_lock l;
                    CPPUNIT_CHECK( s.get( int( nLimit / 2 ) ) == nullptr );
                    ep = s.extract( int( nLimit / 2 ) );
                    CPPUNIT_CHECK( !ep );
                    CPPUNIT_CHECK( ep.empty() );
                }
            }

            // iterator test
            test_iter<Set>();
        }

        template <class Set>
        void test_int_with( Set& s)
        {
            typedef typename Set::value_type    value_type;

            item itm;
            int key;

            // insert/find test
            CPPUNIT_ASSERT( !s.find( 10 ) );
            CPPUNIT_ASSERT( s.insert( 10 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( s.find( 10 ) );

            CPPUNIT_ASSERT( !s.insert( 10 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ));

            CPPUNIT_ASSERT( !s.find_with( 20, less<value_type>() ) );
            CPPUNIT_ASSERT( s.insert( std::make_pair(20, 25) ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( s.find_with( 10, less<value_type>() ) );
            CPPUNIT_ASSERT( s.find( key = 20 ) );
            CPPUNIT_ASSERT( s.find_with( key, less<value_type>(), find_functor() ) );
            {
                copy_found<item> f;
                key = 20;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.nFindCount == 1 );
            }
            CPPUNIT_ASSERT( s.find( key, find_functor() ) );
            {
                copy_found<item> f;
                key = 20;
                CPPUNIT_ASSERT( s.find_with( key, less<value_type>(), std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.nFindCount == 2 );
            }
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));

            CPPUNIT_ASSERT( !s.find( 25 ) );
            CPPUNIT_ASSERT( s.insert( std::make_pair(25, -1), insert_functor() ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            {
                copy_found<item> f;
                key = 25;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 25 );
                CPPUNIT_ASSERT( f.m_found.nVal == 2500 );
            }

            // ensure test
            key = 10;
            {
                copy_found<item> f;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 10 );
                CPPUNIT_ASSERT( f.m_found.nVal == 10 );
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 0 );
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 0 );
            }
            std::pair<bool, bool> ensureResult = s.ensure( key, ensure_functor() );
            CPPUNIT_ASSERT( ensureResult.first && !ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            {
                copy_found<item> f;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 10 );
                CPPUNIT_ASSERT( f.m_found.nVal == 10 );
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 1 );
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 0 );
            }

            ensureResult = s.ensure( std::make_pair(13, 1300), ensure_functor() );
            CPPUNIT_ASSERT( ensureResult.first && ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 4 ));
            {
                copy_found<item> f;
                key = 13;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 13 );
                CPPUNIT_ASSERT( f.m_found.nVal == 1300 );
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 0 );
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 1 );
            }

            // erase test
            CPPUNIT_ASSERT( s.erase(13) );
            CPPUNIT_ASSERT( !s.find( 13 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            CPPUNIT_ASSERT( !s.erase(13) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));

            CPPUNIT_ASSERT( s.find( 10 ));
            CPPUNIT_ASSERT( s.erase_with( 10, less<value_type>() ));
            CPPUNIT_ASSERT( !s.find( 10 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.erase_with(10, less<value_type>()) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));

            CPPUNIT_ASSERT( s.find(20) );
            {
                copy_found<item> f;
                CPPUNIT_ASSERT( s.erase( 20, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );

                CPPUNIT_ASSERT( s.insert(235))
                    CPPUNIT_ASSERT( s.erase_with( 235, less<value_type>(), std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 235 );
                CPPUNIT_ASSERT( f.m_found.nVal == 235 );
            }
            CPPUNIT_ASSERT( !s.find( 20 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ));

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            // emplace test
            CPPUNIT_ASSERT( s.emplace( 151 )) ;  // key = 151,  val = 151
            CPPUNIT_ASSERT( s.emplace( 174, 471 )) ;    // key = 174, val = 471
            CPPUNIT_ASSERT( s.emplace( std::make_pair( 190, 91 ) )) ; // key == 190, val = 91
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));

            CPPUNIT_ASSERT( s.find(151));
            CPPUNIT_ASSERT( s.find_with(174, less<value_type>()));
            CPPUNIT_ASSERT( s.find(190));

            {
                copy_found<item> f;
                key = 151;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 151 );
                CPPUNIT_ASSERT( f.m_found.nVal == 151 );

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
            CPPUNIT_ASSERT( check_size( s, 0 ));
        }


        template <class Set>
        void test_int_nogc()
        {
            typedef typename Set::value_type        value_type;
            typedef typename Set::iterator          iterator;
            typedef typename Set::const_iterator    const_iterator;

            {
                Set s( 52, 4 );
                iterator it;

                CPPUNIT_ASSERT( s.empty() );
                CPPUNIT_ASSERT( check_size( s, 0 ));

                // insert
                it = s.insert( 10 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 10 );
                CPPUNIT_ASSERT( it->val() == 10 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 1 ));
                CPPUNIT_ASSERT( s.insert( 10 ) == s.end() );

                it = s.insert( std::make_pair( 50, 25 ));
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 50 );
                CPPUNIT_ASSERT( it->val() == 25 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 2 ));
                CPPUNIT_ASSERT( s.insert( 50 ) == s.end() );

                // ensure
                std::pair< iterator, bool>  ensureResult;
                ensureResult = s.ensure( 20 );
                CPPUNIT_ASSERT( ensureResult.first != s.end() );
                CPPUNIT_ASSERT( ensureResult.second  );
                CPPUNIT_ASSERT( ensureResult.first->key() == 20 );
                CPPUNIT_ASSERT( ensureResult.first->val() == 20 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                ensureResult = s.ensure( std::make_pair( 20, 200 ));
                CPPUNIT_ASSERT( ensureResult.first != s.end() );
                CPPUNIT_ASSERT( !ensureResult.second  );
                CPPUNIT_ASSERT( ensureResult.first->key() == 20 );
                CPPUNIT_ASSERT( ensureResult.first->val() == 20 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 3 ));
                ensureResult.first->nVal = 22;

                ensureResult = s.ensure( std::make_pair( 30, 33 ));
                CPPUNIT_ASSERT( ensureResult.first != s.end() );
                CPPUNIT_ASSERT( ensureResult.second  );
                CPPUNIT_ASSERT( ensureResult.first->key() == 30 );
                CPPUNIT_ASSERT( ensureResult.first->val() == 33 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 4 ));

                // find
                it = s.find( 10 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 10 );
                CPPUNIT_ASSERT( it->val() == 10 );

                it = s.find_with( 20, less<value_type>() );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 20 );
                CPPUNIT_ASSERT( it->val() == 22 );

                it = s.find( 30 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 30 );
                CPPUNIT_ASSERT( it->val() == 33 );

                it = s.find( 40 );
                CPPUNIT_ASSERT( it == s.end() );

                it = s.find( 50 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 50 );
                CPPUNIT_ASSERT( it->val() == 25 );

                // emplace test
                it = s.emplace( 151 ) ;  // key = 151,  val = 151
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 151 );
                CPPUNIT_ASSERT( it->val() == 151 );

                it = s.emplace( 174, 471 ) ; // key == 174, val = 471
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 174 );
                CPPUNIT_ASSERT( it->val() == 471 );

                it = s.emplace( std::make_pair( 190, 91 )) ; // key == 190, val = 91
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 190 );
                CPPUNIT_ASSERT( it->val() == 91 );

                it = s.find( 174 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 174 );
                CPPUNIT_ASSERT( it->val() == 471 );

                it = s.find_with( 190, less<value_type>() );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 190 );
                CPPUNIT_ASSERT( it->val() == 91 );

                it = s.find( 151 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 151 );
                CPPUNIT_ASSERT( it->val() == 151 );

                //s.clear();
                //CPPUNIT_ASSERT( s.empty() );
                //CPPUNIT_ASSERT( check_size( s, 0 ));
            }

            {
                Set s( 52, 4 );

                // iterator test
                for ( int i = 0; i < 500; ++i ) {
                    CPPUNIT_ASSERT( s.insert( std::make_pair( i, i * 2) ) != s.end() );
                }
                for ( iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    iterator it2 = it;
                    CPPUNIT_CHECK( it2 == it );
                    CPPUNIT_CHECK( it2 != itEnd );
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                    it->nVal = (*it).nKey;
                }

                Set const& refSet = s;
                for ( const_iterator it = refSet.begin(), itEnd = refSet.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                }
            }
        }

        template <class Set>
        void test_int_nogc_unordered()
        {
            typedef typename Set::value_type        value_type;
            typedef typename Set::iterator          iterator;
            typedef typename Set::const_iterator    const_iterator;

            {
                Set s( 52, 4 );
                iterator it;

                CPPUNIT_ASSERT( s.empty() );
                CPPUNIT_ASSERT( check_size( s, 0 ));

                // insert
                it = s.insert( 10 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 10 );
                CPPUNIT_ASSERT( it->val() == 10 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 1 ));
                CPPUNIT_ASSERT( s.insert( 10 ) == s.end() );

                it = s.insert( std::make_pair( 50, 25 ));
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 50 );
                CPPUNIT_ASSERT( it->val() == 25 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 2 ));
                CPPUNIT_ASSERT( s.insert( 50 ) == s.end() );

                // ensure
                std::pair< iterator, bool>  ensureResult;
                ensureResult = s.ensure( 20 );
                CPPUNIT_ASSERT( ensureResult.first != s.end() );
                CPPUNIT_ASSERT( ensureResult.second  );
                CPPUNIT_ASSERT( ensureResult.first->key() == 20 );
                CPPUNIT_ASSERT( ensureResult.first->val() == 20 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                ensureResult = s.ensure( std::make_pair( 20, 200 ));
                CPPUNIT_ASSERT( ensureResult.first != s.end() );
                CPPUNIT_ASSERT( !ensureResult.second  );
                CPPUNIT_ASSERT( ensureResult.first->key() == 20 );
                CPPUNIT_ASSERT( ensureResult.first->val() == 20 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 3 ));
                ensureResult.first->nVal = 22;

                ensureResult = s.ensure( std::make_pair( 30, 33 ));
                CPPUNIT_ASSERT( ensureResult.first != s.end() );
                CPPUNIT_ASSERT( ensureResult.second  );
                CPPUNIT_ASSERT( ensureResult.first->key() == 30 );
                CPPUNIT_ASSERT( ensureResult.first->val() == 33 );
                CPPUNIT_ASSERT( !s.empty() );
                CPPUNIT_ASSERT( check_size( s, 4 ));

                // find
                it = s.find( 10 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 10 );
                CPPUNIT_ASSERT( it->val() == 10 );

                it = s.find_with( 20, equal<value_type>() );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 20 );
                CPPUNIT_ASSERT( it->val() == 22 );

                it = s.find( 30 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 30 );
                CPPUNIT_ASSERT( it->val() == 33 );

                it = s.find( 40 );
                CPPUNIT_ASSERT( it == s.end() );

                it = s.find( 50 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 50 );
                CPPUNIT_ASSERT( it->val() == 25 );

                // emplace test
                it = s.emplace( 151 ) ;  // key = 151,  val = 151
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 151 );
                CPPUNIT_ASSERT( it->val() == 151 );

                it = s.emplace( 174, 471 ) ; // key == 174, val = 471
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 174 );
                CPPUNIT_ASSERT( it->val() == 471 );

                it = s.emplace( std::make_pair( 190, 91 )) ; // key == 190, val = 91
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 190 );
                CPPUNIT_ASSERT( it->val() == 91 );

                it = s.find( 174 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 174 );
                CPPUNIT_ASSERT( it->val() == 471 );

                it = s.find_with( 190, equal<value_type>() );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 190 );
                CPPUNIT_ASSERT( it->val() == 91 );

                it = s.find( 151 );
                CPPUNIT_ASSERT( it != s.end() );
                CPPUNIT_ASSERT( it->key() == 151 );
                CPPUNIT_ASSERT( it->val() == 151 );

                //s.clear();
                //CPPUNIT_ASSERT( s.empty() );
                //CPPUNIT_ASSERT( check_size( s, 0 ));
            }

            {
                Set s( 52, 4 );

                // iterator test
                for ( int i = 0; i < 500; ++i ) {
                    CPPUNIT_ASSERT( s.insert( std::make_pair( i, i * 2) ) != s.end() );
                }
                for ( iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    iterator it2 = it;
                    CPPUNIT_CHECK( it2 == it );
                    CPPUNIT_CHECK( it2 != itEnd );
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                    it->nVal = (*it).nKey;
                }

                Set const& refSet = s;
                for ( const_iterator it = refSet.begin(), itEnd = refSet.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                }
            }
        }
        template <class Set>
        void test_iter()
        {
            typedef typename Set::value_type        value_type;
            typedef typename Set::iterator          iterator;
            typedef typename Set::const_iterator    const_iterator;

            Set s( 100, 4 );

            const size_t nMaxCount = 500;
            for ( int i = 0; size_t(i) < nMaxCount; ++i ) {
                CPPUNIT_ASSERT( s.insert( std::make_pair( i, i * 2) ));
            }

            {
                typename Set::iterator it( s.begin() );
                typename Set::const_iterator cit( s.cbegin() );
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

            size_t nCount = 0;
            for ( iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT_EX( (*it).nKey * 2 == it->nVal,
                    "Step " << nCount << ": Iterator key=" << it->nKey <<  ", value expected=" << it->nKey * 2 << ", value real=" << it->nVal
                    );
                it->nVal = (*it).nKey;
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == nMaxCount );

            nCount = 0;
            Set const& refSet = s;
            for ( const_iterator it = refSet.begin(), itEnd = refSet.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT_EX( (*it).nKey == it->nVal,
                    "Step " << nCount << ": Iterator key=" << it->nKey <<  ", value expected=" << it->nKey << ", value real=" << it->nVal
                    );
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

        void Michael_RCU_GPT_cmp();
        void Michael_RCU_GPT_less();
        void Michael_RCU_GPT_cmpmix();

        void Michael_RCU_GPB_cmp();
        void Michael_RCU_GPB_less();
        void Michael_RCU_GPB_cmpmix();

        void Michael_RCU_SHT_cmp();
        void Michael_RCU_SHT_less();
        void Michael_RCU_SHT_cmpmix();

        void Michael_RCU_SHB_cmp();
        void Michael_RCU_SHB_less();
        void Michael_RCU_SHB_cmpmix();

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

        CPPUNIT_TEST_SUITE(HashSetHdrTest)
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

            CPPUNIT_TEST(Michael_RCU_GPT_cmp)
            CPPUNIT_TEST(Michael_RCU_GPT_less)
            CPPUNIT_TEST(Michael_RCU_GPT_cmpmix)

            CPPUNIT_TEST(Michael_RCU_SHB_cmp)
            CPPUNIT_TEST(Michael_RCU_SHB_less)
            CPPUNIT_TEST(Michael_RCU_SHB_cmpmix)

            CPPUNIT_TEST(Michael_RCU_SHT_cmp)
            CPPUNIT_TEST(Michael_RCU_SHT_less)
            CPPUNIT_TEST(Michael_RCU_SHT_cmpmix)

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

} // namespace set

#endif // CDSTEST_HDR_SET_H

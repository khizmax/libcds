/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#ifndef CDSTEST_HDR_MICHAEL_H
#define CDSTEST_HDR_MICHAEL_H

#include "cppunit/cppunit_proxy.h"
#include <cds/container/details/michael_list_base.h>

namespace ordlist {
    namespace cc = cds::container;
    namespace co = cds::container::opt;

    class MichaelListTestHeader: public CppUnitMini::TestCase
    {
    public:
        struct stat {
            int nUpdateExistsCall;
            int nUpdateNewCall;

            stat()
            {
                nUpdateExistsCall
                    = nUpdateNewCall
                    = 0;
            }
        };

        struct item {
            int     nKey;
            int     nVal;

            stat    s;

            item(int key)
                : nKey( key )
                , nVal( key * 2 )
                , s()
            {}

            item(int key, int val)
                : nKey( key )
                , nVal(val)
                , s()
            {}

            item( const item& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
                , s()
            {}

            int key() const
            {
                return nKey;
            }
        };

        template <typename T>
        struct lt
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
        };

        struct insert_functor {
            void operator ()( item& i )
            {
                i.nVal = i.nKey * 1033;
            }
        };
        struct dummy_insert_functor {
            void operator ()( item& /*i*/ )
            {
                // This functor should not be called
                TestCase::current_test()->error( "CPPUNIT_ASSERT", "dummy_insert_functor should not be called", __FILE__, __LINE__ );
            }
        };

        struct erase_functor {
            unsigned int nEraseCall;

            erase_functor()
                : nEraseCall(0)
            {}

            void operator()( item const& /*i*/)
            {
                ++nEraseCall;
            }
        };

        static void insert_function( item& i )
        {
            i.nVal = i.nKey * 1024;
        }
        static void dummy_insert_function( item& /*i*/ )
        {
            // This function should not be called
            TestCase::current_test()->error( "CPPUNIT_ASSERT", "dummy_insert_function should not be called", __FILE__, __LINE__ );
        }


        struct check_value {
            unsigned int m_nMultiplier;

            check_value( unsigned int nMultiplier )
                : m_nMultiplier( nMultiplier )
            {}

            check_value( const check_value& s )
                : m_nMultiplier( s.m_nMultiplier )
            {}

            void operator()( item& i, int )
            {
                CPPUNIT_ASSERT_CURRENT( int(i.nKey * m_nMultiplier) == i.nVal );
            }
        };

        struct check_exact_value {
            int m_nExpected;

            check_exact_value( int nExpected )
                : m_nExpected( nExpected )
            {}

            check_exact_value( check_exact_value const& s)
                : m_nExpected( s.m_nExpected )
            {}

            void operator()( item& i, int )
            {
                CPPUNIT_ASSERT_CURRENT( i.nVal == m_nExpected );
            }
        };

        struct dummy_check_value {
            void operator()( item& /*i*/, int )
            {
                // This functor should not be called
                TestCase::current_test()->error( "CPPUNIT_ASSERT", "dummy_check_value should not be called", __FILE__, __LINE__ );
            }
        };

        struct update_functor {
            void operator()( bool /*bNew*/, item& i, int /*n*/ )
            {
                i.nVal = i.nKey * 1024;
            }
        };

        static void update_func( bool /*bNew*/, item& i, int n )
        {
            i.nVal = n * 1033;
        }

        struct other_item
        {
            int nKey;

            other_item()
            {}

            other_item(int n)
                : nKey(n)
            {}
        };

        struct other_less
        {
            template <typename T1, typename T2>
            bool operator()( T1 const& t1, T2 const& t2 ) const
            {
                return t1.nKey < t2.nKey;
            }
        };

    protected:
        template <class OrdList>
        void test_with( OrdList& l )
        {
            typedef typename OrdList::value_type    value_type;

            // The list should be empty
            CPPUNIT_ASSERT( l.empty() );

            // insert test
            CPPUNIT_ASSERT( l.insert( 50 ) );
            CPPUNIT_ASSERT( l.insert( item( 25 )) );
            CPPUNIT_ASSERT( l.insert( item( 100 )) );

            // insert failed - such key exists
            CPPUNIT_ASSERT( !l.insert( 50 ) );
            CPPUNIT_ASSERT( !l.insert( item( 100 )) );

            // clear test

            // The list should not be empty
            CPPUNIT_ASSERT( !l.empty() );
            l.clear();
            // and now the list is empty
            CPPUNIT_ASSERT( l.empty() );

            // Test insert with functor

            CPPUNIT_ASSERT( l.insert( 100, insert_functor() ) );
            // passed by ref
            {
                insert_functor f;
                CPPUNIT_ASSERT( l.insert( item( 25 ), std::ref( f ) ) );
                CPPUNIT_ASSERT( !l.insert( item( 100 ), std::ref( f ) ) );
            }
            // Test insert with function
            CPPUNIT_ASSERT( l.insert( 50, insert_function ));
            CPPUNIT_ASSERT( !l.insert( 25, dummy_insert_function ));
            CPPUNIT_ASSERT( !l.insert( 100, dummy_insert_functor() ));

            // The list should not be empty
            CPPUNIT_ASSERT( !l.empty() );

            // Check inserted values
            {
                int i;
                i = 100;
                CPPUNIT_ASSERT( l.contains( 100 ));
                CPPUNIT_ASSERT( l.find( i, check_value(1033) ));
                {
                    check_value f(1033);
                    i = 25;
                    CPPUNIT_ASSERT( l.contains( 25, lt<value_type>() ));
                    CPPUNIT_ASSERT( l.find_with( i, lt<value_type>(), std::ref( f ) ) );
                }
                i = 50;
                CPPUNIT_ASSERT( l.contains( 50 ));
                CPPUNIT_ASSERT( l.find( i, check_value(1024) ));

                i = 10;
                CPPUNIT_ASSERT( !l.contains( 10, lt<value_type>() ));
                CPPUNIT_ASSERT( !l.find_with( i, lt<value_type>(), dummy_check_value() ));
                i = 75;
                CPPUNIT_ASSERT( !l.contains( 75 ));
                CPPUNIT_ASSERT( !l.find( i, dummy_check_value() ));
                i = 150;
                CPPUNIT_ASSERT( !l.contains( 150 ));
                CPPUNIT_ASSERT( !l.find( i, dummy_check_value() ));
            }

            // The list should not be empty
            CPPUNIT_ASSERT( !l.empty() );
            l.clear();
            // and now the list is empty
            CPPUNIT_ASSERT( l.empty() );

            // Update test
            {
                std::pair<bool, bool>   updateResult;
                update_functor f;
                updateResult = l.update( 100, update_functor() );
                CPPUNIT_ASSERT( updateResult.first );
                CPPUNIT_ASSERT( updateResult.second );

                updateResult = l.update( 200, std::ref( f ) );
                CPPUNIT_ASSERT( updateResult.first );
                CPPUNIT_ASSERT( updateResult.second );

                updateResult = l.update( 50, update_func );
                CPPUNIT_ASSERT( updateResult.first );
                CPPUNIT_ASSERT( updateResult.second );

                int i;
                i = 100;
                CPPUNIT_ASSERT( l.find( i, check_value(1024) ));
                i = 50;
                CPPUNIT_ASSERT( l.find( i, check_value(1033) ));
                i = 200;
                CPPUNIT_ASSERT( l.find( i, check_value(1024) ));

                // update existing key
                updateResult = l.update( 200, update_func );
                CPPUNIT_ASSERT( updateResult.first );
                CPPUNIT_ASSERT( !updateResult.second );
                i = 200;
                CPPUNIT_ASSERT( l.find( i, check_value(1033) ));

                updateResult = l.update( 50, update_functor() );
                CPPUNIT_ASSERT( updateResult.first );
                CPPUNIT_ASSERT( !updateResult.second );
                i = 50;
                CPPUNIT_ASSERT( l.find( i, check_value(1024) ));
            }

            // erase test (list: 50, 100, 200)
            CPPUNIT_ASSERT( !l.empty() );
            CPPUNIT_ASSERT( l.insert(160));
            CPPUNIT_ASSERT( l.insert(250));
            CPPUNIT_ASSERT( !l.empty() );

            CPPUNIT_ASSERT( !l.erase( 150 ));

            CPPUNIT_ASSERT( l.erase( 100 ));
            CPPUNIT_ASSERT( !l.erase( 100 ));

            CPPUNIT_ASSERT( l.erase_with( 200, lt<value_type>() ));
            CPPUNIT_ASSERT( !l.erase_with( 200, lt<value_type>() ));

            {
                erase_functor ef;
                CPPUNIT_ASSERT( ef.nEraseCall == 0 );
                CPPUNIT_ASSERT( l.erase_with( 160, lt<value_type>(), std::ref(ef) ));
                CPPUNIT_ASSERT( ef.nEraseCall == 1 );
                CPPUNIT_ASSERT( !l.erase_with( 160, lt<value_type>(), std::ref(ef) ));
                CPPUNIT_ASSERT( ef.nEraseCall == 1 );

                CPPUNIT_ASSERT( l.erase( 250, std::ref(ef) ));
                CPPUNIT_ASSERT( ef.nEraseCall == 2 );
                CPPUNIT_ASSERT( !l.erase( 250, std::ref(ef) ));
                CPPUNIT_ASSERT( ef.nEraseCall == 2 );
            }

            CPPUNIT_ASSERT( l.erase( 50 ));
            CPPUNIT_ASSERT( !l.erase( 50 ));

            CPPUNIT_ASSERT( l.empty() );

            // clear empty list
            l.clear();
            CPPUNIT_ASSERT( l.empty() );

            {
                int i;

                // insert test
                CPPUNIT_ASSERT( l.emplace( 501 ) );
                CPPUNIT_ASSERT( l.emplace( 251, 152 ));
                CPPUNIT_ASSERT( l.emplace( item( 1001 )) );

                // insert failed - such key exists
                CPPUNIT_ASSERT( !l.emplace( 501, 2 ) );
                CPPUNIT_ASSERT( !l.emplace( 251, 10) );

                i = 501;
                CPPUNIT_ASSERT( l.find( i, check_exact_value(501*2) ));
                i = 251;
                CPPUNIT_ASSERT( l.find( i, check_exact_value(152) ));
                i = 1001;
                CPPUNIT_ASSERT( l.find( i, check_exact_value(1001*2) ));

                l.clear();
                CPPUNIT_ASSERT( l.empty() );
            }

            // Iterator test
            {
                int nCount = 100;
                for ( int i = 0; i < nCount; ++i )
                    CPPUNIT_ASSERT( l.insert(i) );

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
                    it->nVal = i * 2;
                    CPPUNIT_ASSERT( it->nKey == i );
                }

                // Check that we have visited all items
                for ( int i = 0; i < nCount; ++i )
                    CPPUNIT_ASSERT( l.find( i, check_value(2) ));

                l.clear();
                CPPUNIT_ASSERT( l.empty() );

                // Const iterator
                for ( int i = 0; i < nCount; ++i )
                    CPPUNIT_ASSERT( l.insert(i) );

                i = 0;
                const OrdList& rl = l;
                for ( typename OrdList::const_iterator it = rl.begin(), itEnd = rl.end(); it != itEnd; ++it, ++i ) {
                    // it->nVal = i * 2    ;    // not!
                    CPPUNIT_ASSERT( it->nKey == i );
                }

                // Check that we have visited all items
                for ( int i = 0; i < nCount; ++i )
                    CPPUNIT_ASSERT( l.find( i, check_value(2) ));

                l.clear();
                CPPUNIT_ASSERT( l.empty() );
            }
        }

        template <typename OrdList>
        void test()
        {
            typedef typename OrdList::guarded_ptr guarded_ptr;
            typedef typename OrdList::value_type value_type;

            OrdList l;
            test_with(l);

            static int const nLimit = 20;
            int arr[nLimit];
            for ( int i = 0; i < nLimit; i++ )
                arr[i] = i;
            shuffle( arr, arr + nLimit );

            // extract/get
            for ( int i = 0; i < nLimit; ++i )
                l.insert( arr[i] );
            {
                guarded_ptr gp;
                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arr[i];

                    gp = l.get( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey * 2 );
                    gp.release();

                    gp = l.extract( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey*2 );
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
                l.insert( arr[i] );
            {
                guarded_ptr gp;
                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arr[i];
                    other_item key( nKey );

                    gp = l.get_with( key, other_less() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey * 2 );
                    gp.release();

                    gp = l.extract_with( key, other_less() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey*2 );
                    gp.release();

                    gp = l.get_with( key, other_less() );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_CHECK( gp.empty());
                    CPPUNIT_CHECK( !l.extract_with( key, other_less()));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( l.empty());
                CPPUNIT_CHECK( !l.get_with(other_item(arr[0]), other_less()));
                CPPUNIT_CHECK( gp.empty());
                CPPUNIT_CHECK( !l.extract_with( other_item(arr[0]), other_less()));
                CPPUNIT_CHECK( gp.empty());
            }
        }

        template <typename OrdList>
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
                    CPPUNIT_ASSERT( l.insert( a[i] ) );

                typename OrdList::exempt_ptr ep;
                typename OrdList::raw_ptr    rp;

                for ( int i = 0; i < nLimit; ++i ) {
                    {
                        rcu_lock lock;
                        rp = l.get( a[i] );
                        CPPUNIT_ASSERT( rp );
                        CPPUNIT_CHECK( rp->nKey == a[i] );
                        CPPUNIT_CHECK( rp->nVal == a[i] * 2 );
                    }
                    rp.release();

                    ep = l.extract( a[i] );
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->nKey == a[i] );
                    CPPUNIT_CHECK( (*ep).nVal == a[i] * 2 );
                    ep.release();

                    {
                        rcu_lock lock;
                        CPPUNIT_CHECK( !l.get( a[i] ));
                    }
                    ep = l.extract( a[i] );
                    CPPUNIT_CHECK( !ep );
                    CPPUNIT_CHECK( ep.empty() );
                }
                CPPUNIT_ASSERT( l.empty() );

                {
                    rcu_lock lock;
                    CPPUNIT_CHECK( !l.get( a[0] ));
                }
                CPPUNIT_CHECK( !l.extract( a[0] ) );
                CPPUNIT_CHECK( ep.empty() );

                // extract_with/get_with
                for ( int i = 0; i < nLimit; ++i ) {
                    CPPUNIT_ASSERT( l.insert( a[i] ) );
                }

                for ( int i = 0; i < nLimit; ++i ) {
                    other_item itm( a[i] );
                    {
                        rcu_lock lock;
                        rp = l.get_with( itm, other_less() );
                        CPPUNIT_ASSERT( rp );
                        CPPUNIT_CHECK( rp->nKey == a[i] );
                        CPPUNIT_CHECK( rp->nVal == a[i] * 2 );
                    }
                    rp.release();

                    ep = l.extract_with( itm, other_less() );
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->nKey == a[i] );
                    CPPUNIT_CHECK( ep->nVal == a[i] * 2 );
                    ep.release();

                    {
                        rcu_lock lock;
                        CPPUNIT_CHECK( !l.get_with( itm, other_less()));
                    }
                    ep = l.extract_with( itm, other_less() );
                    CPPUNIT_CHECK( !ep );
                    CPPUNIT_CHECK( ep.empty() );
                }
                CPPUNIT_ASSERT( l.empty() );

                {
                    rcu_lock lock;
                    CPPUNIT_CHECK( !l.get_with( other_item( 0 ), other_less()));
                }
                CPPUNIT_CHECK( !l.extract_with( other_item(0), other_less() ));
                CPPUNIT_CHECK( ep.empty() );
            }

        }

        template <class OrdList>
        void nogc_test()
        {
            typedef OrdList list;
            typedef typename list::value_type    value_type;
            typedef std::pair<typename list::iterator, bool> update_result;

            typename list::iterator it;

            list l;
            CPPUNIT_ASSERT( l.empty() );
            CPPUNIT_ASSERT( l.insert(50) != l.end() );
            CPPUNIT_ASSERT( !l.empty() );

            update_result eres = l.update( item(100, 33) );
            CPPUNIT_ASSERT( eres.second );
            CPPUNIT_ASSERT( eres.first != l.end() );
            CPPUNIT_ASSERT( l.insert( item(150) ) != l.end() );

            CPPUNIT_ASSERT( l.insert(100) == l.end() );
            eres = l.update( item(50, 33) );
            CPPUNIT_ASSERT( !eres.second );
            CPPUNIT_ASSERT( eres.first->nVal == eres.first->nKey * 2 );
            eres.first->nVal = 63;

            it = l.contains( 33 );
            CPPUNIT_ASSERT( it == l.end() );

            it = l.contains( 50 );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 50 );
            CPPUNIT_ASSERT( it->nVal == 63 );

            it = l.contains( 100, lt<value_type>() );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 100 );
            CPPUNIT_ASSERT( it->nVal == 33 );

            it = l.contains( 150 );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 150 );
            CPPUNIT_ASSERT( it->nVal == it->nKey * 2 );

            CPPUNIT_ASSERT( !l.empty() );
            l.clear();
            CPPUNIT_ASSERT( l.empty() );

            // insert test
            CPPUNIT_ASSERT( l.emplace( 501 ) != l.end() );
            CPPUNIT_ASSERT( l.emplace( 251, 152 ) != l.end());
            CPPUNIT_ASSERT( l.emplace( item( 1001 )) != l.end() );

            // insert failed - such key exists
            CPPUNIT_ASSERT( l.emplace( 501, 2 ) == l.end() );
            CPPUNIT_ASSERT( l.emplace( 251, 10) == l.end() );

            it = l.contains( 501 );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 501 );
            CPPUNIT_ASSERT( it->nVal == 501 * 2 );

            it = l.contains( 251 );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 251 );
            CPPUNIT_ASSERT( it->nVal == 152 );

            it = l.contains( 1001 );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 1001 );
            CPPUNIT_ASSERT( it->nVal == 1001 * 2 );

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


            l.clear();
            CPPUNIT_ASSERT( l.empty() );
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

        CPPUNIT_TEST_SUITE(MichaelListTestHeader)
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

#endif // #ifndef CDSTEST_HDR_MICHAEL_H

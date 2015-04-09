//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRUSIVE_MICHAEL_H
#define CDSTEST_HDR_INTRUSIVE_MICHAEL_H

#include "cppunit/cppunit_proxy.h"
#include <cds/intrusive/details/michael_list_base.h>

namespace ordlist {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    class IntrusiveMichaelListHeaderTest: public CppUnitMini::TestCase
    {
    public:

        struct stat {
            int nDisposeCount;
            int nEnsureExistsCall;
            int nEnsureNewCall;
            int nFindCall;
            int nEraseCall;

            stat()
                : nDisposeCount(0)
                , nEnsureExistsCall(0)
                , nEnsureNewCall(0)
                , nFindCall(0)
                , nEraseCall(0)
            {}

            stat( const stat& s )
            {
                *this = s;
            }

            stat& operator =(const stat& s)
            {
                memcpy( this, &s, sizeof(s));
                return *this;
            }
        };

        template <typename GC>
        struct base_int_item: public ci::michael_list::node< GC >
        {
            int nKey;
            int nVal;

            mutable stat    s;

            base_int_item()
            {}

            base_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
                , s()
            {}

            base_int_item(const base_int_item& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
                , s()
            {}

            const int& key() const
            {
                return nKey;
            }
        };

        template <typename GC>
        struct member_int_item
        {
            int nKey;
            int nVal;

            ci::michael_list::node< GC > hMember;

            mutable stat s;

            member_int_item()
            {}

            member_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
                , s()
            {}

            member_int_item(const member_int_item& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
                , s()
            {}

            const int& key() const
            {
                return nKey;
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
        };

        struct other_item {
            int nKey;

            other_item( int n )
                : nKey(n)
            {}
        };

        struct other_less {
            template <typename T, typename Q>
            bool operator()( T const& i1, Q const& i2) const
            {
                return i1.nKey < i2.nKey;
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

        struct faked_disposer
        {
            template <typename T>
            void operator ()( T * p )
            {
                ++p->s.nDisposeCount;
            }
        };

        struct ensure_functor
        {
            template <typename T>
            void operator ()(bool bNew, T& item, T& /*val*/ )
            {
                if ( bNew )
                    ++item.s.nEnsureNewCall;
                else
                    ++item.s.nEnsureExistsCall;
            }
        };

        struct find_functor
        {
            template <typename T, typename Q>
            void operator ()( T& item, Q& /*val*/ )
            {
                ++item.s.nFindCall;
            }
        };

        struct erase_functor
        {
            template <typename T>
            void operator()( T const& item )
            {
                item.s.nEraseCall++;
            }
        };

        template <class OrdList>
        void test_int_common()
        {
            typedef typename OrdList::value_type    value_type;

            value_type v1( 10, 50 );
            value_type v2( 5, 25  );
            value_type v3( 20, 100 );
            {
                OrdList l;
                CPPUNIT_ASSERT( l.empty() );

                CPPUNIT_ASSERT( l.insert( v1 ))     ;   // true
                CPPUNIT_ASSERT( l.find( v1.key() ));

                CPPUNIT_ASSERT( v1.s.nFindCall == 0 );
                CPPUNIT_ASSERT( l.find( v1.key(), find_functor() ));
                CPPUNIT_ASSERT( v1.s.nFindCall == 1 );

                CPPUNIT_ASSERT( !l.find( v2.key() ));
                CPPUNIT_ASSERT( !l.find_with( v3.key(), less<value_type>() ));
                CPPUNIT_ASSERT( !l.empty() );

                CPPUNIT_ASSERT( !l.insert( v1 ))    ;   // assertion "is_empty" is not raised since pNext is nullptr

                {
                    value_type v( v1 );
                    CPPUNIT_ASSERT( !l.insert( v )) ;   // false
                }

                std::pair<bool, bool> ret = l.ensure( v2, ensure_functor() );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v2.s.nEnsureNewCall == 1 );
                CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 0 );

                //CPPUNIT_ASSERT( !l.insert( v2 ))    ;   // assertion "is_empty"

                CPPUNIT_ASSERT( l.find_with( v1.key(), less<value_type>() )) ;   // true

                CPPUNIT_ASSERT( v1.s.nFindCall == 1 );
                CPPUNIT_ASSERT( l.find_with( v1.key(), less<value_type>(), find_functor() ));
                CPPUNIT_ASSERT( v1.s.nFindCall == 2 );

                CPPUNIT_ASSERT( l.find( v2.key() ));

                CPPUNIT_ASSERT( v2.s.nFindCall == 0 );
                CPPUNIT_ASSERT( l.find( v2.key(), find_functor() ));
                CPPUNIT_ASSERT( v2.s.nFindCall == 1 );

                CPPUNIT_ASSERT( !l.find( v3.key() ));

                {
                    CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 0 );
                    CPPUNIT_ASSERT( v2.s.nEnsureNewCall == 1 );

                    value_type v( v2 );
                    ret = l.ensure( v, ensure_functor() );

                    CPPUNIT_ASSERT( ret.first );
                    CPPUNIT_ASSERT( !ret.second );
                    CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 1 );
                    CPPUNIT_ASSERT( v2.s.nEnsureNewCall == 1 );
                    CPPUNIT_ASSERT( v.s.nEnsureExistsCall == 0 );
                    CPPUNIT_ASSERT( v.s.nEnsureNewCall == 0 );
                }

                CPPUNIT_ASSERT( !l.empty() );

                CPPUNIT_ASSERT( l.insert( v3 ))     ;   // true
                CPPUNIT_ASSERT( l.find( v3.key() ));

                CPPUNIT_ASSERT( v3.s.nFindCall == 0 );
                CPPUNIT_ASSERT( l.find( v3.key(), find_functor() ));
                CPPUNIT_ASSERT( v3.s.nFindCall == 1 );

                CPPUNIT_ASSERT( l.unlink( v2 ) );
                CPPUNIT_ASSERT( l.find( v1.key() )) ;   // true
                CPPUNIT_ASSERT( !l.find( v2.key() )) ;   // true
                CPPUNIT_ASSERT( l.find( v3.key() )) ;   // true
                CPPUNIT_ASSERT( !l.empty() );
                CPPUNIT_ASSERT( !l.unlink( v2 ) );

                {
                    // v1 key is in the list but v NODE is not in the list
                    value_type v( v1 );
                    CPPUNIT_ASSERT( !l.unlink( v ) );
                }

                CPPUNIT_ASSERT( l.unlink( v1 ) );
                CPPUNIT_ASSERT( !l.unlink( v1 ) );
                CPPUNIT_ASSERT( !l.find( v1.key() ));
                CPPUNIT_ASSERT( !l.find( v2.key() ));
                CPPUNIT_ASSERT( l.find( v3.key() ));
                CPPUNIT_ASSERT( !l.empty() );
                CPPUNIT_ASSERT( !l.unlink( v1 ) );
                CPPUNIT_ASSERT( !l.unlink( v2 ) );

                CPPUNIT_ASSERT( l.unlink( v3 ) );
                CPPUNIT_ASSERT( !l.find_with( v1.key(), less<value_type>() ));
                CPPUNIT_ASSERT( !l.find_with( v2.key(), less<value_type>(), find_functor() ));
                CPPUNIT_ASSERT( !l.find( v3.key(), find_functor() ));
                CPPUNIT_ASSERT( l.empty() );
                CPPUNIT_ASSERT( !l.unlink( v1 ) );
                CPPUNIT_ASSERT( !l.unlink( v2 ) );
                CPPUNIT_ASSERT( !l.unlink( v3 ) );

                // Apply retired pointer to clean links
                OrdList::gc::force_dispose();

                stat s( v3.s );
                ret = l.ensure( v3, ensure_functor() );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v3.s.nEnsureNewCall == s.nEnsureNewCall + 1);
                CPPUNIT_ASSERT( v3.s.nEnsureExistsCall == s.nEnsureExistsCall );
                CPPUNIT_ASSERT( !l.empty() );

                s = v2.s;
                ret = l.ensure( v2, ensure_functor() );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v2.s.nEnsureNewCall == s.nEnsureNewCall + 1);
                CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == s.nEnsureExistsCall );
                CPPUNIT_ASSERT( !l.empty() );

                s = v1.s;
                ret = l.ensure( v1, ensure_functor() );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v1.s.nEnsureNewCall == s.nEnsureNewCall + 1);
                CPPUNIT_ASSERT( v1.s.nEnsureExistsCall == s.nEnsureExistsCall );
                CPPUNIT_ASSERT( !l.empty() );

                // Erase test
                CPPUNIT_ASSERT( v1.s.nEraseCall == 0 );
                CPPUNIT_ASSERT( l.erase( v1.key(), erase_functor()) );
                CPPUNIT_ASSERT( v1.s.nEraseCall == 1 );
                //CPPUNIT_ASSERT( v1.s.nDisposeCount == 0 );
                CPPUNIT_ASSERT( !l.empty() );

                CPPUNIT_ASSERT( l.erase_with( v2.key(), less<value_type>() ) );
                CPPUNIT_ASSERT( !l.erase( v2.key()));
                //CPPUNIT_ASSERT( v2.s.nDisposeCount == 0 );
                CPPUNIT_ASSERT( !l.empty() );

                CPPUNIT_ASSERT( v2.s.nEraseCall == 0 );
                CPPUNIT_ASSERT( !l.erase( v2, erase_functor() ));
                CPPUNIT_ASSERT( v2.s.nEraseCall == 0 );
                CPPUNIT_ASSERT( !l.erase( v1 ));
                //CPPUNIT_ASSERT( v2.s.nDisposeCount == 0 );
                CPPUNIT_ASSERT( !l.empty() );

                CPPUNIT_ASSERT( v3.s.nEraseCall == 0 );
                CPPUNIT_ASSERT( l.erase_with( v3, less<value_type>(), erase_functor() ));
                CPPUNIT_ASSERT( v3.s.nEraseCall == 1 );
                //CPPUNIT_ASSERT( v3.s.nDisposeCount == 0 );
                CPPUNIT_ASSERT( l.empty() );

                // Apply retired pointer to clean links
                OrdList::gc::force_dispose();

                // Unlink test
                CPPUNIT_ASSERT( l.insert( v1 ));
                CPPUNIT_ASSERT( l.insert( v3 ));
                CPPUNIT_ASSERT( !l.empty() );
                CPPUNIT_ASSERT( !l.unlink( v2 ));
                CPPUNIT_ASSERT( l.unlink( v1 ));
                CPPUNIT_ASSERT( !l.unlink( v1 ));
                CPPUNIT_ASSERT( l.unlink( v3 ));
                CPPUNIT_ASSERT( !l.unlink( v3 ));
                CPPUNIT_ASSERT( l.empty() );

                // Apply retired pointer
                OrdList::gc::force_dispose();
                CPPUNIT_ASSERT( v1.s.nDisposeCount == 3 );
                CPPUNIT_ASSERT( v2.s.nDisposeCount == 2 );
                CPPUNIT_ASSERT( v3.s.nDisposeCount == 3 );

                // Destructor test (call disposer)
                CPPUNIT_ASSERT( l.insert( v1 ));
                CPPUNIT_ASSERT( l.insert( v3 ));
                CPPUNIT_ASSERT( l.insert( v2 ));

                // Iterator test
                // begin/end
                {
                    typename OrdList::iterator it = l.begin();
                    typename OrdList::const_iterator cit = l.cbegin();
                    CPPUNIT_ASSERT( it != l.end() );
                    CPPUNIT_ASSERT( it != l.cend() );
                    CPPUNIT_ASSERT( cit != l.end() );
                    CPPUNIT_ASSERT( cit != l.cend() );
                    CPPUNIT_ASSERT( cit == it );

                    CPPUNIT_ASSERT( it->nKey == v2.nKey );
                    CPPUNIT_ASSERT( it->nVal == v2.nVal );
                    CPPUNIT_ASSERT( ++it != l.end() );
                    CPPUNIT_ASSERT( it->nKey == v1.nKey );
                    CPPUNIT_ASSERT( it->nVal == v1.nVal );
                    CPPUNIT_ASSERT( ++it != l.end() );
                    CPPUNIT_ASSERT( it->nKey == v3.nKey );
                    CPPUNIT_ASSERT( it->nVal == v3.nVal );
                    CPPUNIT_ASSERT( ++it == l.end() );
                }

                // cbegin/cend
                {
                    typename OrdList::const_iterator it = l.cbegin();
                    CPPUNIT_ASSERT( it != l.cend() );
                    CPPUNIT_ASSERT( it->nKey == v2.nKey );
                    CPPUNIT_ASSERT( it->nVal == v2.nVal );
                    CPPUNIT_ASSERT( ++it != l.cend() );
                    CPPUNIT_ASSERT( it->nKey == v1.nKey );
                    CPPUNIT_ASSERT( it->nVal == v1.nVal );
                    CPPUNIT_ASSERT( ++it != l.cend() );
                    CPPUNIT_ASSERT( it->nKey == v3.nKey );
                    CPPUNIT_ASSERT( it->nVal == v3.nVal );
                    CPPUNIT_ASSERT( ++it == l.cend() );
                }

                // const begin/end
                {
                    OrdList const & lref = l;
                    typename OrdList::const_iterator it = lref.begin();
                    CPPUNIT_ASSERT( it != l.end() );
                    CPPUNIT_ASSERT( it->nKey == v2.nKey );
                    CPPUNIT_ASSERT( it->nVal == v2.nVal );
                    CPPUNIT_ASSERT( ++it != lref.end() );
                    CPPUNIT_ASSERT( it->nKey == v1.nKey );
                    CPPUNIT_ASSERT( it->nVal == v1.nVal );
                    CPPUNIT_ASSERT( ++it != l.end() );
                    CPPUNIT_ASSERT( it->nKey == v3.nKey );
                    CPPUNIT_ASSERT( it->nVal == v3.nVal );
                    CPPUNIT_ASSERT( ++it == l.end() );
                }
            }

            // Apply retired pointer
            OrdList::gc::force_dispose();

            CPPUNIT_ASSERT( v1.s.nDisposeCount == 4 );
            CPPUNIT_ASSERT( v2.s.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v3.s.nDisposeCount == 4 );
        }

        template <class OrdList>
        void test_int()
        {
            test_int_common<OrdList>();

            OrdList l;
            typename OrdList::guarded_ptr gp;

            static int const nLimit = 20;
            typename OrdList::value_type arrItem[nLimit];

            {
                int a[nLimit];
                for (int i = 0; i < nLimit; ++i)
                    a[i]=i;
                shuffle( a, a + nLimit );

                for (int i = 0; i < nLimit; ++i) {
                    arrItem[i].nKey = a[i];
                    arrItem[i].nVal = a[i] * 2;
                }

                // extract/get
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( l.insert( arrItem[i] ) );

                for ( int i=0; i < nLimit; ++i ) {
                    gp = l.get( arrItem[i].nKey );
                    CPPUNIT_ASSERT_EX( gp, "i=" << i );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == arrItem[i].nKey );
                    CPPUNIT_CHECK( gp->nVal == arrItem[i].nVal );
                    gp.release();

                    gp = l.extract( arrItem[i].nKey );
                    CPPUNIT_ASSERT_EX( gp, "i=" << i );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == arrItem[i].nKey );
                    CPPUNIT_CHECK( gp->nVal == arrItem[i].nVal );
                    gp.release();

                    gp = l.get( arrItem[i].nKey );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_CHECK( gp.empty());
                    CPPUNIT_CHECK( !l.extract( arrItem[i].nKey ));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( l.empty() );
                CPPUNIT_ASSERT( !l.get( nLimit/2 ));
                CPPUNIT_ASSERT( gp.empty());
                CPPUNIT_ASSERT( !l.extract( nLimit/2 ));
                CPPUNIT_ASSERT( gp.empty());

                // Apply retired pointer
                OrdList::gc::force_dispose();

                // extract_with/get_with
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( l.insert( arrItem[i] ) );

                for ( int i=0; i < nLimit; ++i ) {
                    other_item itm( arrItem[i].nKey );
                    gp = l.get_with( itm, other_less() );
                    CPPUNIT_ASSERT_EX( gp, "i=" << i );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == arrItem[i].nKey );
                    CPPUNIT_CHECK( gp->nVal == arrItem[i].nVal );
                    gp.release();

                    gp = l.extract_with( itm, other_less() );
                    CPPUNIT_ASSERT_EX( gp, "i=" << i );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == arrItem[i].nKey );
                    CPPUNIT_CHECK( gp->nVal == arrItem[i].nVal );
                    gp.release();

                    gp = l.get_with( itm, other_less() );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_CHECK( gp.empty());
                    CPPUNIT_CHECK( !l.extract_with( itm, other_less() ));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_ASSERT( l.empty() );
                CPPUNIT_ASSERT( !l.get_with( other_item(nLimit/2), other_less() ));
                CPPUNIT_ASSERT( gp.empty());
                CPPUNIT_ASSERT( !l.extract_with( other_item(nLimit/2), other_less() ));
                CPPUNIT_ASSERT( gp.empty());

                // Apply retired pointer
                OrdList::gc::force_dispose();

                for ( int i=0; i < nLimit; i++ ) {
                    CPPUNIT_ASSERT( arrItem[i].s.nDisposeCount == 2 );
                }
            }
        }

        template <class OrdList>
        void test_rcu_int()
        {
            test_int_common<OrdList>();

            OrdList l;
            static int const nLimit = 20;
            typename OrdList::value_type arrItem[nLimit];

            typedef typename OrdList::rcu_lock rcu_lock;
            typedef typename OrdList::value_type value_type;
            typedef typename OrdList::gc rcu_type;

            {
                int a[nLimit];
                for (int i = 0; i < nLimit; ++i)
                    a[i]=i;
                shuffle( a, a + nLimit );

                for (int i = 0; i < nLimit; ++i) {
                    arrItem[i].nKey = a[i];
                    arrItem[i].nVal = a[i] * 2;
                }

                // extract/get
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( l.insert( arrItem[i] ) );

                typename OrdList::exempt_ptr ep;

                for ( int i = 0; i < nLimit; ++i ) {
                    {
                        rcu_lock lock;
                        value_type * pGet = l.get( a[i] );
                        CPPUNIT_ASSERT( pGet != nullptr );
                        CPPUNIT_CHECK( pGet->nKey == a[i] );
                        CPPUNIT_CHECK( pGet->nVal == a[i] * 2 );
                    }

                    {
                        rcu_lock lock;
                        ep = l.extract( a[i] );
                        CPPUNIT_ASSERT( ep );
                        CPPUNIT_ASSERT( !ep.empty() );
                        CPPUNIT_CHECK( ep->nKey == a[i] );
                        CPPUNIT_CHECK( (*ep).nVal == a[i] * 2 );
                    }
                    ep.release();

                    {
                        rcu_lock lock;
                        CPPUNIT_CHECK( l.get( a[i] ) == nullptr );
                        CPPUNIT_CHECK( !l.extract( a[i] ));
                        CPPUNIT_CHECK( ep.empty() );
                    }
                }
                CPPUNIT_ASSERT( l.empty() );

                {
                    rcu_lock lock;
                    CPPUNIT_CHECK( l.get( a[0] ) == nullptr );
                    ep = l.extract( a[0] );
                    CPPUNIT_CHECK( !ep );
                    CPPUNIT_CHECK( ep.empty() );
                }
                // Apply retired pointer
                OrdList::gc::force_dispose();

                // extract_with/get_with
                for ( int i = 0; i < nLimit; ++i ) {
                    CPPUNIT_ASSERT( l.insert( arrItem[i] ) );
                }

                for ( int i = 0; i < nLimit; ++i ) {
                    other_item itm( a[i] );
                    {
                        rcu_lock lock;
                        value_type * pGet = l.get_with( itm, other_less() );
                        CPPUNIT_ASSERT( pGet != nullptr );
                        CPPUNIT_CHECK( pGet->nKey == a[i] );
                        CPPUNIT_CHECK( pGet->nVal == a[i] * 2 );
                    }

                    {
                        rcu_lock lock;
                        ep = l.extract_with( itm, other_less() );
                        CPPUNIT_ASSERT( ep );
                        CPPUNIT_ASSERT( !ep.empty() );
                        CPPUNIT_CHECK( ep->nKey == a[i] );
                        CPPUNIT_CHECK( ep->nVal == a[i] * 2 );
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
                    CPPUNIT_CHECK( l.get_with( other_item( 0 ), other_less() ) == nullptr );
                    CPPUNIT_CHECK( !l.extract_with( other_item(0), other_less() ));
                    CPPUNIT_CHECK( ep.empty() );
                }
                // Apply retired pointer
                OrdList::gc::force_dispose();
            }
        }

        template <class OrdList>
        void test_nogc_int()
        {
            typedef typename OrdList::value_type    value_type;
            {
                value_type v1( 10, 50 );
                value_type v2( 5, 25  );
                value_type v3( 20, 100 );
                {
                    OrdList l;
                    CPPUNIT_ASSERT( l.empty() );

                    CPPUNIT_ASSERT( l.insert( v1 ))     ;   // true
                    CPPUNIT_ASSERT( l.find( v1.key() ) == &v1 );

                    CPPUNIT_ASSERT( v1.s.nFindCall == 0 );
                    CPPUNIT_ASSERT( l.find( v1.key(), find_functor() ));
                    CPPUNIT_ASSERT( v1.s.nFindCall == 1 );

                    CPPUNIT_ASSERT( l.find_with( v2.key(), less<value_type>() ) == nullptr );
                    CPPUNIT_ASSERT( !l.find_with( v3.key(), less<value_type>(), find_functor() ));
                    CPPUNIT_ASSERT( !l.empty() );

                    CPPUNIT_ASSERT( !l.insert( v1 ))    ;   // assertion "is_empty" is not raised since pNext is nullptr

                    {
                        value_type v( v1 );
                        CPPUNIT_ASSERT( !l.insert( v )) ;   // false
                    }

                    std::pair<bool, bool> ret = l.ensure( v2, ensure_functor() );
                    CPPUNIT_ASSERT( ret.first );
                    CPPUNIT_ASSERT( ret.second );
                    CPPUNIT_ASSERT( v2.s.nEnsureNewCall == 1 );
                    CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 0 );

                    //CPPUNIT_ASSERT( !l.insert( v2 ))    ;   // assertion "is_empty"

                    CPPUNIT_ASSERT( l.find( v1.key() ) == &v1 ) ;   // true

                    CPPUNIT_ASSERT( v1.s.nFindCall == 1 );
                    CPPUNIT_ASSERT( l.find( v1.key(), find_functor() ));
                    CPPUNIT_ASSERT( v1.s.nFindCall == 2 );

                    CPPUNIT_ASSERT( l.find( v2.key() ) == &v2 );

                    CPPUNIT_ASSERT( v2.s.nFindCall == 0 );
                    CPPUNIT_ASSERT( l.find( v2.key(), find_functor() ));
                    CPPUNIT_ASSERT( v2.s.nFindCall == 1 );

                    CPPUNIT_ASSERT( !l.find( v3.key() ));

                    {
                        value_type v( v2 );
                        ret = l.ensure( v, ensure_functor() );

                        CPPUNIT_ASSERT( ret.first );
                        CPPUNIT_ASSERT( !ret.second );
                        CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 1 );
                        CPPUNIT_ASSERT( v.s.nEnsureExistsCall == 0 && v.s.nEnsureNewCall == 0 );
                    }

                    CPPUNIT_ASSERT( !l.empty() );

                    CPPUNIT_ASSERT( l.insert( v3 ))     ;   // true
                    CPPUNIT_ASSERT( l.find( v3.key() ) == &v3 );

                    CPPUNIT_ASSERT( v3.s.nFindCall == 0 );
                    CPPUNIT_ASSERT( l.find( v3.key(), find_functor() ));
                    CPPUNIT_ASSERT( v3.s.nFindCall == 1 );

                    {
                        typename OrdList::iterator it = l.begin();
                        typename OrdList::const_iterator cit = l.cbegin();
                        CPPUNIT_ASSERT( it != l.end() );
                        CPPUNIT_ASSERT( it != l.cend() );
                        CPPUNIT_ASSERT( cit != l.end() );
                        CPPUNIT_ASSERT( cit != l.cend() );
                        CPPUNIT_ASSERT( cit == it );

                        CPPUNIT_ASSERT( it->nKey == v2.nKey );
                        CPPUNIT_ASSERT( it->nVal == v2.nVal );
                        CPPUNIT_ASSERT( ++it != l.end() );
                        CPPUNIT_ASSERT( it->nKey == v1.nKey );
                        CPPUNIT_ASSERT( it->nVal == v1.nVal );
                        CPPUNIT_ASSERT( it++ != l.end() );
                        CPPUNIT_ASSERT( it->nKey == v3.nKey );
                        CPPUNIT_ASSERT( it->nVal == v3.nVal );
                        CPPUNIT_ASSERT( it++ != l.end() );
                        CPPUNIT_ASSERT( it == l.end() );
                    }

                    {
                        OrdList const & lref = l;
                        typename OrdList::const_iterator it = lref.begin();
                        CPPUNIT_ASSERT( it != l.end() );
                        CPPUNIT_ASSERT( it->nKey == v2.nKey );
                        CPPUNIT_ASSERT( it->nVal == v2.nVal );
                        CPPUNIT_ASSERT( ++it != lref.end() );
                        CPPUNIT_ASSERT( it->nKey == v1.nKey );
                        CPPUNIT_ASSERT( it->nVal == v1.nVal );
                        CPPUNIT_ASSERT( it++ != l.end() );
                        CPPUNIT_ASSERT( it->nKey == v3.nKey );
                        CPPUNIT_ASSERT( it->nVal == v3.nVal );
                        CPPUNIT_ASSERT( it++ != lref.end() );
                        CPPUNIT_ASSERT( it == l.end() );
                    }
                }

                // Disposer called on list destruction
                CPPUNIT_ASSERT( v1.s.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v2.s.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v3.s.nDisposeCount == 1 );
            }
        }

        void HP_base_cmp();
        void HP_base_less();
        void HP_base_cmpmix();
        void HP_base_ic();
        void HP_member_cmp();
        void HP_member_less();
        void HP_member_cmpmix();
        void HP_member_ic();

        void DHP_base_cmp();
        void DHP_base_less();
        void DHP_base_cmpmix();
        void DHP_base_ic();
        void DHP_member_cmp();
        void DHP_member_less();
        void DHP_member_cmpmix();
        void DHP_member_ic();

        void RCU_GPI_base_cmp();
        void RCU_GPI_base_less();
        void RCU_GPI_base_cmpmix();
        void RCU_GPI_base_ic();
        void RCU_GPI_member_cmp();
        void RCU_GPI_member_less();
        void RCU_GPI_member_cmpmix();
        void RCU_GPI_member_ic();

        void RCU_GPB_base_cmp();
        void RCU_GPB_base_less();
        void RCU_GPB_base_cmpmix();
        void RCU_GPB_base_ic();
        void RCU_GPB_member_cmp();
        void RCU_GPB_member_less();
        void RCU_GPB_member_cmpmix();
        void RCU_GPB_member_ic();

        void RCU_GPT_base_cmp();
        void RCU_GPT_base_less();
        void RCU_GPT_base_cmpmix();
        void RCU_GPT_base_ic();
        void RCU_GPT_member_cmp();
        void RCU_GPT_member_less();
        void RCU_GPT_member_cmpmix();
        void RCU_GPT_member_ic();

        void RCU_SHB_base_cmp();
        void RCU_SHB_base_less();
        void RCU_SHB_base_cmpmix();
        void RCU_SHB_base_ic();
        void RCU_SHB_member_cmp();
        void RCU_SHB_member_less();
        void RCU_SHB_member_cmpmix();
        void RCU_SHB_member_ic();

        void RCU_SHT_base_cmp();
        void RCU_SHT_base_less();
        void RCU_SHT_base_cmpmix();
        void RCU_SHT_base_ic();
        void RCU_SHT_member_cmp();
        void RCU_SHT_member_less();
        void RCU_SHT_member_cmpmix();
        void RCU_SHT_member_ic();

        void nogc_base_cmp();
        void nogc_base_less();
        void nogc_base_cmpmix();
        void nogc_base_ic();
        void nogc_member_cmp();
        void nogc_member_less();
        void nogc_member_cmpmix();
        void nogc_member_ic();


        CPPUNIT_TEST_SUITE(IntrusiveMichaelListHeaderTest)
            CPPUNIT_TEST(HP_base_cmp)
            CPPUNIT_TEST(HP_base_less)
            CPPUNIT_TEST(HP_base_cmpmix)
            CPPUNIT_TEST(HP_base_ic)
            CPPUNIT_TEST(HP_member_cmp)
            CPPUNIT_TEST(HP_member_less)
            CPPUNIT_TEST(HP_member_cmpmix)
            CPPUNIT_TEST(HP_member_ic)

            CPPUNIT_TEST(DHP_base_cmp)
            CPPUNIT_TEST(DHP_base_less)
            CPPUNIT_TEST(DHP_base_cmpmix)
            CPPUNIT_TEST(DHP_base_ic)
            CPPUNIT_TEST(DHP_member_cmp)
            CPPUNIT_TEST(DHP_member_less)
            CPPUNIT_TEST(DHP_member_cmpmix)
            CPPUNIT_TEST(DHP_member_ic)

            CPPUNIT_TEST(RCU_GPI_base_cmp)
            CPPUNIT_TEST(RCU_GPI_base_less)
            CPPUNIT_TEST(RCU_GPI_base_cmpmix)
            CPPUNIT_TEST(RCU_GPI_base_ic)
            CPPUNIT_TEST(RCU_GPI_member_cmp)
            CPPUNIT_TEST(RCU_GPI_member_less)
            CPPUNIT_TEST(RCU_GPI_member_cmpmix)
            CPPUNIT_TEST(RCU_GPI_member_ic)

            CPPUNIT_TEST(RCU_GPB_base_cmp)
            CPPUNIT_TEST(RCU_GPB_base_less)
            CPPUNIT_TEST(RCU_GPB_base_cmpmix)
            CPPUNIT_TEST(RCU_GPB_base_ic)
            CPPUNIT_TEST(RCU_GPB_member_cmp)
            CPPUNIT_TEST(RCU_GPB_member_less)
            CPPUNIT_TEST(RCU_GPB_member_cmpmix)
            CPPUNIT_TEST(RCU_GPB_member_ic)

            CPPUNIT_TEST(RCU_GPT_base_cmp)
            CPPUNIT_TEST(RCU_GPT_base_less)
            CPPUNIT_TEST(RCU_GPT_base_cmpmix)
            CPPUNIT_TEST(RCU_GPT_base_ic)
            CPPUNIT_TEST(RCU_GPT_member_cmp)
            CPPUNIT_TEST(RCU_GPT_member_less)
            CPPUNIT_TEST(RCU_GPT_member_cmpmix)
            CPPUNIT_TEST(RCU_GPT_member_ic)

            CPPUNIT_TEST(nogc_base_cmp)
            CPPUNIT_TEST(nogc_base_less)
            CPPUNIT_TEST(nogc_base_cmpmix)
            CPPUNIT_TEST(nogc_base_ic)
            CPPUNIT_TEST(nogc_member_cmp)
            CPPUNIT_TEST(nogc_member_less)
            CPPUNIT_TEST(nogc_member_cmpmix)
            CPPUNIT_TEST(nogc_member_ic)

        CPPUNIT_TEST_SUITE_END()
    };
}   // namespace ordlist

#endif // CDSTEST_HDR_INTRUSIVE_MICHAEL_H

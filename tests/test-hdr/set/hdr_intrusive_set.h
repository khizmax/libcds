//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRUSIVE_SET_H
#define CDSTEST_HDR_INTRUSIVE_SET_H

#include "cppunit/cppunit_proxy.h"
#include "size_check.h"

#include <cds/opt/hash.h>
#include <functional>   // ref

// forward declaration
namespace cds { namespace intrusive {} }

namespace set {
    using misc::check_size;

    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    class IntrusiveHashSetHdrTest: public CppUnitMini::TestCase
    {
    protected:
        struct stat
        {
            unsigned int nDisposeCount  ;   // count of disposer calling
            unsigned int nFindCount     ;   // count of find-functor calling
            unsigned int nEnsureNewCount;
            unsigned int nEnsureCount;
            mutable unsigned int nEraseCount;

            stat()
            {
                memset( this, 0, sizeof(*this));
            }
        };

        template <typename Node>
        struct base_int_item
            : public Node
            , public stat

        {
            int nKey;
            int nVal;

            base_int_item()
            {}

            base_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
            {}

            base_int_item(const base_int_item& v )
                : Node()
                , stat()
                , nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey;
            }
        };

        template <typename Node>
        struct member_int_item: public stat
        {
            int nKey;
            int nVal;

            Node hMember;

            stat s;

            member_int_item()
            {}

            member_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
            {}

            member_int_item(const member_int_item& v )
                : stat()
                , nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey;
            }
        };

        struct hash_int {
            size_t operator()( int i ) const
            {
                return co::v::hash<int>()( i );
            }
            template <typename Item>
            size_t operator()( const Item& i ) const
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
                ++p->nDisposeCount;
            }
        };

        struct find_functor
        {
            template <typename Item, typename T>
            void operator()( Item& item, T& /*val*/ )
            {
                ++item.nFindCount;
            }
        };

        struct insert_functor
        {
            template <typename Item>
            void operator()(Item& item )
            {
                item.nVal = item.nKey * 100;
            }
        };

        struct ensure_functor
        {
            template <typename Item>
            void operator()( bool bNew, Item& item, Item& /*val*/ )
            {
                if ( bNew )
                    ++item.nEnsureNewCount;
                else
                    ++item.nEnsureCount;
            }
        };

        struct erase_functor
        {
            template <typename Item>
            void operator()( Item const& item )
            {
                item.nEraseCount++;
            }
        };

        template <class Set>
        void test_int_with( Set& s )
        {
            typedef typename Set::value_type    value_type;

            value_type v1( 10, 50 );
            value_type v2( 5, 25  );
            value_type v3( 20, 100 );
            int key;

            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            // insert/find test
            CPPUNIT_ASSERT( !s.find( v1.key() ));
            CPPUNIT_ASSERT( s.insert( v1 ));
            CPPUNIT_ASSERT( s.find( v1.key() ));
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( !s.find_with( v2.key(), less<value_type>() ));
            CPPUNIT_ASSERT( s.insert( v2 ));
            CPPUNIT_ASSERT( v2.nFindCount == 0 );
            CPPUNIT_ASSERT( s.find_with( key = v2.key(), less<value_type>(), find_functor() ));
            CPPUNIT_ASSERT( v2.nFindCount == 1 );
            v2.nFindCount = 0;
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.empty() );

            {
                insert_functor  fi;
                find_functor    ff;
                CPPUNIT_ASSERT( !s.find( v3 ));
                CPPUNIT_ASSERT( v3.nVal != v3.nKey * 100 );
                CPPUNIT_ASSERT( s.insert( v3, std::ref(fi) ));
                CPPUNIT_ASSERT( v3.nVal == v3.nKey * 100 );
                CPPUNIT_ASSERT( v3.nFindCount == 0 );
                CPPUNIT_ASSERT( s.find( v3, std::ref(ff) ));
                CPPUNIT_ASSERT( v3.nFindCount == 1 );
                v3.nFindCount = 0;
                CPPUNIT_ASSERT( check_size( s, 3 ));
                CPPUNIT_ASSERT( !s.empty() );
            }

            // unlink test
            CPPUNIT_ASSERT( s.unlink( v1 ));
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( s.find( v2 ));
            CPPUNIT_ASSERT( s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 2 ));

            CPPUNIT_ASSERT( !s.unlink( v1 ));
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( check_size( s, 2 ));

            CPPUNIT_ASSERT( s.find_with( v3, less<value_type>() ));
            CPPUNIT_ASSERT( s.unlink( v3 ));
            CPPUNIT_ASSERT( !s.find_with( v3, less<value_type>() ));
            CPPUNIT_ASSERT( check_size( s, 1 ));

            CPPUNIT_ASSERT( s.find( v2 ));
            CPPUNIT_ASSERT( s.unlink( v2 ));
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( !s.find( v2 ));
            CPPUNIT_ASSERT( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 0 ));
            CPPUNIT_ASSERT( s.empty() );

            Set::gc::force_dispose();
            // unlink function calls disposer
            CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 1 );

            // ensure test
            {
                ensure_functor f;
                std::pair<bool, bool> ret = s.ensure( v1, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v1.nEnsureCount == 0 );
                CPPUNIT_ASSERT( check_size( s, 1 ));

                ret = s.ensure( v2, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v2.nEnsureCount == 0 );
                CPPUNIT_ASSERT( check_size( s, 2 ));

                ret = s.ensure( v3, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v3.nEnsureCount == 0 );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                CPPUNIT_ASSERT( s.find( v1 ));
                CPPUNIT_ASSERT( s.find( v2 ));
                CPPUNIT_ASSERT( s.find( v3 ));

                ret = s.ensure( v1, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( !ret.second );
                CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v1.nEnsureCount == 1 );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                ret = s.ensure( v2, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( !ret.second );
                CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v2.nEnsureCount == 1 );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                ret = s.ensure( v3, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( !ret.second );
                CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v3.nEnsureCount == 1 );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                CPPUNIT_ASSERT( s.find( v1 ));
                CPPUNIT_ASSERT( s.find( v2 ));
                CPPUNIT_ASSERT( s.find( v3 ));
            }

            // erase test
            CPPUNIT_ASSERT( s.find( v1 ));
            CPPUNIT_ASSERT( s.find_with( v2, less<value_type>() ));
            CPPUNIT_ASSERT( s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 3 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( s.erase( v1 ));
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( s.find( v2 ));
            CPPUNIT_ASSERT( s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( v1.nEraseCount == 0 );
            CPPUNIT_ASSERT( !s.erase( v1, erase_functor() ));
            CPPUNIT_ASSERT( v1.nEraseCount == 0 );
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( s.find( v2 ));
            CPPUNIT_ASSERT( s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( v2.nEraseCount == 0 );
            CPPUNIT_ASSERT( s.erase_with( v2.key(), less<value_type>(), erase_functor() ));
            CPPUNIT_ASSERT( v2.nEraseCount == 1 );
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( !s.find( v2 ));
            CPPUNIT_ASSERT( s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( !s.erase( v2.key() ));
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( !s.find( v2 ));
            CPPUNIT_ASSERT( s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( s.erase( v3 ));
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( !s.find( v2 ));
            CPPUNIT_ASSERT( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 0 ));
            CPPUNIT_ASSERT( s.empty() );

            // dispose test
            Set::gc::force_dispose();
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 2 );

            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( s.insert( v1 ));
            CPPUNIT_ASSERT( s.insert( v3 ));
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.empty() );

            {
                value_type v( v1 );
                CPPUNIT_ASSERT( !s.unlink( v ) );
            }

            CPPUNIT_ASSERT( s.unlink( v3 ) );
            CPPUNIT_ASSERT( s.find( v1 ));
            CPPUNIT_ASSERT( !s.find( v2 ));
            CPPUNIT_ASSERT( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( !s.unlink( v3 ));
            CPPUNIT_ASSERT( s.find( v1 ));
            CPPUNIT_ASSERT( !s.find( v2 ));
            CPPUNIT_ASSERT( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( s.unlink( v1 ));
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( !s.find( v2 ));
            CPPUNIT_ASSERT( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 0 ));
            CPPUNIT_ASSERT( s.empty() );

            CPPUNIT_ASSERT( !s.unlink( v2 ));
            CPPUNIT_ASSERT( !s.find( v1 ));
            CPPUNIT_ASSERT( !s.find( v2 ));
            CPPUNIT_ASSERT( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 0 ));
            CPPUNIT_ASSERT( s.empty() );

            Set::gc::force_dispose();
            CPPUNIT_ASSERT( v1.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 3 );

            // clear test
            CPPUNIT_ASSERT( s.insert( v1 ));
            CPPUNIT_ASSERT( s.insert( v3 ));
            CPPUNIT_ASSERT( s.insert( v2 ));

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));
            Set::gc::force_dispose();
            CPPUNIT_CHECK( v1.nDisposeCount == 4 );
            CPPUNIT_CHECK( v2.nDisposeCount == 3 );
            CPPUNIT_CHECK( v3.nDisposeCount == 4 );
        }


        template <class Set>
        void test_int()
        {
            {
                Set s( 64, 4 );
                test_int_with( s );
            }

            // Iterator test
            test_iter<Set>();

            // extract/get test
            {
                typedef typename Set::value_type    value_type;
                typedef typename Set::guarded_ptr   guarded_ptr;

                static size_t const nLimit = 1024;
                value_type arrItems[nLimit];
                int arr[nLimit];
                for ( size_t i = 0; i < nLimit; ++i )
                    arr[i] = (int) i;
                shuffle( arr, arr + nLimit );

                for ( size_t i = 0; i < nLimit; ++i ) {
                    arrItems[i].nKey = arr[i];
                    arrItems[i].nVal = arr[i] * 2;
                }

                Set s( nLimit, 2 );
                for ( size_t i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrItems[i] ));

                guarded_ptr gp;
                for ( size_t i = 0; i < nLimit; i += 2 ) {
                    int nKey = arr[i];
                    gp = s.get( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey * 2 );
                    gp.release();

                    gp = s.extract( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty() );
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey * 2 );
                    gp.release();

                    gp = s.get( nKey );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_ASSERT( gp.empty() );
                    CPPUNIT_CHECK( !s.extract( nKey ));
                    CPPUNIT_CHECK( gp.empty() );

                    nKey = arr[i+1];
                    gp = s.get_with( nKey, less<value_type>() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey * 2 );
                    gp.release();

                    gp = s.extract_with( nKey, less<value_type>() );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty() );
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey * 2 );
                    gp.release();

                    gp = s.get_with( nKey, less<value_type>() );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_CHECK( gp.empty());
                    CPPUNIT_CHECK( !s.extract_with( nKey, less<value_type>() ));
                    CPPUNIT_CHECK( gp.empty());
                }
                CPPUNIT_CHECK( s.empty() );
                CPPUNIT_CHECK( check_size( s, 0 ));

                CPPUNIT_CHECK( !s.get( 100 ));
                CPPUNIT_CHECK( !s.extract( 100 ));
                CPPUNIT_CHECK( gp.empty() );

                Set::gc::force_dispose();
            }
        }

        template <class Set>
        void test_iter()
        {
            typedef typename Set::value_type    value_type;
            typedef typename Set::iterator set_iterator;
            typedef typename Set::iterator const_set_iterator;

            value_type  v[500];
            {
                Set s( 100, 4 );

                for ( int i = 0; unsigned(i) < sizeof(v)/sizeof(v[0]); ++i ) {
                    v[i].nKey = i;
                    v[i].nVal = i * 2;

                    CPPUNIT_ASSERT( s.insert( v[i] ));
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

                int nCount = 0;
                for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    set_iterator it2 = it;
                    CPPUNIT_CHECK( it == it2 );
                    CPPUNIT_CHECK( it2 != itEnd );
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                    it->nVal = (*it).nKey;
                    ++nCount;
                }
                CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0]));

                nCount = 0;
                for ( const_set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                    ++nCount;
                }
                CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0]));

                for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                    CPPUNIT_ASSERT( v[i].nKey == v[i].nVal );
                }
            }

            Set::gc::force_dispose();

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nDisposeCount == 1 );
            }
        }

        template <class Set>
        void test_int_nogc()
        {
            typedef typename Set::value_type    value_type;

            value_type v1( 10, 50 );
            value_type v2( 5, 25  );
            value_type v3( 20, 100 );
            int key;

            {
                {
                    Set s( 64, 4 );

                    // insert test
                    CPPUNIT_ASSERT( s.empty() );
                    CPPUNIT_ASSERT( check_size( s, 0 ));

                    // insert/find test
                    CPPUNIT_ASSERT( s.find( v1.key() ) == nullptr );
                    CPPUNIT_ASSERT( s.insert( v1 ));
                    CPPUNIT_ASSERT( s.find_with( v1.key(), less<value_type>() ) == &v1 );
                    CPPUNIT_ASSERT( check_size( s, 1 ));
                    CPPUNIT_ASSERT( !s.empty() );

                    CPPUNIT_ASSERT( s.find( v2.key() ) == nullptr );
                    CPPUNIT_ASSERT( s.insert( v2 ));
                    CPPUNIT_ASSERT( v2.nFindCount == 0 );
                    CPPUNIT_ASSERT( s.find( key = v2.key(), find_functor() ));
                    CPPUNIT_ASSERT( v2.nFindCount == 1 );
                    v2.nFindCount = 0;
                    CPPUNIT_ASSERT( check_size( s, 2 ));
                    CPPUNIT_ASSERT( !s.empty() );

                    {
                        find_functor    ff;
                        CPPUNIT_ASSERT( s.find( v3 ) == nullptr );
                        CPPUNIT_ASSERT( s.insert( v3 ));
                        CPPUNIT_ASSERT( v3.nFindCount == 0 );
                        CPPUNIT_ASSERT( s.find_with( v3, less<value_type>(), std::ref(ff) ));
                        CPPUNIT_ASSERT( v3.nFindCount == 1 );
                        v3.nFindCount = 0;
                        CPPUNIT_ASSERT( check_size( s, 3 ));
                        CPPUNIT_ASSERT( !s.empty() );
                    }
                }

                // s.clear()   ; // not all set supports clear() method
                CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v3.nDisposeCount == 1 );


                // ensure test
                {
                    Set s( 64, 4 );

                    ensure_functor f;
                    std::pair<bool, bool> ret = s.ensure( v1, f );
                    CPPUNIT_ASSERT( ret.first );
                    CPPUNIT_ASSERT( ret.second );
                    CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 );
                    CPPUNIT_ASSERT( v1.nEnsureCount == 0 );
                    CPPUNIT_ASSERT( check_size( s, 1 ));

                    ret = s.ensure( v2, f );
                    CPPUNIT_ASSERT( ret.first );
                    CPPUNIT_ASSERT( ret.second );
                    CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 );
                    CPPUNIT_ASSERT( v2.nEnsureCount == 0 );
                    CPPUNIT_ASSERT( check_size( s, 2 ));

                    ret = s.ensure( v3, f );
                    CPPUNIT_ASSERT( ret.first );
                    CPPUNIT_ASSERT( ret.second );
                    CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 );
                    CPPUNIT_ASSERT( v3.nEnsureCount == 0 );
                    CPPUNIT_ASSERT( check_size( s, 3 ));

                    CPPUNIT_ASSERT( s.find( v1 ) == &v1 );
                    CPPUNIT_ASSERT( s.find( v2 ) == &v2 );
                    CPPUNIT_ASSERT( s.find( v3 ) == &v3 );

                    ret = s.ensure( v1, f );
                    CPPUNIT_ASSERT( ret.first );
                    CPPUNIT_ASSERT( !ret.second );
                    CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 );
                    CPPUNIT_ASSERT( v1.nEnsureCount == 1 );
                    CPPUNIT_ASSERT( check_size( s, 3 ));

                    ret = s.ensure( v2, f );
                    CPPUNIT_ASSERT( ret.first );
                    CPPUNIT_ASSERT( !ret.second );
                    CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 );
                    CPPUNIT_ASSERT( v2.nEnsureCount == 1 );
                    CPPUNIT_ASSERT( check_size( s, 3 ));

                    ret = s.ensure( v3, f );
                    CPPUNIT_ASSERT( ret.first );
                    CPPUNIT_ASSERT( !ret.second );
                    CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 );
                    CPPUNIT_ASSERT( v3.nEnsureCount == 1 );
                    CPPUNIT_ASSERT( check_size( s, 3 ));

                    CPPUNIT_ASSERT( s.find( v1 ) == &v1 );
                    CPPUNIT_ASSERT( s.find( v2 ) == &v2 );
                    CPPUNIT_ASSERT( s.find( v3 ) == &v3 );
                }
            }

            // dispose on destruct test
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 2 );

            // Iterator test
            test_iter<Set>();
        }


#define ASSERT_RCU_FIND( _expr ) { rcu_lock rl; CPPUNIT_ASSERT( _expr ); }
        template <class Set>
        void test_rcu_int_with( Set& s )
        {
            typedef typename Set::value_type    value_type;
            typedef typename Set::gc::scoped_lock rcu_lock;

            value_type v1( 10, 50 );
            value_type v2( 5, 25  );
            value_type v3( 20, 100 );
            value_type v4( 40, 400 );
            int key;

            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            // insert/find test

            ASSERT_RCU_FIND( !s.find_with( v1.key(), less<value_type>() ));
            CPPUNIT_ASSERT( s.insert( v1 ));
            ASSERT_RCU_FIND( s.find_with( v1.key(), less<value_type>()) );
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            ASSERT_RCU_FIND( !s.find( v2.key() ));
            CPPUNIT_ASSERT( s.insert( v2 ));
            CPPUNIT_ASSERT( v2.nFindCount == 0 );
            CPPUNIT_ASSERT( s.find( key = v2.key(), find_functor() ));
            CPPUNIT_ASSERT( v2.nFindCount == 1 );
            v2.nFindCount = 0;
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.empty() );

            {
                insert_functor  fi;
                find_functor    ff;
                ASSERT_RCU_FIND( !s.find( v3 ));
                CPPUNIT_ASSERT( v3.nVal != v3.nKey * 100 );
                CPPUNIT_ASSERT( s.insert( v3, std::ref(fi) ));
                CPPUNIT_ASSERT( v3.nVal == v3.nKey * 100 );
                CPPUNIT_ASSERT( v3.nFindCount == 0 );
                CPPUNIT_ASSERT( s.find_with( v3, less<value_type>(), std::ref(ff) ));
                CPPUNIT_ASSERT( v3.nFindCount == 1 );
                v3.nFindCount = 0;
                CPPUNIT_ASSERT( check_size( s, 3 ));
                CPPUNIT_ASSERT( !s.empty() );
            }

            // unlink test
            CPPUNIT_ASSERT( s.unlink( v1 ));
            ASSERT_RCU_FIND( !s.find( v1 ));
            ASSERT_RCU_FIND( s.find( v2 ));
            ASSERT_RCU_FIND( s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 2 ));

            CPPUNIT_ASSERT( !s.unlink( v1 ));
            ASSERT_RCU_FIND( !s.find( v1 ));
            CPPUNIT_ASSERT( check_size( s, 2 ));

            ASSERT_RCU_FIND( s.find( v3 ) );
            CPPUNIT_ASSERT( s.unlink( v3 ));
            ASSERT_RCU_FIND( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 1 ));

            ASSERT_RCU_FIND( s.find( v2 ));
            CPPUNIT_ASSERT( s.unlink( v2 ));
            ASSERT_RCU_FIND( !s.find( v1 ));
            ASSERT_RCU_FIND( !s.find( v2 ));
            ASSERT_RCU_FIND( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 0 ));
            CPPUNIT_ASSERT( s.empty() );

            Set::gc::force_dispose();
            // unlink function calls disposer
            CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 1 );

            // ensure test
            {
                ensure_functor f;
                std::pair<bool, bool> ret = s.ensure( v1, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v1.nEnsureCount == 0 );
                CPPUNIT_ASSERT( check_size( s, 1 ));

                ret = s.ensure( v2, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v2.nEnsureCount == 0 );
                CPPUNIT_ASSERT( check_size( s, 2 ));

                ret = s.ensure( v3, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v3.nEnsureCount == 0 );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                ASSERT_RCU_FIND( s.find( v1 ));
                ASSERT_RCU_FIND( s.find( v2 ));
                ASSERT_RCU_FIND( s.find( v3 ));

                ret = s.ensure( v1, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( !ret.second );
                CPPUNIT_ASSERT( v1.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v1.nEnsureCount == 1 );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                ret = s.ensure( v2, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( !ret.second );
                CPPUNIT_ASSERT( v2.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v2.nEnsureCount == 1 );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                ret = s.ensure( v3, f );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( !ret.second );
                CPPUNIT_ASSERT( v3.nEnsureNewCount == 1 );
                CPPUNIT_ASSERT( v3.nEnsureCount == 1 );
                CPPUNIT_ASSERT( check_size( s, 3 ));

                ASSERT_RCU_FIND( s.find( v1 ) );
                ASSERT_RCU_FIND( s.find( v2 ));
                ASSERT_RCU_FIND( s.find( v3 ));
            }

            // erase test
            ASSERT_RCU_FIND( s.find( v1 ));
            ASSERT_RCU_FIND( s.find_with( v2, less<value_type>() ));
            ASSERT_RCU_FIND( s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 3 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( v1.nEraseCount == 0 );
            CPPUNIT_ASSERT( s.erase_with( v1, less<value_type>(), erase_functor() ));
            CPPUNIT_ASSERT( v1.nEraseCount == 1 );
            ASSERT_RCU_FIND( !s.find( v1 ));
            ASSERT_RCU_FIND( s.find( v2 ) );
            ASSERT_RCU_FIND( s.find( v3 ) );
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( !s.erase( v1 ));
            ASSERT_RCU_FIND( !s.find( v1 ));
            ASSERT_RCU_FIND( s.find( v2 ));
            ASSERT_RCU_FIND( s.find( v3 ) );
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.empty() );

            ASSERT_RCU_FIND( !s.find( v4 ));
            CPPUNIT_ASSERT( s.insert(v4));
            ASSERT_RCU_FIND( s.find( v4.key() ));
            CPPUNIT_ASSERT( s.erase_with( v4.key(), less<value_type>() ));
            ASSERT_RCU_FIND( !s.find( v4.key() ));

            CPPUNIT_ASSERT( s.erase( v2.key() ));
            ASSERT_RCU_FIND( !s.find( v1 ));
            ASSERT_RCU_FIND( !s.find( v2 ));
            ASSERT_RCU_FIND( s.find( v3 ) );
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( !s.erase( v2.key() ));
            ASSERT_RCU_FIND( !s.find( v1 ));
            ASSERT_RCU_FIND( !s.find( v2 ));
            ASSERT_RCU_FIND( s.find( v3 ) );
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( v3.nEraseCount == 0 );
            CPPUNIT_ASSERT( s.erase( v3, erase_functor() ));
            CPPUNIT_ASSERT( v3.nEraseCount == 1 );
            ASSERT_RCU_FIND( !s.find( v1 ));
            ASSERT_RCU_FIND( !s.find( v2 ));
            ASSERT_RCU_FIND( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 0 ));
            CPPUNIT_ASSERT( s.empty() );

            // dispose test
            Set::gc::force_dispose();
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v4.nDisposeCount == 1 );

            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( s.insert( v1 ));
            CPPUNIT_ASSERT( s.insert( v3 ));
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.empty() );

            {
                value_type v( v1 );
                CPPUNIT_ASSERT( !s.unlink( v ) );
            }

            CPPUNIT_ASSERT( s.unlink( v3 ) );
            ASSERT_RCU_FIND( s.find( v1 ));
            ASSERT_RCU_FIND( !s.find( v2 ));
            ASSERT_RCU_FIND( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( !s.unlink( v3 ));
            ASSERT_RCU_FIND( s.find( v1 ));
            ASSERT_RCU_FIND( !s.find( v2 ));
            ASSERT_RCU_FIND( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( s.unlink( v1 ));
            ASSERT_RCU_FIND( !s.find( v1 ));
            ASSERT_RCU_FIND( !s.find( v2 ));
            ASSERT_RCU_FIND( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 0 ));
            CPPUNIT_ASSERT( s.empty() );

            CPPUNIT_ASSERT( !s.unlink( v2 ));
            ASSERT_RCU_FIND( !s.find( v1 ));
            ASSERT_RCU_FIND( !s.find( v2 ));
            ASSERT_RCU_FIND( !s.find( v3 ));
            CPPUNIT_ASSERT( check_size( s, 0 ));
            CPPUNIT_ASSERT( s.empty() );

            Set::gc::force_dispose();
            CPPUNIT_ASSERT( v1.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v4.nDisposeCount == 1 );

            // clear test
            CPPUNIT_ASSERT( s.insert( v1 ));
            CPPUNIT_ASSERT( s.insert( v3 ));
            CPPUNIT_ASSERT( s.insert( v2 ));

            s.clear();
            Set::gc::force_dispose();
            CPPUNIT_ASSERT( v1.nDisposeCount == 4 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 4 );
            CPPUNIT_ASSERT( v4.nDisposeCount == 1 );
        }
#undef ASSERT_RCU_FIND

        template <class Set>
        void test_rcu_int()
        {
            {
                Set s( 64, 4 );
                test_rcu_int_with( s );
            }

            // Iterator test
            test_iter<Set>();

            // Extract tests
            typedef typename Set::gc    rcu;
            typedef typename Set::value_type    value_type;
            typedef typename Set::rcu_lock      rcu_lock;

            typename Set::exempt_ptr ep;

            {
                static size_t const nLimit = 1024;
                value_type arrItems[nLimit];
                int arr[nLimit];
                for ( size_t i = 0; i < nLimit; ++i )
                    arr[i] = (int) i;
                shuffle( arr, arr + nLimit );

                for ( size_t i = 0; i < nLimit; ++i ) {
                    arrItems[i].nKey = arr[i];
                    arrItems[i].nVal = arr[i] * 2;
                }

                Set s( nLimit, 2 );
                for ( size_t i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrItems[i] ));

                for ( size_t i = 0; i < nLimit; i += 2 ) {
                    value_type * pVal;
                    int nKey = arr[i];
                    {
                        rcu_lock l;
                        pVal = s.get( nKey );
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->nKey == nKey );
                        CPPUNIT_CHECK( pVal->nVal == nKey * 2 );

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
                        CPPUNIT_CHECK( !s.extract( nKey ));
                        CPPUNIT_CHECK( ep.empty() );

                        nKey = arr[i+1];
                        pVal = s.get_with( nKey, less<value_type>() );
                        CPPUNIT_ASSERT( pVal != nullptr );
                        CPPUNIT_CHECK( pVal->nKey == nKey );
                        CPPUNIT_CHECK( pVal->nVal == nKey * 2 );

                        ep = s.extract_with( nKey, less<value_type>() );
                        CPPUNIT_ASSERT( ep );
                        CPPUNIT_ASSERT( !ep.empty() );
                        CPPUNIT_CHECK( pVal->nKey == ep->nKey );
                        CPPUNIT_CHECK( pVal->nVal == (*ep).nVal );
                    }
                    ep.release();
                    {
                        rcu_lock l;
                        CPPUNIT_CHECK( s.get_with( nKey, less<value_type>() ) == nullptr );
                        ep = s.extract_with( nKey, less<value_type>() );
                        CPPUNIT_CHECK( !ep );
                        CPPUNIT_CHECK( ep.empty() );
                    }
                }
                CPPUNIT_CHECK( s.empty() );
                CPPUNIT_CHECK( check_size( s, 0 ));
                {
                    rcu_lock l;
                    CPPUNIT_CHECK( s.get( 100 ) == nullptr );
                    ep = s.extract( 100 );
                    CPPUNIT_CHECK( !ep );
                    CPPUNIT_CHECK( ep.empty() );
                }

                Set::gc::force_dispose();
            }
        }

        // MichaelHashSet + MichaelList
        void HP_base_cmp();
        void HP_base_less();
        void HP_base_cmpmix();
        void HP_member_cmp();
        void HP_member_less();
        void HP_member_cmpmix();

        void DHP_base_cmp();
        void DHP_base_less();
        void DHP_base_cmpmix();
        void DHP_member_cmp();
        void DHP_member_less();
        void DHP_member_cmpmix();

        void RCU_GPI_base_cmp();
        void RCU_GPI_base_less();
        void RCU_GPI_base_cmpmix();
        void RCU_GPI_member_cmp();
        void RCU_GPI_member_less();
        void RCU_GPI_member_cmpmix();

        void RCU_GPB_base_cmp();
        void RCU_GPB_base_less();
        void RCU_GPB_base_cmpmix();
        void RCU_GPB_member_cmp();
        void RCU_GPB_member_less();
        void RCU_GPB_member_cmpmix();

        void RCU_GPT_base_cmp();
        void RCU_GPT_base_less();
        void RCU_GPT_base_cmpmix();
        void RCU_GPT_member_cmp();
        void RCU_GPT_member_less();
        void RCU_GPT_member_cmpmix();

        void RCU_SHB_base_cmp();
        void RCU_SHB_base_less();
        void RCU_SHB_base_cmpmix();
        void RCU_SHB_member_cmp();
        void RCU_SHB_member_less();
        void RCU_SHB_member_cmpmix();

        void RCU_SHT_base_cmp();
        void RCU_SHT_base_less();
        void RCU_SHT_base_cmpmix();
        void RCU_SHT_member_cmp();
        void RCU_SHT_member_less();
        void RCU_SHT_member_cmpmix();

        void nogc_base_cmp();
        void nogc_base_less();
        void nogc_base_cmpmix();
        void nogc_member_cmp();
        void nogc_member_less();
        void nogc_member_cmpmix();

        // MichaelHashSet + LazyList
        void HP_base_cmp_lazy();
        void HP_base_less_lazy();
        void HP_base_cmpmix_lazy();
        void HP_member_cmp_lazy();
        void HP_member_less_lazy();
        void HP_member_cmpmix_lazy();

        void DHP_base_cmp_lazy();
        void DHP_base_less_lazy();
        void DHP_base_cmpmix_lazy();
        void DHP_member_cmp_lazy();
        void DHP_member_less_lazy();
        void DHP_member_cmpmix_lazy();

        void RCU_GPI_base_cmp_lazy();
        void RCU_GPI_base_less_lazy();
        void RCU_GPI_base_cmpmix_lazy();
        void RCU_GPI_member_cmp_lazy();
        void RCU_GPI_member_less_lazy();
        void RCU_GPI_member_cmpmix_lazy();

        void RCU_GPB_base_cmp_lazy();
        void RCU_GPB_base_less_lazy();
        void RCU_GPB_base_cmpmix_lazy();
        void RCU_GPB_member_cmp_lazy();
        void RCU_GPB_member_less_lazy();
        void RCU_GPB_member_cmpmix_lazy();

        void RCU_GPT_base_cmp_lazy();
        void RCU_GPT_base_less_lazy();
        void RCU_GPT_base_cmpmix_lazy();
        void RCU_GPT_member_cmp_lazy();
        void RCU_GPT_member_less_lazy();
        void RCU_GPT_member_cmpmix_lazy();

        void RCU_SHB_base_cmp_lazy();
        void RCU_SHB_base_less_lazy();
        void RCU_SHB_base_cmpmix_lazy();
        void RCU_SHB_member_cmp_lazy();
        void RCU_SHB_member_less_lazy();
        void RCU_SHB_member_cmpmix_lazy();

        void RCU_SHT_base_cmp_lazy();
        void RCU_SHT_base_less_lazy();
        void RCU_SHT_base_cmpmix_lazy();
        void RCU_SHT_member_cmp_lazy();
        void RCU_SHT_member_less_lazy();
        void RCU_SHT_member_cmpmix_lazy();

        void nogc_base_cmp_lazy();
        void nogc_base_less_lazy();
        void nogc_base_cmpmix_lazy();
        void nogc_member_cmp_lazy();
        void nogc_member_less_lazy();
        void nogc_member_cmpmix_lazy();

        // SplitListSet + MichaelList [dyn - dynamic bucket table, st - static bucket table]
        void split_dyn_HP_base_cmp();
        void split_dyn_HP_base_less();
        void split_dyn_HP_base_cmpmix();
        void split_dyn_HP_base_cmpmix_stat();
        void split_dyn_HP_member_cmp();
        void split_dyn_HP_member_less();
        void split_dyn_HP_member_cmpmix();
        void split_dyn_HP_member_cmpmix_stat();
        void split_st_HP_base_cmp();
        void split_st_HP_base_less();
        void split_st_HP_base_cmpmix();
        void split_st_HP_base_cmpmix_stat();
        void split_st_HP_member_cmp();
        void split_st_HP_member_less();
        void split_st_HP_member_cmpmix();
        void split_st_HP_member_cmpmix_stat();

        void split_dyn_DHP_base_cmp();
        void split_dyn_DHP_base_less();
        void split_dyn_DHP_base_cmpmix();
        void split_dyn_DHP_base_cmpmix_stat();
        void split_dyn_DHP_member_cmp();
        void split_dyn_DHP_member_less();
        void split_dyn_DHP_member_cmpmix();
        void split_dyn_DHP_member_cmpmix_stat();
        void split_st_DHP_base_cmp();
        void split_st_DHP_base_less();
        void split_st_DHP_base_cmpmix();
        void split_st_DHP_base_cmpmix_stat();
        void split_st_DHP_member_cmp();
        void split_st_DHP_member_less();
        void split_st_DHP_member_cmpmix();
        void split_st_DHP_member_cmpmix_stat();

        void split_dyn_RCU_GPI_base_cmp();
        void split_dyn_RCU_GPI_base_less();
        void split_dyn_RCU_GPI_base_cmpmix();
        void split_dyn_RCU_GPI_base_cmpmix_stat();
        void split_dyn_RCU_GPI_member_cmp();
        void split_dyn_RCU_GPI_member_less();
        void split_dyn_RCU_GPI_member_cmpmix();
        void split_dyn_RCU_GPI_member_cmpmix_stat();
        void split_st_RCU_GPI_base_cmp();
        void split_st_RCU_GPI_base_less();
        void split_st_RCU_GPI_base_cmpmix();
        void split_st_RCU_GPI_base_cmpmix_stat();
        void split_st_RCU_GPI_member_cmp();
        void split_st_RCU_GPI_member_less();
        void split_st_RCU_GPI_member_cmpmix();
        void split_st_RCU_GPI_member_cmpmix_stat();

        void split_dyn_RCU_GPB_base_cmp();
        void split_dyn_RCU_GPB_base_less();
        void split_dyn_RCU_GPB_base_cmpmix();
        void split_dyn_RCU_GPB_base_cmpmix_stat();
        void split_dyn_RCU_GPB_member_cmp();
        void split_dyn_RCU_GPB_member_less();
        void split_dyn_RCU_GPB_member_cmpmix();
        void split_dyn_RCU_GPB_member_cmpmix_stat();
        void split_st_RCU_GPB_base_cmp();
        void split_st_RCU_GPB_base_less();
        void split_st_RCU_GPB_base_cmpmix();
        void split_st_RCU_GPB_base_cmpmix_stat();
        void split_st_RCU_GPB_member_cmp();
        void split_st_RCU_GPB_member_less();
        void split_st_RCU_GPB_member_cmpmix();
        void split_st_RCU_GPB_member_cmpmix_stat();

        void split_dyn_RCU_GPT_base_cmp();
        void split_dyn_RCU_GPT_base_less();
        void split_dyn_RCU_GPT_base_cmpmix();
        void split_dyn_RCU_GPT_base_cmpmix_stat();
        void split_dyn_RCU_GPT_member_cmp();
        void split_dyn_RCU_GPT_member_less();
        void split_dyn_RCU_GPT_member_cmpmix();
        void split_dyn_RCU_GPT_member_cmpmix_stat();
        void split_st_RCU_GPT_base_cmp();
        void split_st_RCU_GPT_base_less();
        void split_st_RCU_GPT_base_cmpmix();
        void split_st_RCU_GPT_base_cmpmix_stat();
        void split_st_RCU_GPT_member_cmp();
        void split_st_RCU_GPT_member_less();
        void split_st_RCU_GPT_member_cmpmix();
        void split_st_RCU_GPT_member_cmpmix_stat();

        void split_dyn_RCU_SHB_base_cmp();
        void split_dyn_RCU_SHB_base_less();
        void split_dyn_RCU_SHB_base_cmpmix();
        void split_dyn_RCU_SHB_base_cmpmix_stat();
        void split_dyn_RCU_SHB_member_cmp();
        void split_dyn_RCU_SHB_member_less();
        void split_dyn_RCU_SHB_member_cmpmix();
        void split_dyn_RCU_SHB_member_cmpmix_stat();
        void split_st_RCU_SHB_base_cmp();
        void split_st_RCU_SHB_base_less();
        void split_st_RCU_SHB_base_cmpmix();
        void split_st_RCU_SHB_base_cmpmix_stat();
        void split_st_RCU_SHB_member_cmp();
        void split_st_RCU_SHB_member_less();
        void split_st_RCU_SHB_member_cmpmix();
        void split_st_RCU_SHB_member_cmpmix_stat();

        void split_dyn_RCU_SHT_base_cmp();
        void split_dyn_RCU_SHT_base_less();
        void split_dyn_RCU_SHT_base_cmpmix();
        void split_dyn_RCU_SHT_base_cmpmix_stat();
        void split_dyn_RCU_SHT_member_cmp();
        void split_dyn_RCU_SHT_member_less();
        void split_dyn_RCU_SHT_member_cmpmix();
        void split_dyn_RCU_SHT_member_cmpmix_stat();
        void split_st_RCU_SHT_base_cmp();
        void split_st_RCU_SHT_base_less();
        void split_st_RCU_SHT_base_cmpmix();
        void split_st_RCU_SHT_base_cmpmix_stat();
        void split_st_RCU_SHT_member_cmp();
        void split_st_RCU_SHT_member_less();
        void split_st_RCU_SHT_member_cmpmix();
        void split_st_RCU_SHT_member_cmpmix_stat();

        void split_dyn_nogc_base_cmp();
        void split_dyn_nogc_base_less();
        void split_dyn_nogc_base_cmpmix();
        void split_dyn_nogc_base_cmpmix_stat();
        void split_dyn_nogc_member_cmp();
        void split_dyn_nogc_member_less();
        void split_dyn_nogc_member_cmpmix();
        void split_dyn_nogc_member_cmpmix_stat();
        void split_st_nogc_base_cmp();
        void split_st_nogc_base_less();
        void split_st_nogc_base_cmpmix();
        void split_st_nogc_base_cmpmix_stat();
        void split_st_nogc_member_cmp();
        void split_st_nogc_member_less();
        void split_st_nogc_member_cmpmix();
        void split_st_nogc_member_cmpmix_stat();


        // SplitListSet + LazyList [dyn - dynamic bucket table, st - static bucket table]
        void split_dyn_HP_base_cmp_lazy();
        void split_dyn_HP_base_less_lazy();
        void split_dyn_HP_base_cmpmix_lazy();
        void split_dyn_HP_base_cmpmix_stat_lazy();
        void split_dyn_HP_member_cmp_lazy();
        void split_dyn_HP_member_less_lazy();
        void split_dyn_HP_member_cmpmix_lazy();
        void split_dyn_HP_member_cmpmix_stat_lazy();
        void split_st_HP_base_cmp_lazy();
        void split_st_HP_base_less_lazy();
        void split_st_HP_base_cmpmix_lazy();
        void split_st_HP_base_cmpmix_stat_lazy();
        void split_st_HP_member_cmp_lazy();
        void split_st_HP_member_less_lazy();
        void split_st_HP_member_cmpmix_lazy();
        void split_st_HP_member_cmpmix_stat_lazy();

        void split_dyn_DHP_base_cmp_lazy();
        void split_dyn_DHP_base_less_lazy();
        void split_dyn_DHP_base_cmpmix_lazy();
        void split_dyn_DHP_base_cmpmix_stat_lazy();
        void split_dyn_DHP_member_cmp_lazy();
        void split_dyn_DHP_member_less_lazy();
        void split_dyn_DHP_member_cmpmix_lazy();
        void split_dyn_DHP_member_cmpmix_stat_lazy();
        void split_st_DHP_base_cmp_lazy();
        void split_st_DHP_base_less_lazy();
        void split_st_DHP_base_cmpmix_lazy();
        void split_st_DHP_base_cmpmix_stat_lazy();
        void split_st_DHP_member_cmp_lazy();
        void split_st_DHP_member_less_lazy();
        void split_st_DHP_member_cmpmix_lazy();
        void split_st_DHP_member_cmpmix_stat_lazy();

        void split_dyn_RCU_GPI_base_cmp_lazy();
        void split_dyn_RCU_GPI_base_less_lazy();
        void split_dyn_RCU_GPI_base_cmpmix_lazy();
        void split_dyn_RCU_GPI_base_cmpmix_stat_lazy();
        void split_dyn_RCU_GPI_member_cmp_lazy();
        void split_dyn_RCU_GPI_member_less_lazy();
        void split_dyn_RCU_GPI_member_cmpmix_lazy();
        void split_dyn_RCU_GPI_member_cmpmix_stat_lazy();
        void split_st_RCU_GPI_base_cmp_lazy();
        void split_st_RCU_GPI_base_less_lazy();
        void split_st_RCU_GPI_base_cmpmix_lazy();
        void split_st_RCU_GPI_base_cmpmix_stat_lazy();
        void split_st_RCU_GPI_member_cmp_lazy();
        void split_st_RCU_GPI_member_less_lazy();
        void split_st_RCU_GPI_member_cmpmix_lazy();
        void split_st_RCU_GPI_member_cmpmix_stat_lazy();

        void split_dyn_RCU_GPB_base_cmp_lazy();
        void split_dyn_RCU_GPB_base_less_lazy();
        void split_dyn_RCU_GPB_base_cmpmix_lazy();
        void split_dyn_RCU_GPB_base_cmpmix_stat_lazy();
        void split_dyn_RCU_GPB_member_cmp_lazy();
        void split_dyn_RCU_GPB_member_less_lazy();
        void split_dyn_RCU_GPB_member_cmpmix_lazy();
        void split_dyn_RCU_GPB_member_cmpmix_stat_lazy();
        void split_st_RCU_GPB_base_cmp_lazy();
        void split_st_RCU_GPB_base_less_lazy();
        void split_st_RCU_GPB_base_cmpmix_lazy();
        void split_st_RCU_GPB_base_cmpmix_stat_lazy();
        void split_st_RCU_GPB_member_cmp_lazy();
        void split_st_RCU_GPB_member_less_lazy();
        void split_st_RCU_GPB_member_cmpmix_lazy();
        void split_st_RCU_GPB_member_cmpmix_stat_lazy();

        void split_dyn_RCU_GPT_base_cmp_lazy();
        void split_dyn_RCU_GPT_base_less_lazy();
        void split_dyn_RCU_GPT_base_cmpmix_lazy();
        void split_dyn_RCU_GPT_base_cmpmix_stat_lazy();
        void split_dyn_RCU_GPT_member_cmp_lazy();
        void split_dyn_RCU_GPT_member_less_lazy();
        void split_dyn_RCU_GPT_member_cmpmix_lazy();
        void split_dyn_RCU_GPT_member_cmpmix_stat_lazy();
        void split_st_RCU_GPT_base_cmp_lazy();
        void split_st_RCU_GPT_base_less_lazy();
        void split_st_RCU_GPT_base_cmpmix_lazy();
        void split_st_RCU_GPT_base_cmpmix_stat_lazy();
        void split_st_RCU_GPT_member_cmp_lazy();
        void split_st_RCU_GPT_member_less_lazy();
        void split_st_RCU_GPT_member_cmpmix_lazy();
        void split_st_RCU_GPT_member_cmpmix_stat_lazy();

        void split_dyn_RCU_SHB_base_cmp_lazy();
        void split_dyn_RCU_SHB_base_less_lazy();
        void split_dyn_RCU_SHB_base_cmpmix_lazy();
        void split_dyn_RCU_SHB_base_cmpmix_stat_lazy();
        void split_dyn_RCU_SHB_member_cmp_lazy();
        void split_dyn_RCU_SHB_member_less_lazy();
        void split_dyn_RCU_SHB_member_cmpmix_lazy();
        void split_dyn_RCU_SHB_member_cmpmix_stat_lazy();
        void split_st_RCU_SHB_base_cmp_lazy();
        void split_st_RCU_SHB_base_less_lazy();
        void split_st_RCU_SHB_base_cmpmix_lazy();
        void split_st_RCU_SHB_base_cmpmix_stat_lazy();
        void split_st_RCU_SHB_member_cmp_lazy();
        void split_st_RCU_SHB_member_less_lazy();
        void split_st_RCU_SHB_member_cmpmix_lazy();
        void split_st_RCU_SHB_member_cmpmix_stat_lazy();

        void split_dyn_RCU_SHT_base_cmp_lazy();
        void split_dyn_RCU_SHT_base_less_lazy();
        void split_dyn_RCU_SHT_base_cmpmix_lazy();
        void split_dyn_RCU_SHT_base_cmpmix_stat_lazy();
        void split_dyn_RCU_SHT_member_cmp_lazy();
        void split_dyn_RCU_SHT_member_less_lazy();
        void split_dyn_RCU_SHT_member_cmpmix_lazy();
        void split_dyn_RCU_SHT_member_cmpmix_stat_lazy();
        void split_st_RCU_SHT_base_cmp_lazy();
        void split_st_RCU_SHT_base_less_lazy();
        void split_st_RCU_SHT_base_cmpmix_lazy();
        void split_st_RCU_SHT_base_cmpmix_stat_lazy();
        void split_st_RCU_SHT_member_cmp_lazy();
        void split_st_RCU_SHT_member_less_lazy();
        void split_st_RCU_SHT_member_cmpmix_lazy();
        void split_st_RCU_SHT_member_cmpmix_stat_lazy();

        void split_dyn_nogc_base_cmp_lazy();
        void split_dyn_nogc_base_less_lazy();
        void split_dyn_nogc_base_cmpmix_lazy();
        void split_dyn_nogc_base_cmpmix_stat_lazy();
        void split_dyn_nogc_member_cmp_lazy();
        void split_dyn_nogc_member_less_lazy();
        void split_dyn_nogc_member_cmpmix_lazy();
        void split_dyn_nogc_member_cmpmix_stat_lazy();
        void split_st_nogc_base_cmp_lazy();
        void split_st_nogc_base_less_lazy();
        void split_st_nogc_base_cmpmix_lazy();
        void split_st_nogc_base_cmpmix_stat_lazy();
        void split_st_nogc_member_cmp_lazy();
        void split_st_nogc_member_less_lazy();
        void split_st_nogc_member_cmpmix_lazy();
        void split_st_nogc_member_cmpmix_stat_lazy();

        CPPUNIT_TEST_SUITE(IntrusiveHashSetHdrTest)
            CPPUNIT_TEST(HP_base_cmp)
            CPPUNIT_TEST(HP_base_less)
            CPPUNIT_TEST(HP_base_cmpmix)
            CPPUNIT_TEST(HP_member_cmp)
            CPPUNIT_TEST(HP_member_less)
            CPPUNIT_TEST(HP_member_cmpmix)

            CPPUNIT_TEST(DHP_base_cmp)
            CPPUNIT_TEST(DHP_base_less)
            CPPUNIT_TEST(DHP_base_cmpmix)
            CPPUNIT_TEST(DHP_member_cmp)
            CPPUNIT_TEST(DHP_member_less)
            CPPUNIT_TEST(DHP_member_cmpmix)

            CPPUNIT_TEST(RCU_GPI_base_cmp)
            CPPUNIT_TEST(RCU_GPI_base_less)
            CPPUNIT_TEST(RCU_GPI_base_cmpmix)
            CPPUNIT_TEST(RCU_GPI_member_cmp)
            CPPUNIT_TEST(RCU_GPI_member_less)
            CPPUNIT_TEST(RCU_GPI_member_cmpmix)

            CPPUNIT_TEST(RCU_GPB_base_cmp)
            CPPUNIT_TEST(RCU_GPB_base_less)
            CPPUNIT_TEST(RCU_GPB_base_cmpmix)
            CPPUNIT_TEST(RCU_GPB_member_cmp)
            CPPUNIT_TEST(RCU_GPB_member_less)
            CPPUNIT_TEST(RCU_GPB_member_cmpmix)

            CPPUNIT_TEST(RCU_GPT_base_cmp)
            CPPUNIT_TEST(RCU_GPT_base_less)
            CPPUNIT_TEST(RCU_GPT_base_cmpmix)
            CPPUNIT_TEST(RCU_GPT_member_cmp)
            CPPUNIT_TEST(RCU_GPT_member_less)
            CPPUNIT_TEST(RCU_GPT_member_cmpmix)

            CPPUNIT_TEST(RCU_SHB_base_cmp)
            CPPUNIT_TEST(RCU_SHB_base_less)
            CPPUNIT_TEST(RCU_SHB_base_cmpmix)
            CPPUNIT_TEST(RCU_SHB_member_cmp)
            CPPUNIT_TEST(RCU_SHB_member_less)
            CPPUNIT_TEST(RCU_SHB_member_cmpmix)

            CPPUNIT_TEST(RCU_SHT_base_cmp)
            CPPUNIT_TEST(RCU_SHT_base_less)
            CPPUNIT_TEST(RCU_SHT_base_cmpmix)
            CPPUNIT_TEST(RCU_SHT_member_cmp)
            CPPUNIT_TEST(RCU_SHT_member_less)
            CPPUNIT_TEST(RCU_SHT_member_cmpmix)

            CPPUNIT_TEST(nogc_base_cmp)
            CPPUNIT_TEST(nogc_base_less)
            CPPUNIT_TEST(nogc_base_cmpmix)
            CPPUNIT_TEST(nogc_member_cmp)
            CPPUNIT_TEST(nogc_member_less)
            CPPUNIT_TEST(nogc_member_cmpmix)

            CPPUNIT_TEST(HP_base_cmp_lazy)
            CPPUNIT_TEST(HP_base_less_lazy)
            CPPUNIT_TEST(HP_base_cmpmix_lazy)
            CPPUNIT_TEST(HP_member_cmp_lazy)
            CPPUNIT_TEST(HP_member_less_lazy)
            CPPUNIT_TEST(HP_member_cmpmix_lazy)

            CPPUNIT_TEST(DHP_base_cmp_lazy)
            CPPUNIT_TEST(DHP_base_less_lazy)
            CPPUNIT_TEST(DHP_base_cmpmix_lazy)
            CPPUNIT_TEST(DHP_member_cmp_lazy)
            CPPUNIT_TEST(DHP_member_less_lazy)
            CPPUNIT_TEST(DHP_member_cmpmix_lazy)

            CPPUNIT_TEST(RCU_GPI_base_cmp_lazy)
            CPPUNIT_TEST(RCU_GPI_base_less_lazy)
            CPPUNIT_TEST(RCU_GPI_base_cmpmix_lazy)
            CPPUNIT_TEST(RCU_GPI_member_cmp_lazy)
            CPPUNIT_TEST(RCU_GPI_member_less_lazy)
            CPPUNIT_TEST(RCU_GPI_member_cmpmix_lazy)

            CPPUNIT_TEST(RCU_GPB_base_cmp_lazy)
            CPPUNIT_TEST(RCU_GPB_base_less_lazy)
            CPPUNIT_TEST(RCU_GPB_base_cmpmix_lazy)
            CPPUNIT_TEST(RCU_GPB_member_cmp_lazy)
            CPPUNIT_TEST(RCU_GPB_member_less_lazy)
            CPPUNIT_TEST(RCU_GPB_member_cmpmix_lazy)

            CPPUNIT_TEST(RCU_GPT_base_cmp_lazy)
            CPPUNIT_TEST(RCU_GPT_base_less_lazy)
            CPPUNIT_TEST(RCU_GPT_base_cmpmix_lazy)
            CPPUNIT_TEST(RCU_GPT_member_cmp_lazy)
            CPPUNIT_TEST(RCU_GPT_member_less_lazy)
            CPPUNIT_TEST(RCU_GPT_member_cmpmix_lazy)

            CPPUNIT_TEST(RCU_SHB_base_cmp_lazy)
            CPPUNIT_TEST(RCU_SHB_base_less_lazy)
            CPPUNIT_TEST(RCU_SHB_base_cmpmix_lazy)
            CPPUNIT_TEST(RCU_SHB_member_cmp_lazy)
            CPPUNIT_TEST(RCU_SHB_member_less_lazy)
            CPPUNIT_TEST(RCU_SHB_member_cmpmix_lazy)

            CPPUNIT_TEST(RCU_SHT_base_cmp_lazy)
            CPPUNIT_TEST(RCU_SHT_base_less_lazy)
            CPPUNIT_TEST(RCU_SHT_base_cmpmix_lazy)
            CPPUNIT_TEST(RCU_SHT_member_cmp_lazy)
            CPPUNIT_TEST(RCU_SHT_member_less_lazy)
            CPPUNIT_TEST(RCU_SHT_member_cmpmix_lazy)

            CPPUNIT_TEST(nogc_base_cmp_lazy)
            CPPUNIT_TEST(nogc_base_less_lazy)
            CPPUNIT_TEST(nogc_base_cmpmix_lazy)
            CPPUNIT_TEST(nogc_member_cmp_lazy)
            CPPUNIT_TEST(nogc_member_less_lazy)
            CPPUNIT_TEST(nogc_member_cmpmix_lazy)

            CPPUNIT_TEST(split_dyn_HP_base_cmp)
            CPPUNIT_TEST(split_dyn_HP_base_less)
            CPPUNIT_TEST(split_dyn_HP_base_cmpmix)
            CPPUNIT_TEST( split_dyn_HP_base_cmpmix_stat )
            CPPUNIT_TEST( split_dyn_HP_member_cmp )
            CPPUNIT_TEST(split_dyn_HP_member_less)
            CPPUNIT_TEST(split_dyn_HP_member_cmpmix)
            CPPUNIT_TEST( split_dyn_HP_member_cmpmix_stat )
            CPPUNIT_TEST( split_st_HP_base_cmp )
            CPPUNIT_TEST(split_st_HP_base_less)
            CPPUNIT_TEST(split_st_HP_base_cmpmix)
            CPPUNIT_TEST( split_st_HP_base_cmpmix_stat )
            CPPUNIT_TEST( split_st_HP_member_cmp )
            CPPUNIT_TEST(split_st_HP_member_less)
            CPPUNIT_TEST(split_st_HP_member_cmpmix)
            CPPUNIT_TEST( split_st_HP_member_cmpmix_stat )

            CPPUNIT_TEST(split_dyn_DHP_base_cmp)
            CPPUNIT_TEST(split_dyn_DHP_base_less)
            CPPUNIT_TEST(split_dyn_DHP_base_cmpmix)
            CPPUNIT_TEST( split_dyn_DHP_base_cmpmix_stat )
            CPPUNIT_TEST( split_dyn_DHP_member_cmp )
            CPPUNIT_TEST(split_dyn_DHP_member_less)
            CPPUNIT_TEST(split_dyn_DHP_member_cmpmix)
            CPPUNIT_TEST( split_dyn_DHP_member_cmpmix_stat )
            CPPUNIT_TEST( split_st_DHP_base_cmp )
            CPPUNIT_TEST(split_st_DHP_base_less)
            CPPUNIT_TEST(split_st_DHP_base_cmpmix)
            CPPUNIT_TEST( split_st_DHP_base_cmpmix_stat )
            CPPUNIT_TEST( split_st_DHP_member_cmp )
            CPPUNIT_TEST(split_st_DHP_member_less)
            CPPUNIT_TEST(split_st_DHP_member_cmpmix)
            CPPUNIT_TEST( split_st_DHP_member_cmpmix_stat )

            CPPUNIT_TEST(split_dyn_RCU_GPI_base_cmp)
            CPPUNIT_TEST(split_dyn_RCU_GPI_base_less)
            CPPUNIT_TEST(split_dyn_RCU_GPI_base_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_GPI_base_cmpmix_stat )
            CPPUNIT_TEST( split_dyn_RCU_GPI_member_cmp )
            CPPUNIT_TEST(split_dyn_RCU_GPI_member_less)
            CPPUNIT_TEST(split_dyn_RCU_GPI_member_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_GPI_member_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_GPI_base_cmp )
            CPPUNIT_TEST(split_st_RCU_GPI_base_less)
            CPPUNIT_TEST(split_st_RCU_GPI_base_cmpmix)
            CPPUNIT_TEST( split_st_RCU_GPI_base_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_GPI_member_cmp )
            CPPUNIT_TEST(split_st_RCU_GPI_member_less)
            CPPUNIT_TEST(split_st_RCU_GPI_member_cmpmix)
            CPPUNIT_TEST( split_st_RCU_GPI_member_cmpmix_stat )

            CPPUNIT_TEST(split_dyn_RCU_GPB_base_cmp)
            CPPUNIT_TEST(split_dyn_RCU_GPB_base_less)
            CPPUNIT_TEST(split_dyn_RCU_GPB_base_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_GPB_base_cmpmix_stat )
            CPPUNIT_TEST( split_dyn_RCU_GPB_member_cmp )
            CPPUNIT_TEST(split_dyn_RCU_GPB_member_less)
            CPPUNIT_TEST(split_dyn_RCU_GPB_member_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_GPB_member_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_GPB_base_cmp )
            CPPUNIT_TEST(split_st_RCU_GPB_base_less)
            CPPUNIT_TEST(split_st_RCU_GPB_base_cmpmix)
            CPPUNIT_TEST( split_st_RCU_GPB_base_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_GPB_member_cmp )
            CPPUNIT_TEST(split_st_RCU_GPB_member_less)
            CPPUNIT_TEST(split_st_RCU_GPB_member_cmpmix)
            CPPUNIT_TEST( split_st_RCU_GPB_member_cmpmix_stat )

            CPPUNIT_TEST(split_dyn_RCU_GPT_base_cmp)
            CPPUNIT_TEST(split_dyn_RCU_GPT_base_less)
            CPPUNIT_TEST(split_dyn_RCU_GPT_base_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_GPT_base_cmpmix_stat )
            CPPUNIT_TEST( split_dyn_RCU_GPT_member_cmp )
            CPPUNIT_TEST(split_dyn_RCU_GPT_member_less)
            CPPUNIT_TEST(split_dyn_RCU_GPT_member_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_GPT_member_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_GPT_base_cmp )
            CPPUNIT_TEST(split_st_RCU_GPT_base_less)
            CPPUNIT_TEST(split_st_RCU_GPT_base_cmpmix)
            CPPUNIT_TEST( split_st_RCU_GPT_base_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_GPT_member_cmp )
            CPPUNIT_TEST(split_st_RCU_GPT_member_less)
            CPPUNIT_TEST(split_st_RCU_GPT_member_cmpmix)
            CPPUNIT_TEST( split_st_RCU_GPT_member_cmpmix_stat )

            CPPUNIT_TEST(split_dyn_RCU_SHB_base_cmp)
            CPPUNIT_TEST(split_dyn_RCU_SHB_base_less)
            CPPUNIT_TEST(split_dyn_RCU_SHB_base_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_SHB_base_cmpmix_stat )
            CPPUNIT_TEST( split_dyn_RCU_SHB_member_cmp )
            CPPUNIT_TEST(split_dyn_RCU_SHB_member_less)
            CPPUNIT_TEST(split_dyn_RCU_SHB_member_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_SHB_member_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_SHB_base_cmp )
            CPPUNIT_TEST(split_st_RCU_SHB_base_less)
            CPPUNIT_TEST(split_st_RCU_SHB_base_cmpmix)
            CPPUNIT_TEST( split_st_RCU_SHB_base_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_SHB_member_cmp )
            CPPUNIT_TEST(split_st_RCU_SHB_member_less)
            CPPUNIT_TEST(split_st_RCU_SHB_member_cmpmix)
            CPPUNIT_TEST( split_st_RCU_SHB_member_cmpmix_stat )

            CPPUNIT_TEST(split_dyn_RCU_SHT_base_cmp)
            CPPUNIT_TEST(split_dyn_RCU_SHT_base_less)
            CPPUNIT_TEST(split_dyn_RCU_SHT_base_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_SHT_base_cmpmix_stat )
            CPPUNIT_TEST( split_dyn_RCU_SHT_member_cmp )
            CPPUNIT_TEST(split_dyn_RCU_SHT_member_less)
            CPPUNIT_TEST(split_dyn_RCU_SHT_member_cmpmix)
            CPPUNIT_TEST( split_dyn_RCU_SHT_member_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_SHT_base_cmp )
            CPPUNIT_TEST(split_st_RCU_SHT_base_less)
            CPPUNIT_TEST(split_st_RCU_SHT_base_cmpmix)
            CPPUNIT_TEST( split_st_RCU_SHT_base_cmpmix_stat )
            CPPUNIT_TEST( split_st_RCU_SHT_member_cmp )
            CPPUNIT_TEST(split_st_RCU_SHT_member_less)
            CPPUNIT_TEST(split_st_RCU_SHT_member_cmpmix)
            CPPUNIT_TEST( split_st_RCU_SHT_member_cmpmix_stat )

            CPPUNIT_TEST(split_dyn_nogc_base_cmp)
            CPPUNIT_TEST(split_dyn_nogc_base_less)
            CPPUNIT_TEST(split_dyn_nogc_base_cmpmix)
            CPPUNIT_TEST( split_dyn_nogc_base_cmpmix_stat )
            CPPUNIT_TEST( split_dyn_nogc_member_cmp )
            CPPUNIT_TEST(split_dyn_nogc_member_less)
            CPPUNIT_TEST(split_dyn_nogc_member_cmpmix)
            CPPUNIT_TEST( split_dyn_nogc_member_cmpmix_stat )
            CPPUNIT_TEST( split_st_nogc_base_cmp )
            CPPUNIT_TEST(split_st_nogc_base_less)
            CPPUNIT_TEST(split_st_nogc_base_cmpmix)
            CPPUNIT_TEST( split_st_nogc_base_cmpmix_stat )
            CPPUNIT_TEST( split_st_nogc_member_cmp )
            CPPUNIT_TEST(split_st_nogc_member_less)
            CPPUNIT_TEST(split_st_nogc_member_cmpmix)
            CPPUNIT_TEST( split_st_nogc_member_cmpmix_stat )

            CPPUNIT_TEST(split_dyn_HP_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_HP_base_less_lazy)
            CPPUNIT_TEST(split_dyn_HP_base_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_HP_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_dyn_HP_member_cmp_lazy )
            CPPUNIT_TEST(split_dyn_HP_member_less_lazy)
            CPPUNIT_TEST(split_dyn_HP_member_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_HP_member_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_HP_base_cmp_lazy )
            CPPUNIT_TEST(split_st_HP_base_less_lazy)
            CPPUNIT_TEST(split_st_HP_base_cmpmix_lazy)
            CPPUNIT_TEST( split_st_HP_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_HP_member_cmp_lazy )
            CPPUNIT_TEST(split_st_HP_member_less_lazy)
            CPPUNIT_TEST(split_st_HP_member_cmpmix_lazy)
            CPPUNIT_TEST( split_st_HP_member_cmpmix_stat_lazy )

            CPPUNIT_TEST(split_dyn_DHP_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_DHP_base_less_lazy)
            CPPUNIT_TEST(split_dyn_DHP_base_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_DHP_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_dyn_DHP_member_cmp_lazy )
            CPPUNIT_TEST(split_dyn_DHP_member_less_lazy)
            CPPUNIT_TEST(split_dyn_DHP_member_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_DHP_member_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_DHP_base_cmp_lazy )
            CPPUNIT_TEST(split_st_DHP_base_less_lazy)
            CPPUNIT_TEST(split_st_DHP_base_cmpmix_lazy)
            CPPUNIT_TEST( split_st_DHP_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_DHP_member_cmp_lazy )
            CPPUNIT_TEST(split_st_DHP_member_less_lazy)
            CPPUNIT_TEST(split_st_DHP_member_cmpmix_lazy)
            CPPUNIT_TEST( split_st_DHP_member_cmpmix_stat_lazy )

            CPPUNIT_TEST(split_dyn_RCU_GPI_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_RCU_GPI_base_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_GPI_base_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_GPI_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_dyn_RCU_GPI_member_cmp_lazy )
            CPPUNIT_TEST(split_dyn_RCU_GPI_member_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_GPI_member_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_GPI_member_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_GPI_base_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_GPI_base_less_lazy)
            CPPUNIT_TEST(split_st_RCU_GPI_base_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_GPI_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_GPI_member_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_GPI_member_less_lazy)
            CPPUNIT_TEST(split_st_RCU_GPI_member_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_GPI_member_cmpmix_stat_lazy )

            CPPUNIT_TEST(split_dyn_RCU_GPB_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_RCU_GPB_base_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_GPB_base_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_GPB_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_dyn_RCU_GPB_member_cmp_lazy )
            CPPUNIT_TEST(split_dyn_RCU_GPB_member_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_GPB_member_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_GPB_member_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_GPB_base_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_GPB_base_less_lazy)
            CPPUNIT_TEST(split_st_RCU_GPB_base_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_GPB_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_GPB_member_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_GPB_member_less_lazy)
            CPPUNIT_TEST(split_st_RCU_GPB_member_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_GPB_member_cmpmix_stat_lazy )

            CPPUNIT_TEST(split_dyn_RCU_GPT_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_RCU_GPT_base_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_GPT_base_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_GPT_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_dyn_RCU_GPT_member_cmp_lazy )
            CPPUNIT_TEST(split_dyn_RCU_GPT_member_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_GPT_member_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_GPT_member_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_GPT_base_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_GPT_base_less_lazy)
            CPPUNIT_TEST(split_st_RCU_GPT_base_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_GPT_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_GPT_member_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_GPT_member_less_lazy)
            CPPUNIT_TEST(split_st_RCU_GPT_member_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_GPT_member_cmpmix_stat_lazy )

            CPPUNIT_TEST(split_dyn_RCU_SHB_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_RCU_SHB_base_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_SHB_base_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_SHB_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_dyn_RCU_SHB_member_cmp_lazy )
            CPPUNIT_TEST(split_dyn_RCU_SHB_member_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_SHB_member_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_SHB_member_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_SHB_base_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_SHB_base_less_lazy)
            CPPUNIT_TEST(split_st_RCU_SHB_base_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_SHB_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_SHB_member_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_SHB_member_less_lazy)
            CPPUNIT_TEST(split_st_RCU_SHB_member_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_SHB_member_cmpmix_stat_lazy )

            CPPUNIT_TEST(split_dyn_RCU_SHT_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_RCU_SHT_base_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_SHT_base_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_SHT_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_dyn_RCU_SHT_member_cmp_lazy )
            CPPUNIT_TEST(split_dyn_RCU_SHT_member_less_lazy)
            CPPUNIT_TEST(split_dyn_RCU_SHT_member_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_RCU_SHT_member_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_SHT_base_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_SHT_base_less_lazy)
            CPPUNIT_TEST(split_st_RCU_SHT_base_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_SHT_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_RCU_SHT_member_cmp_lazy )
            CPPUNIT_TEST(split_st_RCU_SHT_member_less_lazy)
            CPPUNIT_TEST(split_st_RCU_SHT_member_cmpmix_lazy)
            CPPUNIT_TEST( split_st_RCU_SHT_member_cmpmix_stat_lazy )

            CPPUNIT_TEST(split_dyn_nogc_base_cmp_lazy)
            CPPUNIT_TEST(split_dyn_nogc_base_less_lazy)
            CPPUNIT_TEST(split_dyn_nogc_base_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_nogc_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_dyn_nogc_member_cmp_lazy )
            CPPUNIT_TEST(split_dyn_nogc_member_less_lazy)
            CPPUNIT_TEST(split_dyn_nogc_member_cmpmix_lazy)
            CPPUNIT_TEST( split_dyn_nogc_member_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_nogc_base_cmp_lazy )
            CPPUNIT_TEST(split_st_nogc_base_less_lazy)
            CPPUNIT_TEST(split_st_nogc_base_cmpmix_lazy)
            CPPUNIT_TEST( split_st_nogc_base_cmpmix_stat_lazy )
            CPPUNIT_TEST( split_st_nogc_member_cmp_lazy )
            CPPUNIT_TEST(split_st_nogc_member_less_lazy)
            CPPUNIT_TEST(split_st_nogc_member_cmpmix_lazy)
            CPPUNIT_TEST( split_st_nogc_member_cmpmix_stat_lazy )
        CPPUNIT_TEST_SUITE_END()

    };

}   // namespace set

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_SET_H
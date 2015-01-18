/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "cppunit/cppunit_proxy.h"
#include <cds/opt/hash.h>

// cds::intrusive namespace forward declaration
namespace cds { namespace intrusive {}}

namespace set {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    // MichaelHashSet
    class IntrusiveCuckooSetHdrTest: public CppUnitMini::TestCase
    {
    public:
        struct stat
        {
            unsigned int nDisposeCount  ;   // count of disposer calling
            unsigned int nFindCount     ;   // count of find-functor calling
            unsigned int nFindArgCount;
            unsigned int nInsertCount;
            unsigned int nEnsureNewCount;
            unsigned int nEnsureCount;
            unsigned int nEraseCount;

            stat()
            {
                memset( this, 0, sizeof(*this));
            }

            stat& operator=( stat const& s)
            {
                memcpy( this, &s, sizeof(*this));
                return *this;
            }
        };

        struct item
        {
            int nKey;
            int nVal;

            item()
            {}

            item(int key, int val)
                : nKey( key )
                , nVal(val)
            {}

            item(const item& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey;
            }

            int val() const
            {
                return nVal;
            }
        };

        template <typename Hook>
        struct base_item
            : public item
            , public Hook
            , public stat

        {
            base_item()
            {}

            base_item(int key, int val)
                : item( key, val )
            {}

            base_item(const base_item& v )
                : item( static_cast<item const&>(v) )
                , stat()
            {}
        };

        template <typename Hook>
        struct member_item
            : public item
            , public stat
        {
            Hook hMember;

            member_item()
            {}

            member_item(int key, int val)
                : item( key, val )
            {}

            member_item(const member_item& v )
                : item( static_cast<item const&>(v))
                , stat()
            {}
        };

        struct find_key {
            int nKey;

            find_key( int key )
                : nKey(key)
            {}
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
            size_t operator()( find_key const& i) const
            {
                return co::v::hash<int>()( i.nKey );
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

        struct insert_functor {
            template <typename Item>
            void operator()( Item& e)
            {
                ++e.nInsertCount;
            }
        };

        struct ensure_functor {
            template <typename Item>
            void operator()( bool bNew, Item& e, Item& arg )
            {
                if ( bNew ) {
                    ++e.nEnsureNewCount;
                    CPPUNIT_ASSERT_CURRENT( &e == &arg );
                }
                else
                    ++e.nEnsureCount;
            }
        };

        struct erase_functor {
            template< typename Item >
            void operator()( Item& e )
            {
                ++e.nEraseCount;
            }
        };

        struct find_functor {
            template <typename Item, typename What>
            void operator()( Item& e, What& )
            {
                ++e.nFindCount;
            }

            template <typename Item>
            void operator()( Item& e, Item& w )
            {
                ++e.nFindCount;
                ++w.nFindArgCount;
            }
        };

        struct less2 {
            template <typename Item>
            bool operator()( Item const& e, find_key const& k ) const
            {
                return e.key() < k.nKey;
            }
            template <typename Item>
            bool operator()( find_key const& k, Item const& e ) const
            {
                return k.nKey < e.key();
            }
            template <typename Item>
            bool operator()( Item const& e, int k ) const
            {
                return e.key() < k;
            }
            template <typename Item>
            bool operator()( int k, Item const& e ) const
            {
                return k < e.key();
            }
        };

        struct equal_to2 {
            template <typename Item>
            bool operator()( Item const& e, find_key const& k ) const
            {
                return e.key() == k.nKey;
            }
            template <typename Item>
            bool operator()( find_key const& k, Item const& e ) const
            {
                return k.nKey == e.key();
            }
            template <typename Item>
            bool operator()( Item const& e, int k ) const
            {
                return e.key() == k;
            }
            template <typename Item>
            bool operator()( int k, Item const& e ) const
            {
                return k == e.key();
            }
        };

        template <typename T>
        struct auto_dispose {
            T * m_pArr;
            auto_dispose( T * pArr ): m_pArr( pArr ) {}
            ~auto_dispose() { delete[] m_pArr; }
        };

        template <class Set>
        void test_with(Set& s)
        {
            typedef typename Set::value_type    value_type;

            int const k1 = 10;
            int const k2 = 25;
            int const k3 = 51;

            int const v1 = 25;
            int const v2 = 56;
            int const v3 = 23;

            value_type e1( k1, v1 );
            value_type e2( k2, v2 );
            value_type e3( k3, v3);

            stat s1 = e1;
            stat s2 = e2;
            stat s3 = e3;

            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( s.size() == 0 );

            CPPUNIT_ASSERT( !s.find(k1));
            CPPUNIT_ASSERT( !s.find_with(k2, typename std::conditional<Set::c_isSorted, less2, equal_to2>::type() ));
            CPPUNIT_ASSERT( !s.find(k3));

            CPPUNIT_ASSERT( s.insert(e1));
            CPPUNIT_ASSERT( s.find(e1));
            CPPUNIT_ASSERT( s.find(k1));
            CPPUNIT_ASSERT( s.find_with(k1, typename std::conditional<Set::c_isSorted, less2, equal_to2>::type()));
            CPPUNIT_ASSERT( !s.find(e2));
            CPPUNIT_ASSERT( !s.find(e3));

            CPPUNIT_ASSERT( e2.nInsertCount == 0 );
            CPPUNIT_ASSERT( s.insert(e2, insert_functor() ));
            CPPUNIT_ASSERT( e2.nInsertCount == 1 );
            CPPUNIT_ASSERT( s.find(e1, find_functor() ));
            CPPUNIT_ASSERT( e1.nFindCount == 1 );
            CPPUNIT_ASSERT( e1.nFindArgCount == 1 );
            CPPUNIT_ASSERT( s.find(k1, find_functor() ));
            CPPUNIT_ASSERT( e1.nFindCount == 2 );
            CPPUNIT_ASSERT( e1.nFindArgCount == 1 );
            CPPUNIT_ASSERT( s.find_with(k2, typename std::conditional<Set::c_isSorted, less2, equal_to2>::type(), find_functor() ));
            CPPUNIT_ASSERT( e2.nFindCount == 1 );
            CPPUNIT_ASSERT( e2.nFindArgCount == 0 );
            CPPUNIT_ASSERT( s.find(e2, find_functor() ));
            CPPUNIT_ASSERT( e2.nFindCount == 2 );
            CPPUNIT_ASSERT( e2.nFindArgCount == 1 );
            CPPUNIT_ASSERT( !s.find(k3, find_functor()));
            CPPUNIT_ASSERT( e3.nFindCount == 0 );
            CPPUNIT_ASSERT( e3.nFindArgCount == 0 );
            CPPUNIT_ASSERT( !s.find(e3, find_functor()));
            CPPUNIT_ASSERT( e3.nFindCount == 0 );
            CPPUNIT_ASSERT( e3.nFindArgCount == 0 );

            s1 = e1 ; s2 = e2 ; s3 = e3;

            CPPUNIT_ASSERT( e3.nEnsureNewCount == 0 );
            CPPUNIT_ASSERT( e3.nEnsureCount == 0 );
            CPPUNIT_ASSERT( s.ensure( e3, ensure_functor() ) == std::make_pair(true, true));
            CPPUNIT_ASSERT( e3.nEnsureNewCount == 1 );
            CPPUNIT_ASSERT( e3.nEnsureCount == 0 );
            CPPUNIT_ASSERT( s.find_with(find_key(k1), typename std::conditional<Set::c_isSorted, less2, equal_to2>::type(), find_functor() ));
            CPPUNIT_ASSERT( e1.nFindCount == s1.nFindCount + 1 );
            CPPUNIT_ASSERT( e1.nFindArgCount == s1.nFindArgCount );
            CPPUNIT_ASSERT( s.find_with(k1, typename std::conditional<Set::c_isSorted, less2, equal_to2>::type(), find_functor() ));
            CPPUNIT_ASSERT( e1.nFindCount == s1.nFindCount + 2 );
            CPPUNIT_ASSERT( e1.nFindArgCount == s1.nFindArgCount );
            CPPUNIT_ASSERT( s.find_with(k2, typename std::conditional<Set::c_isSorted, less2, equal_to2>::type(), find_functor() ));
            CPPUNIT_ASSERT( e2.nFindCount == s2.nFindCount + 1 );
            CPPUNIT_ASSERT( e2.nFindArgCount == s2.nFindArgCount );
            CPPUNIT_ASSERT( s.find_with(find_key(k2), typename std::conditional<Set::c_isSorted, less2, equal_to2>::type() ));
            CPPUNIT_ASSERT( e2.nFindCount == s2.nFindCount + 1 )        ;   // unchanged, no find_functor
            CPPUNIT_ASSERT( e2.nFindArgCount == s2.nFindArgCount );
            CPPUNIT_ASSERT( s.find_with(k3, typename std::conditional<Set::c_isSorted, less2, equal_to2>::type() ));
            CPPUNIT_ASSERT( e3.nFindCount == s3.nFindCount )            ;   // unchanged, no find_functor
            CPPUNIT_ASSERT( e3.nFindArgCount == s3.nFindArgCount );
            CPPUNIT_ASSERT( s.find_with(find_key(k3), typename std::conditional<Set::c_isSorted, less2, equal_to2>::type(), find_functor() ));
            CPPUNIT_ASSERT( e3.nFindCount == s3.nFindCount + 1 );
            CPPUNIT_ASSERT( e3.nFindArgCount == s3.nFindArgCount );

            s1 = e1 ; s2 = e2 ; s3 = e3;

            // insert existing elements
            {
                value_type eu( k2, 1000 );
                CPPUNIT_ASSERT( !s.insert( eu ));
                CPPUNIT_ASSERT( !s.insert( eu, insert_functor() ));
                CPPUNIT_ASSERT( e2.nInsertCount == s2.nInsertCount );

                CPPUNIT_ASSERT( s.ensure( eu, ensure_functor()) == std::make_pair(true, false));
                CPPUNIT_ASSERT( e2.nInsertCount == s2.nInsertCount );
                CPPUNIT_ASSERT( e2.nEnsureCount == s2.nEnsureCount + 1 );
                CPPUNIT_ASSERT( e2.nEnsureNewCount == s2.nEnsureNewCount  );
            }

            s1 = e1 ; s2 = e2 ; s3 = e3;

            // unlink & erase test
            {
                value_type eu( k2, 10 );
                CPPUNIT_ASSERT( !s.unlink(eu));
            }

            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( s.size() == 3 );

            CPPUNIT_ASSERT( s.unlink( e1 ) );
            CPPUNIT_ASSERT( s.erase_with( k2, typename std::conditional<Set::c_isSorted, less2, equal_to2>::type() ) == &e2 );
            CPPUNIT_ASSERT( s.erase( e2 ) == nullptr );
            CPPUNIT_ASSERT( e3.nEraseCount == 0 );
            CPPUNIT_ASSERT( s.erase_with( k3, typename std::conditional<Set::c_isSorted, less2, equal_to2>::type(), erase_functor()) == &e3 );
            CPPUNIT_ASSERT( e3.nEraseCount == 1 );
            CPPUNIT_ASSERT( s.erase( k3, erase_functor() ) == nullptr );
            CPPUNIT_ASSERT( e3.nEraseCount == 1 );

            CPPUNIT_ASSERT( s.insert( e3 ) );
            CPPUNIT_ASSERT( s.erase( e3 ) == &e3 );
            CPPUNIT_ASSERT( e3.nEraseCount == 1 );

            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( s.size() == 0 );

            s1 = e1 ; s2 = e2 ; s3 = e3;

            // clear & clear_and_dispose test
            CPPUNIT_ASSERT( s.insert(e1));
            CPPUNIT_ASSERT( s.insert(e2));
            CPPUNIT_ASSERT( s.insert(e3));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( s.size() == 3 );
            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( s.size() == 0 );

            CPPUNIT_ASSERT( s.insert(e1));
            CPPUNIT_ASSERT( s.insert(e2));
            CPPUNIT_ASSERT( s.insert(e3));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( s.size() == 3 );

            CPPUNIT_ASSERT( e1.nDisposeCount == 0 );
            CPPUNIT_ASSERT( e2.nDisposeCount == 0 );
            CPPUNIT_ASSERT( e3.nDisposeCount == 0 );
            s.clear_and_dispose( faked_disposer() );
            CPPUNIT_ASSERT( e1.nDisposeCount == 1 );
            CPPUNIT_ASSERT( e2.nDisposeCount == 1 );
            CPPUNIT_ASSERT( e3.nDisposeCount == 1 );
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( s.size() == 0 );

            // resize test (up to 64K elements)
            size_t const nSize = 64 * 1024;
            value_type * arr = new value_type[nSize];
            auto_dispose<value_type> ad(arr);
            for ( size_t i = 0; i < nSize; ++i ) {
                value_type * p = new (arr + i) value_type( (int) i, (int) i * 2 );
                CPPUNIT_ASSERT_EX( s.insert( *p, insert_functor() ), "i=" << i );
                CPPUNIT_ASSERT_EX( p->nInsertCount == 1, "i=" << i );
                //for ( size_t j = 0; j <= i; ++j ) {
                //    if ( !s.find((int) j) ) {
                //        CPPUNIT_MSG( "Key " << j << " is not found after inserting key " << i );
                //    }
                //}
            }

            for ( size_t i = 0; i < nSize; ++i )
                CPPUNIT_ASSERT_EX( s.find((int) i), "Key " << i << " is not found" );

            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( s.size() == nSize );
            s.clear_and_dispose( faked_disposer() );
            for ( size_t i = 0; i < nSize; ++i ) {
                CPPUNIT_ASSERT_EX( arr[i].nDisposeCount == 1, "i=" << i );
            }
        }

        template <class Set>
        void test()
        {
            // default ctor
            {
                Set s;
                test_with(s);
            }

            // ctor with explicit initial capacity
            {
                Set s(256);
                test_with(s);
            }
        }

        template <class Set>
        void test_cuckoo()
        {
            unsigned int nProbesetSize = Set::node_type::probeset_size ? Set::node_type::probeset_size : 4;
            Set s( 256, nProbesetSize, nProbesetSize / 2 );
            test_with( s );
        }

        // ***********************************************************
        // Cuckoo hashing (striped)

        void Cuckoo_striped_list_basehook_equal();
        void Cuckoo_striped_vector_basehook_equal();
        void Cuckoo_striped_list_basehook_sort_cmp();
        void Cuckoo_striped_vector_basehook_sort_cmp();
        void Cuckoo_striped_list_basehook_sort_less();
        void Cuckoo_striped_vector_basehook_sort_less();
        void Cuckoo_striped_list_basehook_sort_cmpmix();
        void Cuckoo_striped_vector_basehook_sort_cmpmix();
        void Cuckoo_striped_vector_basehook_sort_cmpmix_stat();

        void Cuckoo_striped_list_basehook_equal_storehash();
        void Cuckoo_striped_vector_basehook_equal_storehash();
        void Cuckoo_striped_list_basehook_sort_cmp_storehash();
        void Cuckoo_striped_vector_basehook_sort_cmp_storehash();
        void Cuckoo_striped_list_basehook_sort_less_storehash();
        void Cuckoo_striped_vector_basehook_sort_less_storehash();
        void Cuckoo_striped_list_basehook_sort_cmpmix_storehash();
        void Cuckoo_striped_vector_basehook_sort_cmpmix_storehash();

        void Cuckoo_striped_list_memberhook_equal();
        void Cuckoo_striped_vector_memberhook_equal();
        void Cuckoo_striped_list_memberhook_sort_cmp();
        void Cuckoo_striped_vector_memberhook_sort_cmp();
        void Cuckoo_striped_list_memberhook_sort_less();
        void Cuckoo_striped_vector_memberhook_sort_less();
        void Cuckoo_striped_list_memberhook_sort_cmpmix();
        void Cuckoo_striped_vector_memberhook_sort_cmpmix();

        void Cuckoo_striped_list_memberhook_equal_storehash();
        void Cuckoo_striped_vector_memberhook_equal_storehash();
        void Cuckoo_striped_list_memberhook_sort_cmp_storehash();
        void Cuckoo_striped_vector_memberhook_sort_cmp_storehash();
        void Cuckoo_striped_list_memberhook_sort_less_storehash();
        void Cuckoo_striped_vector_memberhook_sort_less_storehash();
        void Cuckoo_striped_list_memberhook_sort_cmpmix_storehash();
        void Cuckoo_striped_vector_memberhook_sort_cmpmix_storehash();

        // ***********************************************************
        // Cuckoo hashing (refinable)

        void Cuckoo_refinable_list_basehook_equal();
        void Cuckoo_refinable_vector_basehook_equal();
        void Cuckoo_refinable_list_basehook_sort_cmp();
        void Cuckoo_refinable_vector_basehook_sort_cmp();
        void Cuckoo_refinable_list_basehook_sort_less();
        void Cuckoo_refinable_vector_basehook_sort_less();
        void Cuckoo_refinable_list_basehook_sort_cmpmix();
        void Cuckoo_refinable_vector_basehook_sort_cmpmix();
        void Cuckoo_refinable_vector_basehook_sort_cmpmix_stat();

        void Cuckoo_refinable_list_basehook_equal_storehash();
        void Cuckoo_refinable_vector_basehook_equal_storehash();
        void Cuckoo_refinable_list_basehook_sort_cmp_storehash();
        void Cuckoo_refinable_vector_basehook_sort_cmp_storehash();
        void Cuckoo_refinable_list_basehook_sort_less_storehash();
        void Cuckoo_refinable_vector_basehook_sort_less_storehash();
        void Cuckoo_refinable_list_basehook_sort_cmpmix_storehash();
        void Cuckoo_refinable_vector_basehook_sort_cmpmix_storehash();

        void Cuckoo_refinable_list_memberhook_equal();
        void Cuckoo_refinable_vector_memberhook_equal();
        void Cuckoo_refinable_list_memberhook_sort_cmp();
        void Cuckoo_refinable_vector_memberhook_sort_cmp();
        void Cuckoo_refinable_list_memberhook_sort_less();
        void Cuckoo_refinable_vector_memberhook_sort_less();
        void Cuckoo_refinable_list_memberhook_sort_cmpmix();
        void Cuckoo_refinable_vector_memberhook_sort_cmpmix();

        void Cuckoo_refinable_list_memberhook_equal_storehash();
        void Cuckoo_refinable_vector_memberhook_equal_storehash();
        void Cuckoo_refinable_list_memberhook_sort_cmp_storehash();
        void Cuckoo_refinable_vector_memberhook_sort_cmp_storehash();
        void Cuckoo_refinable_list_memberhook_sort_less_storehash();
        void Cuckoo_refinable_vector_memberhook_sort_less_storehash();
        void Cuckoo_refinable_list_memberhook_sort_cmpmix_storehash();
        void Cuckoo_refinable_vector_memberhook_sort_cmpmix_storehash();

        CPPUNIT_TEST_SUITE(IntrusiveCuckooSetHdrTest)
            // ***********************************************************
            // Cuckoo hashing (striped)

            CPPUNIT_TEST( Cuckoo_striped_list_basehook_equal)
            CPPUNIT_TEST( Cuckoo_striped_vector_basehook_equal)
            CPPUNIT_TEST( Cuckoo_striped_list_basehook_sort_cmp)
            CPPUNIT_TEST( Cuckoo_striped_vector_basehook_sort_cmp)
            CPPUNIT_TEST( Cuckoo_striped_list_basehook_sort_less)
            CPPUNIT_TEST( Cuckoo_striped_vector_basehook_sort_less)
            CPPUNIT_TEST( Cuckoo_striped_list_basehook_sort_cmpmix)
            CPPUNIT_TEST( Cuckoo_striped_vector_basehook_sort_cmpmix)
            CPPUNIT_TEST( Cuckoo_striped_vector_basehook_sort_cmpmix_stat)

            CPPUNIT_TEST( Cuckoo_striped_list_basehook_equal_storehash)
            CPPUNIT_TEST( Cuckoo_striped_vector_basehook_equal_storehash)
            CPPUNIT_TEST( Cuckoo_striped_list_basehook_sort_cmp_storehash)
            CPPUNIT_TEST( Cuckoo_striped_vector_basehook_sort_cmp_storehash)
            CPPUNIT_TEST( Cuckoo_striped_list_basehook_sort_less_storehash)
            CPPUNIT_TEST( Cuckoo_striped_vector_basehook_sort_less_storehash)
            CPPUNIT_TEST( Cuckoo_striped_list_basehook_sort_cmpmix_storehash)
            CPPUNIT_TEST( Cuckoo_striped_vector_basehook_sort_cmpmix_storehash)

            CPPUNIT_TEST( Cuckoo_striped_list_memberhook_equal)
            CPPUNIT_TEST( Cuckoo_striped_vector_memberhook_equal)
            CPPUNIT_TEST( Cuckoo_striped_list_memberhook_sort_cmp)
            CPPUNIT_TEST( Cuckoo_striped_vector_memberhook_sort_cmp)
            CPPUNIT_TEST( Cuckoo_striped_list_memberhook_sort_less)
            CPPUNIT_TEST( Cuckoo_striped_vector_memberhook_sort_less)
            CPPUNIT_TEST( Cuckoo_striped_list_memberhook_sort_cmpmix)
            CPPUNIT_TEST( Cuckoo_striped_vector_memberhook_sort_cmpmix)

            CPPUNIT_TEST( Cuckoo_striped_list_memberhook_equal_storehash)
            CPPUNIT_TEST( Cuckoo_striped_vector_memberhook_equal_storehash)
            CPPUNIT_TEST( Cuckoo_striped_list_memberhook_sort_cmp_storehash)
            CPPUNIT_TEST( Cuckoo_striped_vector_memberhook_sort_cmp_storehash)
            CPPUNIT_TEST( Cuckoo_striped_list_memberhook_sort_less_storehash)
            CPPUNIT_TEST( Cuckoo_striped_vector_memberhook_sort_less_storehash)
            CPPUNIT_TEST( Cuckoo_striped_list_memberhook_sort_cmpmix_storehash)
            CPPUNIT_TEST( Cuckoo_striped_vector_memberhook_sort_cmpmix_storehash)

            // ***********************************************************
            // Cuckoo hashing (refinable)

            CPPUNIT_TEST( Cuckoo_refinable_list_basehook_equal)
            CPPUNIT_TEST( Cuckoo_refinable_vector_basehook_equal)
            CPPUNIT_TEST( Cuckoo_refinable_list_basehook_sort_cmp)
            CPPUNIT_TEST( Cuckoo_refinable_vector_basehook_sort_cmp)
            CPPUNIT_TEST( Cuckoo_refinable_list_basehook_sort_less)
            CPPUNIT_TEST( Cuckoo_refinable_vector_basehook_sort_less)
            CPPUNIT_TEST( Cuckoo_refinable_list_basehook_sort_cmpmix)
            CPPUNIT_TEST( Cuckoo_refinable_vector_basehook_sort_cmpmix)
            CPPUNIT_TEST( Cuckoo_refinable_vector_basehook_sort_cmpmix_stat)

            CPPUNIT_TEST( Cuckoo_refinable_list_basehook_equal_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_vector_basehook_equal_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_list_basehook_sort_cmp_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_vector_basehook_sort_cmp_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_list_basehook_sort_less_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_vector_basehook_sort_less_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_list_basehook_sort_cmpmix_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_vector_basehook_sort_cmpmix_storehash)

            CPPUNIT_TEST( Cuckoo_refinable_list_memberhook_equal)
            CPPUNIT_TEST( Cuckoo_refinable_vector_memberhook_equal)
            CPPUNIT_TEST( Cuckoo_refinable_list_memberhook_sort_cmp)
            CPPUNIT_TEST( Cuckoo_refinable_vector_memberhook_sort_cmp)
            CPPUNIT_TEST( Cuckoo_refinable_list_memberhook_sort_less)
            CPPUNIT_TEST( Cuckoo_refinable_vector_memberhook_sort_less)
            CPPUNIT_TEST( Cuckoo_refinable_list_memberhook_sort_cmpmix)
            CPPUNIT_TEST( Cuckoo_refinable_vector_memberhook_sort_cmpmix)

            CPPUNIT_TEST( Cuckoo_refinable_list_memberhook_equal_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_vector_memberhook_equal_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_list_memberhook_sort_cmp_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_vector_memberhook_sort_cmp_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_list_memberhook_sort_less_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_vector_memberhook_sort_less_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_list_memberhook_sort_cmpmix_storehash)
            CPPUNIT_TEST( Cuckoo_refinable_vector_memberhook_sort_cmpmix_storehash)

        CPPUNIT_TEST_SUITE_END()
    };
} // namespace set

//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRUSIVE_SKIPLIST_SET_H
#define CDSTEST_HDR_INTRUSIVE_SKIPLIST_SET_H

#include "set/hdr_intrusive_set.h"

namespace set {

    class IntrusiveSkipListSet: public IntrusiveHashSetHdrTest
    {
        typedef IntrusiveHashSetHdrTest base_class;

        static size_t const c_nArrSize = 1000;

    protected:
        struct other_key {
            int nKey;

            other_key()
            {}

            other_key( int key )
                : nKey(key)
            {}

            template <typename Q>
            other_key& operator=( Q const& src )
            {
                nKey = src.nKey;
                return *this;
            }
        };

        template <typename StoredType>
        struct other_key_less
        {
            bool operator ()( StoredType const& n, other_key k ) const
            {
                return n.nKey < k.nKey;
            }
            bool operator ()( other_key k, StoredType const& n ) const
            {
                return k.nKey < n.nKey;
            }
        };

        struct copy_other_key
        {
            template <typename Q>
            void operator()( other_key& dest, Q const& src ) const
            {
                dest.nKey = src.nKey;
            }
        };

    protected:
        template <class Set, typename PrintStat>
        void test_skiplist()
        {
            {
                Set s;
                base_class::test_int_with( s );
            }

            test_skiplist_<Set, PrintStat >();
        }

        template <class Set, typename PrintStat>
        void test_skiplist_()
        {
            Set s;
            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            typedef typename Set::value_type        value_type;
            typedef typename Set::iterator          set_iterator;
            typedef typename Set::const_iterator    const_set_iterator;
            typedef typename base_class::less<value_type>   less;

            value_type  v[c_nArrSize];
            int nCount = 0;
            int nPrevKey = 0;

            // Test iterator - ascending order
            for ( int i = 0; i < (int) (sizeof(v)/sizeof(v[0])); ++i ) {
                v[i].nKey = i;
                v[i].nVal = i * 2;

                CPPUNIT_ASSERT( s.insert( v[i] ));
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) ));
            //CPPUNIT_MSG( PrintStat()(s, "Iterator test, ascending insert order") );

            nCount = 0;
            nPrevKey = 0;
            for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                CPPUNIT_ASSERT( s.find( it->nKey ));
                it->nVal = (*it).nKey;
                ++nCount;
                if ( it != s.begin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) ));
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0]));

            nCount = 0;
            for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                ++nCount;
                if ( it != s.cbegin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) ));
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0]));

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nKey == v[i].nVal );
                CPPUNIT_ASSERT( s.find_with( v[i].nKey, less() ));
            }

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));
            Set::gc::force_dispose();

            for ( size_t i = 0; i < (int) sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nDisposeCount == 1 );
            }

            // Test iterator - descending order
            for ( int i = (int) sizeof(v)/sizeof(v[0]) - 1; i >= 0; --i ) {
                v[i].nKey = i;
                v[i].nVal = i * 2;

                CPPUNIT_ASSERT( s.insert( v[i] ));
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) ));

            //CPPUNIT_MSG( PrintStat()(s, "Iterator test, descending insert order") );

            nCount = 0;
            for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                it->nVal = (*it).nKey;
                ++nCount;
                if ( it != s.begin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) ));
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0]));

            nCount = 0;
            for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                ++nCount;
                if ( it != s.cbegin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) ));
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0]));

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nKey == v[i].nVal );
            }

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));
            Set::gc::force_dispose();

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nDisposeCount == 2 );
            }

            // Test iterator - random order
            fill_skiplist( s, v );
            //CPPUNIT_MSG( PrintStat()(s, "Iterator test, random insert order") );

            nCount = 0;
            for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                it->nVal = (*it).nKey;
                ++nCount;
                if ( it != s.begin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) ));
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0]));

            nCount = 0;
            for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                ++nCount;
                if ( it != s.cbegin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( check_size( s, sizeof(v)/sizeof(v[0]) ));
            CPPUNIT_ASSERT( nCount == sizeof(v)/sizeof(v[0]));

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nKey == v[i].nVal );
            }

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));
            Set::gc::force_dispose();

            for ( size_t i = 0; i < sizeof(v)/sizeof(v[0]); ++i ) {
                CPPUNIT_ASSERT( v[i].nDisposeCount == 3 );
            }

            CPPUNIT_MSG( "extract test" );
            // extract/get test
            {
                typename Set::guarded_ptr gp;

                // extract
                fill_skiplist( s, v );
                for ( int i = c_nArrSize - 1; i >= 0; i -= 1 ) {
                    gp = s.get( i );
                    CPPUNIT_CHECK( gp );
                    CPPUNIT_CHECK( gp->nKey == i );
                    CPPUNIT_CHECK( gp->nVal == i * 2 );
                    gp->nVal *= 2;
                    gp.release();

                    gp = s.extract( i );
                    CPPUNIT_CHECK( gp );
                    CPPUNIT_CHECK_EX( gp->nKey == i, "i=" << i << ", gp->nKey=" << gp->nKey);
                    CPPUNIT_CHECK_EX( (*gp).nVal == i * 4, "i=" << i << ", gp->nVal=" << gp->nVal );
                    gp.release();

                    gp = s.extract( i );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_CHECK( !s.get( i ));
                }
                CPPUNIT_CHECK( s.empty() );
                Set::gc::force_dispose();

                // extract_with
                fill_skiplist( s, v );
                for ( int i = c_nArrSize - 1; i >= 0; i -= 1 ) {
                    gp = s.get_with( other_key( i ), other_key_less<typename Set::value_type>() );
                    CPPUNIT_CHECK( gp );
                    CPPUNIT_CHECK( gp->nKey == i );
                    CPPUNIT_CHECK( (*gp).nVal == i * 2 );
                    gp->nVal *= 2;
                    gp.release();

                    gp = s.extract_with( other_key( i ), other_key_less<typename Set::value_type>() );
                    CPPUNIT_CHECK( gp );
                    CPPUNIT_CHECK_EX( gp->nKey == i, "i=" << i << ", gp->nKey=" << gp->nKey);
                    CPPUNIT_CHECK_EX( (*gp).nVal == i * 4, "i=" << i << ", gp->nVal=" << gp->nVal );
                    gp.release();

                    gp = s.extract_with( other_key( i ), other_key_less<typename Set::value_type>() );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_CHECK( !s.get_with( other_key(i), other_key_less<typename Set::value_type>() ));
                }
                CPPUNIT_CHECK( s.empty() );
                Set::gc::force_dispose();

                // extract_min
                {
                    fill_skiplist( s, v );
                    int nPrevKey;
                    gp = s.extract_min();
                    CPPUNIT_ASSERT( gp );
                    nPrevKey = gp->nKey;
                    while ( !s.empty() ) {
                        gp = s.extract_min();
                        CPPUNIT_CHECK( gp );
                        CPPUNIT_ASSERT( !gp.empty());
                        CPPUNIT_CHECK( gp->nKey == nPrevKey + 1 );
                        CPPUNIT_CHECK( (*gp).nVal == (nPrevKey + 1) * 2 );
                        nPrevKey = gp->nKey;
                        gp.release();
                    }
                    gp.release();
                    CPPUNIT_CHECK( !s.extract_min());
                    CPPUNIT_CHECK( gp.empty());
                }
                Set::gc::force_dispose();

                // extract_max
                {
                    fill_skiplist( s, v );
                    int nPrevKey;
                    gp = s.extract_max();
                    CPPUNIT_ASSERT( gp );
                    nPrevKey = gp->nKey;
                    while ( !s.empty() ) {
                        gp = s.extract_max();
                        CPPUNIT_CHECK( gp );
                        CPPUNIT_ASSERT( !gp.empty() );
                        CPPUNIT_CHECK( gp->nKey == nPrevKey - 1 );
                        CPPUNIT_CHECK( (*gp).nVal == (nPrevKey - 1) * 2 );
                        nPrevKey = gp->nKey;
                        gp.release();
                    }
                    gp.release();
                    CPPUNIT_CHECK( !s.extract_min());
                    CPPUNIT_CHECK( gp.empty());

                    CPPUNIT_CHECK( !s.extract_max());
                }
                Set::gc::force_dispose();
            }

            CPPUNIT_MSG( PrintStat()(s, nullptr) );
        }

        template <typename Set>
        void fill_skiplist( Set& s, typename Set::value_type * pArr )
        {
            int nRand[c_nArrSize];
            for ( int i = 0; i < (int) c_nArrSize; ++i ) {
                nRand[i] = i;
            }
            shuffle( nRand, nRand + c_nArrSize );

            for ( int i = 0; i < (int) c_nArrSize; ++i ) {
                pArr[i].nKey = nRand[i];
                pArr[i].nVal = nRand[i] * 2;
                CPPUNIT_ASSERT( s.insert( pArr[i] ));
            }
            CPPUNIT_CHECK( check_size( s, c_nArrSize ));
        }

        template <class Set, typename PrintStat>
        void test_skiplist_nogc()
        {
            typedef typename Set::value_type    value_type;
            typedef typename Set::iterator set_iterator;
            typedef typename Set::iterator const_set_iterator;
            typedef typename base_class::less<value_type>   less;

            value_type v1( 10, 50 );
            value_type v2( 5, 25  );
            value_type v3( 20, 100 );
            int key;

            Set s;

            // insert test
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            // insert/find test
            CPPUNIT_ASSERT( s.find( v1.key() ) == nullptr );
            CPPUNIT_ASSERT( s.insert( v1 ));
            CPPUNIT_ASSERT( s.find( v1.key() ) == &v1 );
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( !s.empty() );

            CPPUNIT_ASSERT( s.find_with( v2.key(), less() ) == nullptr );
            CPPUNIT_ASSERT( s.insert( v2 ));
            CPPUNIT_ASSERT( v2.nFindCount == 0 );
            CPPUNIT_ASSERT( s.find_with( key = v2.key(), less(), find_functor() ));
            CPPUNIT_ASSERT( v2.nFindCount == 1 );
            v2.nFindCount = 0;
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.empty() );

            {
                find_functor    ff;
                CPPUNIT_ASSERT( s.find( v3 ) == nullptr );
                CPPUNIT_ASSERT( s.insert( v3 ));
                CPPUNIT_ASSERT( v3.nFindCount == 0 );
                CPPUNIT_ASSERT( s.find( v3, std::ref(ff) ));
                CPPUNIT_ASSERT( v3.nFindCount == 1 );
                v3.nFindCount = 0;
                CPPUNIT_ASSERT( check_size( s, 3 ));
                CPPUNIT_ASSERT( !s.empty() );
            }

            CPPUNIT_ASSERT( !s.empty() );
            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));
            //CPPUNIT_MSG( PrintStat()(s, "Insert test") );

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
            CPPUNIT_ASSERT( s.find_with( v2, base_class::less<value_type>() ) == &v2 );
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

            CPPUNIT_ASSERT( !s.empty() );
            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            //CPPUNIT_MSG( PrintStat()(s, "Ensure test") );

            // get_min test
            CPPUNIT_CHECK( s.get_min() == nullptr );
            CPPUNIT_CHECK( s.get_max() == nullptr );

            {
                value_type  v[1000];
                for ( int i = 999; i >= 0; --i ) {
                    v[i].nKey = i;
                    v[i].nVal = i * 2;

                    CPPUNIT_ASSERT( s.insert( v[i] ));
                    value_type * pVal = s.get_min();
                    CPPUNIT_ASSERT( pVal != nullptr );
                    CPPUNIT_CHECK( pVal->nKey == i );
                    CPPUNIT_CHECK( pVal->nVal == i * 2 );
                }

                CPPUNIT_ASSERT( !s.empty() );
                s.clear();
                CPPUNIT_ASSERT( s.empty() );
                CPPUNIT_ASSERT( check_size( s, 0 ));
            }

            // Iterator test
            {
                value_type  v[500];

                for ( int i = 0; unsigned(i) < sizeof(v)/sizeof(v[0]); ++i ) {
                    v[i].nKey = i;
                    v[i].nVal = i * 2;

                    CPPUNIT_ASSERT( s.insert( v[i] ));

                    value_type * pVal = s.get_max();
                    CPPUNIT_ASSERT( pVal != nullptr );
                    CPPUNIT_CHECK( pVal->nKey == i );
                    CPPUNIT_CHECK( pVal->nVal == i * 2 );
                }

                int nCount = 0;
                for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
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

                //CPPUNIT_MSG( PrintStat()(s, "Iterator test") );
                s.clear();
            }

            // Test empty set
            CPPUNIT_ASSERT( s.begin() == s.end() );
            CPPUNIT_ASSERT( s.cbegin() == s.cend() );

            CPPUNIT_MSG( PrintStat()(s, nullptr) );
        }

    public:
        // Skip-list - gc::HP
        void skiplist_hp_base_cmp();
        void skiplist_hp_base_less();
        void skiplist_hp_base_cmpmix();
        void skiplist_hp_base_cmp_stat();
        void skiplist_hp_base_less_stat();
        void skiplist_hp_base_cmpmix_stat();
        void skiplist_hp_base_cmp_xorshift();
        void skiplist_hp_base_less_xorshift();
        void skiplist_hp_base_cmpmix_xorshift();
        void skiplist_hp_base_cmp_xorshift_stat();
        void skiplist_hp_base_less_xorshift_stat();
        void skiplist_hp_base_cmpmix_xorshift_stat();
        void skiplist_hp_base_cmp_pascal();
        void skiplist_hp_base_less_pascal();
        void skiplist_hp_base_cmpmix_pascal();
        void skiplist_hp_base_cmp_pascal_stat();
        void skiplist_hp_base_less_pascal_stat();
        void skiplist_hp_base_cmpmix_pascal_stat();

        void skiplist_hp_member_cmp();
        void skiplist_hp_member_less();
        void skiplist_hp_member_cmpmix();
        void skiplist_hp_member_cmp_stat();
        void skiplist_hp_member_less_stat();
        void skiplist_hp_member_cmpmix_stat();
        void skiplist_hp_member_cmp_xorshift();
        void skiplist_hp_member_less_xorshift();
        void skiplist_hp_member_cmpmix_xorshift();
        void skiplist_hp_member_cmp_xorshift_stat();
        void skiplist_hp_member_less_xorshift_stat();
        void skiplist_hp_member_cmpmix_xorshift_stat();
        void skiplist_hp_member_cmp_pascal();
        void skiplist_hp_member_less_pascal();
        void skiplist_hp_member_cmpmix_pascal();
        void skiplist_hp_member_cmp_pascal_stat();
        void skiplist_hp_member_less_pascal_stat();
        void skiplist_hp_member_cmpmix_pascal_stat();

        // Skip-list - gc::DHP
        void skiplist_dhp_base_cmp();
        void skiplist_dhp_base_less();
        void skiplist_dhp_base_cmpmix();
        void skiplist_dhp_base_cmp_stat();
        void skiplist_dhp_base_less_stat();
        void skiplist_dhp_base_cmpmix_stat();
        void skiplist_dhp_base_cmp_xorshift();
        void skiplist_dhp_base_less_xorshift();
        void skiplist_dhp_base_cmpmix_xorshift();
        void skiplist_dhp_base_cmp_xorshift_stat();
        void skiplist_dhp_base_less_xorshift_stat();
        void skiplist_dhp_base_cmpmix_xorshift_stat();
        void skiplist_dhp_base_cmp_pascal();
        void skiplist_dhp_base_less_pascal();
        void skiplist_dhp_base_cmpmix_pascal();
        void skiplist_dhp_base_cmp_pascal_stat();
        void skiplist_dhp_base_less_pascal_stat();
        void skiplist_dhp_base_cmpmix_pascal_stat();

        void skiplist_dhp_member_cmp();
        void skiplist_dhp_member_less();
        void skiplist_dhp_member_cmpmix();
        void skiplist_dhp_member_cmp_stat();
        void skiplist_dhp_member_less_stat();
        void skiplist_dhp_member_cmpmix_stat();
        void skiplist_dhp_member_cmp_xorshift();
        void skiplist_dhp_member_less_xorshift();
        void skiplist_dhp_member_cmpmix_xorshift();
        void skiplist_dhp_member_cmp_xorshift_stat();
        void skiplist_dhp_member_less_xorshift_stat();
        void skiplist_dhp_member_cmpmix_xorshift_stat();
        void skiplist_dhp_member_cmp_pascal();
        void skiplist_dhp_member_less_pascal();
        void skiplist_dhp_member_cmpmix_pascal();
        void skiplist_dhp_member_cmp_pascal_stat();
        void skiplist_dhp_member_less_pascal_stat();
        void skiplist_dhp_member_cmpmix_pascal_stat();

        // Skip-list - gc::nogc
        void skiplist_nogc_base_cmp();
        void skiplist_nogc_base_less();
        void skiplist_nogc_base_cmpmix();
        void skiplist_nogc_base_cmp_stat();
        void skiplist_nogc_base_less_stat();
        void skiplist_nogc_base_cmpmix_stat();
        void skiplist_nogc_base_cmp_xorshift();
        void skiplist_nogc_base_less_xorshift();
        void skiplist_nogc_base_cmpmix_xorshift();
        void skiplist_nogc_base_cmp_xorshift_stat();
        void skiplist_nogc_base_less_xorshift_stat();
        void skiplist_nogc_base_cmpmix_xorshift_stat();
        void skiplist_nogc_base_cmp_pascal();
        void skiplist_nogc_base_less_pascal();
        void skiplist_nogc_base_cmpmix_pascal();
        void skiplist_nogc_base_cmp_pascal_stat();
        void skiplist_nogc_base_less_pascal_stat();
        void skiplist_nogc_base_cmpmix_pascal_stat();

        void skiplist_nogc_member_cmp();
        void skiplist_nogc_member_less();
        void skiplist_nogc_member_cmpmix();
        void skiplist_nogc_member_cmp_stat();
        void skiplist_nogc_member_less_stat();
        void skiplist_nogc_member_cmpmix_stat();
        void skiplist_nogc_member_cmp_xorshift();
        void skiplist_nogc_member_less_xorshift();
        void skiplist_nogc_member_cmpmix_xorshift();
        void skiplist_nogc_member_cmp_xorshift_stat();
        void skiplist_nogc_member_less_xorshift_stat();
        void skiplist_nogc_member_cmpmix_xorshift_stat();
        void skiplist_nogc_member_cmp_pascal();
        void skiplist_nogc_member_less_pascal();
        void skiplist_nogc_member_cmpmix_pascal();
        void skiplist_nogc_member_cmp_pascal_stat();
        void skiplist_nogc_member_less_pascal_stat();
        void skiplist_nogc_member_cmpmix_pascal_stat();

        CPPUNIT_TEST_SUITE(IntrusiveSkipListSet)
            CPPUNIT_TEST(skiplist_hp_base_cmp)
            CPPUNIT_TEST(skiplist_hp_base_less)
            CPPUNIT_TEST(skiplist_hp_base_cmpmix)
            CPPUNIT_TEST(skiplist_hp_base_cmp_stat)
            CPPUNIT_TEST(skiplist_hp_base_less_stat)
            CPPUNIT_TEST(skiplist_hp_base_cmpmix_stat)
            CPPUNIT_TEST(skiplist_hp_base_cmp_xorshift)
            CPPUNIT_TEST(skiplist_hp_base_less_xorshift)
            CPPUNIT_TEST(skiplist_hp_base_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_hp_base_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_hp_base_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_hp_base_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_hp_base_cmp_pascal)
            CPPUNIT_TEST(skiplist_hp_base_less_pascal)
            CPPUNIT_TEST(skiplist_hp_base_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_hp_base_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_hp_base_less_pascal_stat)
            CPPUNIT_TEST(skiplist_hp_base_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_hp_member_cmp)
            CPPUNIT_TEST(skiplist_hp_member_less)
            CPPUNIT_TEST(skiplist_hp_member_cmpmix)
            CPPUNIT_TEST(skiplist_hp_member_cmp_stat)
            CPPUNIT_TEST(skiplist_hp_member_less_stat)
            CPPUNIT_TEST(skiplist_hp_member_cmpmix_stat)
            CPPUNIT_TEST(skiplist_hp_member_cmp_xorshift)
            CPPUNIT_TEST(skiplist_hp_member_less_xorshift)
            CPPUNIT_TEST(skiplist_hp_member_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_hp_member_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_hp_member_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_hp_member_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_hp_member_cmp_pascal)
            CPPUNIT_TEST(skiplist_hp_member_less_pascal)
            CPPUNIT_TEST(skiplist_hp_member_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_hp_member_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_hp_member_less_pascal_stat)
            CPPUNIT_TEST(skiplist_hp_member_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_dhp_base_cmp)
            CPPUNIT_TEST(skiplist_dhp_base_less)
            CPPUNIT_TEST(skiplist_dhp_base_cmpmix)
            CPPUNIT_TEST(skiplist_dhp_base_cmp_stat)
            CPPUNIT_TEST(skiplist_dhp_base_less_stat)
            CPPUNIT_TEST(skiplist_dhp_base_cmpmix_stat)
            CPPUNIT_TEST(skiplist_dhp_base_cmp_xorshift)
            CPPUNIT_TEST(skiplist_dhp_base_less_xorshift)
            CPPUNIT_TEST(skiplist_dhp_base_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_dhp_base_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_dhp_base_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_dhp_base_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_dhp_base_cmp_pascal)
            CPPUNIT_TEST(skiplist_dhp_base_less_pascal)
            CPPUNIT_TEST(skiplist_dhp_base_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_dhp_base_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_dhp_base_less_pascal_stat)
            CPPUNIT_TEST(skiplist_dhp_base_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_dhp_member_cmp)
            CPPUNIT_TEST(skiplist_dhp_member_less)
            CPPUNIT_TEST(skiplist_dhp_member_cmpmix)
            CPPUNIT_TEST(skiplist_dhp_member_cmp_stat)
            CPPUNIT_TEST(skiplist_dhp_member_less_stat)
            CPPUNIT_TEST(skiplist_dhp_member_cmpmix_stat)
            CPPUNIT_TEST(skiplist_dhp_member_cmp_xorshift)
            CPPUNIT_TEST(skiplist_dhp_member_less_xorshift)
            CPPUNIT_TEST(skiplist_dhp_member_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_dhp_member_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_dhp_member_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_dhp_member_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_dhp_member_cmp_pascal)
            CPPUNIT_TEST(skiplist_dhp_member_less_pascal)
            CPPUNIT_TEST(skiplist_dhp_member_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_dhp_member_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_dhp_member_less_pascal_stat)
            CPPUNIT_TEST(skiplist_dhp_member_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_nogc_base_cmp)
            CPPUNIT_TEST(skiplist_nogc_base_less)
            CPPUNIT_TEST(skiplist_nogc_base_cmpmix)
            CPPUNIT_TEST(skiplist_nogc_base_cmp_stat)
            CPPUNIT_TEST(skiplist_nogc_base_less_stat)
            CPPUNIT_TEST(skiplist_nogc_base_cmpmix_stat)
            CPPUNIT_TEST(skiplist_nogc_base_cmp_xorshift)
            CPPUNIT_TEST(skiplist_nogc_base_less_xorshift)
            CPPUNIT_TEST(skiplist_nogc_base_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_nogc_base_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_nogc_base_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_nogc_base_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_nogc_base_cmp_pascal)
            CPPUNIT_TEST(skiplist_nogc_base_less_pascal)
            CPPUNIT_TEST(skiplist_nogc_base_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_nogc_base_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_nogc_base_less_pascal_stat)
            CPPUNIT_TEST(skiplist_nogc_base_cmpmix_pascal_stat)

            CPPUNIT_TEST(skiplist_nogc_member_cmp)
            CPPUNIT_TEST(skiplist_nogc_member_less)
            CPPUNIT_TEST(skiplist_nogc_member_cmpmix)
            CPPUNIT_TEST(skiplist_nogc_member_cmp_stat)
            CPPUNIT_TEST(skiplist_nogc_member_less_stat)
            CPPUNIT_TEST(skiplist_nogc_member_cmpmix_stat)
            CPPUNIT_TEST(skiplist_nogc_member_cmp_xorshift)
            CPPUNIT_TEST(skiplist_nogc_member_less_xorshift)
            CPPUNIT_TEST(skiplist_nogc_member_cmpmix_xorshift)
            CPPUNIT_TEST(skiplist_nogc_member_cmp_xorshift_stat)
            CPPUNIT_TEST(skiplist_nogc_member_less_xorshift_stat)
            CPPUNIT_TEST(skiplist_nogc_member_cmpmix_xorshift_stat)
            CPPUNIT_TEST(skiplist_nogc_member_cmp_pascal)
            CPPUNIT_TEST(skiplist_nogc_member_less_pascal)
            CPPUNIT_TEST(skiplist_nogc_member_cmpmix_pascal)
            CPPUNIT_TEST(skiplist_nogc_member_cmp_pascal_stat)
            CPPUNIT_TEST(skiplist_nogc_member_less_pascal_stat)
            CPPUNIT_TEST(skiplist_nogc_member_cmpmix_pascal_stat)

        CPPUNIT_TEST_SUITE_END()
    };
} // namespace set

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_SKIPLIST_SET_H

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

#ifndef CDSTEST_HDR_SKIPLIST_SET_H
#define CDSTEST_HDR_SKIPLIST_SET_H

#include "set/hdr_set.h"

namespace set {

    class SkipListSetHdrTest: public HashSetHdrTest
    {
        typedef HashSetHdrTest base_class;

        typedef base_class::other_item  wrapped_item;
        typedef base_class::other_less  wrapped_less;

        template <class Set, typename PrintStat >
        void test()
        {
            Set s;
            base_class::test_int_with( s );

            static int const nLimit = 10000;
            typedef typename Set::iterator          set_iterator;
            typedef typename Set::const_iterator    const_set_iterator;
            typedef typename Set::guarded_ptr       guarded_ptr;

            int nCount = 0;
            int nPrevKey = 0;
            guarded_ptr gp;
            int arrRandom[nLimit];
            for ( int i = 0; i < nLimit; ++i )
                arrRandom[i] = i;
            shuffle( arrRandom, arrRandom + nLimit );


            // Test iterator - ascending order
            s.clear();
            CPPUNIT_ASSERT( s.empty() );

            for ( int i = 0; i < nLimit; ++i ) {
                gp = s.get( i );
                CPPUNIT_CHECK( !gp );
                CPPUNIT_CHECK( gp.empty() );

                CPPUNIT_ASSERT( s.insert(i) );

                gp = s.get( i );
                CPPUNIT_CHECK( gp );
                CPPUNIT_ASSERT( !gp.empty());
                CPPUNIT_CHECK( gp->nKey == i );
                CPPUNIT_CHECK( gp->nVal == i );
                gp.release();
            }
            CPPUNIT_MSG( PrintStat()(s, "Iterator test, ascending insert order") );

            nCount = 0;
            nPrevKey = 0;
            for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                CPPUNIT_ASSERT( s.contains( it->nKey ));
                it->nVal = (*it).nKey * 2;
                ++nCount;
                if ( it != s.begin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;

                // get
                gp = s.get( it->nKey );
                CPPUNIT_CHECK( gp );
                CPPUNIT_ASSERT( !gp.empty() );
                CPPUNIT_CHECK( gp->nKey == it->nKey );
                CPPUNIT_CHECK( gp->nVal == it->nKey * 2 );
                gp.release();
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                ++nCount;
                if ( it != s.cbegin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            // Test iterator - descending order
            s.clear();
            CPPUNIT_ASSERT( s.empty() );

            for ( int i = nLimit; i > 0; --i ) {
                gp = s.get_with( i - 1, base_class::less<typename Set::value_type>());
                CPPUNIT_CHECK( !gp );
                CPPUNIT_CHECK( gp.empty() );

                CPPUNIT_ASSERT( s.insert( std::make_pair(i - 1, (i-1) * 2) ));

                // get_with
                gp = s.get_with( i - 1, base_class::less<typename Set::value_type>());
                CPPUNIT_CHECK( gp );
                CPPUNIT_ASSERT( !gp.empty());
                CPPUNIT_CHECK( gp->nKey == i-1 );
                CPPUNIT_CHECK( gp->nVal == (i-1) * 2 );
                gp.release();
            }
            CPPUNIT_MSG( PrintStat()(s, "Iterator test, descending insert order") );

            nCount = 0;
            nPrevKey = 0;
            for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                CPPUNIT_ASSERT( s.contains( it->nKey ));
                it->nVal = (*it).nKey;
                ++nCount;
                if ( it != s.begin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                ++nCount;
                if ( it != s.cbegin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            // Test iterator - random order
            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            {
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrRandom[i]) );
                CPPUNIT_MSG( PrintStat()(s, "Iterator test, random insert order") );
            }

            nCount = 0;
            nPrevKey = 0;
            for ( set_iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                CPPUNIT_ASSERT( s.contains( it->nKey ));
                it->nVal = (*it).nKey * 2;
                ++nCount;
                if ( it != s.begin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                ++nCount;
                if ( it != s.cbegin() ) {
                    CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                }
                nPrevKey = it->nKey;
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            // extract test
            {
                typedef typename base_class::less<typename Set::value_type> less_predicate;
                typename Set::guarded_ptr gp;

                // extract/get
                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrRandom[i];
                    gp = s.get( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey * 2);
                    gp.release();

                    gp = s.extract( nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey * 2);
                    gp.release();

                    gp = s.get( nKey );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_ASSERT( gp.empty());
                    CPPUNIT_ASSERT( !s.extract(nKey));
                }
                CPPUNIT_ASSERT( s.empty() );

                // extract_with/get_with
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrRandom[i]) );

                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrRandom[i];
                    gp = s.get_with( wrapped_item( nKey ), wrapped_less());
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey );
                    gp.release();

                    gp = s.extract_with( wrapped_item( nKey ), wrapped_less());
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == nKey );
                    CPPUNIT_CHECK( gp->nVal == nKey );
                    gp.release();

                    gp = s.get_with( wrapped_item( nKey ), wrapped_less());
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_ASSERT( !s.extract_with( wrapped_item(nKey), wrapped_less() ));
                }
                CPPUNIT_ASSERT( s.empty() );

                // extract_min
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrRandom[i]) );

                for ( int i = 0; i < nLimit; ++i ) {
                    gp = s.extract_min();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == i );
                    CPPUNIT_CHECK( gp->nVal == i );
                    gp.release();

                    gp = s.get( i );
                    CPPUNIT_CHECK( !gp );
                }
                CPPUNIT_ASSERT( s.empty() );
                CPPUNIT_CHECK( !s.extract_min());
                CPPUNIT_CHECK( !s.extract_max());

                // extract_max
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrRandom[i]) );

                for ( int i = nLimit-1; i >= 0; --i ) {
                    gp = s.extract_max();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == i );
                    CPPUNIT_CHECK( gp->nVal == i );
                    gp.release();

                    gp = s.get( i );
                    CPPUNIT_CHECK( !gp );
                }
                CPPUNIT_ASSERT( s.empty() );
                CPPUNIT_CHECK( !s.extract_min());
                CPPUNIT_CHECK( !s.extract_max());
            }

            CPPUNIT_MSG( PrintStat()(s, nullptr) );
        }

        template <class Set, typename PrintStat >
        void test_nogc()
        {
            typedef typename Set::value_type        value_type;
            typedef typename Set::iterator          iterator;
            typedef typename Set::const_iterator    const_iterator;

            Set s;
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

            // update
            std::pair< iterator, bool>  updateResult;
            updateResult = s.update(20, false);
            CPPUNIT_ASSERT(updateResult.first == s.end());
            CPPUNIT_ASSERT(!updateResult.second);
            CPPUNIT_ASSERT(check_size(s, 2));

            updateResult = s.update( 20 );
            CPPUNIT_ASSERT( updateResult.first != s.end() );
            CPPUNIT_ASSERT( updateResult.second  );
            CPPUNIT_ASSERT( updateResult.first->key() == 20 );
            CPPUNIT_ASSERT( updateResult.first->val() == 20 );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));

            updateResult = s.update( std::make_pair( 20, 200 ));
            CPPUNIT_ASSERT( updateResult.first != s.end() );
            CPPUNIT_ASSERT( !updateResult.second  );
            CPPUNIT_ASSERT( updateResult.first->key() == 20 );
            CPPUNIT_ASSERT( updateResult.first->val() == 20 );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            updateResult.first->nVal = 22;

            updateResult = s.update( std::make_pair( 30, 33 ));
            CPPUNIT_ASSERT( updateResult.first != s.end() );
            CPPUNIT_ASSERT( updateResult.second  );
            CPPUNIT_ASSERT( updateResult.first->key() == 30 );
            CPPUNIT_ASSERT( updateResult.first->val() == 33 );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 4 ));

            // find
            it = s.contains( 10 );
            CPPUNIT_ASSERT( it != s.end() );
            CPPUNIT_ASSERT( it->key() == 10 );
            CPPUNIT_ASSERT( it->val() == 10 );

            it = s.contains( 20 );
            CPPUNIT_ASSERT( it != s.end() );
            CPPUNIT_ASSERT( it->key() == 20 );
            CPPUNIT_ASSERT( it->val() == 22 );

            it = s.contains( 30, base_class::less<value_type>() );
            CPPUNIT_ASSERT( it != s.end() );
            CPPUNIT_ASSERT( it->key() == 30 );
            CPPUNIT_ASSERT( it->val() == 33 );

            it = s.contains( 40 );
            CPPUNIT_ASSERT( it == s.end() );

            it = s.contains( 50 );
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

            it = s.contains( 174 );
            CPPUNIT_ASSERT( it != s.end() );
            CPPUNIT_ASSERT( it->key() == 174 );
            CPPUNIT_ASSERT( it->val() == 471 );

            it = s.contains( 190 );
            CPPUNIT_ASSERT( it != s.end() );
            CPPUNIT_ASSERT( it->key() == 190 );
            CPPUNIT_ASSERT( it->val() == 91 );

            it = s.contains( 151 );
            CPPUNIT_ASSERT( it != s.end() );
            CPPUNIT_ASSERT( it->key() == 151 );
            CPPUNIT_ASSERT( it->val() == 151 );

            CPPUNIT_ASSERT( !s.empty() );
            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            // get_min test
            for ( int i = 500; i > 0; --i ) {
                CPPUNIT_ASSERT( s.insert( std::make_pair( i, i * 2) ) != s.end() );

                typename Set::value_type * pVal = s.get_min();
                CPPUNIT_ASSERT( pVal != nullptr );
                CPPUNIT_CHECK( pVal->nKey == i );
                CPPUNIT_CHECK( pVal->nVal ==  i * 2 );
            }
            CPPUNIT_ASSERT( !s.empty() );
            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            CPPUNIT_CHECK( s.get_min() == nullptr );
            CPPUNIT_CHECK( s.get_max() == nullptr );

            // iterator test
            for ( int i = 0; i < 500; ++i ) {
                CPPUNIT_ASSERT( s.insert( std::make_pair( i, i * 2) ) != s.end() );

                typename Set::value_type * pVal = s.get_max();
                CPPUNIT_ASSERT( pVal != nullptr );
                CPPUNIT_CHECK( pVal->nKey == i );
                CPPUNIT_CHECK( pVal->nVal == i * 2 );
            }
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 500 ));

            for ( iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                it->nVal = (*it).nKey;
            }

            Set const& refSet = s;
            for ( const_iterator it = refSet.begin(), itEnd = refSet.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( (*it).nKey == it->nVal );
            }
        }

    public:
        void SkipList_HP_less();
        void SkipList_HP_cmp();
        void SkipList_HP_cmpless();
        void SkipList_HP_less_stat();
        void SkipList_HP_cmp_stat();
        void SkipList_HP_cmpless_stat();
        void SkipList_HP_xorshift_less();
        void SkipList_HP_xorshift_cmp();
        void SkipList_HP_xorshift_cmpless();
        void SkipList_HP_xorshift_less_stat();
        void SkipList_HP_xorshift_cmp_stat();
        void SkipList_HP_xorshift_cmpless_stat();
        void SkipList_HP_turbopas_less();
        void SkipList_HP_turbopas_cmp();
        void SkipList_HP_turbopas_cmpless();
        void SkipList_HP_turbopas_less_stat();
        void SkipList_HP_turbopas_cmp_stat();
        void SkipList_HP_turbopas_cmpless_stat();
        void SkipList_HP_michaelalloc_less();
        void SkipList_HP_michaelalloc_cmp();
        void SkipList_HP_michaelalloc_cmpless();
        void SkipList_HP_michaelalloc_less_stat();
        void SkipList_HP_michaelalloc_cmp_stat();
        void SkipList_HP_michaelalloc_cmpless_stat();

        void SkipList_DHP_less();
        void SkipList_DHP_cmp();
        void SkipList_DHP_cmpless();
        void SkipList_DHP_less_stat();
        void SkipList_DHP_cmp_stat();
        void SkipList_DHP_cmpless_stat();
        void SkipList_DHP_xorshift_less();
        void SkipList_DHP_xorshift_cmp();
        void SkipList_DHP_xorshift_cmpless();
        void SkipList_DHP_xorshift_less_stat();
        void SkipList_DHP_xorshift_cmp_stat();
        void SkipList_DHP_xorshift_cmpless_stat();
        void SkipList_DHP_turbopas_less();
        void SkipList_DHP_turbopas_cmp();
        void SkipList_DHP_turbopas_cmpless();
        void SkipList_DHP_turbopas_less_stat();
        void SkipList_DHP_turbopas_cmp_stat();
        void SkipList_DHP_turbopas_cmpless_stat();
        void SkipList_DHP_michaelalloc_less();
        void SkipList_DHP_michaelalloc_cmp();
        void SkipList_DHP_michaelalloc_cmpless();
        void SkipList_DHP_michaelalloc_less_stat();
        void SkipList_DHP_michaelalloc_cmp_stat();
        void SkipList_DHP_michaelalloc_cmpless_stat();

        void SkipList_NOGC_less();
        void SkipList_NOGC_cmp();
        void SkipList_NOGC_cmpless();
        void SkipList_NOGC_less_stat();
        void SkipList_NOGC_cmp_stat();
        void SkipList_NOGC_cmpless_stat();
        void SkipList_NOGC_xorshift_less();
        void SkipList_NOGC_xorshift_cmp();
        void SkipList_NOGC_xorshift_cmpless();
        void SkipList_NOGC_xorshift_less_stat();
        void SkipList_NOGC_xorshift_cmp_stat();
        void SkipList_NOGC_xorshift_cmpless_stat();
        void SkipList_NOGC_turbopas_less();
        void SkipList_NOGC_turbopas_cmp();
        void SkipList_NOGC_turbopas_cmpless();
        void SkipList_NOGC_turbopas_less_stat();
        void SkipList_NOGC_turbopas_cmp_stat();
        void SkipList_NOGC_turbopas_cmpless_stat();
        void SkipList_NOGC_michaelalloc_less();
        void SkipList_NOGC_michaelalloc_cmp();
        void SkipList_NOGC_michaelalloc_cmpless();
        void SkipList_NOGC_michaelalloc_less_stat();
        void SkipList_NOGC_michaelalloc_cmp_stat();
        void SkipList_NOGC_michaelalloc_cmpless_stat();

        CPPUNIT_TEST_SUITE(SkipListSetHdrTest)
            CPPUNIT_TEST(SkipList_HP_less)
            CPPUNIT_TEST(SkipList_HP_cmp)
            CPPUNIT_TEST(SkipList_HP_cmpless)
            CPPUNIT_TEST(SkipList_HP_less_stat)
            CPPUNIT_TEST(SkipList_HP_cmp_stat)
            CPPUNIT_TEST(SkipList_HP_cmpless_stat)
            CPPUNIT_TEST(SkipList_HP_xorshift_less)
            CPPUNIT_TEST(SkipList_HP_xorshift_cmp)
            CPPUNIT_TEST(SkipList_HP_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_HP_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_HP_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_HP_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_HP_turbopas_less)
            CPPUNIT_TEST(SkipList_HP_turbopas_cmp)
            CPPUNIT_TEST(SkipList_HP_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_HP_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_HP_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_HP_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_less)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_HP_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_DHP_less)
            CPPUNIT_TEST(SkipList_DHP_cmp)
            CPPUNIT_TEST(SkipList_DHP_cmpless)
            CPPUNIT_TEST(SkipList_DHP_less_stat)
            CPPUNIT_TEST(SkipList_DHP_cmp_stat)
            CPPUNIT_TEST(SkipList_DHP_cmpless_stat)
            CPPUNIT_TEST(SkipList_DHP_xorshift_less)
            CPPUNIT_TEST(SkipList_DHP_xorshift_cmp)
            CPPUNIT_TEST(SkipList_DHP_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_DHP_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_DHP_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_DHP_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_DHP_turbopas_less)
            CPPUNIT_TEST(SkipList_DHP_turbopas_cmp)
            CPPUNIT_TEST(SkipList_DHP_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_DHP_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_DHP_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_DHP_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_DHP_michaelalloc_less)
            CPPUNIT_TEST(SkipList_DHP_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_DHP_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_DHP_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_DHP_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_DHP_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_NOGC_less)
            CPPUNIT_TEST(SkipList_NOGC_cmp)
            CPPUNIT_TEST(SkipList_NOGC_cmpless)
            CPPUNIT_TEST(SkipList_NOGC_less_stat)
            CPPUNIT_TEST(SkipList_NOGC_cmp_stat)
            CPPUNIT_TEST(SkipList_NOGC_cmpless_stat)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_less)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_cmp)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_NOGC_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_less)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_cmp)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_NOGC_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_less)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_NOGC_michaelalloc_cmpless_stat)

        CPPUNIT_TEST_SUITE_END()

    };
}

#endif // #ifndef CDSTEST_HDR_SKIPLIST_SET_H

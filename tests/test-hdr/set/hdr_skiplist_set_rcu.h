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

#ifndef CDSTEST_HDR_SKIPLIST_SET_RCU_H
#define CDSTEST_HDR_SKIPLIST_SET_RCU_H

#include "set/hdr_set.h"

namespace set {

    class SkipListSetRCUHdrTest: public HashSetHdrTest
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
            typedef typename Set::gc::scoped_lock   rcu_lock;

            int nCount = 0;
            int nPrevKey = 0;

            int arrRandom[nLimit];
            for ( int i = 0; i < nLimit; ++i )
                arrRandom[i] = i;
            shuffle( arrRandom, arrRandom + nLimit );

            // Test iterator - ascending order
            s.clear();
            CPPUNIT_ASSERT( s.empty() );

            for ( int i = 0; i < nLimit; ++i ) {
                CPPUNIT_ASSERT( s.insert(i) );
            }
            CPPUNIT_MSG( PrintStat()(s, "Iterator test, ascending insert order") );

            nCount = 0;
            nPrevKey = 0;
            {
                rcu_lock l;
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
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            {
                rcu_lock l;
                for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                    ++nCount;
                    if ( it != s.cbegin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                    }
                    nPrevKey = it->nKey;
                }
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            // Test iterator - descending order
            s.clear();
            CPPUNIT_ASSERT( s.empty() );

            for ( int i = nLimit; i > 0; --i ) {
                CPPUNIT_ASSERT( s.insert( std::make_pair(i - 1, (i-1) * 2) ));
            }
            CPPUNIT_MSG( PrintStat()(s, "Iterator test, descending insert order") );

            nCount = 0;
            nPrevKey = 0;
            {
                rcu_lock l;
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
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            {
                rcu_lock l;
                for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey == it->nVal );
                    ++nCount;
                    if ( it != s.cbegin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                    }
                    nPrevKey = it->nKey;
                }
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            // Test iterator - random order
            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            {
                for ( int i = 0; i < nLimit; ++i ) {
                    CPPUNIT_ASSERT( s.insert( arrRandom[i] ) );
                }
                CPPUNIT_MSG( PrintStat()(s, "Iterator test, random insert order") );
            }

            nCount = 0;
            nPrevKey = 0;
            {
                rcu_lock l;
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
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            nCount = 0;
            {
                rcu_lock l;
                for ( const_set_iterator it = s.cbegin(), itEnd = s.cend(); it != itEnd; ++it ) {
                    CPPUNIT_ASSERT( (*it).nKey * 2 == it->nVal );
                    ++nCount;
                    if ( it != s.cbegin() ) {
                        CPPUNIT_ASSERT( nPrevKey + 1 == it->nKey );
                    }
                    nPrevKey = it->nKey;
                }
            }
            CPPUNIT_ASSERT( nCount == nLimit );

            // extract/get tests
            {
                typedef typename base_class::less<typename Set::value_type> less_predicate;
                typename Set::exempt_ptr ep;
                typename Set::raw_ptr rp;

                // extract/get
                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrRandom[i];
                    {
                        rcu_lock l;
                        rp = s.get( nKey );
                        CPPUNIT_ASSERT( rp );
                        CPPUNIT_CHECK( rp->nKey == nKey );
                        CPPUNIT_CHECK( rp->nVal == nKey * 2 );
                    }
                    rp.release();

                    ep = s.extract( nKey );
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->nKey == nKey );
                    CPPUNIT_CHECK( ep->nVal == nKey * 2 );
                    ep.release();

                    {
                        rcu_lock l;
                        CPPUNIT_CHECK( !s.get( nKey ));
                    }
                    ep = s.extract( nKey );
                    CPPUNIT_CHECK( !ep );
                }
                CPPUNIT_CHECK( s.empty());

                // extract_with/get_with
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrRandom[i]) );

                for ( int i = 0; i < nLimit; ++i ) {
                    int nKey = arrRandom[i];
                    {
                        rcu_lock l;
                        rp = s.get_with( wrapped_item(nKey), wrapped_less() );
                        CPPUNIT_ASSERT( rp );
                        CPPUNIT_CHECK( rp->nKey == nKey );
                        CPPUNIT_CHECK( rp->nVal == nKey );
                    }
                    rp.release();

                    ep = s.extract_with( wrapped_item( nKey ), wrapped_less() );
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->nKey == nKey );
                    CPPUNIT_CHECK( ep->nVal == nKey );
                    ep.release();

                    {
                        rcu_lock l;
                        CPPUNIT_CHECK( !s.get_with( wrapped_item( nKey ), wrapped_less() ));
                    }
                    ep = s.extract_with( wrapped_item( nKey ), wrapped_less() );
                    CPPUNIT_CHECK( !ep );
                }
                CPPUNIT_CHECK( s.empty());

                // extract_min
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrRandom[i]) );

                for ( int i = 0; i < nLimit; ++i ) {
                    ep = s.extract_min();
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->nKey == i );
                    CPPUNIT_CHECK( ep->nVal == i );
                    CPPUNIT_CHECK( !s.contains(i) );
                    ep.release();
                }
                CPPUNIT_CHECK( s.empty());

                // extract_max
                for ( int i = 0; i < nLimit; ++i )
                    CPPUNIT_ASSERT( s.insert( arrRandom[i]) );

                for ( int i = nLimit-1; i >= 0; --i ) {
                    ep = s.extract_max();
                    CPPUNIT_ASSERT( ep );
                    CPPUNIT_ASSERT( !ep.empty() );
                    CPPUNIT_CHECK( ep->nKey == i );
                    CPPUNIT_CHECK( ep->nVal == i );
                    CPPUNIT_CHECK( !s.contains(i) );
                    ep.release();
                }
                CPPUNIT_CHECK( s.empty());
                ep = s.extract_min();
                CPPUNIT_CHECK( !ep );
                CPPUNIT_CHECK( !s.extract_max() );
            }

            CPPUNIT_MSG( PrintStat()(s, nullptr) );
        }

    public:
        void SkipList_RCU_GPI_less();
        void SkipList_RCU_GPI_cmp();
        void SkipList_RCU_GPI_cmpless();
        void SkipList_RCU_GPI_less_stat();
        void SkipList_RCU_GPI_cmp_stat();
        void SkipList_RCU_GPI_cmpless_stat();
        void SkipList_RCU_GPI_xorshift_less();
        void SkipList_RCU_GPI_xorshift_cmp();
        void SkipList_RCU_GPI_xorshift_cmpless();
        void SkipList_RCU_GPI_xorshift_less_stat();
        void SkipList_RCU_GPI_xorshift_cmp_stat();
        void SkipList_RCU_GPI_xorshift_cmpless_stat();
        void SkipList_RCU_GPI_turbopas_less();
        void SkipList_RCU_GPI_turbopas_cmp();
        void SkipList_RCU_GPI_turbopas_cmpless();
        void SkipList_RCU_GPI_turbopas_less_stat();
        void SkipList_RCU_GPI_turbopas_cmp_stat();
        void SkipList_RCU_GPI_turbopas_cmpless_stat();
        void SkipList_RCU_GPI_michaelalloc_less();
        void SkipList_RCU_GPI_michaelalloc_cmp();
        void SkipList_RCU_GPI_michaelalloc_cmpless();
        void SkipList_RCU_GPI_michaelalloc_less_stat();
        void SkipList_RCU_GPI_michaelalloc_cmp_stat();
        void SkipList_RCU_GPI_michaelalloc_cmpless_stat();

        void SkipList_RCU_GPB_less();
        void SkipList_RCU_GPB_cmp();
        void SkipList_RCU_GPB_cmpless();
        void SkipList_RCU_GPB_less_stat();
        void SkipList_RCU_GPB_cmp_stat();
        void SkipList_RCU_GPB_cmpless_stat();
        void SkipList_RCU_GPB_xorshift_less();
        void SkipList_RCU_GPB_xorshift_cmp();
        void SkipList_RCU_GPB_xorshift_cmpless();
        void SkipList_RCU_GPB_xorshift_less_stat();
        void SkipList_RCU_GPB_xorshift_cmp_stat();
        void SkipList_RCU_GPB_xorshift_cmpless_stat();
        void SkipList_RCU_GPB_turbopas_less();
        void SkipList_RCU_GPB_turbopas_cmp();
        void SkipList_RCU_GPB_turbopas_cmpless();
        void SkipList_RCU_GPB_turbopas_less_stat();
        void SkipList_RCU_GPB_turbopas_cmp_stat();
        void SkipList_RCU_GPB_turbopas_cmpless_stat();
        void SkipList_RCU_GPB_michaelalloc_less();
        void SkipList_RCU_GPB_michaelalloc_cmp();
        void SkipList_RCU_GPB_michaelalloc_cmpless();
        void SkipList_RCU_GPB_michaelalloc_less_stat();
        void SkipList_RCU_GPB_michaelalloc_cmp_stat();
        void SkipList_RCU_GPB_michaelalloc_cmpless_stat();

        void SkipList_RCU_GPT_less();
        void SkipList_RCU_GPT_cmp();
        void SkipList_RCU_GPT_cmpless();
        void SkipList_RCU_GPT_less_stat();
        void SkipList_RCU_GPT_cmp_stat();
        void SkipList_RCU_GPT_cmpless_stat();
        void SkipList_RCU_GPT_xorshift_less();
        void SkipList_RCU_GPT_xorshift_cmp();
        void SkipList_RCU_GPT_xorshift_cmpless();
        void SkipList_RCU_GPT_xorshift_less_stat();
        void SkipList_RCU_GPT_xorshift_cmp_stat();
        void SkipList_RCU_GPT_xorshift_cmpless_stat();
        void SkipList_RCU_GPT_turbopas_less();
        void SkipList_RCU_GPT_turbopas_cmp();
        void SkipList_RCU_GPT_turbopas_cmpless();
        void SkipList_RCU_GPT_turbopas_less_stat();
        void SkipList_RCU_GPT_turbopas_cmp_stat();
        void SkipList_RCU_GPT_turbopas_cmpless_stat();
        void SkipList_RCU_GPT_michaelalloc_less();
        void SkipList_RCU_GPT_michaelalloc_cmp();
        void SkipList_RCU_GPT_michaelalloc_cmpless();
        void SkipList_RCU_GPT_michaelalloc_less_stat();
        void SkipList_RCU_GPT_michaelalloc_cmp_stat();
        void SkipList_RCU_GPT_michaelalloc_cmpless_stat();

        void SkipList_RCU_SHB_less();
        void SkipList_RCU_SHB_cmp();
        void SkipList_RCU_SHB_cmpless();
        void SkipList_RCU_SHB_less_stat();
        void SkipList_RCU_SHB_cmp_stat();
        void SkipList_RCU_SHB_cmpless_stat();
        void SkipList_RCU_SHB_xorshift_less();
        void SkipList_RCU_SHB_xorshift_cmp();
        void SkipList_RCU_SHB_xorshift_cmpless();
        void SkipList_RCU_SHB_xorshift_less_stat();
        void SkipList_RCU_SHB_xorshift_cmp_stat();
        void SkipList_RCU_SHB_xorshift_cmpless_stat();
        void SkipList_RCU_SHB_turbopas_less();
        void SkipList_RCU_SHB_turbopas_cmp();
        void SkipList_RCU_SHB_turbopas_cmpless();
        void SkipList_RCU_SHB_turbopas_less_stat();
        void SkipList_RCU_SHB_turbopas_cmp_stat();
        void SkipList_RCU_SHB_turbopas_cmpless_stat();
        void SkipList_RCU_SHB_michaelalloc_less();
        void SkipList_RCU_SHB_michaelalloc_cmp();
        void SkipList_RCU_SHB_michaelalloc_cmpless();
        void SkipList_RCU_SHB_michaelalloc_less_stat();
        void SkipList_RCU_SHB_michaelalloc_cmp_stat();
        void SkipList_RCU_SHB_michaelalloc_cmpless_stat();

        void SkipList_RCU_SHT_less();
        void SkipList_RCU_SHT_cmp();
        void SkipList_RCU_SHT_cmpless();
        void SkipList_RCU_SHT_less_stat();
        void SkipList_RCU_SHT_cmp_stat();
        void SkipList_RCU_SHT_cmpless_stat();
        void SkipList_RCU_SHT_xorshift_less();
        void SkipList_RCU_SHT_xorshift_cmp();
        void SkipList_RCU_SHT_xorshift_cmpless();
        void SkipList_RCU_SHT_xorshift_less_stat();
        void SkipList_RCU_SHT_xorshift_cmp_stat();
        void SkipList_RCU_SHT_xorshift_cmpless_stat();
        void SkipList_RCU_SHT_turbopas_less();
        void SkipList_RCU_SHT_turbopas_cmp();
        void SkipList_RCU_SHT_turbopas_cmpless();
        void SkipList_RCU_SHT_turbopas_less_stat();
        void SkipList_RCU_SHT_turbopas_cmp_stat();
        void SkipList_RCU_SHT_turbopas_cmpless_stat();
        void SkipList_RCU_SHT_michaelalloc_less();
        void SkipList_RCU_SHT_michaelalloc_cmp();
        void SkipList_RCU_SHT_michaelalloc_cmpless();
        void SkipList_RCU_SHT_michaelalloc_less_stat();
        void SkipList_RCU_SHT_michaelalloc_cmp_stat();
        void SkipList_RCU_SHT_michaelalloc_cmpless_stat();

        CPPUNIT_TEST_SUITE(SkipListSetRCUHdrTest)
            CPPUNIT_TEST(SkipList_RCU_GPI_less)
            CPPUNIT_TEST(SkipList_RCU_GPI_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPI_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPI_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPI_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_RCU_GPB_less)
            CPPUNIT_TEST(SkipList_RCU_GPB_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPB_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPB_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPB_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_RCU_GPT_less)
            CPPUNIT_TEST(SkipList_RCU_GPT_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPT_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPT_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_GPT_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_RCU_SHB_less)
            CPPUNIT_TEST(SkipList_RCU_SHB_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHB_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHB_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHB_michaelalloc_cmpless_stat)

            CPPUNIT_TEST(SkipList_RCU_SHT_less)
            CPPUNIT_TEST(SkipList_RCU_SHT_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHT_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHT_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_less)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_xorshift_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_less)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_turbopas_cmpless_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_less)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_cmp)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_cmpless)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_less_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_cmp_stat)
            CPPUNIT_TEST(SkipList_RCU_SHT_michaelalloc_cmpless_stat)

        CPPUNIT_TEST_SUITE_END()

    };
}

#endif // #ifndef CDSTEST_HDR_SKIPLIST_SET_RCU_H

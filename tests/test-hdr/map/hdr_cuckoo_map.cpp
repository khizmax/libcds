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

#include "map/hdr_cuckoo_map.h"
#include <cds/container/cuckoo_map.h>

namespace map {

    namespace {
        typedef CuckooMapHdrTest::hash_int    hash1;
        struct hash2: private hash1
        {
            typedef hash1 base_class;

            size_t operator()( int i ) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
        };
    }

    void CuckooMapHdrTest::Cuckoo_striped_list()
    {
        CPPUNIT_MESSAGE( "equal");
        {
            struct map_traits : public cc::cuckoo::traits {
                typedef std::equal_to<int> equal_to;
                typedef co::hash_tuple< hash1, hash2 > hash;
            };
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type, map_traits > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "equal, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::equal_to< std::equal_to< int > >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "cmp");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "cmp, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::striping<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::less< std::less<int> >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::striping<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::less< std::less<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp+equal");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::equal_to< std::equal_to<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::striping<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp+equal, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::equal_to< std::equal_to< int > >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

    }


    void CuckooMapHdrTest::Cuckoo_striped_vector()
    {
        CPPUNIT_MESSAGE( "equal");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::equal_to< std::equal_to<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "equal, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::equal_to< std::equal_to< int > >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "cmp");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "cmp, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::striping<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::less< std::less<int> >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::striping<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::less< std::less<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp+equal");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::equal_to< std::equal_to<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::striping<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp+equal, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::striping<> >
                    ,co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::equal_to< std::equal_to< int > >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }
    }


    void CuckooMapHdrTest::Cuckoo_refinable_list()
    {
        CPPUNIT_MESSAGE( "equal");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::equal_to< std::equal_to<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "equal, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::equal_to< std::equal_to< int > >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "cmp");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "cmp, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::less< std::less<int> >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::less< std::less<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp+equal");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::equal_to< std::equal_to<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp+equal, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::equal_to< std::equal_to< int > >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

    }


    void CuckooMapHdrTest::Cuckoo_refinable_vector()
    {
        CPPUNIT_MESSAGE( "equal");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::equal_to< std::equal_to<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "equal, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::equal_to< std::equal_to< int > >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "cmp");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< false >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "cmp, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,cc::cuckoo::store_hash< false >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::less< std::less<int> >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::less< std::less<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp+equal");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::equal_to< std::equal_to<int> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                    ,co::mutex_policy< cc::cuckoo::refinable<> >
                    ,cc::cuckoo::store_hash< false >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }

        CPPUNIT_MESSAGE( "less+cmp+equal, store hash");
        {
            typedef cc::CuckooMap< CuckooMapHdrTest::key_type, CuckooMapHdrTest::value_type,
                cc::cuckoo::make_traits<
                    co::mutex_policy< cc::cuckoo::refinable<> >
                    ,co::less< std::less<int> >
                    ,co::compare< CuckooMapHdrTest::cmp >
                    ,co::equal_to< std::equal_to< int > >
                    ,cc::cuckoo::store_hash< true >
                    ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                    ,co::hash< std::tuple< hash1, hash2 > >
                >::type
            > map_t;

            test_cuckoo<map_t>();
        }
    }

}   // namespace map

CPPUNIT_TEST_SUITE_REGISTRATION(map::CuckooMapHdrTest);

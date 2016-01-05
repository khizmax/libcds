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

#include "set/hdr_cuckoo_set.h"
#include <cds/container/cuckoo_set.h>

namespace set {

    void CuckooSetHdrTest::Cuckoo_Striped_list_unord()
    {
        struct set_traits : public cc::cuckoo::traits {
            typedef equal<item> equal_to;
            typedef co::hash_tuple< hash1, hash2 > hash;
        };

        typedef cc::CuckooSet< item, set_traits > set_t;

        test_int<set_t, equal< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_list_unord_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::equal_to< equal< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, equal< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_list_cmp()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_list_cmp_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_list_less()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::striping<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_list_less_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::less< less< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_list_less_cmp()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::striping<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_list_less_cmp_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::less< less< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_list_less_cmp_eq()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::equal_to< equal< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::striping<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_list_less_cmp_eq_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::equal_to< equal< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }


    // Vector
    void CuckooSetHdrTest::Cuckoo_Striped_vector_unord()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::equal_to< equal< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            >::type
        > set_t;

        test_int<set_t, equal< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_vector_unord_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::equal_to< equal< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, equal< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_vector_cmp()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_vector_cmp_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_vector_less()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::striping<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_vector_less_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::less< less< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_vector_less_cmp()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::striping<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_vector_less_cmp_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::less< less< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_vector_less_cmp_eq()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::equal_to< equal< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::striping<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

    void CuckooSetHdrTest::Cuckoo_Striped_vector_less_cmp_eq_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::equal_to< equal< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

    // Refinable set
    void CuckooSetHdrTest::Cuckoo_Refinable_list_unord()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::equal_to< equal< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
            >::type
        > set_t;

        test_int<set_t, equal< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_list_unord_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::equal_to< equal< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, equal< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_list_cmp()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_list_cmp_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_list_less()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_list_less_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::less< less< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_list_less_cmp()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_list_less_cmp_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::less< less< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_list_less_cmp_eq()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::equal_to< equal< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_list_less_cmp_eq_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::equal_to< equal< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }



    // Refinable Vector
    void CuckooSetHdrTest::Cuckoo_Refinable_vector_unord()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::equal_to< equal< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
            >::type
        > set_t;

        test_int<set_t, equal< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_vector_unord_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::equal_to< equal< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, equal< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_vector_cmp()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_vector_cmp_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_vector_less()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_vector_less_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::less< less< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_vector_less_cmp()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            >::type
        > set_t;

        test_int<set_t, less< item > >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_vector_less_cmp_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::less< less< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< cmp< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_vector_less_cmp_eq()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::equal_to< equal< item > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::mutex_policy< cc::cuckoo::refinable<> >
                ,cc::cuckoo::store_hash< false >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

    void CuckooSetHdrTest::Cuckoo_Refinable_vector_less_cmp_eq_storehash()
    {
        typedef cc::CuckooSet< item,
             cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,co::less< less< item > >
                ,co::compare< cmp< item > >
                ,co::equal_to< equal< item > >
                ,cc::cuckoo::store_hash< true >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                ,co::hash< std::tuple< hash1, hash2 > >
            >::type
        > set_t;

        test_int<set_t, less<item> >();
    }

} // namespace set

CPPUNIT_TEST_SUITE_REGISTRATION(set::CuckooSetHdrTest);

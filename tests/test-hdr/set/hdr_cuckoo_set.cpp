//$$CDS-header$$

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
        //     cc::cuckoo::make_traits<
        //        co::equal_to< equal< item > >
        //        ,co::hash< std::tuple< hash1, hash2 > >
        //    >::type
        //> set_t;

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

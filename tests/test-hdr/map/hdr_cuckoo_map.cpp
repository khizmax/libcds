//$$CDS-header$$

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

//$$CDS-header$$

#include "set/hdr_intrusive_cuckoo_set.h"
#include <cds/intrusive/cuckoo_set.h>

#include "set/intrusive_cuckoo_set_common.h"
#include "../unit/print_cuckoo_stat.h"

namespace set {

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_basehook_equal()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::list, 0 > >  item_type;
        struct set_traits : public ci::cuckoo::traits {
            typedef co::hash_tuple< hash1, hash2 > hash;
            typedef set::equal_to<item_type> equal_to;
        };
        typedef ci::CuckooSet< item_type, set_traits > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_basehook_equal()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::vector<4>, 0 > >  item_type;

        struct set_traits : public ci::cuckoo::traits {
            typedef ci::cuckoo::base_hook< ci::cuckoo::probeset_type< item_type::probeset_type >> hook;
            typedef co::hash_tuple< hash1, hash2 > hash;
            typedef set::equal_to<item_type> equal_to;
        };
        typedef ci::CuckooSet< item_type, set_traits > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_basehook_sort_cmp()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::list, 0 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_basehook_sort_cmp()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::vector<4>, 0 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_basehook_sort_less()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::list, 0 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_basehook_sort_less()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::vector<4>, 0 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_basehook_sort_cmpmix()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::list, 0 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_basehook_sort_cmpmix()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::vector<8>, 0 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_basehook_sort_cmpmix_stat()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::vector<8>, 0 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
                ,co::stat< ci::cuckoo::stat >
            >::type
        > set_type;

        unsigned int nProbesetSize = set_type::node_type::probeset_size ? set_type::node_type::probeset_size : 4;
        set_type s( 256, nProbesetSize, nProbesetSize / 2 );
        test_with( s );
        CPPUNIT_MSG( s.statistics() << s.mutex_policy_statistics() );
    }


    // base hook, store hash
    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_basehook_equal_storehash()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::list, 2 > >  item_type;
        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                    ,ci::cuckoo::store_hash< item_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::equal_to< equal_to<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_basehook_equal_storehash()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::vector<4>, 2 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                    ,ci::cuckoo::store_hash< item_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::equal_to< equal_to<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_basehook_sort_cmp_storehash()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::list, 2 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                    ,ci::cuckoo::store_hash< item_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_basehook_sort_cmp_storehash()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::vector<4>, 2 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                    ,ci::cuckoo::store_hash< item_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_basehook_sort_less_storehash()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::list, 2 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                    ,ci::cuckoo::store_hash< item_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_basehook_sort_less_storehash()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::vector<4>, 2 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                    ,ci::cuckoo::store_hash< item_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_basehook_sort_cmpmix_storehash()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::list, 2 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                    ,ci::cuckoo::store_hash< item_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_basehook_sort_cmpmix_storehash()
    {
        typedef IntrusiveCuckooSetHdrTest::base_item< ci::cuckoo::node< ci::cuckoo::vector<8>, 2 > >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::base_hook<
                    ci::cuckoo::probeset_type< item_type::probeset_type >
                    ,ci::cuckoo::store_hash< item_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }


    // Member hook
    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_memberhook_equal()
    {
        typedef IntrusiveCuckooSetHdrTest::member_item< ci::cuckoo::node< ci::cuckoo::list, 0 > >  item_type;
        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember)> >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::equal_to< equal_to<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_memberhook_equal()
    {
        typedef ci::cuckoo::node< ci::cuckoo::vector<4>, 0 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::equal_to< equal_to<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_memberhook_sort_cmp()
    {
        typedef ci::cuckoo::node< ci::cuckoo::list, 0 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_memberhook_sort_cmp()
    {
        typedef ci::cuckoo::node< ci::cuckoo::vector<4>, 0 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_memberhook_sort_less()
    {
        typedef ci::cuckoo::node< ci::cuckoo::list, 0 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_memberhook_sort_less()
    {
        typedef ci::cuckoo::node< ci::cuckoo::vector<4>, 0 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_memberhook_sort_cmpmix()
    {
        typedef ci::cuckoo::node< ci::cuckoo::list, 0 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_memberhook_sort_cmpmix()
    {
        typedef ci::cuckoo::node< ci::cuckoo::vector<8>, 0 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }


    // member hook, store hash
    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_memberhook_equal_storehash()
    {
        typedef ci::cuckoo::node< ci::cuckoo::list, 2 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;
        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                    ,ci::cuckoo::store_hash< node_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::equal_to< equal_to<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_memberhook_equal_storehash()
    {
        typedef ci::cuckoo::node< ci::cuckoo::vector<4>, 2 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                    ,ci::cuckoo::store_hash< node_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::equal_to< equal_to<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_memberhook_sort_cmp_storehash()
    {
        typedef ci::cuckoo::node< ci::cuckoo::list, 2 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                    ,ci::cuckoo::store_hash< node_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_memberhook_sort_cmp_storehash()
    {
        typedef ci::cuckoo::node< ci::cuckoo::vector<4>, 2 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                    ,ci::cuckoo::store_hash< node_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_memberhook_sort_less_storehash()
    {
        typedef ci::cuckoo::node< ci::cuckoo::list, 2 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                    ,ci::cuckoo::store_hash< node_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_memberhook_sort_less_storehash()
    {
        typedef ci::cuckoo::node< ci::cuckoo::vector<4>, 2 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                    ,ci::cuckoo::store_hash< node_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_list_memberhook_sort_cmpmix_storehash()
    {
        typedef ci::cuckoo::node< ci::cuckoo::list, 2 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                    ,ci::cuckoo::store_hash< node_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

    void IntrusiveCuckooSetHdrTest::Cuckoo_striped_vector_memberhook_sort_cmpmix_storehash()
    {
        typedef ci::cuckoo::node< ci::cuckoo::vector<8>, 2 > node_type;
        typedef IntrusiveCuckooSetHdrTest::member_item< node_type >  item_type;

        typedef ci::CuckooSet< item_type
            ,ci::cuckoo::make_traits<
                ci::opt::hook< ci::cuckoo::member_hook< offsetof(item_type, hMember),
                    ci::cuckoo::probeset_type< node_type::probeset_type >
                    ,ci::cuckoo::store_hash< node_type::hash_array_size >
                > >
                ,co::hash< std::tuple< hash1, hash2 > >
                ,co::less< IntrusiveCuckooSetHdrTest::less<item_type> >
                ,co::compare< IntrusiveCuckooSetHdrTest::cmp<item_type> >
            >::type
        > set_type;

        test_cuckoo<set_type>();
    }

}   // namespace set

CPPUNIT_TEST_SUITE_REGISTRATION(set::IntrusiveCuckooSetHdrTest);

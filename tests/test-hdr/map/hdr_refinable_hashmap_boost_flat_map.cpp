//$$CDS-header$$

#include "map/hdr_striped_map.h"

#include <boost/version.hpp>
#include <cds/details/defs.h>
#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == CDS_COMPILER_MSVC12 && BOOST_VERSION <= 105500
namespace map {
    void StripedMapHdrTest::Refinable_boost_flat_map()
    {
        CPPUNIT_MESSAGE("Skipped; for Microsoft Visual C++ 2013 and boost::container::flat_map you should use boost version 1.56 or above");
    }
}

#elif BOOST_VERSION >= 104800

#include <cds/container/striped_map/boost_flat_map.h>
#include <cds/container/striped_map.h>
#include <cds/sync/spinlock.h>

namespace map {

    namespace {
        typedef boost::container::flat_map<StripedMapHdrTest::key_type, StripedMapHdrTest::value_type, StripedMapHdrTest::less > map_t;

        struct my_copy_policy {
            typedef map_t::iterator iterator;

            void operator()( map_t& m, iterator /*itInsert*/, iterator itWhat )
            {
                m.insert( *itWhat );
            }
        };
    }

    void StripedMapHdrTest::Refinable_boost_flat_map()
    {
        CPPUNIT_MESSAGE( "cmp");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp >
        >   map_cmp;
        test_striped< map_cmp >();

        CPPUNIT_MESSAGE( "less");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less >
        >   map_less;
        test_striped< map_less >();

        CPPUNIT_MESSAGE( "cmpmix");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp >
            , co::less< less >
        >   map_cmpmix;
        test_striped< map_cmpmix >();

        // Spinlock as lock policy
        CPPUNIT_MESSAGE( "spinlock");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<cds::sync::spin> >
            , co::hash< hash_int >
            , co::less< less >
        >   map_spin;
        test_striped< map_spin >();

        // Resizing policy
        CPPUNIT_MESSAGE( "load_factor_resizing<0>(1024)");
        {
            typedef cc::StripedMap< map_t
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , co::hash< hash_int >
                , co::less< less >
                , co::resizing_policy< cc::striped_set::load_factor_resizing<0> >
            >   map_less_resizing_lf;
            map_less_resizing_lf m(30, cc::striped_set::load_factor_resizing<0>(1024));
            test_striped_with(m);
        }

        CPPUNIT_MESSAGE( "load_factor_resizing<256>");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
        >   map_less_resizing_lf16;
        test_striped< map_less_resizing_lf16 >();

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<0>(1024)");
        {
            typedef cc::StripedMap< map_t
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , co::hash< hash_int >
                , co::compare< cmp >
                , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<0> >
            >   map_less_resizing_sbt;
            map_less_resizing_sbt m(30, cc::striped_set::single_bucket_size_threshold<0>(1024));
            test_striped_with(m);
        }

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<256>");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less >
            , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<256> >
        >   map_less_resizing_sbt16;
        test_striped< map_less_resizing_sbt16 >();

        // Copy policy
        CPPUNIT_MESSAGE( "load_factor_resizing<256>, copy_item");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::copy_item >
        >   set_copy_item;
        test_striped< set_copy_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, swap_item");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::swap_item >
        >   set_swap_item;
        test_striped< set_swap_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, move_item");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::move_item >
        >   set_move_item;
        test_striped< set_move_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, special copy_policy");
        typedef cc::StripedMap< map_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< my_copy_policy >
        >   set_special_copy_item;
        test_striped< set_special_copy_item >();

    }
}   // namespace map

/*
#else
    namespace map {
        void StripedMapHdrTest::Refinable_boost_flat_map()
        {
            CPPUNIT_MESSAGE( "Skipped; for boost::container::flat_map is not compatible with MS VC++ 11" );
        }
    }
#endif
*/

#else // BOOST_VERSION < 104800

namespace map {
    void StripedMapHdrTest::Refinable_boost_flat_map()
    {
        CPPUNIT_MESSAGE( "Skipped; for boost::container::flat_map you should use boost version 1.48 or above" );
    }
}
#endif  // BOOST_VERSION

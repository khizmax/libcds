/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "set/hdr_striped_set.h"

#include <boost/version.hpp>
#include <cds/details/defs.h>
#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == CDS_COMPILER_MSVC12 && BOOST_VERSION <= 105500
namespace set {
    void StripedSetHdrTest::Refinable_boost_vector()
    {
        CPPUNIT_MESSAGE("Skipped; for Microsoft Visual C++ 2013 and boost::container::vector you should use boost version 1.56 or above");
    }
}

#elif BOOST_VERSION >= 104800

#include <cds/details/defs.h>

#include <cds/container/striped_set/boost_vector.h>
#include <cds/container/striped_set.h>
#include <cds/lock/spinlock.h>

namespace set {

    namespace {
        struct my_copy_policy {
            typedef boost::container::vector<StripedSetHdrTest::item> vector_type;
            typedef vector_type::iterator iterator;

            void operator()( vector_type& vec, iterator itInsert, iterator itWhat )
            {
                vec.insert( itInsert, std::make_pair(itWhat->key(), itWhat->val()) );
            }
        };

        typedef boost::container::vector<StripedSetHdrTest::item>  sequence_t;
    }

    void StripedSetHdrTest::Refinable_boost_vector()
    {
        CPPUNIT_MESSAGE( "cmp");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
        >   set_cmp;
        test_striped2< set_cmp >();

        CPPUNIT_MESSAGE( "less");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less<item> >
        >   set_less;
        test_striped2< set_less >();

        CPPUNIT_MESSAGE( "cmpmix");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::less< less<item> >
        >   set_cmpmix;
        test_striped2< set_cmpmix >();

        // Spinlock as lock policy
        CPPUNIT_MESSAGE( "spinlock");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<cds::lock::ReentrantSpin> >
            , co::hash< hash_int >
            , co::less< less<item> >
        >   set_spin;
        test_striped2< set_spin >();

        // Resizing policy
        CPPUNIT_MESSAGE( "load_factor_resizing<0>(8)");
        {
            typedef cc::StripedSet< sequence_t
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , co::hash< hash_int >
                , co::less< less<item> >
                , co::resizing_policy< cc::striped_set::load_factor_resizing<0> >
            >   set_less_resizing_lf;
            set_less_resizing_lf s(30, cc::striped_set::load_factor_resizing<0>(8));
            test_striped_with(s);
        }

        CPPUNIT_MESSAGE( "load_factor_resizing<4>");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<4> >
        >   set_less_resizing_lf16;
        test_striped2< set_less_resizing_lf16 >();

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<0>(8)");
        {
            typedef cc::StripedSet< sequence_t
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , co::hash< hash_int >
                , co::less< less<item> >
                , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<0> >
            >   set_less_resizing_sbt;
            set_less_resizing_sbt s(30, cc::striped_set::single_bucket_size_threshold<0>(8));
            test_striped_with(s);
        }

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<6>");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less<item> >
            , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<6> >
        >   set_less_resizing_sbt16;
        test_striped2< set_less_resizing_sbt16 >();


        // Copy policy
        CPPUNIT_MESSAGE( "copy_item");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::copy_policy< cc::striped_set::copy_item >
        >   set_copy_item;
        test_striped2< set_copy_item >();

        CPPUNIT_MESSAGE( "swap_item");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::copy_policy< cc::striped_set::swap_item >
        >   set_swap_item;
        test_striped2< set_swap_item >();

        CPPUNIT_MESSAGE( "move_item");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::copy_policy< cc::striped_set::move_item >
        >   set_move_item;
        test_striped2< set_move_item >();

        CPPUNIT_MESSAGE( "special copy policy");
        typedef cc::StripedSet< sequence_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::copy_policy< my_copy_policy >
        >   set_special_copy_item;
        test_striped2< set_special_copy_item >();
    }

}   // namespace set

#else // BOOST_VERSION < 104800

namespace set {
    void StripedSetHdrTest::Refinable_boost_vector()
    {
        CPPUNIT_MESSAGE( "Skipped; for boost::container::vector you should use boost version 1.48 or above" );
    }
}
#endif  // BOOST_VERSION

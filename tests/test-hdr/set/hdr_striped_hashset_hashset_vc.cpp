/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "set/hdr_striped_set.h"
#include <cds/container/striped_set/std_hash_set.h>
#include <cds/container/striped_set.h>
#include <cds/lock/spinlock.h>

#if (CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)) && _MSC_VER < 1600

namespace stdext {
    inline size_t hash_value(set::StripedSetHdrTest::item const& _Keyval)
    {
        return set::StripedSetHdrTest::hash_int()( _Keyval );
    }
}

namespace set {

    namespace {
        typedef stdext::hash_compare<StripedSetHdrTest::item, StripedSetHdrTest::less<StripedSetHdrTest::item> >    hash_set_t;

        struct my_copy_policy {
            typedef stdext::hash_set< StripedSetHdrTest::item, hash_set_t > set_type;
            typedef set_type::iterator iterator;

            void operator()( set_type& set, iterator itWhat )
            {
                set.insert( std::make_pair(itWhat->key(), itWhat->val()) );
            }
        };

        typedef stdext::hash_set<StripedSetHdrTest::item, hash_set_t > set_t;
    }

    void StripedSetHdrTest::Striped_hashset()
    {
        CPPUNIT_MESSAGE( "cmp");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            ,co::mutex_policy< cc::striped_set::striping<> >
        >   set_cmp;
        test_striped< set_cmp >();

        CPPUNIT_MESSAGE( "less");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::less< less<item> >
            ,co::mutex_policy< cc::striped_set::striping<> >
        >   set_less;
        test_striped< set_less >();

        CPPUNIT_MESSAGE( "cmpmix");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::less< less<item> >
            ,co::mutex_policy< cc::striped_set::striping<> >
        >   set_cmpmix;
        test_striped< set_cmpmix >();

        // Spinlock as lock policy
        CPPUNIT_MESSAGE( "spinlock");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::less< less<item> >
            ,co::mutex_policy< cc::striped_set::striping< cds::lock::Spin > >
        >   set_spin;
        test_striped< set_spin >();

        // Resizing policy
        CPPUNIT_MESSAGE( "load_factor_resizing<0>(1024)");
        {
            typedef cc::StripedSet< set_t
                , co::hash< hash_int >
                , co::less< less<item> >
                , co::resizing_policy< cc::striped_set::load_factor_resizing<0> >
            >   set_less_resizing_lf;
            set_less_resizing_lf s(30, cc::striped_set::load_factor_resizing<0>( 1024 ));
            test_striped_with( s );
        }

        CPPUNIT_MESSAGE( "load_factor_resizing<256>");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::less< less<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
        >   set_less_resizing_lf16;
        test_striped< set_less_resizing_lf16 >();

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<0>(1024");
        {
            typedef cc::StripedSet< set_t
                , co::hash< hash_int >
                , co::less< less<item> >
                , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<0> >
            >   set_less_resizing_sbt;
            set_less_resizing_sbt s( 30, cc::striped_set::single_bucket_size_threshold<0>(1024));
            test_striped_with(s);
        }

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<256>");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::less< less<item> >
            , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<256> >
        >   set_less_resizing_sbt16;
        test_striped< set_less_resizing_sbt16 >();

        // Copy policy
        CPPUNIT_MESSAGE( "load_factor_resizing<256>, copy_item");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::copy_item >
        >   set_copy_item;
        test_striped< set_copy_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, swap_item");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::swap_item >
        >   set_swap_item;
        test_striped< set_swap_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, move_item");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::move_item >
        >   set_move_item;
        test_striped< set_move_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, special copy_item");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< my_copy_policy >
        >   set_special_copy_item;
        test_striped< set_special_copy_item >();
    }

}   // namespace set
#endif // #if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION < 1600

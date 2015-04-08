//$$CDS-header$$

#include "set/hdr_striped_set.h"
#include <cds/container/striped_set/std_vector.h>
#include <cds/container/striped_set.h>
#include <cds/sync/spinlock.h>

namespace set {

    namespace {
        struct my_copy_policy {
            typedef std::vector<StripedSetHdrTest::item> vector_type;
            typedef vector_type::iterator iterator;

            void operator()( vector_type& vec, iterator itInsert, iterator itWhat )
            {
                vec.insert( itInsert, std::make_pair(itWhat->key(), itWhat->val()) );
            }
        };

        typedef std::vector<StripedSetHdrTest::item> sequence_t;
    }

    void StripedSetHdrTest::Refinable_vector()
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
            , co::mutex_policy< cc::striped_set::refinable<cds::sync::reentrant_spin> >
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

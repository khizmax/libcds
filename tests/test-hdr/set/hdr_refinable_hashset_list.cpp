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

#include "set/hdr_striped_set.h"
#include <cds/container/striped_set/std_list.h>
#include <cds/container/striped_set.h>
#include <cds/sync/spinlock.h>

namespace set {

    namespace {
        struct my_copy_policy {
            typedef std::list<StripedSetHdrTest::item> list_type;
            typedef list_type::iterator iterator;

            void operator()( list_type& list, iterator itInsert, iterator itWhat )
            {
                list.insert( itInsert, std::make_pair(itWhat->key(), itWhat->val()) );
            }
        };

        typedef std::list<StripedSetHdrTest::item> sequence_t;
    }

    void StripedSetHdrTest::Refinable_list()
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

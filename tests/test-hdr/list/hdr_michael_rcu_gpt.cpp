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

#include "list/hdr_michael.h"
#include <cds/urcu/general_threaded.h>
#include <cds/container/michael_list_rcu.h>

namespace ordlist {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_threaded<> >    rcu_type;

        struct RCU_GPT_cmp_traits : public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
        };
    }

    void MichaelListTestHeader::RCU_GPT_cmp()
    {
        // traits-based version
        typedef cc::MichaelList< rcu_type, item, RCU_GPT_cmp_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelList< rcu_type, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPT_less_traits : public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
        };
    }
    void MichaelListTestHeader::RCU_GPT_less()
    {
        // traits-based version
        typedef cc::MichaelList< rcu_type, item, RCU_GPT_less_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelList< rcu_type, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPT_cmpmix_traits : public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>  less;
        };
    }
    void MichaelListTestHeader::RCU_GPT_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelList< rcu_type, item, RCU_GPT_cmpmix_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelList< rcu_type, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPT_ic_traits : public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelListTestHeader::RCU_GPT_ic()
    {
        // traits-based version
        typedef cc::MichaelList< rcu_type, item, RCU_GPT_ic_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelList< rcu_type, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

}   // namespace ordlist


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

#include "list/hdr_michael_kv.h"
#include <cds/urcu/general_buffered.h>
#include <cds/container/michael_kvlist_rcu.h>

namespace ordlist {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;
        struct RCU_GPB_cmp_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
        };
    }
    void MichaelKVListTestHeader::RCU_GPB_cmp()
    {
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_GPB_cmp_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPB_less_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
        };
    }
    void MichaelKVListTestHeader::RCU_GPB_less()
    {
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_GPB_less_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPB_cmpmix_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>  less;
        };
    }
    void MichaelKVListTestHeader::RCU_GPB_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_GPB_cmpmix_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPB_ic_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelKVListTestHeader::RCU_GPB_ic()
    {
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_GPB_ic_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

}   // namespace ordlist


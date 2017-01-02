/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#ifndef CDSUNIT_SET_TYPE_LAZY_LIST_H
#define CDSUNIT_SET_TYPE_LAZY_LIST_H

#include "set_type.h"

#include <cds/container/lazy_list_hp.h>
#include <cds/container/lazy_list_dhp.h>
#include <cds/container/lazy_list_rcu.h>

namespace set {

    template <typename Key, typename Val>
    struct lazy_list_type
    {
        typedef typename set_type_base< Key, Val >::key_val key_val;
        typedef typename set_type_base< Key, Val >::compare compare;
        typedef typename set_type_base< Key, Val >::less    less;

        struct traits_LazyList_cmp :
            public cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_cmp > LazyList_HP_cmp;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_cmp > LazyList_DHP_cmp;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_cmp > LazyList_RCU_GPI_cmp;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_cmp > LazyList_RCU_GPB_cmp;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_cmp > LazyList_RCU_GPT_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_cmp > LazyList_RCU_SHB_cmp;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_cmp > LazyList_RCU_SHT_cmp;
#endif

        struct traits_LazyList_cmp_stat : public traits_LazyList_cmp
        {
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_cmp_stat > LazyList_HP_cmp_stat;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_cmp_stat > LazyList_DHP_cmp_stat;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_cmp_stat > LazyList_RCU_GPI_cmp_stat;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_cmp_stat > LazyList_RCU_GPB_cmp_stat;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_cmp_stat > LazyList_RCU_GPT_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_cmp_stat > LazyList_RCU_SHB_cmp_stat;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_cmp_stat > LazyList_RCU_SHT_cmp_stat;
#endif

        struct traits_LazyList_cmp_seqcst :
            public cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP, key_val,  traits_LazyList_cmp_seqcst > LazyList_HP_cmp_seqcst;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_cmp_seqcst > LazyList_DHP_cmp_seqcst;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_cmp_seqcst > LazyList_RCU_GPI_cmp_seqcst;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_cmp_seqcst > LazyList_RCU_GPB_cmp_seqcst;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_cmp_seqcst > LazyList_RCU_GPT_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_cmp_seqcst > LazyList_RCU_SHB_cmp_seqcst;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_cmp_seqcst > LazyList_RCU_SHT_cmp_seqcst;
#endif

        struct traits_LazyList_less:
            public cc::lazy_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_less > LazyList_HP_less;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_less > LazyList_DHP_less;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_less > LazyList_RCU_GPI_less;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_less > LazyList_RCU_GPB_less;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_less > LazyList_RCU_GPT_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_less > LazyList_RCU_SHB_less;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_less > LazyList_RCU_SHT_less;
#endif

        struct traits_LazyList_less_stat: public traits_LazyList_less
        {
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyList< cds::gc::HP, key_val, traits_LazyList_less_stat > LazyList_HP_less_stat;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_less_stat > LazyList_DHP_less_stat;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_less_stat > LazyList_RCU_GPI_less_stat;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_less_stat > LazyList_RCU_GPB_less_stat;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_less_stat > LazyList_RCU_GPT_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_less_stat > LazyList_RCU_SHB_less_stat;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_less_stat > LazyList_RCU_SHT_less_stat;
#endif

        struct traits_LazyList_less_seqcst :
            public cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP, key_val,  traits_LazyList_less_seqcst > LazyList_HP_less_seqcst;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_less_seqcst > LazyList_DHP_less_seqcst;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_less_seqcst > LazyList_RCU_GPI_less_seqcst;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_less_seqcst > LazyList_RCU_GPB_less_seqcst;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_less_seqcst > LazyList_RCU_GPT_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_less_seqcst > LazyList_RCU_SHB_less_seqcst;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_less_seqcst > LazyList_RCU_SHT_less_seqcst;
#endif

    };

} // namespace set

#endif // #ifndef CDSUNIT_SET_TYPE_LAZY_LIST_H

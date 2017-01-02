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

#ifndef CDSUNIT_MAP_TYPE_LAZY_LIST_H
#define CDSUNIT_MAP_TYPE_LAZY_LIST_H

#include "map_type.h"

#include <cds/container/lazy_kvlist_hp.h>
#include <cds/container/lazy_kvlist_dhp.h>
#include <cds/container/lazy_kvlist_rcu.h>
#include <cds/container/lazy_kvlist_nogc.h>

#include <cds_test/stat_lazy_list_out.h>

namespace map {

    template <typename Key, typename Value>
    struct lazy_list_type
    {
        typedef typename map_type_base<Key, Value>::key_compare compare;
        typedef typename map_type_base<Key, Value>::key_less    less;
        typedef typename map_type_base<Key, Value>::equal_to    equal_to;

        struct traits_LazyList_cmp:
            public cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp > LazyList_HP_cmp;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp > LazyList_DHP_cmp;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp > LazyList_NOGC_cmp;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp > LazyList_RCU_GPI_cmp;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp > LazyList_RCU_GPB_cmp;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp > LazyList_RCU_GPT_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp > LazyList_RCU_SHB_cmp;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_cmp > LazyList_RCU_SHT_cmp;
#endif

        struct traits_LazyList_cmp_stat: public traits_LazyList_cmp
        {
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_stat > LazyList_HP_cmp_stat;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_stat > LazyList_DHP_cmp_stat;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_stat > LazyList_NOGC_cmp_stat;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_stat > LazyList_RCU_GPI_cmp_stat;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_stat > LazyList_RCU_GPB_cmp_stat;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_stat > LazyList_RCU_GPT_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_stat > LazyList_RCU_SHB_cmp_stat;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_cmp_stat > LazyList_RCU_SHT_cmp_stat;
#endif

        struct traits_LazyList_unord :
            public cc::lazy_list::make_traits<
                co::equal_to< equal_to >
                ,co::sort< false >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_unord > LazyList_NOGC_unord;

        struct traits_LazyList_cmp_seqcst :
            public cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_seqcst > LazyList_HP_cmp_seqcst;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_seqcst > LazyList_DHP_cmp_seqcst;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_seqcst > LazyList_NOGC_cmp_seqcst;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_seqcst > LazyList_RCU_GPI_cmp_seqcst;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_seqcst > LazyList_RCU_GPB_cmp_seqcst;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_seqcst > LazyList_RCU_GPT_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_seqcst > LazyList_RCU_SHB_cmp_seqcst;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_cmp_seqcst > LazyList_RCU_SHT_cmp_seqcst;
#endif

        struct traits_LazyList_less :
            public cc::lazy_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less > LazyList_HP_less;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less > LazyList_DHP_less;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less > LazyList_NOGC_less;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less > LazyList_RCU_GPI_less;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less > LazyList_RCU_GPB_less;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less > LazyList_RCU_GPT_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less > LazyList_RCU_SHB_less;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_less > LazyList_RCU_SHT_less;
#endif

        struct traits_LazyList_less_stat: public traits_LazyList_less
        {
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_stat > LazyList_HP_less_stat;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_stat > LazyList_DHP_less_stat;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_stat > LazyList_NOGC_less_stat;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_stat > LazyList_RCU_GPI_less_stat;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_stat > LazyList_RCU_GPB_less_stat;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_stat > LazyList_RCU_GPT_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_stat > LazyList_RCU_SHB_less_stat;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_less_stat > LazyList_RCU_SHT_less_stat;
#endif

        struct traits_LazyList_less_seqcst :
            public cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_seqcst > LazyList_HP_less_seqcst;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_seqcst > LazyList_DHP_less_seqcst;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_seqcst > LazyList_NOGC_less_seqcst;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_seqcst > LazyList_RCU_GPI_less_seqcst;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_seqcst > LazyList_RCU_GPB_less_seqcst;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_seqcst > LazyList_RCU_GPT_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_seqcst > LazyList_RCU_SHB_less_seqcst;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_less_seqcst > LazyList_RCU_SHT_less_seqcst;
#endif

    };

} // namespace map

#endif // ifndef CDSUNIT_MAP_TYPE_LAZY_LIST_H

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

#ifndef CDSUNIT_SET_TYPE_ITRERABLE_LIST_H
#define CDSUNIT_SET_TYPE_ITRERABLE_LIST_H

#include "set_type.h"

#include <cds/container/iterable_list_hp.h>
#include <cds/container/iterable_list_dhp.h>
//#include <cds/container/michael_list_rcu.h>

namespace set {

    template <typename Key, typename Val>
    struct iterable_list_type
    {
        typedef typename set_type_base< Key, Val >::key_val key_val;
        typedef typename set_type_base< Key, Val >::compare compare;
        typedef typename set_type_base< Key, Val >::less    less;

        struct traits_IterableList_cmp_stdAlloc:
            public cc::iterable_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_cmp_stdAlloc > IterableList_HP_cmp_stdAlloc;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_cmp_stdAlloc > IterableList_DHP_cmp_stdAlloc;
        //typedef cc::IterableList< rcu_gpi, key_val, traits_IterableList_cmp_stdAlloc > IterableList_RCU_GPI_cmp_stdAlloc;
        //typedef cc::IterableList< rcu_gpb, key_val, traits_IterableList_cmp_stdAlloc > IterableList_RCU_GPB_cmp_stdAlloc;
        //typedef cc::IterableList< rcu_gpt, key_val, traits_IterableList_cmp_stdAlloc > IterableList_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        //typedef cc::IterableList< rcu_shb, key_val, traits_IterableList_cmp_stdAlloc > IterableList_RCU_SHB_cmp_stdAlloc;
        //typedef cc::IterableList< rcu_sht, key_val, traits_IterableList_cmp_stdAlloc > IterableList_RCU_SHT_cmp_stdAlloc;
#endif

        struct traits_IterableList_cmp_stdAlloc_stat: public traits_IterableList_cmp_stdAlloc
        {
            typedef cc::iterable_list::stat<> stat;
        };
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_cmp_stdAlloc_stat > IterableList_HP_cmp_stdAlloc_stat;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_cmp_stdAlloc_stat > IterableList_DHP_cmp_stdAlloc_stat;
        //typedef cc::IterableList< rcu_gpi, key_val, traits_IterableList_cmp_stdAlloc_stat > IterableList_RCU_GPI_cmp_stdAlloc_stat;
        //typedef cc::IterableList< rcu_gpb, key_val, traits_IterableList_cmp_stdAlloc_stat > IterableList_RCU_GPB_cmp_stdAlloc_stat;
        //typedef cc::IterableList< rcu_gpt, key_val, traits_IterableList_cmp_stdAlloc_stat > IterableList_RCU_GPT_cmp_stdAlloc_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        //typedef cc::IterableList< rcu_shb, key_val, traits_IterableList_cmp_stdAlloc_stat > IterableList_RCU_SHB_cmp_stdAlloc_stat;
        //typedef cc::IterableList< rcu_sht, key_val, traits_IterableList_cmp_stdAlloc_stat > IterableList_RCU_SHT_cmp_stdAlloc_stat;
#endif

        struct traits_IterableList_cmp_stdAlloc_seqcst : public traits_IterableList_cmp_stdAlloc
        {
            typedef co::v::sequential_consistent memory_model;
        };
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_cmp_stdAlloc_seqcst > IterableList_HP_cmp_stdAlloc_seqcst;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_cmp_stdAlloc_seqcst > IterableList_DHP_cmp_stdAlloc_seqcst;
        //typedef cc::IterableList< rcu_gpi, key_val, traits_IterableList_cmp_stdAlloc_seqcst > IterableList_RCU_GPI_cmp_stdAlloc_seqcst;
        //typedef cc::IterableList< rcu_gpb, key_val, traits_IterableList_cmp_stdAlloc_seqcst > IterableList_RCU_GPB_cmp_stdAlloc_seqcst;
        //typedef cc::IterableList< rcu_gpt, key_val, traits_IterableList_cmp_stdAlloc_seqcst > IterableList_RCU_GPT_cmp_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        //typedef cc::IterableList< rcu_shb, key_val, traits_IterableList_cmp_stdAlloc_seqcst > IterableList_RCU_SHB_cmp_stdAlloc_seqcst;
        //typedef cc::IterableList< rcu_sht, key_val, traits_IterableList_cmp_stdAlloc_seqcst > IterableList_RCU_SHT_cmp_stdAlloc_seqcst;
#endif

        struct traits_IterableList_less_stdAlloc :
            public cc::iterable_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_less_stdAlloc > IterableList_HP_less_stdAlloc;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_less_stdAlloc > IterableList_DHP_less_stdAlloc;
        //typedef cc::IterableList< rcu_gpi, key_val, traits_IterableList_less_stdAlloc > IterableList_RCU_GPI_less_stdAlloc;
        //typedef cc::IterableList< rcu_gpb, key_val, traits_IterableList_less_stdAlloc > IterableList_RCU_GPB_less_stdAlloc;
        //typedef cc::IterableList< rcu_gpt, key_val, traits_IterableList_less_stdAlloc > IterableList_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        //typedef cc::IterableList< rcu_shb, key_val, traits_IterableList_less_stdAlloc > IterableList_RCU_SHB_less_stdAlloc;
        //typedef cc::IterableList< rcu_sht, key_val, traits_IterableList_less_stdAlloc > IterableList_RCU_SHT_less_stdAlloc;
#endif

        struct traits_IterableList_less_stdAlloc_stat : public traits_IterableList_less_stdAlloc
        {
            typedef cc::iterable_list::stat<> stat;
        };
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_less_stdAlloc_stat > IterableList_HP_less_stdAlloc_stat;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_less_stdAlloc_stat > IterableList_DHP_less_stdAlloc_stat;
        //typedef cc::IterableList< rcu_gpi, key_val, traits_IterableList_less_stdAlloc_stat > IterableList_RCU_GPI_less_stdAlloc_stat;
        //typedef cc::IterableList< rcu_gpb, key_val, traits_IterableList_less_stdAlloc_stat > IterableList_RCU_GPB_less_stdAlloc_stat;
        //typedef cc::IterableList< rcu_gpt, key_val, traits_IterableList_less_stdAlloc_stat > IterableList_RCU_GPT_less_stdAlloc_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        //typedef cc::IterableList< rcu_shb, key_val, traits_IterableList_less_stdAlloc_stat > IterableList_RCU_SHB_less_stdAlloc_stat;
        //typedef cc::IterableList< rcu_sht, key_val, traits_IterableList_less_stdAlloc_stat > IterableList_RCU_SHT_less_stdAlloc_stat;
#endif

        struct traits_IterableList_less_stdAlloc_seqcst :
            public cc::iterable_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::IterableList< cds::gc::HP,  key_val, traits_IterableList_less_stdAlloc_seqcst > IterableList_HP_less_stdAlloc_seqcst;
        typedef cc::IterableList< cds::gc::DHP, key_val, traits_IterableList_less_stdAlloc_seqcst > IterableList_DHP_less_stdAlloc_seqcst;
        //typedef cc::IterableList< rcu_gpi, key_val, traits_IterableList_less_stdAlloc_seqcst > IterableList_RCU_GPI_less_stdAlloc_seqcst;
        //typedef cc::IterableList< rcu_gpb, key_val, traits_IterableList_less_stdAlloc_seqcst > IterableList_RCU_GPB_less_stdAlloc_seqcst;
        //typedef cc::IterableList< rcu_gpt, key_val, traits_IterableList_less_stdAlloc_seqcst > IterableList_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        //typedef cc::IterableList< rcu_shb, key_val, traits_IterableList_less_stdAlloc_seqcst > IterableList_RCU_SHB_less_stdAlloc_seqcst;
        //typedef cc::IterableList< rcu_sht, key_val, traits_IterableList_less_stdAlloc_seqcst > IterableList_RCU_SHT_less_stdAlloc_seqcst;
#endif

    };

} // namespace set

#endif // #ifndef CDSUNIT_SET_TYPE_ITRERABLE_LIST_H

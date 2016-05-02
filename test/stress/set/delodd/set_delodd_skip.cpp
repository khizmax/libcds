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

#include "set_delodd.h"
#include "set_type_skip_list.h"

#define CDSSTRESS_SkipListSet( skiplist_set_type ) \
    TEST_F( Set_DelOdd, skiplist_set_type ) \
    { \
        typedef set::set_type< tag_SkipListSet, key_thread, size_t >::skiplist_set_type set_type; \
        run_test_extract<set_type>(); \
    }

namespace set {

    CDSSTRESS_SkipListSet( SkipListSet_hp_less_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_dhp_less_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpi_less_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpb_less_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpt_less_pascal )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SkipListSet( SkipListSet_rcu_shb_less_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_sht_less_pascal )
#endif

    CDSSTRESS_SkipListSet( SkipListSet_hp_less_pascal_seqcst )
    CDSSTRESS_SkipListSet( SkipListSet_dhp_less_pascal_seqcst )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpi_less_pascal_seqcst )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpb_less_pascal_seqcst )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpt_less_pascal_seqcst )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SkipListSet( SkipListSet_rcu_shb_less_pascal_seqcst )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_sht_less_pascal_seqcst )
#endif

    CDSSTRESS_SkipListSet( SkipListSet_hp_less_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_dhp_less_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpi_less_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpb_less_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpt_less_pascal_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SkipListSet( SkipListSet_rcu_shb_less_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_sht_less_pascal_stat )
#endif

    CDSSTRESS_SkipListSet( SkipListSet_hp_cmp_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_dhp_cmp_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpi_cmp_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpb_cmp_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpt_cmp_pascal )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SkipListSet( SkipListSet_rcu_shb_cmp_pascal )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_sht_cmp_pascal )
#endif

    CDSSTRESS_SkipListSet( SkipListSet_hp_cmp_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_dhp_cmp_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpi_cmp_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpb_cmp_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpt_cmp_pascal_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SkipListSet( SkipListSet_rcu_shb_cmp_pascal_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_sht_cmp_pascal_stat )
#endif

    CDSSTRESS_SkipListSet( SkipListSet_hp_less_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_dhp_less_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpi_less_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpb_less_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpt_less_xorshift )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SkipListSet( SkipListSet_rcu_shb_less_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_sht_less_xorshift )
#endif

    CDSSTRESS_SkipListSet( SkipListSet_hp_less_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_dhp_less_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpi_less_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpb_less_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpt_less_xorshift_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SkipListSet( SkipListSet_rcu_shb_less_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_sht_less_xorshift_stat )
#endif

    CDSSTRESS_SkipListSet( SkipListSet_hp_cmp_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_dhp_cmp_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpi_cmp_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpb_cmp_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpt_cmp_xorshift )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SkipListSet( SkipListSet_rcu_shb_cmp_xorshift )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_sht_cmp_xorshift )
#endif

    CDSSTRESS_SkipListSet( SkipListSet_hp_cmp_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_dhp_cmp_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpi_cmp_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpb_cmp_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_gpt_cmp_xorshift_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SkipListSet( SkipListSet_rcu_shb_cmp_xorshift_stat )
    CDSSTRESS_SkipListSet( SkipListSet_rcu_sht_cmp_xorshift_stat )
#endif



} // namespace set

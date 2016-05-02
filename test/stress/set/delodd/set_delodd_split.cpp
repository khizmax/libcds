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
#include "set_type_split_list.h"

#define CDSSTRESS_SplitListSet( splitlist_set_type ) \
    TEST_P( Set_DelOdd_LF, splitlist_set_type ) \
    { \
        typedef set::set_type< tag_SplitListSet, key_thread, size_t >::splitlist_set_type set_type; \
        run_test_extract<set_type>(); \
    }

namespace set {

    CDSSTRESS_SplitListSet( SplitList_Michael_HP_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_DHP_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPI_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPB_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPT_dyn_cmp )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHB_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHT_dyn_cmp )
#endif

    CDSSTRESS_SplitListSet( SplitList_Michael_HP_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_DHP_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPI_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPB_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPT_dyn_cmp_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHB_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHT_dyn_cmp_stat )
#endif

    CDSSTRESS_SplitListSet( SplitList_Michael_HP_dyn_cmp_seqcst )
    CDSSTRESS_SplitListSet( SplitList_Michael_DHP_dyn_cmp_seqcst )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPI_dyn_cmp_seqcst )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPB_dyn_cmp_seqcst )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPT_dyn_cmp_seqcst )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHB_dyn_cmp_seqcst )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHT_dyn_cmp_seqcst )
#endif

    CDSSTRESS_SplitListSet( SplitList_Michael_HP_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_DHP_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPI_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPB_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPT_st_cmp )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHB_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHT_st_cmp )
#endif

    CDSSTRESS_SplitListSet( SplitList_Michael_HP_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_DHP_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPI_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPB_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPT_dyn_less )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHB_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHT_dyn_less )
#endif

    CDSSTRESS_SplitListSet( SplitList_Michael_HP_st_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_DHP_st_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPI_st_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPB_st_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPT_st_less )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHB_st_less )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHT_st_less )
#endif

    CDSSTRESS_SplitListSet( SplitList_Michael_HP_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_DHP_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPI_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPB_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_GPT_st_less_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHB_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Michael_RCU_SHT_st_less_stat )
#endif

    CDSSTRESS_SplitListSet( SplitList_Lazy_HP_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_DHP_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPI_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPB_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPT_dyn_cmp )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHB_dyn_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHT_dyn_cmp )
#endif

    CDSSTRESS_SplitListSet( SplitList_Lazy_HP_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_DHP_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPI_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPB_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPT_dyn_cmp_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHB_dyn_cmp_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHT_dyn_cmp_stat )
#endif

    CDSSTRESS_SplitListSet( SplitList_Lazy_HP_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_DHP_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPI_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPB_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPT_st_cmp )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHB_st_cmp )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHT_st_cmp )
#endif

    CDSSTRESS_SplitListSet( SplitList_Lazy_HP_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_DHP_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPI_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPB_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPT_dyn_less )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHB_dyn_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHT_dyn_less )
#endif

    CDSSTRESS_SplitListSet( SplitList_Lazy_HP_st_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_DHP_st_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPI_st_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPB_st_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPT_st_less )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHB_st_less )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHT_st_less )
#endif

    CDSSTRESS_SplitListSet( SplitList_Lazy_HP_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_DHP_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPI_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPB_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_GPT_st_less_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHB_st_less_stat )
    CDSSTRESS_SplitListSet( SplitList_Lazy_RCU_SHT_st_less_stat )
#endif
} // namespace set

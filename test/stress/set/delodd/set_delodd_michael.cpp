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
#include "set_type_michael.h"

#define CDSSTRESS_MichaelSet( michael_set_type ) \
    TEST_P( Set_DelOdd_LF, michael_set_type ) \
    { \
        typedef set::set_type< tag_MichaelHashSet, key_thread, size_t >::michael_set_type set_type; \
        run_test_extract<set_type>(); \
    }

namespace set {

    CDSSTRESS_MichaelSet( MichaelSet_HP_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_DHP_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPI_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPB_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPT_cmp_stdAlloc )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_MichaelSet( MichaelSet_RCU_SHB_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_SHT_cmp_stdAlloc )
#endif

    CDSSTRESS_MichaelSet( MichaelSet_HP_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_DHP_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPI_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPB_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPT_less_stdAlloc )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_MichaelSet( MichaelSet_RCU_SHB_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_SHT_less_stdAlloc )
#endif


    CDSSTRESS_MichaelSet( MichaelSet_HP_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_DHP_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPI_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPB_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPT_cmp_michaelAlloc )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_MichaelSet( MichaelSet_RCU_SHB_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_SHT_cmp_michaelAlloc )
#endif

    CDSSTRESS_MichaelSet( MichaelSet_HP_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_DHP_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPI_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPB_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_GPT_less_michaelAlloc )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_MichaelSet( MichaelSet_RCU_SHB_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_RCU_SHT_less_michaelAlloc )
#endif


    CDSSTRESS_MichaelSet( MichaelSet_Lazy_HP_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_DHP_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPI_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPB_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPT_cmp_stdAlloc )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_SHB_cmp_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_SHT_cmp_stdAlloc )
#endif

    CDSSTRESS_MichaelSet( MichaelSet_Lazy_HP_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_DHP_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPI_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPB_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPT_less_stdAlloc )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_SHB_less_stdAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_SHT_less_stdAlloc )
#endif

    CDSSTRESS_MichaelSet( MichaelSet_Lazy_HP_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_DHP_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPI_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPB_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPT_cmp_michaelAlloc )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_SHB_cmp_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_SHT_cmp_michaelAlloc )
#endif

    CDSSTRESS_MichaelSet( MichaelSet_Lazy_HP_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_DHP_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPI_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPB_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_GPT_less_michaelAlloc )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_SHB_less_michaelAlloc )
    CDSSTRESS_MichaelSet( MichaelSet_Lazy_RCU_SHT_less_michaelAlloc )
#endif
} // namespace set

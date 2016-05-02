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
#include "set_type_ellen_bintree.h"

#define CDSSTRESS_EllenBinTreeSet( ellen_set_type ) \
    TEST_F( Set_DelOdd, ellen_set_type ) \
    { \
        typedef set::set_type< tag_EllenBinTreeSet, key_thread, size_t >::ellen_set_type set_type; \
        run_test_extract<set_type>(); \
    }

namespace set {

    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_hp )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_dhp )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_gpi )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_gpb )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_gpt )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_shb )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_sht )
#endif
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_yield_hp )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_yield_dhp )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_yield_rcu_gpb )

    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_hp_stat )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_dhp_stat )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_gpi_stat )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_gpb_stat )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_gpt_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_shb_stat )
    CDSSTRESS_EllenBinTreeSet( EllenBinTreeSet_rcu_sht_stat )
#endif

} // namespace set

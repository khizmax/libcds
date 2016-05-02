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
#include "set_type_cuckoo.h"

#define CDSSTRESS_CuckooSet( cuckoo_set_type ) \
    TEST_F( Set_DelOdd, cuckoo_set_type ) \
    { \
        typedef set::set_type< tag_CuckooSet, key_thread, size_t >::cuckoo_set_type set_type; \
        run_test<set_type>(); \
    }

namespace set {

    CDSSTRESS_CuckooSet( CuckooStripedSet_list_unord )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_list_unord )
    CDSSTRESS_CuckooSet( CuckooStripedSet_list_unord_stat )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_list_unord_stat )
    CDSSTRESS_CuckooSet( CuckooStripedSet_list_unord_storehash )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_list_unord_storehash )
    CDSSTRESS_CuckooSet( CuckooStripedSet_list_ord )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_list_ord )
    CDSSTRESS_CuckooSet( CuckooStripedSet_list_ord_stat )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_list_ord_stat )
    CDSSTRESS_CuckooSet( CuckooStripedSet_list_ord_storehash )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_list_ord_storehash )
    CDSSTRESS_CuckooSet( CuckooStripedSet_vector_unord )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_vector_unord )
    CDSSTRESS_CuckooSet( CuckooStripedSet_vector_unord_stat )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_vector_unord_stat )
    CDSSTRESS_CuckooSet( CuckooStripedSet_vector_unord_storehash )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_vector_unord_storehash )
    CDSSTRESS_CuckooSet( CuckooStripedSet_vector_ord )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_vector_ord )
    CDSSTRESS_CuckooSet( CuckooStripedSet_vector_ord_stat )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_vector_ord_stat )
    CDSSTRESS_CuckooSet( CuckooStripedSet_vector_ord_storehash )
    CDSSTRESS_CuckooSet( CuckooRefinableSet_vector_ord_storehash )
} // namespace set

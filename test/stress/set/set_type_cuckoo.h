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

#ifndef CDSUNIT_SET_TYPE_CUCKOO_H
#define CDSUNIT_SET_TYPE_CUCKOO_H

#include "set/set_type.h"

#include <cds/container/cuckoo_set.h>
#include <cds_test/stat_cuckoo_out.h>

namespace set {

    template <typename V, typename Traits>
    class CuckooSet : public cc::CuckooSet< V, Traits >
    {
    public:
        typedef cc::CuckooSet< V, Traits > cuckoo_base_class;

    public:
        template <typename Config>
        CuckooSet( Config const& cfg )
            : cuckoo_base_class(
                cfg.s_nCuckooInitialSize,
                static_cast<unsigned int>( cfg.s_nCuckooProbesetSize ),
                static_cast<unsigned int>( cfg.s_nCuckooProbesetThreshold )
            )
        {}

        template <typename Q, typename Pred>
        bool erase_with( Q const& key, Pred /*pred*/ )
        {
            return cuckoo_base_class::erase_with( key, typename std::conditional< cuckoo_base_class::c_isSorted, Pred, typename Pred::equal_to>::type() );
        }

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = false;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
        static CDS_CONSTEXPR bool const c_bEraseExactKey = false;
    };

    struct tag_CuckooSet;

    template <typename Key, typename Val>
    struct set_type< tag_CuckooSet, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::compare compare;
        typedef typename base_class::equal_to equal_to;
        typedef typename base_class::less less;
        typedef typename base_class::hash hash;
        typedef typename base_class::hash2 hash2;

        // ***************************************************************************
        // CuckooSet

        template <typename Traits>
        struct traits_CuckooStripedSet : public Traits
        {
            typedef cc::cuckoo::striping<> mutex_policy;
        };
        template <typename Traits>
        struct traits_CuckooRefinableSet : public Traits
        {
            typedef cc::cuckoo::refinable<> mutex_policy;
        };

        struct traits_CuckooSet_list_unord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::list >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_unord>> CuckooStripedSet_list_unord;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_unord>> CuckooRefinableSet_list_unord;

        struct traits_CuckooSet_list_unord_stat : public traits_CuckooSet_list_unord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_unord_stat>> CuckooStripedSet_list_unord_stat;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_unord_stat>> CuckooRefinableSet_list_unord_stat;

        struct traits_CuckooSet_list_unord_storehash : public traits_CuckooSet_list_unord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_unord_storehash>> CuckooStripedSet_list_unord_storehash;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_unord_storehash>> CuckooRefinableSet_list_unord_storehash;

        struct traits_CuckooSet_list_ord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::list >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_ord>> CuckooStripedSet_list_ord;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_ord>> CuckooRefinableSet_list_ord;

        struct traits_CuckooSet_list_ord_stat : public traits_CuckooSet_list_ord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_ord_stat>> CuckooStripedSet_list_ord_stat;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_ord_stat>> CuckooRefinableSet_list_ord_stat;

        struct traits_CuckooSet_list_ord_storehash : public traits_CuckooSet_list_ord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_ord_storehash>> CuckooStripedSet_list_ord_storehash;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_ord_storehash>> CuckooRefinableSet_list_ord_storehash;


        struct traits_CuckooSet_vector_unord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_unord>> CuckooStripedSet_vector_unord;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_unord>> CuckooRefinableSet_vector_unord;

        struct traits_CuckooSet_vector_unord_stat : public traits_CuckooSet_vector_unord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_unord_stat>> CuckooStripedSet_vector_unord_stat;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_unord_stat>> CuckooRefinableSet_vector_unord_stat;

        struct traits_CuckooSet_vector_unord_storehash : public traits_CuckooSet_vector_unord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_unord_storehash>> CuckooStripedSet_vector_unord_storehash;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_unord_storehash>> CuckooRefinableSet_vector_unord_storehash;

        struct traits_CuckooSet_vector_ord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_ord>> CuckooStripedSet_vector_ord;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_ord>> CuckooRefinableSet_vector_ord;

        struct traits_CuckooSet_vector_ord_stat : public traits_CuckooSet_vector_ord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_ord_stat>> CuckooStripedSet_vector_ord_stat;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_ord_stat>> CuckooRefinableSet_vector_ord_stat;

        struct traits_CuckooSet_vector_ord_storehash : public traits_CuckooSet_vector_ord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_ord_storehash>> CuckooStripedSet_vector_ord_storehash;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_ord_storehash>> CuckooRefinableSet_vector_ord_storehash;

    };

    template <typename T, typename Traits >
    static inline void print_stat( cds_test::property_stream& o, cc::CuckooSet< T, Traits > const& s )
    {
        o << s.statistics() << s.mutex_policy_statistics();
    }

    template <typename V, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, CuckooSet< V, Traits > const& s )
    {
        typedef CuckooSet< V, Traits > set_type;
        print_stat( o, static_cast<typename set_type::cuckoo_base_class const&>(s) );
    }

} // namespace set

#define CDSSTRESS_CuckooSet_case( fixture, test_case, cuckoo_set_type, key_type, value_type ) \
    TEST_F( fixture, cuckoo_set_type ) \
    { \
        typedef set::set_type< tag_CuckooSet, key_type, value_type >::cuckoo_set_type set_type; \
        test_case<set_type>(); \
    }

#define CDSSTRESS_CuckooSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_list_unord,                  key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_list_unord,                key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_list_unord_stat,             key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_list_unord_stat,           key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_list_unord_storehash,        key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_list_unord_storehash,      key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_list_ord,                    key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_list_ord,                  key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_list_ord_stat,               key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_list_ord_stat,             key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_list_ord_storehash,          key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_list_ord_storehash,        key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_vector_unord,                key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_vector_unord,              key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_vector_unord_stat,           key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_vector_unord_stat,         key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_vector_unord_storehash,      key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_vector_unord_storehash,    key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_vector_ord,                  key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_vector_ord,                key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_vector_ord_stat,             key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_vector_ord_stat,           key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooStripedSet_vector_ord_storehash,        key_type, value_type ) \
    CDSSTRESS_CuckooSet_case( fixture, test_case, CuckooRefinableSet_vector_ord_storehash,      key_type, value_type ) \


#endif // #ifndef CDSUNIT_SET_TYPE_CUCKOO_H

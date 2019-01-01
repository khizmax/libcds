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

#ifndef CDSUNIT_MAP_TYPE_HOPSCOTCH_H
#define CDSUNIT_MAP_TYPE_HOPSCOTCH_H

#include "map_type.h"
#include <test/unit/striped-map/test_map.h>
#include <cds/container/details/hopscotch_hashmap_base.h>
#include <cds/container/hopscotch_hashmap.h>
#include <cds_test/stat_hopscotch_out.h>
#include <cds_test/hash_func.h>

namespace map {

    template <typename K, typename V>
    class HopscotchHashmap: public cc::hopscotch_hashmap< K, V >
    {
    public:
        typedef cc::hopscotch_hashmap< K, V > base_class;

    public:
        template <typename Config>
        HopscotchHashmap( Config const& cfg )
            : base_class(
                cfg.s_nHopscotchInitialSize,
                static_cast<unsigned int>( cfg.s_nHopscotchProbesetSize ),
                static_cast<unsigned int>( cfg.s_nHopscotchProbesetThreshold )
            )
        {}

        template <typename Q, typename Pred>
        bool erase_with( Q const& key, Pred  )
        {
            return base_class::erase_with( key, typename std::conditional< base_class::c_isSorted, Pred, typename Pred::equal_to>::type());
        }

        // for testing
        static constexpr bool const c_bExtractSupported = false;
        static constexpr bool const c_bLoadFactorDepended = false;
        static constexpr bool const c_bEraseExactKey = false;
    };

    struct tag_HopscotchHashmap;

    template <typename Key, typename Value>
    struct map_type< tag_HopscotchHashmap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::key_compare compare;
        typedef typename base_class::key_less    less;
        typedef typename base_class::equal_to    equal_to;
        typedef typename base_class::key_hash    hash;
        typedef typename base_class::hash2       hash2;

        template <typename Traits>
        struct traits_HopscotchStripedMap: public Traits
        {
            typedef cc::hopscotch_hashmap_ns::striping<> mutex_policy;
        };
        template <typename Traits>
        struct traits_HopscotchRefinableMap : public Traits
        {
            typedef cc::hopscotch_hashmap_ns::refinable<> mutex_policy;
        };

        struct traits_HopscotchHashmap_list_unord :
            public cc::hopscotch_hashmap_ns::make_traits <
                cc::hopscotch_hashmap_ns::probeset_type< cc::hopscotch_hashmap_ns::list >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_list_unord;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_list_unord;

        struct traits_HopscotchHashmap_list_unord_stat : public traits_HopscotchHashmap_list_unord
        {
            typedef cc::hopscotch_hashmap_ns::stat stat;
        };
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_list_unord_stat;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_list_unord_stat;

        struct traits_HopscotchHashmap_list_unord_storehash : public traits_HopscotchHashmap_list_unord
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_list_unord_storehash;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_list_unord_storehash;

        struct traits_HopscotchHashmap_list_ord :
            public cc::hopscotch_hashmap_ns::make_traits <
                cc::hopscotch_hashmap_ns::probeset_type< cc::hopscotch_hashmap_ns::list >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_list_ord;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_list_ord;

        struct traits_HopscotchHashmap_list_ord_stat : public traits_HopscotchHashmap_list_ord
        {
            typedef cc::hopscotch_hashmap_ns::stat stat;
        };
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_list_ord_stat;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_list_ord_stat;

        struct traits_HopscotchHashmap_list_ord_storehash : public traits_HopscotchHashmap_list_ord
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_list_ord_storehash;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_list_ord_storehash;

        struct traits_HopscotchHashmap_vector_unord :
            public cc::hopscotch_hashmap_ns::make_traits <
                cc::hopscotch_hashmap_ns::probeset_type< cc::hopscotch_hashmap_ns::vector<4> >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_vector_unord;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_vector_unord;

        struct traits_HopscotchHashmap_vector_unord_stat : public traits_HopscotchHashmap_vector_unord
        {
            typedef cc::hopscotch_hashmap_ns::stat stat;
        };
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_vector_unord_stat;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_vector_unord_stat;

        struct traits_HopscotchHashmap_vector_unord_storehash : public traits_HopscotchHashmap_vector_unord
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_vector_unord_storehash;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_vector_unord_storehash;

        struct traits_HopscotchHashmap_vector_ord :
            public cc::hopscotch_hashmap_ns::make_traits <
                cc::hopscotch_hashmap_ns::probeset_type< cc::hopscotch_hashmap_ns::vector<4> >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_vector_ord;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_vector_ord;

        struct traits_HopscotchHashmap_vector_ord_stat : public traits_HopscotchHashmap_vector_ord
        {
            typedef cc::hopscotch_hashmap_ns::stat stat;
        };
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_vector_ord_stat;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_vector_ord_stat;

        struct traits_HopscotchHashmap_vector_ord_storehash : public traits_HopscotchHashmap_vector_ord
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashmap< Key, Value > HopscotchStripedMap_vector_ord_storehash;
        typedef HopscotchHashmap< Key, Value > HopscotchRefinableMap_vector_ord_storehash;

#if CDS_BUILD_BITS == 64

        struct traits_HopscotchHashmap_list_unord_city64:
            public cc::hopscotch_hashmap::make_traits <
                cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< cds_test::city64, hash2 > >
            > ::type
        {};
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_list_unord_city64>> HopscotchStripedMap_list_unord_city64;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_list_unord_city64>> HopscotchRefinableMap_list_unord_city64;

        struct traits_HopscotchHashmap_list_unord_city64_stat: public traits_HopscotchHashmap_list_unord_city64
        {
            typedef cc::hopscotch_hashmap::stat stat;
        };
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_list_unord_city64_stat>> HopscotchStripedMap_list_unord_city64_stat;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_list_unord_city64_stat>> HopscotchRefinableMap_list_unord_city64_stat;

        struct traits_HopscotchHashmap_list_unord_city64_storehash: public traits_HopscotchHashmap_list_unord_city64
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_list_unord_city64_storehash>> HopscotchStripedMap_list_unord_city64_storehash;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_list_unord_city64_storehash>> HopscotchRefinableMap_list_unord_city64_storehash;

        struct traits_HopscotchHashmap_list_ord_city64:
            public cc::hopscotch_hashmap::make_traits <
            cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
            , co::compare< compare >
            , co::hash< std::tuple< cds_test::city64, hash2 > >
            >::type
        {};
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_list_ord_city64>> HopscotchStripedMap_list_ord_city64;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_list_ord_city64>> HopscotchRefinableMap_list_ord_city64;

        struct traits_HopscotchHashmap_list_ord_city64_stat: public traits_HopscotchHashmap_list_ord_city64
        {
            typedef cc::hopscotch_hashmap::stat stat;
        };
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_list_ord_city64_stat>> HopscotchStripedMap_list_ord_city64_stat;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_list_ord_city64_stat>> HopscotchRefinableMap_list_ord_city64_stat;

        struct traits_HopscotchHashmap_list_ord_city64_storehash: public traits_HopscotchHashmap_list_ord_city64
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_list_ord_city64_storehash>> HopscotchStripedMap_list_ord_city64_storehash;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_list_ord_city64_storehash>> HopscotchRefinableMap_list_ord_city64_storehash;

        struct traits_HopscotchHashmap_vector_unord_city64:
            public cc::hopscotch_hashmap::make_traits <
            cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<4> >
            , co::equal_to< equal_to >
            , co::hash< std::tuple< cds_test::city64, hash2 > >
            >::type
        {};
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_vector_unord_city64>> HopscotchStripedMap_vector_unord_city64;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_vector_unord_city64>> HopscotchRefinableMap_vector_unord_city64;

        struct traits_HopscotchHashmap_vector_unord_city64_stat: public traits_HopscotchHashmap_vector_unord_city64
        {
            typedef cc::hopscotch_hashmap::stat stat;
        };
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_vector_unord_city64_stat>> HopscotchStripedMap_vector_unord_city64_stat;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_vector_unord_city64_stat>> HopscotchRefinableMap_vector_unord_city64_stat;

        struct traits_HopscotchHashmap_vector_unord_city64_storehash: public traits_HopscotchHashmap_vector_unord_city64
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_vector_unord_city64_storehash>> HopscotchStripedMap_vector_unord_city64_storehash;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_vector_unord_city64_storehash>> HopscotchRefinableMap_vector_unord_city64_storehash;

        struct traits_HopscotchHashmap_vector_ord_city64:
            public cc::hopscotch_hashmap::make_traits <
            cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<4> >
            , co::compare< compare >
            , co::hash< std::tuple< cds_test::city64, hash2 > >
            >::type
        {};
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_vector_ord_city64>> HopscotchStripedMap_vector_ord_city64;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_vector_ord_city64>> HopscotchRefinableMap_vector_ord_city64;

        struct traits_HopscotchHashmap_vector_ord_city64_stat: public traits_HopscotchHashmap_vector_ord_city64
        {
            typedef cc::hopscotch_hashmap::stat stat;
        };
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_vector_ord_city64_stat>> HopscotchStripedMap_vector_ord_city64_stat;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_vector_ord_city64_stat>> HopscotchRefinableMap_vector_ord_city64_stat;

        struct traits_HopscotchHashmap_vector_ord_city64_storehash: public traits_HopscotchHashmap_vector_ord_city64
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashmap< Key, Value, traits_HopscotchStripedMap<traits_HopscotchHashmap_vector_ord_city64_storehash>> HopscotchStripedMap_vector_ord_city64_storehash;
        typedef HopscotchHashmap< Key, Value, traits_HopscotchRefinableMap<traits_HopscotchHashmap_vector_ord_city64_storehash>> HopscotchRefinableMap_vector_ord_city64_storehash;
#endif // CDS_BUILD_BITS == 64
    };

    template <typename Key, typename T, typename Traits >
    static inline void print_stat( cds_test::property_stream& o, cc::hopscotch_hashmap< Key, T > const& s )
    {
        o << s.statistics() << s.mutex_policy_statistics();
    }

    template <typename Key, typename V, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, HopscotchHashmap< Key, V > const& s )
    {
        typedef HopscotchHashmap< Key, V, Traits > map_type;
        print_stat( o, static_cast<typename map_type::base_class const&>(s));
    }

}   // namespace map


#define CDSSTRESS_HopscotchHashmap_case( fixture, test_case, hopscotch_hashmap_type, key_type, value_type ) \
    TEST_F( fixture, hopscotch_map_type ) \
    { \
        typedef map::map_type< tag_HopscotchHashmap, key_type, value_type >::hopscotch_hashmap_type map_type; \
        test_case<map_type>(); \
    }

#define CDSSTRESS_HopscotchHashmap( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_unord,              key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_unord,            key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_unord_stat,         key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_unord_stat,       key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_unord_storehash,    key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_unord_storehash,  key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_ord,                key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_ord,              key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_ord_stat,           key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_ord_stat,         key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_ord_storehash,      key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_ord_storehash,    key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_unord,            key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_unord,          key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_unord_stat,       key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_unord_stat,     key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_unord_storehash,  key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_unord_storehash, key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_ord,              key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_ord,            key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_ord_stat,         key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_ord_stat,       key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_ord_storehash,    key_type, value_type ) \
    CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_ord_storehash,  key_type, value_type )

#if CDS_BUILD_BITS == 64
#   define CDSSTRESS_HopscotchHashmap_city64( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_unord_city64,              key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_unord_city64,            key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_unord_city64_stat,         key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_unord_city64_stat,       key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_unord_city64_storehash,    key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_unord_city64_storehash,  key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_ord_city64,                key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_ord_city64,              key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_ord_city64_stat,           key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_ord_city64_stat,         key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_list_ord_city64_storehash,      key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_list_ord_city64_storehash,    key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_unord_city64,            key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_unord_city64,          key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_unord_city64_stat,       key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_unord_city64_stat,     key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_unord_city64_storehash,  key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_unord_city64_storehash, key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_ord_city64,              key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_ord_city64,            key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_ord_city64_stat,         key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_ord_city64_stat,       key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchStripedMap_vector_ord_city64_storehash,    key_type, value_type ) \
        CDSSTRESS_HopscotchHashmap_case( fixture, test_case, HopscotchRefinableMap_vector_ord_city64_storehash,  key_type, value_type )
#endif
#endif // ifndef CDSUNIT_MAP_TYPE_HOPSCOTCH_H

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

#ifndef CDSUNIT_MAP_TYPE_STRIPED_H
#define CDSUNIT_MAP_TYPE_STRIPED_H

#include "map_type.h"
#include <cds/container/striped_map/std_list.h>
#include <cds/container/striped_map/std_map.h>
#include <cds/container/striped_map/std_hash_map.h>

#include <boost/version.hpp>
#if BOOST_VERSION >= 104800
#   include <cds/container/striped_map/boost_list.h>
#   include <cds/container/striped_map/boost_slist.h>
#   include <cds/container/striped_map/boost_map.h>
#   include <cds/container/striped_map/boost_flat_map.h>
#endif
#include <cds/container/striped_map/boost_unordered_map.h>
#include <cds/container/striped_map.h>

namespace map {

    struct tag_StripedMap;

    template <typename Key, typename Value>
    struct map_type< tag_StripedMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value >      base_class;
        typedef typename base_class::key_compare compare;
        typedef typename base_class::key_less    less;
        typedef typename base_class::equal_to    equal_to;
        typedef typename base_class::key_hash    hash;
        typedef typename base_class::hash2       hash2;

        // for sequential containers
        template <class BucketEntry, typename... Options>
        class StripedHashMap_seq:
            public cc::StripedMap< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            >
        {
            typedef cc::StripedMap< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            StripedHashMap_seq( Config const& cfg )
                : base_class( cfg.s_nMapSize / cfg.s_nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( cfg.s_nLoadFactor )) )
            {}

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
        };

        // for non-sequential ordered containers
        template <class BucketEntry, typename... Options>
        class StripedHashMap_ord:
            public cc::StripedMap< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            >
        {
            typedef cc::StripedMap< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            StripedHashMap_ord( Config const& cfg )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( cfg.s_nMaxLoadFactor * 1024 )) )
            {}

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
        };


        typedef StripedHashMap_seq<
            std::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > StripedMap_list;

        typedef StripedHashMap_ord<
            std::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > StripedMap_hashmap;

        typedef StripedHashMap_ord<
            std::map< Key, Value, less >
            , co::hash< hash2 >
        > StripedMap_map;

        typedef StripedHashMap_ord<
            boost::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > StripedMap_boost_unordered_map;

#   if BOOST_VERSION >= 104800
        typedef StripedHashMap_seq<
            boost::container::slist< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > StripedMap_slist;

        typedef StripedHashMap_seq<
            boost::container::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > StripedMap_boost_list;

        typedef StripedHashMap_ord<
            boost::container::map< Key, Value, less >
            , co::hash< hash2 >
        > StripedMap_boost_map;

        typedef StripedHashMap_ord<
            boost::container::flat_map< Key, Value, less >
            , co::hash< hash2 >
        > StripedMap_boost_flat_map;
#   endif  // BOOST_VERSION >= 104800


        // ***************************************************************************
        // RefinableHashMap

        // for sequential containers
        template <class BucketEntry, typename... Options>
        class RefinableHashMap_seq:
            public cc::StripedMap< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            >
        {
            typedef cc::StripedMap< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            RefinableHashMap_seq( Config const& cfg )
                : base_class( cfg.s_nMapSize / cfg.s_nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( cfg.s_nLoadFactor )))
            {}

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
        };

        // for non-sequential ordered containers
        template <class BucketEntry, typename... Options>
        class RefinableHashMap_ord:
            public cc::StripedMap< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            >
        {
            typedef cc::StripedMap< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            RefinableHashMap_ord( Config const& cfg )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( cfg.s_nMaxLoadFactor * 1024 )) )
            {}

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
        };


        typedef RefinableHashMap_seq<
            std::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableMap_list;

#   if BOOST_VERSION >= 104800
        typedef RefinableHashMap_seq<
            boost::container::slist< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableMap_slist;
#   endif

        typedef RefinableHashMap_ord<
            std::map< Key, Value, less >
            , co::hash< hash2 >
        > RefinableMap_map;

        typedef RefinableHashMap_ord<
            std::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > RefinableMap_hashmap;

        typedef RefinableHashMap_ord<
            boost::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > RefinableMap_boost_unordered_map;

#   if BOOST_VERSION >= 104800
        typedef RefinableHashMap_seq<
            boost::container::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableMap_boost_list;

        typedef RefinableHashMap_ord<
            boost::container::map< Key, Value, less >
            , co::hash< hash2 >
        > RefinableMap_boost_map;

        typedef RefinableHashMap_ord<
            boost::container::flat_map< Key, Value, less >
            , co::hash< hash2 >
        > RefinableMap_boost_flat_map;
#   endif // #if BOOST_VERSION >= 104800

    };
}   // namespace map

#define CDSSTRESS_StripedMap_case( fixture, test_case, striped_map_type, key_type, value_type ) \
    TEST_P( fixture, striped_map_type ) \
    { \
        typedef map::map_type< tag_StripedMap, key_type, value_type >::striped_map_type map_type; \
        test_case<map_type>(); \
    }

#define CDSSTRESS_StripedMap( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_StripedMap_case( fixture, test_case, StripedMap_list,         key_type, value_type ) \
    CDSSTRESS_StripedMap_case( fixture, test_case, StripedMap_hashmap,      key_type, value_type ) \
    CDSSTRESS_StripedMap_case( fixture, test_case, StripedMap_map,          key_type, value_type ) \
    CDSSTRESS_StripedMap_case( fixture, test_case, RefinableMap_list,       key_type, value_type ) \
    CDSSTRESS_StripedMap_case( fixture, test_case, RefinableMap_map,        key_type, value_type ) \
    CDSSTRESS_StripedMap_case( fixture, test_case, RefinableMap_hashmap,    key_type, value_type ) \

#endif // ifndef CDSUNIT_MAP_TYPE_STRIPED_H

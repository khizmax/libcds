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

#ifndef CDSUNIT_SET_TYPE_HOPSCOTCH_H
#define CDSUNIT_SET_TYPE_HOPSCOTCH_H

#include "set_type.h"
#include <cds/container/hopscotch_hashset.h>
#include <cds_test/stat_hopscotch_out.h>
#include <cds_test/hash_func.h>

namespace set {

    template <typename V, typename Traits>
    class HopscotchHashset: public cc::HopscotchHashset< V, Traits >
    {
    public:
        typedef cc::HopscotchHashset< V, Traits > base_class;

    public:
        template <typename Config>
        HopscotchHashset( Config const& cfg )
            : base_class()
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

    struct tag_HopscotchHashset;

    template <typename Key, typename Value>
    struct set_type< tag_HopscotchHashset, Key, Value >: public set_type_base< Key, Value >
    {
        typedef set_type_base< Key, Value > base_class;
		typedef typename base_class::key_val key_val;
		typedef typename base_class::compare compare;
		typedef typename base_class::equal_to equal_to;
		typedef typename base_class::less less;
		typedef typename base_class::hash hash;
		typedef typename base_class::hash2 hash2;

        template <typename Traits>
        struct traits_HopscotchStripedSet: public Traits
        {
            typedef cc::hopscotch_hashset::striping<> mutex_policy;
        };
        template <typename Traits>
        struct traits_HopscotchRefinableSet : public Traits
        {
            typedef cc::hopscotch_hashset::refinable<> mutex_policy;
        };

        struct traits_HopscotchHashset_list_unord :
            public cc::hopscotch_hashset::make_traits <
                cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_unord> > HopscotchStripedSet_list_unord;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_unord> > HopscotchRefinableSet_list_unord;

        struct traits_HopscotchHashset_list_unord_stat : public traits_HopscotchHashset_list_unord
        {
            typedef cc::hopscotch_hashset::stat stat;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_unord_stat> > HopscotchStripedSet_list_unord_stat;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_unord_stat> > HopscotchRefinableSet_list_unord_stat;

        struct traits_HopscotchHashset_list_unord_storehash : public traits_HopscotchHashset_list_unord
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_unord_storehash> > HopscotchStripedSet_list_unord_storehash;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_unord_storehash> > HopscotchRefinableSet_list_unord_storehash;

        struct traits_HopscotchHashset_list_ord :
            public cc::hopscotch_hashset::make_traits <
                cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_ord> > HopscotchStripedSet_list_ord;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_ord> > HopscotchRefinableSet_list_ord;

        struct traits_HopscotchHashset_list_ord_stat : public traits_HopscotchHashset_list_ord
        {
            typedef cc::hopscotch_hashset::stat stat;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_ord_stat> > HopscotchStripedSet_list_ord_stat;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_ord_stat> > HopscotchRefinableSet_list_ord_stat;

        struct traits_HopscotchHashset_list_ord_storehash : public traits_HopscotchHashset_list_ord
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_ord_storehash> > HopscotchStripedSet_list_ord_storehash;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_ord_storehash> > HopscotchRefinableSet_list_ord_storehash;

        struct traits_HopscotchHashset_vector_unord :
            public cc::hopscotch_hashset::make_traits <
                cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<4> >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_unord> > HopscotchStripedSet_vector_unord;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_unord> > HopscotchRefinableSet_vector_unord;

        struct traits_HopscotchHashset_vector_unord_stat : public traits_HopscotchHashset_vector_unord
        {
            typedef cc::hopscotch_hashset::stat stat;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_unord_stat> > HopscotchStripedSet_vector_unord_stat;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_unord_stat> > HopscotchRefinableSet_vector_unord_stat;

        struct traits_HopscotchHashset_vector_unord_storehash : public traits_HopscotchHashset_vector_unord
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_unord_storehash> > HopscotchStripedSet_vector_unord_storehash;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_unord_storehash> > HopscotchRefinableSet_vector_unord_storehash;

        struct traits_HopscotchHashset_vector_ord :
            public cc::hopscotch_hashset::make_traits <
                cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<4> >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_ord> > HopscotchStripedSet_vector_ord;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_ord> > HopscotchRefinableSet_vector_ord;

        struct traits_HopscotchHashset_vector_ord_stat : public traits_HopscotchHashset_vector_ord
        {
            typedef cc::hopscotch_hashset::stat stat;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_ord_stat> > HopscotchStripedSet_vector_ord_stat;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_ord_stat> > HopscotchRefinableSet_vector_ord_stat;

        struct traits_HopscotchHashset_vector_ord_storehash : public traits_HopscotchHashset_vector_ord
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_ord_storehash> > HopscotchStripedSet_vector_ord_storehash;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_ord_storehash> > HopscotchRefinableSet_vector_ord_storehash;

#if CDS_BUILD_BITS == 64

        struct traits_HopscotchHashset_list_unord_city64:
            public cc::hopscotch_hashset::make_traits <
                cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< cds_test::city64, hash2 > >
            > ::type
        {};
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_unord_city64>> HopscotchStripedSet_list_unord_city64;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_unord_city64>> HopscotchRefinableSet_list_unord_city64;

        struct traits_HopscotchHashset_list_unord_city64_stat: public traits_HopscotchHashset_list_unord_city64
        {
            typedef cc::hopscotch_hashset::stat stat;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_unord_city64_stat>> HopscotchStripedSet_list_unord_city64_stat;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_unord_city64_stat>> HopscotchRefinableSet_list_unord_city64_stat;

        struct traits_HopscotchHashset_list_unord_city64_storehash: public traits_HopscotchHashset_list_unord_city64
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_unord_city64_storehash>> HopscotchStripedSet_list_unord_city64_storehash;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_unord_city64_storehash>> HopscotchRefinableSet_list_unord_city64_storehash;

        struct traits_HopscotchHashset_list_ord_city64:
            public cc::hopscotch_hashset::make_traits <
            cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
            , co::compare< compare >
            , co::hash< std::tuple< cds_test::city64, hash2 > >
            >::type
        {};
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_ord_city64>> HopscotchStripedSet_list_ord_city64;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_ord_city64>> HopscotchRefinableSet_list_ord_city64;

        struct traits_HopscotchHashset_list_ord_city64_stat: public traits_HopscotchHashset_list_ord_city64
        {
            typedef cc::hopscotch_hashset::stat stat;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_ord_city64_stat>> HopscotchStripedSet_list_ord_city64_stat;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_ord_city64_stat>> HopscotchRefinableSet_list_ord_city64_stat;

        struct traits_HopscotchHashset_list_ord_city64_storehash: public traits_HopscotchHashset_list_ord_city64
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_list_ord_city64_storehash>> HopscotchStripedSet_list_ord_city64_storehash;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_list_ord_city64_storehash>> HopscotchRefinableSet_list_ord_city64_storehash;

        struct traits_HopscotchHashset_vector_unord_city64:
            public cc::hopscotch_hashset::make_traits <
            cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<4> >
            , co::equal_to< equal_to >
            , co::hash< std::tuple< cds_test::city64, hash2 > >
            >::type
        {};
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_unord_city64>> HopscotchStripedSet_vector_unord_city64;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_unord_city64>> HopscotchRefinableSet_vector_unord_city64;

        struct traits_HopscotchHashset_vector_unord_city64_stat: public traits_HopscotchHashset_vector_unord_city64
        {
            typedef cc::hopscotch_hashset::stat stat;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_unord_city64_stat>> HopscotchStripedSet_vector_unord_city64_stat;
        typedef HopscotchHashset< Key, Value, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_unord_city64_stat>> HopscotchRefinableSet_vector_unord_city64_stat;

        struct traits_HopscotchHashset_vector_unord_city64_storehash: public traits_HopscotchHashset_vector_unord_city64
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_unord_city64_storehash>> HopscotchStripedSet_vector_unord_city64_storehash;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_unord_city64_storehash>> HopscotchRefinableSet_vector_unord_city64_storehash;

        struct traits_HopscotchHashset_vector_ord_city64:
            public cc::hopscotch_hashset::make_traits <
            cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<4> >
            , co::compare< compare >
            , co::hash< std::tuple< cds_test::city64, hash2 > >
            >::type
        {};
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_ord_city64>> HopscotchStripedSet_vector_ord_city64;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_ord_city64>> HopscotchRefinableSet_vector_ord_city64;

        struct traits_HopscotchHashset_vector_ord_city64_stat: public traits_HopscotchHashset_vector_ord_city64
        {
            typedef cc::hopscotch_hashset::stat stat;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_ord_city64_stat>> HopscotchStripedSet_vector_ord_city64_stat;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_ord_city64_stat>> HopscotchRefinableSet_vector_ord_city64_stat;

        struct traits_HopscotchHashset_vector_ord_city64_storehash: public traits_HopscotchHashset_vector_ord_city64
        {
            static constexpr const bool store_hash = true;
        };
        typedef HopscotchHashset< key_val, traits_HopscotchStripedSet<traits_HopscotchHashset_vector_ord_city64_storehash>> HopscotchStripedSet_vector_ord_city64_storehash;
        typedef HopscotchHashset< key_val, traits_HopscotchRefinableSet<traits_HopscotchHashset_vector_ord_city64_storehash>> HopscotchRefinableSet_vector_ord_city64_storehash;
#endif // CDS_BUILD_BITS == 64
    };

    template <typename T, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, cc::HopscotchHashset< T, Traits > const& s )
    {
        o << s.statistics() << s.mutex_policy_statistics();
    }

    template <typename V, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, HopscotchHashset< V, Traits > const& s )
    {
        typedef HopscotchHashset< V, Traits > set_type;
        print_stat( o, static_cast<typename set_type::base_class const&>(s));
    }

}   // namespace set


#define CDSSTRESS_HopscotchHashset_case( fixture, test_case, hopscotch_hashset_type, key_type, value_type ) \
    TEST_F( fixture, hopscotch_hashset_type ) \
    { \
        typedef set::set_type< tag_HopscotchHashset, key_type, value_type >::hopscotch_hashset_type set_type; \
        test_case<set_type>(); \
    }

#define CDSSTRESS_HopscotchHashset( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_unord,              key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_unord,            key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_unord_stat,         key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_unord_stat,       key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_unord_storehash,    key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_unord_storehash,  key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_ord,                key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_ord,              key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_ord_stat,           key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_ord_stat,         key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_ord_storehash,      key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_ord_storehash,    key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_unord,            key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_unord,          key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_unord_stat,       key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_unord_stat,     key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_unord_storehash,  key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_unord_storehash, key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_ord,              key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_ord,            key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_ord_stat,         key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_ord_stat,       key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_ord_storehash,    key_type, value_type ) \
    CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_ord_storehash,  key_type, value_type )

#if CDS_BUILD_BITS == 64
#   define CDSSTRESS_HopscotchHashset_city64( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_unord_city64,              key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_unord_city64,            key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_unord_city64_stat,         key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_unord_city64_stat,       key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_unord_city64_storehash,    key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_unord_city64_storehash,  key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_ord_city64,                key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_ord_city64,              key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_ord_city64_stat,           key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_ord_city64_stat,         key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_list_ord_city64_storehash,      key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_list_ord_city64_storehash,    key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_unord_city64,            key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_unord_city64,          key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_unord_city64_stat,       key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_unord_city64_stat,     key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_unord_city64_storehash,  key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_unord_city64_storehash, key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_ord_city64,              key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_ord_city64,            key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_ord_city64_stat,         key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_ord_city64_stat,       key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchStripedSet_vector_ord_city64_storehash,    key_type, value_type ) \
        CDSSTRESS_HopscotchHashset_case( fixture, test_case, HopscotchRefinableSet_vector_ord_city64_storehash,  key_type, value_type )
#endif
#endif // ifndef CDSUNIT_SET_TYPE_HOPSCOTCH_H
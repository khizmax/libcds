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

#ifndef CDSUNIT_SET_TYPE_STRIPED_H
#define CDSUNIT_SET_TYPE_STRIPED_H

#include "set_type.h"

#include <cds/container/striped_set/std_list.h>
#include <cds/container/striped_set/std_vector.h>
#include <cds/container/striped_set/std_set.h>
#include <cds/container/striped_set/std_hash_set.h>
#include <cds/container/striped_set/boost_unordered_set.h>

#include <boost/version.hpp>
#if BOOST_VERSION >= 104800
#   include <cds/container/striped_set/boost_slist.h>
#   include <cds/container/striped_set/boost_list.h>
#   include <cds/container/striped_set/boost_vector.h>
#   include <cds/container/striped_set/boost_stable_vector.h>
#   include <cds/container/striped_set/boost_set.h>
#   include <cds/container/striped_set/boost_flat_set.h>
#endif
#include <cds/container/striped_set.h>

namespace set {

    struct tag_StripedSet;

    template <typename Key, typename Val>
    struct set_type< tag_StripedSet, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::compare compare;
        typedef typename base_class::less less;
        typedef typename base_class::equal_to equal_to;
        typedef typename base_class::hash hash;
        typedef typename base_class::hash2 hash2;


        // ***************************************************************************
        // StripedSet

        // for sequential containers
        template <class BucketEntry, typename... Options>
        class StripedHashSet_seq:
            public cc::StripedSet< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            StripedHashSet_seq( Config const& cfg )
                : base_class( cfg.s_nSetSize / cfg.s_nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( cfg.s_nLoadFactor )))
            {}

            empty_stat statistics() const
            {
                return empty_stat();
            }

            /*
            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less pred )
            {
                return base_class::erase( v );
            }
            */

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
            static CDS_CONSTEXPR bool const c_bEraseExactKey = false;
        };

        template <class BucketEntry, typename... Options>
        class StripedHashSet_seq_rational:
            public cc::StripedSet< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::rational_load_factor_resizing<0> >
                , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::rational_load_factor_resizing<0> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            StripedHashSet_seq_rational( Config const& cfg ) // LoadFactor = 1 / nDenominator
                : base_class( cfg.s_nSetSize / cfg.s_nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( 1, cfg.s_nLoadFactor )))
            {}

            empty_stat statistics() const
            {
                return empty_stat();
            }

            /*
            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less pred )
            {
                return base_class::erase( v );
            }
            */

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
            static CDS_CONSTEXPR bool const c_bEraseExactKey = false;
        };

        // for non-sequential ordered containers
        template <class BucketEntry, typename... Options>
        class StripedHashSet_ord:
            public cc::StripedSet< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            StripedHashSet_ord( Config const& cfg )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( cfg.s_nMaxLoadFactor * 1024 )))
            {}

            empty_stat statistics() const
            {
                return empty_stat();
            }

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
            static CDS_CONSTEXPR bool const c_bEraseExactKey = false;
        };

        template <class BucketEntry, typename... Options>
        class StripedHashSet_ord_rational:
            public cc::StripedSet< BucketEntry,
                co::resizing_policy<cc::striped_set::rational_load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
               co::resizing_policy<cc::striped_set::rational_load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            StripedHashSet_ord_rational( Config const& cfg ) // LoadFactor = 1 / nDenominator
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( 1024, cfg.s_nLoadFactor )))
            {}

            empty_stat statistics() const
            {
                return empty_stat();
            }

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
            static CDS_CONSTEXPR bool const c_bEraseExactKey = false;
        };

        typedef StripedHashSet_seq<
            std::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_list;

        typedef StripedHashSet_seq_rational<
            std::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_rational_list;

        typedef StripedHashSet_seq<
            std::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_vector;

        typedef StripedHashSet_seq_rational<
            std::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_rational_vector;

#if BOOST_VERSION >= 104800
        typedef StripedHashSet_seq<
            boost::container::slist< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_boost_slist;

        typedef StripedHashSet_seq_rational<
            boost::container::slist< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_rational_boost_slist;

        typedef StripedHashSet_seq<
            boost::container::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_boost_list;

        typedef StripedHashSet_seq_rational<
            boost::container::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_rational_boost_list;

        typedef StripedHashSet_seq<
            boost::container::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_boost_vector;

        typedef StripedHashSet_seq_rational<
            boost::container::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_rational_boost_vector;

        typedef StripedHashSet_seq<
            boost::container::stable_vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_boost_stable_vector;

        typedef StripedHashSet_seq_rational<
            boost::container::stable_vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_rational_boost_stable_vector;
#endif

        typedef StripedHashSet_ord<
            std::set< key_val, less >
            , co::hash< hash2 >
        > StripedSet_set;

        typedef StripedHashSet_ord_rational<
            std::set< key_val, less >
            , co::hash< hash2 >
        > StripedSet_rational_set;

        typedef StripedHashSet_ord<
            std::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > StripedSet_hashset;

        typedef StripedHashSet_ord_rational<
            std::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > StripedSet_rational_hashset;

#if BOOST_VERSION >= 104800
        typedef StripedHashSet_ord<
            boost::container::set< key_val, less >
            , co::hash< hash2 >
        > StripedSet_boost_set;

        typedef StripedHashSet_ord_rational<
            boost::container::set< key_val, less >
            , co::hash< hash2 >
        > StripedSet_rational_boost_set;

        typedef StripedHashSet_ord<
            boost::container::flat_set< key_val, less >
            , co::hash< hash2 >
        > StripedSet_boost_flat_set;

        typedef StripedHashSet_ord_rational<
            boost::container::flat_set< key_val, less >
            , co::hash< hash2 >
        > StripedSet_rational_boost_flat_set;
#endif

        typedef StripedHashSet_ord<
            boost::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > StripedSet_boost_unordered_set;

        typedef StripedHashSet_ord_rational<
            boost::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > StripedSet_rational_boost_unordered_set;


        // ***************************************************************************
        // RefinableSet

        // for sequential containers
        template <class BucketEntry, typename... Options>
        class RefinableHashSet_seq:
            public cc::StripedSet< BucketEntry,
            co::mutex_policy< cc::striped_set::refinable<> >
            ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
            , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
                co::mutex_policy< cc::striped_set::refinable<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            RefinableHashSet_seq( Config const& cfg )
                : base_class( cfg.s_nSetSize / cfg.s_nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( cfg.s_nLoadFactor )))
            {}

            empty_stat statistics() const
            {
                return empty_stat();
            }

            /*
            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less pred )
            {
                return base_class::erase( v );
            }
            */

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
        };

        template <class BucketEntry, typename... Options>
        class RefinableHashSet_seq_rational:
            public cc::StripedSet< BucketEntry,
            co::mutex_policy< cc::striped_set::refinable<> >
            ,co::resizing_policy<cc::striped_set::rational_load_factor_resizing<0> >
            , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
                co::mutex_policy< cc::striped_set::refinable<> >
                ,co::resizing_policy<cc::striped_set::rational_load_factor_resizing<0> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            RefinableHashSet_seq_rational( Config const& cfg ) // LoadFactor = 1 / nDenominator
                : base_class( cfg.s_nSetSize / cfg.s_nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( 1, cfg.s_nLoadFactor )))
            {}

            empty_stat statistics() const
            {
                return empty_stat();
            }

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
        };

        // for non-sequential ordered containers
        template <class BucketEntry, typename... Options>
        class RefinableHashSet_ord:
            public cc::StripedSet< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            RefinableHashSet_ord( Config const& cfg )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( cfg.s_nMaxLoadFactor * 1024 )))
            {}

            empty_stat statistics() const
            {
                return empty_stat();
            }

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
        };

        template <class BucketEntry, typename... Options>
        class RefinableHashSet_ord_rational:
            public cc::StripedSet< BucketEntry,
                co::resizing_policy<cc::striped_set::rational_load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
                co::resizing_policy<cc::striped_set::rational_load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            template <class Config>
            RefinableHashSet_ord_rational( Config const& cfg ) // LoadFactor = 1 / nDenominator
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( 1024, cfg.s_nLoadFactor )))
            {}

            empty_stat statistics() const
            {
                return empty_stat();
            }

            // for testing
            static CDS_CONSTEXPR bool const c_bExtractSupported = false;
            static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
        };

        typedef RefinableHashSet_seq<
            std::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_list;

        typedef RefinableHashSet_seq_rational<
            std::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_rational_list;

        typedef RefinableHashSet_seq<
            std::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_vector;

        typedef RefinableHashSet_seq_rational<
            std::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_rational_vector;

#if BOOST_VERSION >= 104800
        typedef RefinableHashSet_seq<
            boost::container::slist< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_boost_slist;

        typedef RefinableHashSet_seq_rational<
            boost::container::slist< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_rational_boost_slist;

        typedef RefinableHashSet_seq<
            boost::container::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_boost_list;

        typedef RefinableHashSet_seq_rational<
            boost::container::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_rational_boost_list;

        typedef RefinableHashSet_seq<
            boost::container::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_boost_vector;

        typedef RefinableHashSet_seq_rational<
            boost::container::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_rational_boost_vector;

        typedef RefinableHashSet_seq<
            boost::container::stable_vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_boost_stable_vector;

        typedef RefinableHashSet_seq_rational<
            boost::container::stable_vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_rational_boost_stable_vector;
#endif

        typedef RefinableHashSet_ord<
            std::set< key_val, less >
            , co::hash< hash2 >
        > RefinableSet_set;

        typedef RefinableHashSet_ord_rational<
            std::set< key_val, less >
            , co::hash< hash2 >
        > RefinableSet_rational_set;

        typedef RefinableHashSet_ord<
            std::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > RefinableSet_hashset;

        typedef RefinableHashSet_ord_rational<
            std::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > RefinableSet_rational_hashset;

#if BOOST_VERSION >= 104800
        typedef RefinableHashSet_ord<
            boost::container::set< key_val, less >
            , co::hash< hash2 >
        > RefinableSet_boost_set;

        typedef RefinableHashSet_ord_rational<
            boost::container::set< key_val, less >
            , co::hash< hash2 >
        > RefinableSet_rational_boost_set;

        typedef RefinableHashSet_ord<
            boost::container::flat_set< key_val, less >
            , co::hash< hash2 >
        > RefinableSet_boost_flat_set;

        typedef RefinableHashSet_ord_rational<
            boost::container::flat_set< key_val, less >
            , co::hash< hash2 >
        > RefinableSet_rational_boost_flat_set;
#endif

        typedef RefinableHashSet_ord<
            boost::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > RefinableSet_boost_unordered_set;

        typedef RefinableHashSet_ord_rational<
            boost::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > RefinableSet_rational_boost_unordered_set;
    };

} // namespace set

#define CDSSTRESS_StripedSet_case( fixture, test_case, striped_set_type, key_type, value_type ) \
    TEST_P( fixture, striped_set_type ) \
    { \
        typedef set::set_type< tag_StripedSet, key_type, value_type >::striped_set_type set_type; \
        test_case<set_type>(); \
    }

#define CDSSTRESS_StripedSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, StripedSet_list,                 key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, StripedSet_rational_list,        key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, StripedSet_vector,               key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, StripedSet_rational_vector,      key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, StripedSet_set,                  key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, StripedSet_rational_set,         key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, StripedSet_hashset,              key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, StripedSet_rational_hashset,     key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, RefinableSet_list,               key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, RefinableSet_rational_list,      key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, RefinableSet_vector,             key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, RefinableSet_rational_vector,    key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, RefinableSet_set,                key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, RefinableSet_rational_set,       key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, RefinableSet_hashset,            key_type, value_type ) \
    CDSSTRESS_StripedSet_case( fixture, test_case, RefinableSet_rational_hashset,   key_type, value_type )

#endif // #ifndef CDSUNIT_SET_TYPE_STRIPED_H

//$$CDS-header$$

#ifndef CDSUNIT_SET_TYPE_STRIPED_H
#define CDSUNIT_SET_TYPE_STRIPED_H

#include "set2/set_type.h"

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

namespace set2 {

    template <typename Key, typename Val>
    struct set_type< cc::striped_set::implementation_tag, Key, Val >: public set_type_base< Key, Val >
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
            StripedHashSet_seq( size_t nCapacity, size_t nLoadFactor )
                : base_class( nCapacity / nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
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
            StripedHashSet_seq_rational( size_t nCapacity, size_t nDenominator ) // LoadFactor = 1 / nDenominator
                : base_class( nCapacity * nDenominator / 16, *(new(&m_placeHolder) resizing_policy_t( 1, nDenominator )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
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
            StripedHashSet_ord( size_t /*nCapacity*/, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
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
            StripedHashSet_ord_rational( size_t /*nCapacity*/, size_t nDenominator ) // LoadFactor = 1 / nDenominator
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( 1024, nDenominator )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
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
            RefinableHashSet_seq( size_t nCapacity, size_t nLoadFactor )
                : base_class( nCapacity / nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
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
            RefinableHashSet_seq_rational( size_t nCapacity, size_t nDenominator ) // LoadFactor = 1 / nDenominator
                : base_class( nCapacity * nDenominator / 16, *(new(&m_placeHolder) resizing_policy_t( 1, nDenominator )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
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
            RefinableHashSet_ord( size_t /*nCapacity*/, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
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
            RefinableHashSet_ord_rational( size_t /*nCapacity*/, size_t nDenominator ) // LoadFactor = 1 / nDenominator
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( 1024, nDenominator )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
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

} // namespace set2

#endif // #ifndef CDSUNIT_SET_TYPE_STRIPED_H

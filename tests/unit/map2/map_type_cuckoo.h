//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_CUCKOO_H
#define CDSUNIT_MAP_TYPE_CUCKOO_H

#include "map2/map_type.h"

#include <cds/container/cuckoo_map.h>

#include "print_cuckoo_stat.h"

namespace map2 {

    template <typename K, typename V, typename Traits>
    class CuckooMap :
        public cc::CuckooMap< K, V, Traits >
    {
    public:
        typedef cc::CuckooMap< K, V, Traits > cuckoo_base_class;

    public:
        CuckooMap( size_t nCapacity, size_t nLoadFactor )
            : cuckoo_base_class( nCapacity / (nLoadFactor * 16), (unsigned int) 4 )
        {}

        template <typename Q, typename Pred>
        bool erase_with( Q const& key, Pred /*pred*/ )
        {
            return cuckoo_base_class::erase_with( key, typename std::conditional< cuckoo_base_class::c_isSorted, Pred, typename Pred::equal_to>::type() );
        }
    };

    template <typename Key, typename Value>
    struct map_type< cds::intrusive::cuckoo::implementation_tag, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;
        typedef typename base_class::equal_to   equal_to;
        typedef typename base_class::key_hash   key_hash;
        typedef typename base_class::hash       hash;
        typedef typename base_class::hash2      hash2;

        template <typename Traits>
        struct traits_CuckooStripedMap: public Traits
        {
            typedef cc::cuckoo::striping<> mutex_policy;
        };
        template <typename Traits>
        struct traits_CuckooRefinableMap : public Traits
        {
            typedef cc::cuckoo::refinable<> mutex_policy;
        };

        struct traits_CuckooMap_list_unord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::list >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_unord>> CuckooStripedMap_list_unord;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_unord>> CuckooRefinableMap_list_unord;

        struct traits_CuckooMap_list_unord_stat : public traits_CuckooMap_list_unord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_unord_stat>> CuckooStripedMap_list_unord_stat;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_unord_stat>> CuckooRefinableMap_list_unord_stat;

        struct traits_CuckooMap_list_unord_storehash : public traits_CuckooMap_list_unord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_unord_storehash>> CuckooStripedMap_list_unord_storehash;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_unord_storehash>> CuckooRefinableMap_list_unord_storehash;

        struct traits_CuckooMap_list_ord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::list >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_ord>> CuckooStripedMap_list_ord;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_ord>> CuckooRefinableMap_list_ord;

        struct traits_CuckooMap_list_ord_stat : public traits_CuckooMap_list_ord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_ord_stat>> CuckooStripedMap_list_ord_stat;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_ord_stat>> CuckooRefinableMap_list_ord_stat;

        struct traits_CuckooMap_list_ord_storehash : public traits_CuckooMap_list_ord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_ord_storehash>> CuckooStripedMap_list_ord_storehash;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_ord_storehash>> CuckooRefinableMap_list_ord_storehash;

        struct traits_CuckooMap_vector_unord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_unord>> CuckooStripedMap_vector_unord;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_unord>> CuckooRefinableMap_vector_unord;

        struct traits_CuckooMap_vector_unord_stat : public traits_CuckooMap_vector_unord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_unord_stat>> CuckooStripedMap_vector_unord_stat;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_unord_stat>> CuckooRefinableMap_vector_unord_stat;

        struct traits_CuckooMap_vector_unord_storehash : public traits_CuckooMap_vector_unord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_unord_storehash>> CuckooStripedMap_vector_unord_storehash;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_unord_storehash>> CuckooRefinableMap_vector_unord_storehash;

        struct traits_CuckooMap_vector_ord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_ord>> CuckooStripedMap_vector_ord;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_ord>> CuckooRefinableMap_vector_ord;

        struct traits_CuckooMap_vector_ord_stat : public traits_CuckooMap_vector_ord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_ord_stat>> CuckooStripedMap_vector_ord_stat;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_ord_stat>> CuckooRefinableMap_vector_ord_stat;

        struct traits_CuckooMap_vector_ord_storehash : public traits_CuckooMap_vector_ord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_ord_storehash>> CuckooStripedMap_vector_ord_storehash;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_ord_storehash>> CuckooRefinableMap_vector_ord_storehash;

    };

    template <typename K, typename T, typename Traits >
    static inline void print_stat( cc::CuckooMap< K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() << m.mutex_policy_statistics() );
    }

    template <typename K, typename V, typename Traits>
    static inline void print_stat( CuckooMap< K, V, Traits > const& m )
    {
        typedef CuckooMap< K, V, Traits > map_type;
        print_stat( static_cast<typename map_type::cuckoo_base_class const&>(m) );
    }

}   // namespace map2

#endif // ifndef CDSUNIT_MAP_TYPE_CUCKOO_H

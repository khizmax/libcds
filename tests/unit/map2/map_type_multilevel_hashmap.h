//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_MULTILEVEL_HASHMAP_H
#define CDSUNIT_MAP_TYPE_MULTILEVEL_HASHMAP_H

#include "map2/map_type.h"

#include <cds/container/multilevel_hashmap_hp.h>
#include <cds/container/multilevel_hashmap_dhp.h>

#include "print_multilevel_hashset_stat.h"

namespace map2 {

    template <class GC, typename Key, typename T, typename Traits = cc::multilevel_hashmap::traits>
    class MultiLevelHashMap : public cc::MultiLevelHashMap< GC, Key, T, Traits >
    {
        typedef cc::MultiLevelHashMap< GC, Key, T, Traits > base_class;
    public:
        template <typename Config>
        MultiLevelHashMap( Config const& cfg)
            : base_class( cfg.c_nMultiLevelMap_HeadBits, cfg.c_nMultiLevelMap_ArrayBits )
        {}

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = true;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
        static CDS_CONSTEXPR bool const c_erase_with_supported = false;
        static CDS_CONSTEXPR bool const c_extract_with_supported = false;
    };

    struct tag_MultiLevelHashMap;

    template <typename Key, typename Value>
    struct map_type< map2::tag_MultiLevelHashMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;

        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value >    MultiLevelHashMap_hp_stdhash;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value >    MultiLevelHashMap_dhp_stdhash;

        struct traits_MultiLevelHashMap_stat: public cc::multilevel_hashmap::make_traits<
                co::stat< cc::multilevel_hashmap::stat<>>
            >::type
        {};

        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_stat >    MultiLevelHashMap_hp_stdhash_stat;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_stat >    MultiLevelHashMap_dhp_stdhash_stat;
    };

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( cc::MultiLevelHashMap< GC, K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() );
    }

}   // namespace map2

#endif // #ifndef CDSUNIT_MAP_TYPE_MULTILEVEL_HASHMAP_H

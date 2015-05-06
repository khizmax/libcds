//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_STD_H
#define CDSUNIT_MAP_TYPE_STD_H

#include "map2/map_type.h"

#include "map2/std_map.h"
#include "map2/std_hash_map.h"

#include "lock/nolock.h"

namespace map2 {

    struct std_implementation_tag;

    template <typename Key, typename Value>
    struct map_type< std_implementation_tag, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;

        typedef StdMap< Key, Value, cds::sync::spin >     StdMap_Spin;
        typedef StdMap< Key, Value, lock::NoLock>         StdMap_NoLock;

        typedef StdHashMap< Key, Value, cds::sync::spin > StdHashMap_Spin;
        typedef StdHashMap< Key, Value, lock::NoLock >    StdHashMap_NoLock;
    };
}   // namespace map2

#endif // ifndef CDSUNIT_MAP_TYPE_STD_H

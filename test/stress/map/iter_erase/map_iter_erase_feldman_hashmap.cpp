// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_iter_erase.h"
#include "map_type_feldman_hashmap.h"

namespace map {

    template <class Map>
    void Map_Iter_Del3::run_feldman()
    {
        typedef typename Map::traits original_traits;
        struct traits: public original_traits {
            enum { hash_size = sizeof( uint32_t ) + sizeof( uint16_t ) };
        };
        typedef typename Map::template rebind_traits< traits >::result map_type;

        run_test_extract<map_type>();
    }

    template <class Map>
    void Map_Iter_Del3_reverse::run_feldman()
    {
        typedef typename Map::traits original_traits;
        struct traits: public original_traits {
            enum { hash_size = sizeof( uint32_t ) + sizeof( uint16_t ) };
        };
        typedef typename Map::template rebind_traits< traits >::result map_type;

        run_test_extract<map_type, typename map_type::reverse_iterator>();
    }

    CDSSTRESS_FeldmanHashMap_fixed_HP( Map_Iter_Del3, run_feldman, key_thread, size_t )
    CDSSTRESS_FeldmanHashMap_fixed_HP( Map_Iter_Del3_reverse, run_feldman, key_thread, size_t )

} // namespace map

// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "set_delodd.h"
#include "set_type_feldman_hashset.h"

namespace set {

    template <class Set>
    void Set_DelOdd::run_feldman()
    {
        typedef typename Set::traits original_traits;
        struct traits: public original_traits {
            enum { hash_size = sizeof( uint32_t ) + sizeof( uint16_t ) };
        };
        typedef typename Set::template rebind_traits< traits >::result set_type;

        run_test_extract<set_type>();
    }

    CDSSTRESS_FeldmanHashSet_fixed( Set_DelOdd, run_feldman, key_thread, size_t )

} // namespace set

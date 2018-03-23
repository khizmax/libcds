// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "set_iter_erase.h"
#include "set_type_feldman_hashset.h"

namespace set {

    template <class Set>
    void Set_Iter_Del3::run_feldman()
    {
        typedef typename Set::traits original_traits;
        struct traits: public original_traits {
            enum { hash_size = sizeof( uint32_t ) + sizeof( uint16_t ) };
        };
        typedef typename Set::template rebind_traits< traits >::result set_type;

        run_test_extract<set_type>();
    }

    template <class Set>
    void Set_Iter_Del3_reverse::run_feldman()
    {
        typedef typename Set::traits original_traits;
        struct traits: public original_traits {
            enum { hash_size = sizeof( uint32_t ) + sizeof( uint16_t ) };
        };
        typedef typename Set::template rebind_traits< traits >::result set_type;

        run_test_extract<set_type, typename set_type::reverse_iterator>();
    }

    //TODO: add erase_at() to FeldmanHashSet<RCU>
    CDSSTRESS_FeldmanHashSet_fixed_HP( Set_Iter_Del3, run_feldman, key_thread, size_t )
    CDSSTRESS_FeldmanHashSet_fixed_HP( Set_Iter_Del3_reverse, run_feldman, key_thread, size_t )

} // namespace set

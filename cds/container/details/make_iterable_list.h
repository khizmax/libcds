// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_ITERABLE_LIST_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_ITERABLE_LIST_H

#include <cds/intrusive/details/iterable_list_base.h>
#include <cds/details/binary_functor_wrapper.h>
#include <memory>

namespace cds { namespace container {

    //@cond
    namespace details {

        template <class GC, typename T, class Traits>
        struct make_iterable_list
        {
            typedef GC      gc;
            typedef T       value_type;

            typedef Traits original_traits;

            typedef typename std::allocator_traits<
                typename original_traits::allocator
            >::template rebind_alloc< value_type > data_allocator_type;
            typedef cds::details::Allocator< value_type, data_allocator_type > cxx_data_allocator;

            typedef typename original_traits::memory_model memory_model;

            struct data_disposer
            {
                void operator ()( value_type* data )
                {
                    cxx_data_allocator().Delete( data );
                }
            };

            template <typename Less>
            using less_wrapper = cds::opt::details::make_comparator_from_less<Less>;

            struct intrusive_traits: public original_traits
            {
                typedef data_disposer disposer;
            };

            typedef intrusive::IterableList<gc, value_type, intrusive_traits> type;

            typedef typename type::key_comparator key_comparator;
        };
    }   // namespace details
    //@endcond

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_ITERABLE_LIST_H

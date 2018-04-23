// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_ITERABLE_KVLIST_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_ITERABLE_KVLIST_H

#include <cds/details/binary_functor_wrapper.h>
#include <cds/opt/compare.h>
#include <memory>

namespace cds { namespace container {

    //@cond
    namespace details {

        template <class GC, typename K, typename T, class Traits>
        struct make_iterable_kvlist
        {
            typedef Traits original_type_traits;

            typedef GC       gc;
            typedef K        key_type;
            typedef T        mapped_type;
            typedef std::pair<key_type const, mapped_type> value_type;

            typedef typename std::allocator_traits<
                typename original_type_traits::allocator
            >::template rebind_alloc<value_type> data_allocator_type;
            typedef cds::details::Allocator< value_type, data_allocator_type > cxx_data_allocator;

            typedef typename original_type_traits::memory_model memory_model;

            struct data_disposer
            {
                void operator ()( value_type * pData )
                {
                    cxx_data_allocator().Delete( pData );
                }
            };

            struct key_field_accessor {
                key_type const& operator()( value_type const& data )
                {
                    return data.first;
                }
            };

            template <typename Less>
            struct less_wrapper
            {
                template <typename Q>
                bool operator()( value_type const& lhs, Q const& rhs ) const
                {
                    return Less()( lhs.first, rhs );
                }

                template <typename Q>
                bool operator()( Q const& lhs, value_type const& rhs ) const
                {
                    return Less()( lhs, rhs.first );
                }
            };

            typedef typename opt::details::make_comparator< key_type, original_type_traits >::type key_comparator;

            struct base_traits: public original_type_traits
            {
                typedef data_disposer disposer;
                typedef cds::details::compare_wrapper< value_type, key_comparator, key_field_accessor > compare;
            };

            typedef container::IterableList<gc, value_type, base_traits> type;
        };
    }   // namespace details
    //@endcond

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_ITERABLE_KVLIST_H

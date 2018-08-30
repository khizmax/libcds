// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_LAZY_LIST_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_LAZY_LIST_H

#include <cds/details/binary_functor_wrapper.h>
#include <memory>

namespace cds { namespace container {

    //@cond
    namespace details {

        template <class GC, typename T, class Traits>
        struct make_lazy_list
        {
            typedef GC      gc;
            typedef T       value_type;
            typedef Traits original_type_traits;

            struct node_type : public intrusive::lazy_list::node<gc, typename original_type_traits::lock_type>
            {
                value_type  m_Value;

                node_type()
                {}

                template <typename Q>
                node_type( Q const& v )
                    : m_Value(v)
                {}
                template <typename... Args>
                node_type( Args&&... args )
                    : m_Value( std::forward<Args>(args)...)
                {}
            };

            typedef typename std::allocator_traits<
                typename original_type_traits::allocator
            >::template rebind_alloc<node_type> allocator_type;
            typedef cds::details::Allocator< node_type, allocator_type >                cxx_allocator;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };

            typedef typename std::conditional< original_type_traits::sort,
                typename opt::details::make_comparator< value_type, original_type_traits >::type,
                typename opt::details::make_equal_to< value_type, original_type_traits >::type
            >::type key_comparator;

            struct value_accessor {
                value_type const & operator()( node_type const & node ) const
                {
                    return node.m_Value;
                }
            };

            template <typename Less>
            using less_wrapper = cds::details::compare_wrapper< node_type, cds::opt::details::make_comparator_from_less<Less>, value_accessor >;

            template <typename Equal>
            using equal_to_wrapper = cds::details::predicate_wrapper< node_type, Equal, value_accessor >;

            struct intrusive_traits: public original_type_traits
            {
                typedef intrusive::lazy_list::base_hook< opt::gc<gc>, cds::opt::lock_type< typename original_type_traits::lock_type >>  hook;
                typedef node_deallocator disposer;
                static constexpr const opt::link_check_type link_checker = cds::intrusive::lazy_list::traits::link_checker;

                typedef typename std::conditional< std::is_same< typename original_type_traits::equal_to, cds::opt::none >::value,
                    cds::opt::none,
                    equal_to_wrapper< typename original_type_traits::equal_to >
                >::type equal_to;

                typedef typename std::conditional<
                    original_type_traits::sort
                       || !std::is_same<typename original_type_traits::compare, cds::opt::none>::value
                       || !std::is_same<typename original_type_traits::less, cds::opt::none>::value,
                    cds::details::compare_wrapper<
                        node_type,
                        typename opt::details::make_comparator< value_type, original_type_traits >::type,
                        value_accessor
                    >,
                    cds::opt::none
                >::type compare;
            };

            typedef intrusive::LazyList<gc, node_type, intrusive_traits>  type;
        };
    }   // namespace details
    //@endcond

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_MICHAEL_LIST_H

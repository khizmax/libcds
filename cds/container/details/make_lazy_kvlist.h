// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_LAZY_KVLIST_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_LAZY_KVLIST_H

#include <cds/details/binary_functor_wrapper.h>
#include <memory>

namespace cds { namespace container {

    //@cond
    namespace details {

        template <class GC, typename K, typename T, class Traits>
        struct make_lazy_kvlist
        {
            typedef Traits original_type_traits;

            typedef GC      gc;
            typedef K       key_type;
            typedef T       mapped_type;
            typedef std::pair<key_type const, mapped_type> value_type;

            struct node_type: public intrusive::lazy_list::node<gc, typename original_type_traits::lock_type>
            {
                value_type   m_Data;

                node_type( key_type const& key )
                    : m_Data( key, mapped_type())
                {}

                template <typename Q>
                node_type( Q const& key )
                    : m_Data( key_type( key ), mapped_type())
                {}

                template <typename Q, typename R>
                explicit node_type( std::pair<Q, R> const& pair )
                    : m_Data( pair )
                {}

                node_type( key_type const& key, mapped_type const& value )
                    : m_Data( key, value )
                {}

                template <typename R>
                node_type( key_type const& key, R const& value )
                    : m_Data( key, mapped_type( value ))
                {}

                template <typename Q>
                node_type( Q const& key, mapped_type const& value )
                    : m_Data( key_type( key ), value )
                {}

                template <typename Q, typename R>
                node_type( Q const& key, R const& value )
                    : m_Data( key_type( key ), mapped_type( value ))
                {}

                template <typename Ky, typename... Args>
                node_type( Ky&& key, Args&&... args )
                    : m_Data( key_type( std::forward<Ky>( key )), std::move( mapped_type( std::forward<Args>( args )... )))
                {}
            };

            typedef typename std::allocator_traits<
                typename original_type_traits::allocator
            >::template rebind_alloc< node_type > allocator_type;
            typedef cds::details::Allocator< node_type, allocator_type > cxx_allocator;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };

            struct key_field_accessor {
                key_type const& operator()( node_type const& pair )
                {
                    return pair.m_Data.first;
                }
            };

            typedef typename std::conditional< original_type_traits::sort,
                typename opt::details::make_comparator< value_type, original_type_traits >::type,
                typename opt::details::make_equal_to< value_type, original_type_traits >::type
            >::type key_comparator;


            template <typename Less>
            using less_wrapper = cds::details::compare_wrapper< node_type, cds::opt::details::make_comparator_from_less<Less>, key_field_accessor >;
            
            template <typename Equal>
            using equal_to_wrapper = cds::details::predicate_wrapper< node_type, Equal, key_field_accessor >;

            struct intrusive_traits: public original_type_traits
            {
                typedef intrusive::lazy_list::base_hook< opt::gc<gc>, opt::lock_type< typename original_type_traits::lock_type >> hook;
                typedef node_deallocator disposer;

                typedef typename std::conditional< std::is_same< typename original_type_traits::equal_to, cds::opt::none >::value,
                    cds::opt::none,
                    equal_to_wrapper< typename original_type_traits::equal_to >
                >::type equal_to;

                typedef typename std::conditional<
                    original_type_traits::sort
                        || !std::is_same< typename original_type_traits::compare, cds::opt::none >::value
                        || !std::is_same< typename original_type_traits::less, cds::opt::none >::value,
                    cds::details::compare_wrapper<
                        node_type,
                        typename opt::details::make_comparator< value_type, original_type_traits >::type,
                        key_field_accessor
                    >,
                    cds::opt::none
                >::type compare;

                static const opt::link_check_type link_checker = cds::intrusive::lazy_list::traits::link_checker;
            };

            typedef intrusive::LazyList<gc, node_type, intrusive_traits>  type;
        };
    }   // namespace details
    //@endcond

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_LAZY_KVLIST_H

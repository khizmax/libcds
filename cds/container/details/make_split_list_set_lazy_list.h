// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_SPLIT_LIST_SET_LAZY_LIST_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_SPLIT_LIST_SET_LAZY_LIST_H

#include <memory>

//@cond
namespace cds { namespace container { namespace details {

    template <typename GC, typename T, typename Traits>
    struct make_split_list_set< GC, T, lazy_list_tag, Traits >
    {
        typedef GC      gc;
        typedef T       value_type;
        typedef Traits  original_traits;

        typedef typename cds::opt::select_default<
            typename original_traits::ordered_list_traits,
            cds::container::lazy_list::traits
        >::type         original_ordered_list_traits;

        typedef typename cds::opt::select_default<
            typename original_ordered_list_traits::lock_type,
            typename cds::container::lazy_list::traits::lock_type
        >::type   lock_type;

        typedef cds::intrusive::split_list::node< cds::intrusive::lazy_list::node<gc, lock_type > > primary_node_type;
        struct node_type: public primary_node_type
        {
            value_type  m_Value;

            template <typename Q>
            explicit node_type( Q&& v )
                : m_Value( std::forward<Q>( v ))
            {}

            template <typename Q, typename... Args>
            explicit node_type( Q&& q, Args&&... args )
                : m_Value( std::forward<Q>(q), std::forward<Args>(args)... )
            {}

            node_type() = delete;
        };

        typedef typename cds::opt::select_default<
            typename original_traits::ordered_list_traits,
            typename original_traits::allocator,
            typename cds::opt::select_default<
                typename original_traits::ordered_list_traits::allocator,
                typename original_traits::allocator
            >::type
        >::type node_allocator_;

        typedef typename std::allocator_traits<node_allocator_>::template rebind_alloc<node_type> node_allocator_type;

        typedef cds::details::Allocator< node_type, node_allocator_type >   cxx_node_allocator;
        struct node_deallocator
        {
            void operator ()( node_type * pNode )
            {
                cxx_node_allocator().Delete( pNode );
            }
        };

        typedef typename opt::details::make_comparator< value_type, original_ordered_list_traits >::type key_comparator;

        typedef typename original_traits::key_accessor key_accessor;

        struct value_accessor
        {
            typename key_accessor::key_type const & operator()( node_type const & node ) const
            {
                return key_accessor()(node.m_Value);
            }
        };

        template <typename Predicate>
        using predicate_wrapper = cds::details::predicate_wrapper< node_type, Predicate, value_accessor >;

        struct ordered_list_traits: public original_ordered_list_traits
        {
            typedef cds::intrusive::lazy_list::base_hook<
                opt::gc<gc>
                ,opt::lock_type< lock_type >
            >  hook;
            typedef cds::atomicity::empty_item_counter item_counter;
            typedef node_deallocator                disposer;
            typedef cds::details::compare_wrapper< node_type, key_comparator, value_accessor > compare;
            static constexpr const opt::link_check_type link_checker = cds::intrusive::lazy_list::traits::link_checker;
        };

        struct traits: public original_traits
        {
            struct hash: public original_traits::hash
            {
                typedef typename original_traits::hash  base_class;

                size_t operator()(node_type const& v ) const
                {
                    return base_class::operator()( key_accessor()( v.m_Value ));
                }
                template <typename Q>
                size_t operator()( Q const& k ) const
                {
                    return base_class::operator()( k );
                }
            };
        };

        class ordered_list: public cds::intrusive::LazyList< gc, node_type, ordered_list_traits >
        {};

        typedef cds::intrusive::SplitListSet< gc, ordered_list, traits >   type;
    };
}}}  // namespace cds::container::details
//@endcond

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_SPLIT_LIST_SET_LAZY_LIST_H

/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_SPLIT_LIST_SET_ITERABLE_LIST_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_SPLIT_LIST_SET_ITERABLE_LIST_H

//@cond
namespace cds { namespace container { namespace details {

    template <typename GC, typename T, typename Traits>
    struct make_split_list_set< GC, T, iterable_list_tag, Traits >
    {
        typedef GC      gc;
        typedef T       value_type;
        typedef Traits  original_traits;

        typedef typename cds::opt::select_default<
            typename original_traits::ordered_list_traits,
            cds::container::iterable_list::traits
        >::type original_ordered_list_traits;

        struct node_type: public cds::intrusive::split_list::node< void >
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

        typedef typename node_allocator_::template rebind<node_type>::other node_allocator_type;

        typedef cds::details::Allocator< node_type, node_allocator_type > cxx_node_allocator;
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
            typename key_accessor::key_type const& operator()( node_type const& node ) const
            {
                return key_accessor()(node.m_Value);
            }
        };

        template <typename Predicate>
        struct predicate_wrapper {
            typedef cds::details::predicate_wrapper< node_type, Predicate, value_accessor > type;
        };

        struct ordered_list_traits: public original_ordered_list_traits
        {
            typedef cds::atomicity::empty_item_counter item_counter;
            typedef node_deallocator disposer;
            typedef cds::details::compare_wrapper< node_type, key_comparator, value_accessor > compare;
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

        class ordered_list: public cds::intrusive::IterableList< gc, node_type, ordered_list_traits >
        {};

        typedef cds::intrusive::SplitListSet< gc, ordered_list, traits > type;
    };

}}}  // namespace cds::container::details
//@endcond

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_SPLIT_LIST_SET_ITERABLE_LIST_H

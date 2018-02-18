// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_SINGLE_LINK_STRUCT_H
#define CDSLIB_INTRUSIVE_DETAILS_SINGLE_LINK_STRUCT_H

#include <cds/intrusive/details/base.h>
#include <cds/gc/default_gc.h>
#include <cds/algo/atomic.h>

namespace cds { namespace intrusive {

    /// Definitions common for single-linked data structures
    /** @ingroup cds_intrusive_helper
    */
    namespace single_link {

        /// Container's node
        /**
            Template parameters:
            - GC - garbage collector used
            - Tag - a tag used to distinguish between different implementation
        */
        template <class GC, typename Tag = opt::none>
        struct node
        {
            typedef GC              gc  ;   ///< Garbage collector
            typedef Tag             tag ;   ///< tag

            typedef typename gc::template atomic_ref<node>    atomic_node_ptr; ///< atomic pointer

            /// Rebind node for other template parameters
            template <class GC2, typename Tag2 = tag>
            struct rebind {
                typedef node<GC2, Tag2>  other ;    ///< Rebinding result
            };

            atomic_node_ptr m_pNext ; ///< pointer to the next node in the container

            node() noexcept
            {
                m_pNext.store( nullptr, atomics::memory_order_release );
            }
        };

        //@cond
        struct default_hook {
            typedef cds::gc::default_gc gc;
            typedef opt::none           tag;
        };
        //@endcond

        //@cond
        template < typename HookType, typename... Options>
        struct hook
        {
            typedef typename opt::make_options< default_hook, Options...>::type  options;
            typedef typename options::gc    gc;
            typedef typename options::tag   tag;
            typedef node<gc, tag> node_type;
            typedef HookType      hook_type;
        };
        //@endcond

        /// Base hook
        /**
            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - tag
        */
        template < typename... Options >
        struct base_hook: public hook< opt::base_hook_tag, Options... >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - tag
        */
        template < size_t MemberOffset, typename... Options >
        struct member_hook: public hook< opt::member_hook_tag, Options... >
        {
            //@cond
            static const size_t c_nMemberOffset = MemberOffset;
            //@endcond
        };

        /// Traits hook
        /**
            \p NodeTraits defines type traits for node.
            See \ref node_traits for \p NodeTraits interface description

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - tag
        */
        template <typename NodeTraits, typename... Options >
        struct traits_hook: public hook< opt::traits_hook_tag, Options... >
        {
            //@cond
            typedef NodeTraits node_traits;
            //@endcond
        };

        /// Check link
        template <typename Node>
        struct link_checker {
            //@cond
            typedef Node node_type;
            //@endcond

            /// Checks if the link field of node \p pNode is \p nullptr
            /**
                An asserting is generated if \p pNode link field is not \p nullptr
            */
            static void is_empty( const node_type * pNode )
            {
                assert( pNode->m_pNext.load( atomics::memory_order_relaxed ) == nullptr );
                CDS_UNUSED( pNode );
            }
        };

        //@cond
        template <class GC, typename Node, opt::link_check_type LinkType >
        struct link_checker_selector;

        template <typename GC, typename Node>
        struct link_checker_selector< GC, Node, opt::never_check_link >
        {
            typedef intrusive::opt::v::empty_link_checker<Node>  type;
        };

        template <typename GC, typename Node>
        struct link_checker_selector< GC, Node, opt::debug_check_link >
        {
#       ifdef _DEBUG
            typedef link_checker<Node>  type;
#       else
            typedef intrusive::opt::v::empty_link_checker<Node>  type;
#       endif
        };

        template <typename GC, typename Node>
        struct link_checker_selector< GC, Node, opt::always_check_link >
        {
            typedef link_checker<Node>  type;
        };
        //@endcond

        /// Metafunction for selecting appropriate link checking policy
        template < typename Node, opt::link_check_type LinkType >
        struct get_link_checker
        {
            //@cond
            typedef typename link_checker_selector< typename Node::gc, Node, LinkType>::type type;
            //@endcond
        };

    }   // namespace single_link

}}  // namespace cds::intrusive



#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_SINGLE_LINK_STRUCT_H

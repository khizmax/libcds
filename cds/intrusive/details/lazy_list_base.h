/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSLIB_INTRUSIVE_DETAILS_LAZY_LIST_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_LAZY_LIST_BASE_H

#include <cds/intrusive/details/base.h>
#include <cds/opt/compare.h>
#include <cds/details/marked_ptr.h>
#include <cds/details/make_const_type.h>
#include <cds/sync/spinlock.h>
#include <cds/urcu/options.h>

namespace cds { namespace intrusive {

    /// LazyList ordered list related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace lazy_list {
        /// Lazy list node
        /**
            Template parameters:
            - GC - garbage collector
            - Lock - lock type. Default is \p cds::sync::spin
            - Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <
            class GC
            ,typename Lock =  cds::sync::spin
            ,typename Tag = opt::none
        >
        struct node
        {
            typedef GC      gc          ;   ///< Garbage collector
            typedef Lock    lock_type   ;   ///< Lock type
            typedef Tag     tag         ;   ///< tag

            typedef cds::details::marked_ptr<node, 1>   marked_ptr         ;   ///< marked pointer
            typedef typename gc::template atomic_marked_ptr< marked_ptr>     atomic_marked_ptr   ;   ///< atomic marked pointer specific for GC

            atomic_marked_ptr   m_pNext; ///< pointer to the next node in the list + logical deletion mark
            mutable lock_type   m_Lock;  ///< Node lock

            /// Checks if node is marked
            bool is_marked() const
            {
                return m_pNext.load(atomics::memory_order_relaxed).bits() != 0;
            }

            /// Default ctor
            node()
                : m_pNext( nullptr )
            {}
        };

        //@cond
        template <typename GC, typename Node, typename MemoryModel>
        struct node_cleaner {
            void operator()( Node * p )
            {
                typedef typename Node::marked_ptr marked_ptr;
                p->m_pNext.store( marked_ptr(), MemoryModel::memory_order_release );
            }
        };
        //@endcond

        //@cond
        struct undefined_gc;
        struct default_hook {
            typedef undefined_gc    gc;
            typedef opt::none       tag;
            typedef sync::spin      lock_type;
        };
        //@endcond

        //@cond
        template < typename HookType, typename... Options>
        struct hook
        {
            typedef typename opt::make_options< default_hook, Options...>::type  options;
            typedef typename options::gc        gc;
            typedef typename options::tag       tag;
            typedef typename options::lock_type lock_type;
            typedef node<gc, lock_type, tag>    node_type;
            typedef HookType        hook_type;
        };
        //@endcond

        /// Base hook
        /**
            \p Options are:
            - opt::gc - garbage collector
            - opt::lock_type - lock type used for node locking. Default is sync::spin
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < typename... Options >
        struct base_hook: public hook< opt::base_hook_tag, Options... >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector
            - opt::lock_type - lock type used for node locking. Default is sync::spin
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
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
            - opt::lock_type - lock type used for node locking. Default is sync::spin
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
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
        struct link_checker
        {
            //@cond
            typedef Node node_type;
            //@endcond

            /// Checks if the link field of node \p pNode is \p nullptr
            /**
                An asserting is generated if \p pNode link field is not \p nullptr
            */
            static void is_empty( node_type const * pNode )
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

        /// LazyList traits
        struct traits
        {
            /// Hook used
            /**
                Possible values are: \p lazy_list::base_hook, \p lazy_list::member_hook, \p lazy_list::traits_hook.
            */
            typedef base_hook<>       hook;

            /// Key comparing functor
            /**
                No default functor is provided. If the functor is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// Specifies binary predicate used for comparing keys
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less;

            /// Specifies binary functor used for comparing keys for equality (for unordered list only)
            /**
                If \p equal_to option is not specified, \p compare is used, if \p compare is not specified, \p less is used,
                if \p less is not specified, then \p std::equal_to<T> is used.
            */
            typedef opt::none                       equal_to;

            /// Specifies list ordering policy
            /**
                If \p sort is \p true, than list maintains items in sorted order, otherwise the list is unordered.
                Default is \p true.
                Note that if \p sort is \p false, than lookup operations scan entire list.
            */
            static const bool sort = true;

            /// Back-off strategy
            typedef cds::backoff::Default           back_off;

            /// Disposer for removing items
            typedef opt::v::empty_disposer          disposer;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter     item_counter;

            /// Link fields checking feature
            /**
                Default is \p opt::debug_check_link
            */
            static const opt::link_check_type link_checker = opt::debug_check_link;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_LazyList_rcu "RCU-based LazyList")
            /**
                List of available options see \p opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;
        };

        /// Metafunction converting option list to \p lazy_list::traits
        /**
            Supported \p Options are:
            - \p opt::hook - hook used. Possible values are: \p lazy_list::base_hook, \p lazy_list::member_hook, \p lazy_list::traits_hook.
                If the option is not specified, \p %lazy_list::base_hook and \p gc::HP is used.
            - \p opt::compare - key comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
            - \p opt::equal_to - specifies binary functor for comparing keys for equality. This option is applicable only for unordered list.
                If \p equal_to is not specified, \p compare is used, \p compare is not specified, \p less is used.
            - \p opt::sort - specifies ordering policy. Default value is \p true, i.e. the list is ordered.
                Note: unordering feature is not fully supported yet.
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::v::empty_disposer. Due the nature
                of GC schema the disposer may be called asynchronously.
            - \p opt::link_checker - the type of node's link fields checking. Default is \p opt::debug_check_link
            - \p opt::item_counter - the type of item counting feature. Default is disabled (\p atomicity::empty_item_counter).
                 To enable item counting use \p atomicity::item_counter.
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p opt::rcu_check_deadlock - a deadlock checking policy for \ref cds_intrusive_MichaelList_rcu "RCU-based MichaelList"
                Default is \p opt::v::rcu_throw_deadlock
        */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#   endif
        };

    } // namespace lazy_list

    //@cond
    // Forward declaration
    template < class GC, typename T, class Traits = lazy_list::traits >
    class LazyList;
    //@endcond

}}   // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_LAZY_LIST_BASE_H

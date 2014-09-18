//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_LAZY_LIST_BASE_H
#define __CDS_INTRUSIVE_LAZY_LIST_BASE_H

#include <cds/intrusive/base.h>
#include <cds/opt/compare.h>
#include <cds/details/marked_ptr.h>
#include <cds/ref.h>
#include <cds/details/make_const_type.h>
#include <boost/type_traits/is_same.hpp>
#include <cds/lock/spinlock.h>
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
            - Lock - lock type. Default is cds::lock::Spin
            - Tag - a tag used to distinguish between different implementation. An incomplete type can be used as a tag.
        */
        template <
            class GC
            ,typename Lock = lock::Spin
            ,typename Tag = opt::none
        >
        struct node
        {
            typedef GC      gc          ;   ///< Garbage collector
            typedef Lock    lock_type   ;   ///< Lock type
            typedef Tag     tag         ;   ///< tag

            typedef cds::details::marked_ptr<node, 1>   marked_ptr         ;   ///< marked pointer
            typedef typename gc::template atomic_marked_ptr< marked_ptr>     atomic_marked_ptr   ;   ///< atomic marked pointer specific for GC

            atomic_marked_ptr   m_pNext ; ///< pointer to the next node in the list + logical deletion mark
            mutable lock_type   m_Lock  ; ///< Node lock

            /// Checks if node is marked
            bool is_marked() const
            {
                return m_pNext.load(CDS_ATOMIC::memory_order_relaxed).bits() != 0;
            }

            /// Default ctor
            node()
                : m_pNext( null_ptr<node *>())
            {}
        };

        //@cond
        template <typename GC, typename NodeType, typename Alloc >
        class boundary_nodes
        {
            typedef NodeType node_type;

            node_type   m_Head;
            node_type   m_Tail;

        public:
            node_type * head()
            {
                return &m_Head;
            }
            node_type const * head() const
            {
                return &m_Head;
            }
            node_type * tail()
            {
                return &m_Tail;
            }
            node_type const * tail() const
            {
                return &m_Tail;
            }
        };
        //@endcond

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
            typedef lock::Spin      lock_type;
        };
        //@endcond

        //@cond
        template < typename HookType, CDS_DECL_OPTIONS3>
        struct hook
        {
            typedef typename opt::make_options< default_hook, CDS_OPTIONS3>::type  options;
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
            - opt::gc - garbage collector used.
            - opt::lock_type - lock type used for node locking. Default is lock::Spin
            - opt::tag - tag
        */
        template < CDS_DECL_OPTIONS3 >
        struct base_hook: public hook< opt::base_hook_tag, CDS_OPTIONS3 >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::lock_type - lock type used for node locking. Default is lock::Spin
            - opt::tag - tag
        */
        template < size_t MemberOffset, CDS_DECL_OPTIONS3 >
        struct member_hook: public hook< opt::member_hook_tag, CDS_OPTIONS3 >
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
            - opt::lock_type - lock type used for node locking. Default is lock::Spin
            - opt::tag - tag
        */
        template <typename NodeTraits, CDS_DECL_OPTIONS3 >
        struct traits_hook: public hook< opt::traits_hook_tag, CDS_OPTIONS3 >
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

            /// Checks if the link field of node \p pNode is NULL
            /**
                An asserting is generated if \p pNode link field is not NULL
            */
            static void is_empty( node_type const * pNode )
            {
                assert( pNode->m_pNext.load(CDS_ATOMIC::memory_order_relaxed) == null_ptr<node_type const *>());
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

        /// Type traits for LazyList class
        struct type_traits
        {
            /// Hook used
            /**
                Possible values are: lazy_list::base_hook, lazy_list::member_hook, lazy_list::traits_hook.
            */
            typedef base_hook<>       hook;

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// specifies binary predicate used for key comparison.
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less;

            /// back-off strategy used
            /**
                If the option is not specified, the cds::backoff::Default is used.
            */
            typedef cds::backoff::Default           back_off;

            /// Disposer
            /**
                the functor used for dispose removed items. Default is opt::v::empty_disposer.
            */
            typedef opt::v::empty_disposer          disposer;

            /// Item counter
            /**
                The type for item counting feature.
                Default is no item counter (\ref atomicity::empty_item_counter)
            */
            typedef atomicity::empty_item_counter     item_counter;

            /// Link fields checking feature
            /**
                Default is \ref opt::debug_check_link
            */
            static const opt::link_check_type link_checker = opt::debug_check_link;

            /// Allocator
            /**
                For intrusive lazy list an allocator is needed for dummy tail node allocation.
            */
            typedef CDS_DEFAULT_ALLOCATOR           allocator;

            /// C++ memory ordering model
            /**
                List of available memory ordering see opt::memory_model
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_LazyList_rcu "RCU-based LazyList")
            /**
                List of available options see opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;

            //@cond
            // for internal use only!!!
            typedef opt::none                       boundary_node_type;
            //@endcond
        };

        /// Metafunction converting option list to traits
        /**
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>

            See \ref LazyList, \ref type_traits, \ref cds::opt::make_options.

        */
        template <CDS_DECL_OPTIONS11>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< type_traits, CDS_OPTIONS11 >::type
                ,CDS_OPTIONS11
            >::type   type;
#   endif
        };

    } // namespace lazy_list

    //@cond
    // Forward declaration
    template < class GC, typename T, class Traits = lazy_list::type_traits >
    class LazyList;
    //@endcond


}}   // namespace cds::intrusive

#endif // #ifndef __CDS_INTRUSIVE_LAZY_LIST_BASE_H

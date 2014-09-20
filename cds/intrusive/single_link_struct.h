//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_SINGLE_LINK_STRUCT_H
#define __CDS_INTRUSIVE_SINGLE_LINK_STRUCT_H

#include <cds/intrusive/base.h>
#include <cds/gc/default_gc.h>
#include <cds/cxx11_atomic.h>
#include <cds/gc/hrc.h>

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
        struct node: public GC::container_node
        {
            typedef GC              gc  ;   ///< Garbage collector
            typedef Tag             tag ;   ///< tag

            typedef typename gc::template atomic_ref<node>    atomic_node_ptr    ;    ///< atomic pointer

            /// Rebind node for other template parameters
            template <class GC2, typename Tag2 = tag>
            struct rebind {
                typedef node<GC2, Tag2>  other ;    ///< Rebinding result
            };

            atomic_node_ptr m_pNext ; ///< pointer to the next node in the container

            node()
                : m_pNext( nullptr )
            {}
        };

        //@cond
        // Specialization for HRC GC
        template <typename Tag>
        struct node< gc::HRC, Tag>: public gc::HRC::container_node
        {
            typedef gc::HRC     gc  ;   ///< Garbage collector
            typedef Tag         tag ;   ///< tag

            typedef gc::atomic_ref<node>    atomic_node_ptr    ;    ///< atomic pointer
            atomic_node_ptr m_pNext ; ///< pointer to the next node in the container

            node()
                : m_pNext( nullptr )
            {}

        protected:
            virtual void cleanUp( cds::gc::hrc::ThreadGC * pGC )
            {
                assert( pGC != nullptr );
                typename gc::GuardArray<2> aGuards( *pGC );

                while ( true ) {
                    node * pNext = aGuards.protect( 0, m_pNext );
                    if ( pNext && pNext->m_bDeleted.load(atomics::memory_order_acquire) ) {
                        node * p = aGuards.protect( 1, pNext->m_pNext );
                        m_pNext.compare_exchange_strong( pNext, p, atomics::memory_order_acquire, atomics::memory_order_relaxed );
                        continue;
                    }
                    else {
                        break;
                    }
                }
            }

            virtual void terminate( cds::gc::hrc::ThreadGC * pGC, bool bConcurrent )
            {
                if ( bConcurrent ) {
                    node * pNext = m_pNext.load(atomics::memory_order_relaxed);
                    do {} while ( !m_pNext.compare_exchange_weak( pNext, nullptr, atomics::memory_order_release, atomics::memory_order_relaxed ) );
                }
                else {
                    m_pNext.store( nullptr, atomics::memory_order_relaxed );
                }
            }
        };
        //@endcond

        //@cond
        struct default_hook {
            typedef cds::gc::default_gc gc;
            typedef opt::none           tag;
        };
        //@endcond

        //@cond
        template < typename HookType, CDS_DECL_OPTIONS2>
        struct hook
        {
            typedef typename opt::make_options< default_hook, CDS_OPTIONS2>::type  options;
            typedef typename options::gc    gc;
            typedef typename options::tag   tag;
            typedef node<gc, tag> node_type;
            typedef HookType     hook_type;
        };
        //@endcond


        /// Base hook
        /**
            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - tag
        */
        template < CDS_DECL_OPTIONS2 >
        struct base_hook: public hook< opt::base_hook_tag, CDS_OPTIONS2 >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - tag
        */
        template < size_t MemberOffset, CDS_DECL_OPTIONS2 >
        struct member_hook: public hook< opt::member_hook_tag, CDS_OPTIONS2 >
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
        template <typename NodeTraits, CDS_DECL_OPTIONS2 >
        struct traits_hook: public hook< opt::traits_hook_tag, CDS_OPTIONS2 >
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
            }
        };

        //@cond
        template <class GC, typename Node, opt::link_check_type LinkType >
        struct link_checker_selector;

        template <typename Node>
        struct link_checker_selector< gc::HRC, Node, opt::never_check_link >
        {
            typedef link_checker<Node>  type;
        };

        template <typename Node>
        struct link_checker_selector< gc::HRC, Node, opt::debug_check_link >
        {
            typedef link_checker<Node>  type;
        };

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



#endif // #ifndef __CDS_INTRUSIVE_SINGLE_LINK_STRUCT_H

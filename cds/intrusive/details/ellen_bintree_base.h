// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_ELLEN_BINTREE_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_ELLEN_BINTREE_BASE_H

#include <type_traits>
#include <cds/intrusive/details/base.h>
#include <cds/opt/options.h>
#include <cds/urcu/options.h>
#include <cds/details/marked_ptr.h>

namespace cds { namespace intrusive {

    /// EllenBinTree related declarations
    namespace ellen_bintree {
        //Forwards
        template <class GC> struct base_node;
        template <class GC, typename Tag = opt::none> struct node;
        template <class GC, typename Key> struct internal_node;

        /// Update descriptor
        /**
            Update descriptor is used internally for helping concurrent threads
            to complete modifying operation.
            Usually, you should not use \p update_desc type directly until
            you want to develop special free-list of update descriptor.

            Template parameters:
            - \p LeafNode - leaf node type, see \ref node
            - \p InternalNode - internal node type, see \ref internal_node

            @note Size of update descriptor is constant.
            It does not depends of template arguments.
        */
        template <typename LeafNode, typename InternalNode>
        struct update_desc {
            //@cond
            typedef LeafNode        leaf_node;
            typedef InternalNode    internal_node;

            typedef cds::details::marked_ptr< update_desc, 3 > update_ptr;

            enum {
                Clean = 0,
                DFlag = 1,
                IFlag = 2,
                Mark  = 3
            };

            struct insert_info {
                internal_node *    pParent;
                internal_node *    pNew;
                leaf_node *        pLeaf;
                bool               bRightLeaf;
            };
            struct delete_info {
                internal_node *    pGrandParent;
                internal_node *    pParent;
                leaf_node *        pLeaf;
                update_desc *      pUpdateParent;
                bool               bDisposeLeaf; // true if pLeaf should be disposed, false otherwise (for extract operation, RCU)
                bool               bRightParent;
                bool               bRightLeaf;
            };

            union {
                insert_info     iInfo;
                delete_info     dInfo;
            };

            update_desc *   pNextRetire; // for local retired list (RCU)

            update_desc()
                : pNextRetire( nullptr )
            {}
            //@endcond
        };

        //@cond
        struct alignas( void* ) basic_node
        {
            enum flags {
                internal        = 1,    ///< set for internal node
                key_infinite1   = 2,    ///< set if node's key is Inf1
                key_infinite2   = 4,    ///< set if node's key is Inf2

                key_infinite = key_infinite1 | key_infinite2    ///< Cumulative infinite flags
            };

            atomics::atomic<unsigned int> m_nFlags;   ///< Internal flags

            /// Constructs leaf (bIntrenal == false) or internal (bInternal == true) node
            explicit basic_node( bool bInternal )
            {
                m_nFlags.store( bInternal ? internal: 0, atomics::memory_order_release );
            }

            /// Checks if the node is a leaf
            bool is_leaf() const
            {
                return !is_internal();
            }

            /// Checks if the node is internal
            bool is_internal() const
            {
                return (m_nFlags.load(atomics::memory_order_acquire) & internal) != 0;
            }

            /// Returns infinite key, 0 if the node is not infinite
            unsigned int infinite_key() const
            {
                return m_nFlags.load(atomics::memory_order_acquire) & key_infinite;
            }

            /// Sets infinite key for the node (for internal use only!!!)
            void infinite_key( int nInf )
            {
                unsigned int nFlags = m_nFlags.load(atomics::memory_order_relaxed);
                nFlags &= ~key_infinite;
                switch ( nInf ) {
                case 1:
                    nFlags |= key_infinite1;
                    break;
                case 2:
                    nFlags |= key_infinite2;
                    break;
                case 0:
                    break;
                default:
                    assert( false );
                    break;
                }
                m_nFlags.store( nFlags, atomics::memory_order_release );
            }
        };

        template <class GC>
        struct base_node: public basic_node
        {
            typedef basic_node base_class;

            typedef GC              gc       ;   ///< Garbage collector

            /// Constructs leaf (bIntrenal == false) or internal (bInternal == true) node
            explicit base_node( bool bInternal )
                : base_class( bInternal )
            {}
        };
        //@endcond

        /// Ellen's binary tree leaf node
        /**
            Template parameters:
            - \p GC - one of \ref cds_garbage_collector "garbage collector type"
            - \p Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename GC,
#   ifdef CDS_DOXYGEN_INVOKED
            typename Tag = opt::none
#   else
            typename Tag
#   endif
        >
        struct node
#   ifndef CDS_DOXYGEN_INVOKED
            : public base_node< GC >
#   endif
        {
            //@cond
            typedef base_node< GC >   base_class;
            //@endcond

            typedef GC              gc;   ///< Garbage collector
            typedef Tag             tag;  ///< Tag

            /// Default ctor
            node()
                : base_class( false )
            {}
        };

        /// Ellen's binary tree internal node
        /**
            Template arguments:
            - \p Key - key type
            - \p LeafNode - leaf node type
        */
        template <typename Key, typename LeafNode>
        struct internal_node
#   ifndef CDS_DOXYGEN_INVOKED
            : public base_node<typename LeafNode::gc>
#   endif
        {
            //@cond
            typedef base_node<typename LeafNode::gc>   base_class;
            //@endcond

            typedef Key         key_type;    ///< key type
            typedef LeafNode    leaf_node;   ///< type of leaf node
            typedef update_desc< leaf_node, internal_node > update_desc_type; ///< Update descriptor
            typedef typename update_desc_type::update_ptr  update_ptr; ///< Marked pointer to update descriptor

            key_type                      m_Key;     ///< Regular key
            atomics::atomic<base_class *> m_pLeft;   ///< Left subtree
            atomics::atomic<base_class *> m_pRight;  ///< Right subtree
            atomics::atomic<update_ptr>   m_pUpdate; ///< Update descriptor
            //@cond
            atomics::atomic<uintptr_t>    m_nEmptyUpdate; ///< ABA prevention for m_pUpdate, from 0..2^16 step 4
            //@endcond

            /// Default ctor
            internal_node()
                : base_class( true )
                , m_pLeft( nullptr )
                , m_pRight( nullptr )
                , m_pUpdate( update_ptr())
            {
                m_nEmptyUpdate.store( 0, atomics::memory_order_release );
            }

            //@cond
            update_ptr null_update_desc()
            {
                return update_ptr( reinterpret_cast<update_desc_type *>( ((m_nEmptyUpdate.fetch_add(1, atomics::memory_order_relaxed) + 1 ) << 2) & 0xFFFF ));
            }

            base_class * get_child( bool bRight, atomics::memory_order mo ) const
            {
                return bRight ? m_pRight.load( mo ) : m_pLeft.load( mo );
            }
            //@endcond
        };

        /// Types of EllenBinTree node
        /**
            This struct declares different \p %EllenBinTree node types.
            It can be useful for simplifying \p %EllenBinTree node declaration in your application.

            Template parameters:
            - \p GC - one of \ref cds_garbage_collector "garbage collector type"
            - \p Key - key type
            - \p Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename GC, typename Key, typename Tag = opt::none>
        struct node_types
        {
            typedef node<GC, Tag>                       leaf_node_type;         ///< Leaf node type
            typedef internal_node<Key, leaf_node_type>  internal_node_type;     ///< Internal node type
            typedef update_desc<leaf_node_type, internal_node_type> update_desc_type;  ///< Update descriptor type
        };

        //@cond
        struct undefined_gc;
        struct default_hook {
            typedef undefined_gc    gc;
            typedef opt::none       tag;
        };
        //@endcond

        //@cond
        template < typename HookType, typename... Options>
        struct hook
        {
            typedef typename opt::make_options< default_hook, Options...>::type  options;
            typedef typename options::gc    gc;
            typedef typename options::tag   tag;
            typedef node<gc, tag>           node_type;
            typedef HookType                hook_type;
        };
        //@endcond

        /// Base hook
        /**
            \p Options are:
            - \p opt::gc - garbage collector
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < typename... Options >
        struct base_hook: public hook< opt::base_hook_tag, Options... >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - \p opt::gc - garbage collector
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < size_t MemberOffset, typename... Options >
        struct member_hook: public hook< opt::member_hook_tag, Options... >
        {
            //@cond
            static constexpr const size_t c_nMemberOffset = MemberOffset;
            //@endcond
        };

        /// Traits hook
        /**
            \p NodeTraits defines type traits for node.
            See \ref node_traits for \p NodeTraits interface description

            \p Options are:
            - opt::gc - garbage collector
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename NodeTraits, typename... Options >
        struct traits_hook: public hook< opt::traits_hook_tag, Options... >
        {
            //@cond
            typedef NodeTraits node_traits;
            //@endcond
        };

        /// Key extracting functor option setter
        template <typename Type>
        struct key_extractor {
            //@cond
            template <typename Base> struct pack: public Base
            {
                typedef Type key_extractor;
            };
            //@endcond
        };

        /// Update descriptor allocator option setter
        template <typename Type>
        struct update_desc_allocator {
            //@cond
            template <typename Base> struct pack: public Base
            {
                typedef Type update_desc_allocator;
            };
            //@endcond
        };

        /// EllenBinTree internal statistics
        template <typename Counter = cds::atomicity::event_counter>
        struct stat {
            typedef Counter   event_counter ; ///< Event counter type

            event_counter   m_nInternalNodeCreated  ;   ///< Total count of created internal node
            event_counter   m_nInternalNodeDeleted  ;   ///< Total count of deleted internal node
            event_counter   m_nUpdateDescCreated    ;   ///< Total count of created update descriptors
            event_counter   m_nUpdateDescDeleted    ;   ///< Total count of deleted update descriptors

            event_counter   m_nInsertSuccess        ; ///< Count of success insertion
            event_counter   m_nInsertFailed         ; ///< Count of failed insertion
            event_counter   m_nInsertRetries        ; ///< Count of unsuccessful retries of insertion
            event_counter   m_nUpdateExist          ; ///< Count of \p update() call for existed node
            event_counter   m_nUpdateNew            ; ///< Count of \p update() call for new node
            event_counter   m_nUpdateRetries        ; ///< Count of unsuccessful retries of ensuring
            event_counter   m_nEraseSuccess         ; ///< Count of successful call of \p erase and \p unlink
            event_counter   m_nEraseFailed          ; ///< Count of failed call of \p erase and \p unlink
            event_counter   m_nEraseRetries         ; ///< Count of unsuccessful retries inside erasing/unlinking
            event_counter   m_nFindSuccess          ; ///< Count of successful \p find call
            event_counter   m_nFindFailed           ; ///< Count of failed \p find call
            event_counter   m_nExtractMinSuccess    ; ///< Count of successful call of \p extract_min
            event_counter   m_nExtractMinFailed     ; ///< Count of failed call of \p extract_min
            event_counter   m_nExtractMinRetries    ; ///< Count of unsuccessful retries inside \p extract_min
            event_counter   m_nExtractMaxSuccess    ; ///< Count of successful call of \p extract_max
            event_counter   m_nExtractMaxFailed     ; ///< Count of failed call of \p extract_max
            event_counter   m_nExtractMaxRetries    ; ///< Count of unsuccessful retries inside \p extract_max
            event_counter   m_nSearchRetry          ; ///< How many times the deleting node was encountered while searching

            event_counter   m_nHelpInsert           ; ///< The number of insert help from the other thread
            event_counter   m_nHelpDelete           ; ///< The number of delete help from the other thread
            event_counter   m_nHelpMark             ; ///< The number of delete help (mark phase) from the other thread
            event_counter   m_nHelpGuardSuccess     ; ///< The number of successful guarding of update descriptor data
            event_counter   m_nHelpGuardFailed      ; ///< The number of failed guarding of update descriptor data

            //@cond
            void    onInternalNodeCreated()         { ++m_nInternalNodeCreated  ; }
            void    onInternalNodeDeleted()         { ++m_nInternalNodeDeleted  ; }
            void    onUpdateDescCreated()           { ++m_nUpdateDescCreated    ; }
            void    onUpdateDescDeleted()           { ++m_nUpdateDescDeleted    ; }
            void    onInsertSuccess()               { ++m_nInsertSuccess        ; }
            void    onInsertFailed()                { ++m_nInsertFailed         ; }
            void    onInsertRetry()                 { ++m_nInsertRetries        ; }
            void    onUpdateExist()                 { ++m_nUpdateExist          ; }
            void    onUpdateNew()                   { ++m_nUpdateNew            ; }
            void    onUpdateRetry()                 { ++m_nUpdateRetries        ; }
            void    onEraseSuccess()                { ++m_nEraseSuccess         ; }
            void    onEraseFailed()                 { ++m_nEraseFailed          ; }
            void    onEraseRetry()                  { ++m_nEraseRetries         ; }
            void    onExtractMinSuccess()           { ++m_nExtractMinSuccess    ; }
            void    onExtractMinFailed()            { ++m_nExtractMinFailed     ; }
            void    onExtractMinRetry()             { ++m_nExtractMinRetries    ; }
            void    onExtractMaxSuccess()           { ++m_nExtractMaxSuccess    ; }
            void    onExtractMaxFailed()            { ++m_nExtractMaxFailed     ; }
            void    onExtractMaxRetry()             { ++m_nExtractMaxRetries    ; }
            void    onFindSuccess()                 { ++m_nFindSuccess          ; }
            void    onFindFailed()                  { ++m_nFindFailed           ; }
            void    onSearchRetry()                 { ++m_nSearchRetry          ; }
            void    onHelpInsert()                  { ++m_nHelpInsert           ; }
            void    onHelpDelete()                  { ++m_nHelpDelete           ; }
            void    onHelpMark()                    { ++m_nHelpMark             ; }
            void    onHelpGuardSuccess()            { ++m_nHelpGuardSuccess     ; }
            void    onHelpGuardFailed()             { ++m_nHelpGuardFailed      ; }
            //@endcond
        };

        /// EllenBinTree empty statistics
        struct empty_stat {
            //@cond
            void    onInternalNodeCreated()         const {}
            void    onInternalNodeDeleted()         const {}
            void    onUpdateDescCreated()           const {}
            void    onUpdateDescDeleted()           const {}
            void    onInsertSuccess()               const {}
            void    onInsertFailed()                const {}
            void    onInsertRetry()                 const {}
            void    onUpdateExist()                 const {}
            void    onUpdateNew()                   const {}
            void    onUpdateRetry()                 const {}
            void    onEraseSuccess()                const {}
            void    onEraseFailed()                 const {}
            void    onEraseRetry()                  const {}
            void    onExtractMinSuccess()           const {}
            void    onExtractMinFailed()            const {}
            void    onExtractMinRetry()             const {}
            void    onExtractMaxSuccess()           const {}
            void    onExtractMaxFailed()            const {}
            void    onExtractMaxRetry()             const {}
            void    onFindSuccess()                 const {}
            void    onFindFailed()                  const {}
            void    onSearchRetry()                 const {}
            void    onHelpInsert()                  const {}
            void    onHelpDelete()                  const {}
            void    onHelpMark()                    const {}
            void    onHelpGuardSuccess()            const {}
            void    onHelpGuardFailed()             const {}
            //@endcond
        };

        /// EllenBinTree traits
        struct traits
        {
            /// Hook used (mandatory)
            /**
                Possible values are: \p ellen_bintree::base_hook, \p ellen_bintree::member_hook, \p ellen_bintree::traits_hook.
            */
            typedef base_hook<>       hook;

            /// Key extracting functor (mandatory)
            /**
                You should explicit define a valid functor.
                The functor has the following prototype:
                \code
                struct key_extractor {
                    void operator ()( Key& dest, T const& src );
                };
                \endcode
                It should initialize \p dest key from \p src data.
                The functor is used to initialize internal nodes.
            */
            typedef opt::none key_extractor;

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.

                See \p cds::opt::compare option description for functor interface.

                You should provide \p compare or \p less functor.
                See \ref cds_intrusive_EllenBinTree_rcu_less "predicate requirements".
            */
            typedef opt::none compare;

            /// Specifies binary predicate used for key compare.
            /**
                See \p cds::opt::less option description for predicate interface.

                You should provide \p compare or \p less functor.
                See \ref cds_intrusive_EllenBinTree_rcu_less "predicate requirements".
            */
            typedef opt::none less;

            /// Disposer
            /**
                The functor used for dispose removed items. Default is \p opt::v::empty_disposer.
            */
            typedef opt::v::empty_disposer disposer;

            /// Item counter
            /**
                The type for item counter, by default it is disabled (\p atomicity::empty_item_counter).
                To enable it use \p atomicity::item_counter or \p atomicity::cache_friendly_item_counter
            */
            typedef atomicity::empty_item_counter item_counter;

            /// C++ memory ordering model
            /**
                List of available memory ordering see \p opt::memory_model
            */
            typedef opt::v::relaxed_ordering memory_model;

            /// Allocator for update descriptors
            /**
                The allocator type is used for \p ellen_bintree::update_desc.

                Update descriptor is helping data structure with short lifetime and it is good candidate
                for pooling. The number of simultaneously existing descriptors is bounded and it is
                limited by number of threads working with the tree.
                Therefore, a bounded lock-free container like \p cds::container::VyukovMPMCCycleQueue
                is a good choice for the free-list of update descriptors,
                see \p cds::memory::vyukov_queue_pool free-list implementation.

                Also notice that size of update descriptor is constant and not dependent on the type of data
                stored in the tree so single free-list object can be used for several \p EllenBinTree object.
            */
            typedef CDS_DEFAULT_ALLOCATOR update_desc_allocator;

            /// Allocator for internal nodes
            /**
                The allocator type is used for \p ellen_bintree::internal_node.
            */
            typedef CDS_DEFAULT_ALLOCATOR node_allocator;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p ellen_bintree::empty_stat).
                To enable it use \p ellen_bintree::stat.
            */
            typedef empty_stat stat;

            /// Back-off strategy
            typedef cds::backoff::empty back_off;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_EllenBinTree_rcu "RCU-based EllenBinTree")
            /**
                List of available options see \p opt::rcu_check_deadlock
            */
            typedef cds::opt::v::rcu_throw_deadlock rcu_check_deadlock;
        };

        /// Metafunction converting option list to EllenBinTree traits
        /**
            \p Options are:
            - \p opt::hook - hook used. Possible values are: \p ellen_bintree::base_hook, \p ellen_bintree::member_hook, \p ellen_bintree::traits_hook.
                If the option is not specified, <tt>ellen_bintree::base_hook<></tt> is used.
            - \p ellen_bintree::key_extractor - key extracting functor, mandatory option. The functor has the following prototype:
                \code
                    struct key_extractor {
                        void operator ()( Key& dest, T const& src );
                    };
                \endcode
                It should initialize \p dest key from \p src data. The functor is used to initialize internal nodes.
            - \p opt::compare - key compare functor. No default functor is provided.
                If the option is not specified, \p %opt::less is used.
            - \p opt::less - specifies binary predicate used for key compare. At least \p %opt::compare or \p %opt::less should be defined.
            - \p opt::disposer - the functor used for dispose removed nodes. Default is \p opt::v::empty_disposer. Due the nature
                of GC schema the disposer may be called asynchronously. The disposer is used only for leaf nodes.
            - \p opt::item_counter - the type of item counting feature, by default it is disabled (\p atomicity::empty_item_counter)
                To enable it use \p atomicity::item_counter or \p atomicity::cache_friendly_item_counter
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p ellen_bintree::update_desc_allocator - an allocator of \ref ellen_bintree::update_desc "update descriptors",
                default is \ref CDS_DEFAULT_ALLOCATOR.
                Note that update descriptor is helping data structure with short lifetime and it is good candidate for pooling.
                The number of simultaneously existing descriptors is bounded and depends on the number of threads
                working with the tree and GC internals.
                A bounded lock-free container like \p cds::container::VyukovMPMCCycleQueue is good candidate
                for the free-list of update descriptors, see cds::memory::vyukov_queue_pool free-list implementation.
                Also notice that size of update descriptor is constant and not dependent on the type of data
                stored in the tree so single free-list object can be used for all \p %EllenBinTree objects.
            - \p opt::node_allocator - the allocator for internal nodes. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - \p opt::stat - internal statistics, by default it is disabled (\p ellen_bintree::empty_stat)
                To enable statistics use \p \p ellen_bintree::stat
            - \p opt::backoff - back-off strategy, by default no strategy is used (\p cds::backoff::empty)
            - \p opt::rcu_check_deadlock - a deadlock checking policy for RCU-based tree, default is \p opt::v::rcu_throw_deadlock
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

        //@cond
        namespace details {

            template <typename Key, typename T, typename Compare, typename NodeTraits>
            struct compare
            {
                typedef Compare     key_compare;
                typedef Key         key_type;
                typedef T           value_type;
                typedef NodeTraits  node_traits;

                template <typename Q1, typename Q2>
                int operator()( Q1 const& v1, Q2 const& v2) const
                {
                    return key_compare()( v1, v2 );
                }

                template <typename LeafNode>
                int operator()( internal_node<key_type, LeafNode> const& n1, internal_node<key_type, LeafNode> const& n2 ) const
                {
                    if ( n1.infinite_key())
                        return n2.infinite_key() ? n1.infinite_key() - n2.infinite_key() : 1;
                    else if ( n2.infinite_key())
                        return -1;
                    return operator()( n1.m_Key, n2.m_Key );
                }

                template <typename LeafNode, typename Q>
                int operator()( internal_node<key_type, LeafNode> const& n, Q const& v ) const
                {
                    if ( n.infinite_key())
                        return 1;
                    return operator()( n.m_Key, v );
                }

                template <typename LeafNode, typename Q>
                int operator()( Q const& v, internal_node<key_type, LeafNode> const& n ) const
                {
                    if ( n.infinite_key())
                        return -1;
                    return operator()( v, n.m_Key );
                }

                template <typename GC, typename Tag>
                int operator()( node<GC, Tag> const& n1, node<GC, Tag> const& n2 ) const
                {
                    if ( n1.infinite_key() != n2.infinite_key())
                        return n1.infinite_key() - n2.infinite_key();
                    return operator()( *node_traits::to_value_ptr( n1 ), *node_traits::to_value_ptr( n2 ));
                }

                template <typename GC, typename Tag, typename Q>
                int operator()( node<GC, Tag> const& n, Q const& v ) const
                {
                    if ( n.infinite_key())
                        return 1;
                    return operator()( *node_traits::to_value_ptr( n ), v );
                }

                template <typename GC, typename Tag, typename Q>
                int operator()( Q const& v, node<GC, Tag> const& n ) const
                {
                    if ( n.infinite_key())
                        return -1;
                    return operator()( v, *node_traits::to_value_ptr( n ));
                }

                template <typename GC>
                int operator()( base_node<GC> const& n1, base_node<GC> const& n2 ) const
                {
                    if ( n1.infinite_key() != n2.infinite_key())
                        return n1.infinite_key() - n2.infinite_key();
                    if ( n1.is_leaf()) {
                        if ( n2.is_leaf())
                            return operator()( node_traits::to_leaf_node( n1 ), node_traits::to_leaf_node( n2 ));
                        else
                            return operator()( node_traits::to_leaf_node( n1 ), node_traits::to_internal_node( n2 ));
                    }

                    if ( n2.is_leaf())
                        return operator()( node_traits::to_internal_node( n1 ), node_traits::to_leaf_node( n2 ));
                    else
                        return operator()( node_traits::to_internal_node( n1 ), node_traits::to_internal_node( n2 ));
                }

                template <typename GC, typename Q>
                int operator()( base_node<GC> const& n, Q const& v ) const
                {
                    if ( n.infinite_key())
                        return 1;
                    if ( n.is_leaf())
                        return operator()( node_traits::to_leaf_node( n ), v );
                    return operator()( node_traits::to_internal_node( n ), v );
                }

                template <typename GC, typename Q>
                int operator()( Q const& v, base_node<GC> const& n ) const
                {
                    return -operator()( n, v );
                }

                template <typename GC, typename LeafNode >
                int operator()( base_node<GC> const& i, internal_node<key_type, LeafNode> const& n ) const
                {
                    if ( i.is_leaf())
                        return operator()( static_cast<LeafNode const&>(i), n );
                    return operator()( static_cast<internal_node<key_type, LeafNode> const&>(i), n );
                }

                template <typename GC, typename LeafNode >
                int operator()( internal_node<key_type, LeafNode> const& n, base_node<GC> const& i ) const
                {
                    return -operator()( i, n );
                }

                template <typename GC, typename Tag >
                int operator()( node<GC, Tag> const& n, internal_node<key_type, node<GC, Tag> > const& i ) const
                {
                    if ( !n.infinite_key()) {
                        if ( i.infinite_key())
                            return -1;
                        return operator()( n, i.m_Key );
                    }

                    if ( !i.infinite_key())
                        return 1;
                    return int( n.infinite_key()) - int( i.infinite_key());
                }

                template <typename GC, typename Tag >
                int operator()( internal_node<key_type, node<GC, Tag> > const& i, node<GC, Tag> const& n ) const
                {
                    return -operator()( n, i );
                }

            };

        } // namespace details
        //@endcond
    }   // namespace ellen_bintree

    // Forwards
    template < class GC, typename Key, typename T, class Traits = ellen_bintree::traits >
    class EllenBinTree;

}} // namespace cds::intrusive

#endif  // #ifndef CDSLIB_INTRUSIVE_DETAILS_ELLEN_BINTREE_BASE_H

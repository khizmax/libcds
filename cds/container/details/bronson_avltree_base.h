// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_BRONSON_AVLTREE_BASE_H
#define CDSLIB_CONTAINER_DETAILS_BRONSON_AVLTREE_BASE_H

#include <cds/container/details/base.h>
#include <cds/opt/compare.h>
#include <cds/urcu/options.h>
#include <cds/sync/spinlock.h>
#include <cds/sync/injecting_monitor.h>

namespace cds { namespace container {

    /// BronsonAVLTree related declarations
    namespace bronson_avltree {

        template <typename Key, typename T, typename SyncMonitor >
        struct node;

        //@cond
        template <typename Node, typename T, typename SyncMonitor>
        struct link_node
        {
            typedef Node     node_type;
            typedef T        mapped_type;
            typedef uint32_t version_type;  ///< version type (internal)

            enum
            {
                shrinking = 1,
                unlinked = 2,
                version_flags = shrinking | unlinked
                // the rest is version counter
            };

            atomics::atomic< int >          m_nHeight;  ///< Node height
            atomics::atomic<version_type>   m_nVersion; ///< Version bits
            atomics::atomic<node_type *>    m_pParent;  ///< Parent node
            atomics::atomic<node_type *>    m_pLeft;    ///< Left child
            atomics::atomic<node_type *>    m_pRight;   ///< Right child
            typename SyncMonitor::node_injection m_SyncMonitorInjection;    ///< @ref cds_sync_monitor "synchronization monitor" injected data
            atomics::atomic<mapped_type *>  m_pValue;   ///< Value

        public:
            link_node()
                : m_nHeight( 0 )
                , m_nVersion( 0 )
                , m_pParent( nullptr )
                , m_pLeft( nullptr )
                , m_pRight( nullptr )
            {
                m_pValue.store( nullptr, atomics::memory_order_release );
            }

            link_node( int nHeight, version_type version, node_type * pParent, node_type * pLeft, node_type * pRight )
                : m_nHeight( nHeight )
                , m_nVersion( version )
                , m_pParent( pParent )
                , m_pLeft( pLeft )
                , m_pRight( pRight )
            {
                m_pValue.store( nullptr, atomics::memory_order_release );
            }

            node_type * parent( atomics::memory_order order ) const
            {
                return m_pParent.load( order );
            }

            void parent( node_type * p, atomics::memory_order order )
            {
                m_pParent.store( p, order );
            }

            node_type * child( int nDirection, atomics::memory_order order ) const
            {
                assert( nDirection != 0 );
                return nDirection < 0 ? m_pLeft.load( order ) : m_pRight.load( order );
            }

            void child( node_type * pChild, int nDirection, atomics::memory_order order )
            {
                assert( nDirection != 0 );
                if ( nDirection < 0 )
                    m_pLeft.store( pChild, order );
                else
                    m_pRight.store( pChild, order );
            }

            version_type version( atomics::memory_order order ) const
            {
                return m_nVersion.load( order );
            }

            void version( version_type ver, atomics::memory_order order )
            {
                m_nVersion.store( ver, order );
            }

            void exchange_version( version_type ver, atomics::memory_order order )
            {
                m_nVersion.exchange( ver, order );
            }

            int height( atomics::memory_order order ) const
            {
                return m_nHeight.load( order );
            }

            void height( int h, atomics::memory_order order )
            {
                m_nHeight.store( h, order );
            }

            template <typename BackOff>
            void wait_until_shrink_completed( atomics::memory_order order ) const
            {
                BackOff bkoff;
                while ( is_shrinking( order ))
                    bkoff();
            }

            bool is_unlinked( atomics::memory_order order ) const
            {
                return m_nVersion.load( order ) == unlinked;
            }

            bool is_shrinking( atomics::memory_order order ) const
            {
                return (m_nVersion.load( order ) & shrinking) != 0;
            }

            mapped_type * value( atomics::memory_order order ) const
            {
                return m_pValue.load( order );
            }

            bool is_valued( atomics::memory_order order ) const
            {
                return value( order ) != nullptr;
            }
        };
        //@endcond

        /// BronsonAVLTree internal node
        template <typename Key, typename T, typename SyncMonitor >
        struct node<Key, T*, SyncMonitor>: public link_node< node<Key, T*, SyncMonitor>, T, SyncMonitor >
        {
            //@cond
            typedef link_node< node<Key, T*, SyncMonitor>, T, SyncMonitor > base_class;
            //@endcond

            typedef Key key_type;       ///< key type
            typedef T   mapped_type;    ///< value type
            //@cond
            typedef typename base_class::version_type version_type;
            //@endcond

            key_type const                  m_key;      ///< Key
            node *                          m_pNextRemoved; ///< thread-local list of removed node

        public:
            //@cond
            template <typename Q>
            node( Q&& key )
                : base_class()
                , m_key( std::forward<Q>( key ))
                , m_pNextRemoved( nullptr )
            {}

            template <typename Q>
            node( Q&& key, int nHeight, version_type version, node * pParent, node * pLeft, node * pRight )
                : base_class( nHeight, version, pParent, pLeft, pRight )
                , m_key( std::forward<Q>( key ))
                , m_pNextRemoved( nullptr )
            {}
            //@endcond
        };

        /// BronsonAVLTreeMap internal statistics
        template <typename Counter = cds::atomicity::event_counter>
        struct stat {
            typedef Counter   event_counter; ///< Event counter type

            event_counter   m_nFindSuccess; ///< Count of success \p find() call
            event_counter   m_nFindFailed;  ///< Count of failed \p find() call
            event_counter   m_nFindRetry;   ///< Count of retries during \p find()
            event_counter   m_nFindWaitShrinking;   ///< Count of waiting until shrinking completed duting \p find() call

            event_counter   m_nInsertSuccess;       ///< Count of inserting data node
            event_counter   m_nInsertFailed;        ///< Count of insert failures
            event_counter   m_nRelaxedInsertFailed; ///< Count of false creating of data nodes (only if @ref bronson_avltree::relaxed_insert "relaxed insertion" is enabled)
            event_counter   m_nInsertRetry;         ///< Count of insert retries via concurrent operations
            event_counter   m_nUpdateWaitShrinking; ///< Count of waiting until shrinking completed during \p update() call
            event_counter   m_nUpdateRetry;         ///< Count of update retries via concurrent operations
            event_counter   m_nUpdateRootWaitShrinking;  ///< Count of waiting until root shrinking completed duting \p update() call
            event_counter   m_nUpdateSuccess;       ///< Count of updating data node
            event_counter   m_nUpdateUnlinked;      ///< Count of attempts to update unlinked node
            event_counter   m_nDisposedNode;        ///< Count of disposed node
            event_counter   m_nDisposedValue;       ///< Count of disposed value
            event_counter   m_nExtractedValue;      ///< Count of extracted value
            event_counter   m_nRemoveSuccess;       ///< Count of successfully \p erase() call
            event_counter   m_nRemoveFailed;        ///< Count of failed \p erase() call
            event_counter   m_nRemoveRetry;         ///< Count o erase/extract retries
            event_counter   m_nExtractSuccess;      ///< Count of successfully \p extract() call
            event_counter   m_nExtractFailed;       ///< Count of failed \p extract() call
            event_counter   m_nRemoveWaitShrinking; ///< ount of waiting until shrinking completed during \p erase() or \p extract() call
            event_counter   m_nRemoveRootWaitShrinking;  ///< Count of waiting until root shrinking completed duting \p erase() or \p extract() call
            event_counter   m_nMakeRoutingNode;     ///< How many nodes were converted to routing (valueless) nodes

            event_counter   m_nRightRotation;       ///< Count of single right rotation
            event_counter   m_nLeftRotation;        ///< Count of single left rotation
            event_counter   m_nLeftRightRotation;   ///< Count of double left-over-right rotation
            event_counter   m_nRightLeftRotation;   ///< Count of double right-over-left rotation

            event_counter   m_nRotateAfterRightRotation; ///< Count of rotation required after single right rotation
            event_counter   m_nRemoveAfterRightRotation; ///< Count of removal required after single right rotation
            event_counter   m_nDamageAfterRightRotation; ///< Count of damaged node after single right rotation

            event_counter   m_nRotateAfterLeftRotation;  ///< Count of rotation required after signle left rotation
            event_counter   m_nRemoveAfterLeftRotation;  ///< Count of removal required after single left rotation
            event_counter   m_nDamageAfterLeftRotation;  ///< Count of damaged node after single left rotation

            event_counter   m_nRotateAfterRLRotation;    ///< Count of rotation required after right-over-left rotation
            event_counter   m_nRemoveAfterRLRotation;    ///< Count of removal required after right-over-left rotation
            event_counter   m_nRotateAfterLRRotation;    ///< Count of rotation required after left-over-right rotation
            event_counter   m_nRemoveAfterLRRotation;    ///< Count of removal required after left-over-right rotation

            event_counter   m_nInsertRebalanceReq;  ///< Count of rebalance required after inserting
            event_counter   m_nRemoveRebalanceReq;  ///< Count of rebalance required after removing

            //@cond
            void onFindSuccess()        { ++m_nFindSuccess      ; }
            void onFindFailed()         { ++m_nFindFailed       ; }
            void onFindRetry()          { ++m_nFindRetry        ; }
            void onFindWaitShrinking()  { ++m_nFindWaitShrinking; }

            void onInsertSuccess()          { ++m_nInsertSuccess; }
            void onInsertFailed()           { ++m_nInsertFailed; }
            void onRelaxedInsertFailed()    { ++m_nRelaxedInsertFailed; }
            void onInsertRetry()            { ++m_nInsertRetry      ; }
            void onUpdateWaitShrinking()    { ++m_nUpdateWaitShrinking; }
            void onUpdateRetry()            { ++m_nUpdateRetry; }
            void onUpdateRootWaitShrinking() { ++m_nUpdateRootWaitShrinking; }
            void onUpdateSuccess()          { ++m_nUpdateSuccess;  }
            void onUpdateUnlinked()         { ++m_nUpdateUnlinked; }
            void onDisposeNode()            { ++m_nDisposedNode; }
            void onDisposeValue()           { ++m_nDisposedValue; }
            void onExtractValue()           { ++m_nExtractedValue; }
            void onRemove(bool bSuccess)
            {
                if ( bSuccess )
                    ++m_nRemoveSuccess;
                else
                    ++m_nRemoveFailed;
            }
            void onExtract( bool bSuccess )
            {
                if ( bSuccess )
                    ++m_nExtractSuccess;
                else
                    ++m_nExtractFailed;
            }
            void onRemoveRetry()            { ++m_nRemoveRetry; }
            void onRemoveWaitShrinking()    { ++m_nRemoveWaitShrinking; }
            void onRemoveRootWaitShrinking() { ++m_nRemoveRootWaitShrinking; }
            void onMakeRoutingNode()        { ++m_nMakeRoutingNode; }

            void onRotateRight()            { ++m_nRightRotation; }
            void onRotateLeft()             { ++m_nLeftRotation; }
            void onRotateRightOverLeft()    { ++m_nRightLeftRotation; }
            void onRotateLeftOverRight()    { ++m_nLeftRightRotation; }

            void onRotateAfterRightRotation() { ++m_nRotateAfterRightRotation; }
            void onRemoveAfterRightRotation() { ++m_nRemoveAfterRightRotation; }
            void onDamageAfterRightRotation() { ++m_nDamageAfterRightRotation; }

            void onRotateAfterLeftRotation()  { ++m_nRotateAfterLeftRotation; }
            void onRemoveAfterLeftRotation()  { ++m_nRemoveAfterLeftRotation; }
            void onDamageAfterLeftRotation()  { ++m_nDamageAfterLeftRotation; }

            void onRotateAfterRLRotation()    { ++m_nRotateAfterRLRotation; }
            void onRemoveAfterRLRotation()    { ++m_nRemoveAfterRLRotation; }
            void onRotateAfterLRRotation()    { ++m_nRotateAfterLRRotation; }
            void onRemoveAfterLRRotation()    { ++m_nRemoveAfterLRRotation; }

            void onInsertRebalanceRequired() { ++m_nInsertRebalanceReq; }
            void onRemoveRebalanceRequired() { ++m_nRemoveRebalanceReq; }
            //@endcond
        };

        /// BronsonAVLTreeMap empty statistics
        struct empty_stat {
            //@cond
            void onFindSuccess()        const {}
            void onFindFailed()         const {}
            void onFindRetry()          const {}
            void onFindWaitShrinking()  const {}

            void onInsertSuccess()          const {}
            void onInsertFailed()           const {}
            void onRelaxedInsertFailed()    const {}
            void onInsertRetry()            const {}
            void onUpdateWaitShrinking()    const {}
            void onUpdateRetry()            const {}
            void onUpdateRootWaitShrinking() const {}
            void onUpdateSuccess()          const {}
            void onUpdateUnlinked()         const {}
            void onDisposeNode()            const {}
            void onDisposeValue()           const {}
            void onExtractValue()           const {}
            void onRemove(bool /*bSuccess*/) const {}
            void onExtract(bool /*bSuccess*/) const {}
            void onRemoveRetry()            const {}
            void onRemoveWaitShrinking()    const {}
            void onRemoveRootWaitShrinking() const {}
            void onMakeRoutingNode()        const {}

            void onRotateRight()            const {}
            void onRotateLeft()             const {}
            void onRotateRightOverLeft()    const {}
            void onRotateLeftOverRight()    const {}

            void onRotateAfterRightRotation() const {}
            void onRemoveAfterRightRotation() const {}
            void onDamageAfterRightRotation() const {}

            void onRotateAfterLeftRotation()  const {}
            void onRemoveAfterLeftRotation()  const {}
            void onDamageAfterLeftRotation()  const {}

            void onRotateAfterRLRotation()    const {}
            void onRemoveAfterRLRotation()    const {}
            void onRotateAfterLRRotation()    const {}
            void onRemoveAfterLRRotation()    const {}

            void onInsertRebalanceRequired() const {}
            void onRemoveRebalanceRequired() const {}
            //@endcond
        };

        /// Option to allow relaxed insert into \ref cds_container_BronsonAVLTreeMap_rcu "Bronson et al AVL-tree"
        /**
            By default, this option is disabled and the new node is created under its parent lock.
            In this case, it is guaranteed the new node will be attached to its parent.
            On the other hand, constructing of the new node can be too complex to make it under the lock,
            that can lead to lock contention.

            When this option is enabled, the new node is created before locking the parent node.
            After that, the parent is locked and checked whether the new node can be attached to the parent.
            In this case, false node creating can be performed, but locked section can be significantly small.
        */
        template <bool Enable>
        struct relaxed_insert {
            //@cond
            template <typename Base> struct pack : public Base
            {
                enum { relaxed_insert = Enable };
            };
            //@endcond
        };

        /// \p BronsonAVLTreeMap traits
        /**
            Note that there are two main specialization of Bronson et al AVL-tree:
            - \ref cds_container_BronsonAVLTreeMap_rcu_ptr "pointer-oriented" - the tree node stores an user-provided pointer to value
            - \ref cds_container_BronsonAVLTreeMap_rcu "data-oriented" - the tree node contains a copy of values

            Depends on tree specialization, different traits member can be used.
        */
        struct traits
        {
            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.

                See \p cds::opt::compare option description for functor interface.

                You should provide \p compare or \p less functor.
            */
            typedef opt::none                       compare;

            /// Specifies binary predicate used for key compare.
            /**
                See \p cds::opt::less option description for predicate interface.

                You should provide \p compare or \p less functor.
            */
            typedef opt::none                       less;

            /// Allocator for internal node
            typedef CDS_DEFAULT_ALLOCATOR           node_allocator;

            /// Allocator for node's value (not used in \p BronsonAVLTreeMap<RCU, Key, T*, Traits> specialisation)
            typedef CDS_DEFAULT_ALLOCATOR           allocator;

            /// Disposer (only for pointer-oriented tree specialization)
            /**
                The functor used for dispose removed values.
                The user-provided disposer is used only for pointer-oriented tree specialization
                like \p BronsonAVLTreeMap<GC, Key, T*, Traits>. When the node becomes the routing node without value,
                the disposer will be called to signal that the memory for the value can be safely freed.
                Default is \ref cds::intrusive::opt::delete_disposer "cds::container::opt::v::delete_disposer<>" which calls \p delete operator.
            */
            typedef opt::v::delete_disposer<>       disposer;

            /// @ref cds_sync_monitor "Synchronization monitor" type for node-level locking
            typedef cds::sync::injecting_monitor<cds::sync::spin>  sync_monitor;

            /// Enable relaxed insertion.
            /**
                About relaxed insertion see \p bronson_avltree::relaxed_insert option.
                By default, this option is disabled.
            */
            static bool const relaxed_insert = false;

            /// Item counter
            /**
                The type for item counter, by default it is disabled (\p atomicity::empty_item_counter).
                To enable it use \p atomicity::item_counter or \p atomicity::cache_friendly_item_counter.
            */
            typedef atomicity::empty_item_counter     item_counter;

            /// C++ memory ordering model
            /**
                List of available memory ordering see \p opt::memory_model
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p bronson_avltree::empty_stat).
                To enable it use \p bronson_avltree::stat.
            */
            typedef empty_stat                      stat;

            /// Back-off strategy
            typedef cds::backoff::empty             back_off;

            /// RCU deadlock checking policy
            /**
                List of available options see \p opt::rcu_check_deadlock
            */
            typedef cds::opt::v::rcu_throw_deadlock      rcu_check_deadlock;
        };

        /// Metafunction converting option list to BronsonAVLTreeMap traits
        /**
            Note that there are two main specialization of Bronson et al AVL-tree:
            - \ref cds_container_BronsonAVLTreeMap_rcu_ptr "pointer-oriented" - the tree node stores an user-provided pointer to value
            - \ref cds_container_BronsonAVLTreeMap_rcu "data-oriented" - the tree node contains a copy of values

            Depends on tree specialization, different options can be specified.

            \p Options are:
            - \p opt::compare - key compare functor. No default functor is provided.
                If the option is not specified, \p %opt::less is used.
            - \p opt::less - specifies binary predicate used for key compare. At least \p %opt::compare or \p %opt::less should be defined.
            - \p opt::node_allocator - the allocator for internal nodes. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - \p opt::allocator - the allocator for node's value. Default is \ref CDS_DEFAULT_ALLOCATOR.
                This option is not used in \p BronsonAVLTreeMap<RCU, Key, T*, Traits> specialisation
            - \p cds::intrusive::opt::disposer - the functor used for dispose removed values.
                The user-provided disposer is used only for pointer-oriented tree specialization
                like \p BronsonAVLTreeMap<GC, Key, T*, Traits>. When the node becomes the rounting node without value,
                the disposer will be called to signal that the memory for the value can be safely freed.
                Default is \p cds::intrusive::opt::delete_disposer which calls \p delete operator.
                Due the nature of GC schema the disposer may be called asynchronously.
            - \p opt::sync_monitor -  @ref cds_sync_monitor "synchronization monitor" type for node-level locking,
                default is \p cds::sync::injecting_monitor<cds::sync::spin>
            - \p bronson_avltree::relaxed_insert - enable (\p true) or disable (\p false, the default)
                @ref bronson_avltree::relaxed_insert "relaxed insertion"
            - \p opt::item_counter - the type of item counting feature, by default it is disabled (\p atomicity::empty_item_counter)
                To enable it use \p atomicity::item_counter or \p atomicity::cache_friendly_item_counter
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p opt::stat - internal statistics, by default it is disabled (\p bronson_avltree::empty_stat)
                To enable statistics use \p \p bronson_avltree::stat
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
    } // namespace bronson_avltree

    // Forwards
    template < class GC, typename Key, typename T, class Traits = bronson_avltree::traits >
    class BronsonAVLTreeMap;

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_BRONSON_AVLTREE_BASE_H

//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_IMPL_BRONSON_AVLTREE_MAP_RCU_H
#define CDSLIB_CONTAINER_IMPL_BRONSON_AVLTREE_MAP_RCU_H

#include <cds/container/details/bronson_avltree_base.h>
#include <cds/urcu/details/check_deadlock.h>

namespace cds { namespace container {

    /// Bronson et al AVL-tree (RCU specialization for storing pointer to values)
    /** @ingroup cds_nonintrusive_map
        @ingroup cds_nonintrusive_tree
        @headerfile cds/container/bronson_avltree_map_rcu.h

        This is the specialization of \ref cds_container_BronsonAVLTreeMap_rcu "RCU-based Bronson et al AVL-tree"
        for "key -> value pointer" map. This specialization stores the pointer to user-allocated values instead of the copy
        of the value. When a tree node is removed, the algorithm does not free the value pointer directly, instead, it call
        the disposer functor provided by \p Traits template parameter.

        <b>Template arguments</b>:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - key type
        - \p T - value type to be stored in tree's nodes. Note, the specialization stores the pointer to user-allocated
            value, not the copy.
        - \p Traits - tree traits, default is \p bronson_avltree::traits
            It is possible to declare option-based tree with \p bronson_avltree::make_traits metafunction
            instead of \p Traits template argument.

        @note Before including <tt><cds/container/bronson_avltree_map_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.
    */
    template <
        typename RCU,
        typename Key,
        typename T,
#   ifdef CDS_DOXYGEN_INVOKED
        typename Traits = bronson_avltree::traits
#else
        typename Traits
#endif
    >
    class BronsonAVLTreeMap< cds::urcu::gc<RCU>, Key, T*, Traits >
    {
    public:
        typedef cds::urcu::gc<RCU>  gc;   ///< RCU Garbage collector
        typedef Key     key_type;    ///< type of a key stored in the map
        typedef T *     mapped_type; ///< type of value stored in the map
        typedef Traits  traits;      ///< Traits template parameter

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator;    ///< key compare functor based on \p Traits::compare and \p Traits::less
#   else
        typedef typename opt::details::make_comparator< key_type, traits >::type key_comparator;
#endif
        typedef typename traits::item_counter           item_counter;       ///< Item counting policy
        typedef typename traits::memory_model           memory_model;       ///< Memory ordering, see \p cds::opt::memory_model option
        typedef typename traits::node_allocator         node_allocator_type; ///< allocator for maintaining internal nodes
        typedef typename traits::stat                   stat;               ///< internal statistics
        typedef typename traits::rcu_check_deadlock     rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename traits::back_off               back_off;           ///< Back-off strategy
        typedef typename traits::disposer               disposer;           ///< Value disposer
        typedef typename traits::lock_type              lock_type;          ///< Node lock type

        /// Enabled or disabled @ref bronson_avltree::relaxed_insert "relaxed insertion"
        static bool const c_bRelaxedInsert = traits::relaxed_insert;

    protected:
        //@cond
        typedef bronson_avltree::node< key_type, mapped_type, lock_type > node_type;
        typedef typename node_type::version_type version_type;

        typedef cds::details::Allocator< node_type, node_allocator_type > cxx_allocator;
        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock >   check_deadlock_policy;

        enum class find_result
        {
            not_found,
            found,
            retry
        };

        enum class update_flags
        {
            allow_insert = 1,
            allow_update = 2,
            retry = 4,

            failed = 0,
            result_insert = allow_insert,
            result_update = allow_update
        };

        enum node_condition
        {
            nothing_required = -3,
            rebalance_required = -2,
            unlink_required = -1
        };

        class node_scoped_lock
        {
            node_type *     m_pNode;
        public:
            node_scoped_lock( node_type * pNode )
                : m_pNode( pNode )
            {
                pNode->m_Lock.lock();
            }

            ~node_scoped_lock()
            {
                m_pNode->m_Lock.unlock();
            }
        };

        //@endcond

    protected:
        //@cond
        template <typename K>
        static node_type * alloc_node( K&& key, int nHeight, version_type version, node_type * pParent, node_type * pLeft, node_type * pRight )
        {
            return cxx_allocator().New( std::forward<K>( key ), nHeight, version, pParent, pLeft, pRight );
        }

        static void free_node( node_type * pNode )
        {
            // Free node without disposer
            cxx_allocator().Delete( pNode );
        }

        // RCU safe disposer
        class rcu_disposer
        {
            node_type *     m_pRetiredList; ///< head of retired list

        public:
            rcu_disposer()
                : m_pRetiredList( nullptr )
            {}

            ~rcu_disposer()
            {
                clean();
            }

            void dispose( node_type * pNode )
            {
                mapped_type * pVal = pNode->value( memory_model::memory_order_relaxed );
                if ( pVal ) {
                    pNode->m_pValue.store( nullptr, memory_model::memory_order_relaxed );
                    disposer()(pVal);
                }

                pNode->m_pNextRemoved = m_pRetiredList;
                m_pRetiredList = pNode;
            }

        private:
            struct internal_disposer
            {
                void operator()( node_type * p ) const
                {
                    free_node( p );
                }
            };

            void clean()
            {
                assert( !gc::is_locked() );

                for ( node_type * p = m_pRetiredList; p; ) {
                    node_type * pNext = p->m_pNextRemoved;
                    // Value already disposed
                    gc::template retire_ptr<internal_disposer>( p );
                    p = pNext;
                }
            }
        };

        //@endcond

    protected:
        //@cond
        typename node_type::base_class      m_Root;
        node_type *                         m_pRoot;
        item_counter                        m_ItemCounter;
        mutable stat                        m_stat;
        //@endcond

    public:
        /// Creates empty map
        BronsonAVLTreeMap()
            : m_pRoot( static_cast<node_type *>(&m_Root) )
        {}

        /// Destroys the map
        ~BronsonAVLTreeMap()
        {
            unsafe_clear();
        }

        /// Inserts new node
        /**
            The \p key_type should be constructible from a value of type \p K.

            RCU \p synchronize() can be called. RCU should not be locked.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K const& key, mapped_type * pVal )
        {
            return do_update(key, key_comparator(),
                [pVal]( node_type * pNode ) -> mapped_type* 
                {
                    assert( pNode->m_pValue.load( memory_model::memory_order_relaxed ) == nullptr );
                    return pVal;
                }, 
                update_flags::allow_insert 
            ) == update_flags::result_insert;
        }

        /// Updates the value for \p key
        /**
            The operation performs inserting or updating the value for \p key with lock-free manner.
            If \p bInsert is \p false, only updating of existing node is possible.

            If \p key is not found and inserting is allowed (i.e. \p bInsert is \p true),
            then the new node created from \p key is inserted into the map; note that in this case the \ref key_type should be
            constructible from type \p K.
            Otherwise, the value is changed to \p pVal.

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successfull,
            \p second is \p true if new node has been added or \p false if the node with \p key
            already is in the tree.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, mapped_type * pVal, bool bInsert = true )
        {
            int result = do_update( key, key_comparator(),
                [pVal]( node_type * ) -> mapped_type* 
                {
                    return pVal;
                },
                update_flags::allow_update | (bInsert ? update_flags::allow_insert : 0) 
            );
            return std::make_pair( result != 0, (result & update_flags::result_insert) != 0 );
        }

        /// Delete \p key from the map
        /**
            RCU \p synchronize() method can be called. RCU should not be locked.

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            //TODO
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \p erase(K const&)
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool erase_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            //TODO
        }

        /// Delete \p key from the map
        /**
            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(mapped_type& item) { ... }
            };
            \endcode

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            //TODO
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \p erase(K const&, Func)
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            //TODO
        }

        /// Extracts an item with minimal key from the map
        /**
            Returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the leftmost item.
            If the set is empty, returns empty \p exempt_ptr.

            @note Due the concurrent nature of the map, the function extracts <i>nearly</i> minimum key.
            It means that the function gets leftmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key less than leftmost item's key.
            So, the function returns the item with minimum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not free the item.
            The deallocator will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        exempt_ptr extract_min()
        {
            //TODO
        }

        /// Extracts an item with maximal key from the map
        /**
            Returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the rightmost item.
            If the set is empty, returns empty \p exempt_ptr.

            @note Due the concurrent nature of the map, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key great than leftmost item's key.
            So, the function returns the item with maximum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not free the item.
            The deallocator will be implicitly invoked when the returned object is destroyed or when
            its \p release() is called.
            @note Before reusing \p result object you should call its \p release() method.
        */
        exempt_ptr extract_max()
        {
            //TODO
        }

        /// Extracts an item from the map
        /**
            The function searches an item with key equal to \p key in the tree,
            unlinks it, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to an item found.
            If \p key is not found the function returns an empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not destroy the item found.
            The dealloctor will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            //TODO
        }

        /// Extracts an item from the map using \p pred for searching
        /**
            The function is an analog of \p extract(exempt_ptr&, Q const&)
            but \p pred is used for key compare.
            \p Less has the interface like \p std::less and should meet \ref cds_container_EllenBinTreeSet_rcu_less
            "predicate requirements".
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& val, Less pred )
        {
            CDS_UNUSED( pred );
            //TODO
        }

        /// Find the key \p key
        /**
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( mapped_type& item );
            };
            \endcode
            where \p item is the item found.
            The functor is called under node-level lock.

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return do_find( key, key_comparator(), [&f]( node_type * pNode ) -> bool {
                node_scoped_lock l( pNode );
                mapped_type * pVal = pNode->m_pValue.load( memory_model::memory_order_relaxed );
                if ( pVal ) {
                    f( *pVal );
                    return true;
                }
                return false;
            });
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \p find(K const&, Func)
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool find_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return do_find( key, opt::details::make_comparator_from_less<Less>(), [&f]( node_type * pNode ) -> bool {
                node_scoped_lock l( pNode );
                mapped_type * pVal = pNode->m_pValue.load( memory_model::memory_order_relaxed );
                if ( pVal ) {
                    f( *pVal );
                    return true;
                }
                return false;
            } );
        }

        /// Find the key \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename K>
        bool find( K const& key )
        {
            return do_find( key, key_comparator(), []( node_type * ) -> bool { return true; });
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \p find(K const&)
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool find_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return do_find( key, opt::details::make_comparator_from_less<Less>(), []( node_type * ) -> bool { return true; } );
        }

        /// Finds \p key and return the item found
        /**
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            RCU should be locked before call the function.
            Returned pointer is valid while RCU is locked.
        */
        template <typename Q>
        mapped_type * get( Q const& key ) const
        {
            //TODO
        }

        /// Finds \p key with \p pred predicate and return the item found
        /**
            The function is an analog of \p get(Q const&)
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \p key_type
            and \p Q in any order.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename Q, typename Less>
        mapped_type * get_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            //TODO
        }

        /// Clears the tree (thread safe, not atomic)
        /**
            The function unlink all items from the tree.
            The function is thread safe but not atomic: in multi-threaded environment with parallel insertions
            this sequence
            \code
            set.clear();
            assert( set.empty() );
            \endcode
            the assertion could be raised.

            For each node the \ref disposer will be called after unlinking.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        void clear()
        {
            for ( exempt_ptr ep = extract_min(); !ep.empty(); ep = extract_min() )
                ep.release();
        }

        /// Clears the tree (not thread safe)
        /**
            This function is not thread safe and may be called only when no other thread deals with the tree.
            The function is used in the tree destructor.
        */
        void unsafe_clear()
        {
            //TODO
        }

        /// Checks if the map is empty
        bool empty() const
        {
            return m_Root.m_pRight.load( memory_model::memory_order_relaxed ) == nullptr;
        }

        /// Returns item count in the map
        /**
            Only leaf nodes containing user data are counted.

            The value returned depends on item counter type provided by \p Traits template parameter.
            If it is \p atomicity::empty_item_counter this function always returns 0.

            The function is not suitable for checking the tree emptiness, use \p empty()
            member function for this purpose.
        */
        size_t size() const
        {
            return m_ItemCounter;
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return m_stat;
        }

        /// Checks internal consistency (not atomic, not thread-safe)
        /**
            The debugging function to check internal consistency of the tree.
        */
        bool check_consistency() const
        {
            //TODO
        }

    protected:
        //@cond
        template <typename Q, typename Compare, typename Func>
        bool do_find( Q& key, Compare cmp, Func f ) const
        {
            find_result result;
            {
                rcu_lock l;
                result = try_find( key, cmp, f, m_pRoot, 1, 0 );
            }
            assert( result != find_result::retry );
            return result == find_result::found;
        }

        template <typename K, typename Compare, typename Func>
        int do_update( K const& key, Compare cmp, Func funcUpdate, int nFlags )
        {
            check_deadlock_policy::check();

            rcu_disposer removed_list;
            {
                rcu_lock l;
                return try_udate_root( key, cmp, nFlags, funcUpdate, removed_list );
            }
        }

        //@endcond

    private:
        //@cond
        template <typename Q, typename Compare, typename Func>
        find_result try_find( Q const& key, Compare cmp, Func f, node_type * pNode, int nDir, version_type nVersion ) const
        {
            assert( gc::is_locked() );
            assert( pNode );

            while ( true ) {
                node_type * pChild = pNode->child( nDir ).load( memory_model::memory_order_relaxed );
                if ( !pChild ) {
                    if ( pNode->version( memory_model::memory_order_acquire ) != nVersion ) {
                        m_stat.onFindRetry();
                        return find_result::retry;
                    }

                    m_stat.onFindFailed();
                    return find_result::not_found;
                }

                int nCmp = cmp( key, pChild->m_key );
                if ( nCmp == 0 ) {
                    if ( pChild->is_valued( memory_model::memory_order_relaxed ) ) {
                        // key found
                        if ( f( pChild ) ) {
                            m_stat.onFindSuccess();
                            return find_result::found;
                        }
                    }

                    m_stat.onFindFailed();
                    return find_result::not_found;
                }

                version_type nChildVersion = pChild->version( memory_model::memory_order_acquire );
                if ( nChildVersion & node_type::shrinking ) {
                    m_stat.onFindWaitShrinking();
                    pChild->template wait_until_shrink_completed<back_off>( memory_model::memory_order_relaxed );

                    if ( pNode->version( memory_model::memory_order_acquire ) != nVersion ) {
                        m_stat.onFindRetry();
                        return find_result::retry;
                    }
                }
                else if ( nChildVersion != node_type::unlinked ) {

                    if ( pNode->version( memory_model::memory_order_acquire ) != nVersion ) {
                        m_stat.onFindRetry();
                        return find_result::retry;
                    }

                    find_result found = try_find( key, cmp, f, pChild, nCmp, nChildVersion );
                    if ( found != find_result::retry )
                        return found;
                }
            }
        }

        template <typename K, typename Compare, typename Func>
        int try_update_root( K const* key, Compare cmp, int nFlags, Func funcUpdate, rcu_disposer& disp )
        {
            assert( gc::is_locked() );

            int result;
            do {
                node_type * pChild = m_Root.child( 1, memory_model::memory_order_acquire );
                if ( pChild ) {
                    version_type nChildVersion = pChild->version( memory_model::memory_order_relaxed );
                    if ( nChildVersion & node_type::shrinking ) {
                        m_stat.onUpdateRootWaitShrinking();
                        pChild->template wait_until_shrink_completed<back_off>( memory_model::memory_order_relaxed );
                        // retry
                    }
                    else if ( pChild == m_Root.child( 1, memory_model::memory_order_acquire ) ) {
                        result = try_update( key, cmp, nFlags, funcUpdate, m_pRoot, pChild, nChildVersion, disp );
                    }
                } 
                else {
                    // the tree is empty
                    if ( nFlags & update_flags::allow_insert ) {
                        // insert into tree as right child of the root
                        {
                            node_scoped_lock l( m_pRoot );

                            node_type * pNew = alloc_node( key, 1, 0, m_pRoot, nullptr, nullptr );
                            mapped_type * pVal = funcUpdate( pNew );
                            assert( pVal != nullptr );
                            pNew->m_pValue.store( pVal, memory_model::memory_order_release );

                            m_pRoot->child( pNew, 1, memory_model::memory_order_relaxed );
                            m_pRoot->height( 2, memory_model::memory_order_relaxed );
                        }

                        ++m_ItemCounter;
                        m_stat.onInsertSuccess();
                        return update_flags::result_insert;
                    }

                    return update_flags::failed;
                }
            } while ( result != update_flags::retry );
            return result;
        }

        template <typename K, typename Compare, typename Func>
        int try_update( K const& key, Compare cmp, int nFlags, Func funcUpdate, node_type * pParent, node_type * pNode, version_type nVersion, rcu_disposer& disp )
        {
            assert( gc::is_locked() );
            assert( nVersion != node_type::unlinked );

            int nCmp = cmp( key, pNode->m_key );
            if ( nCmp == 0 ) {
                if ( nFlags & update_flags::allow_update ) {
                    return try_update_node( funcUpdate, pNode );
                }
                return update_flags::failed;
            }

            int result;
            do {
                node_type * pChild = pNode->child( nCmp ).load( memory_model::memory_order_relaxed );
                if ( pNode->version( memory_model::memory_order_acquire ) != nVersion )
                    return update_flags::retry;

                if ( pChild == nullptr ) {
                    // insert new node
                    if ( nFlags & update_flags::allow_insert )
                        result = try_insert_node( key, funcUpdate, pNode, nCmp, nVersion, disp );
                    else
                        result = update_flags::failed;
                }
                else {
                    // update child
                    result = update_flags::retry;
                    version_type nChildVersion = pChild->version( memory_model::memory_order_acquire );
                    if ( nChildVersion & node_type::shrinking ) {
                        m_stat.onUpdateWaitShrinking();
                        pChild->template wait_until_shrink_completed<back_off>( memory_model::memory_order_relaxed );
                        // retry
                    }
                    else if ( pChild == pNode->child( nCmp ).load( memory_model::memory_order_relaxed ) ) {
                        // this second read is important, because it is protected by nChildVersion

                        // validate the read that our caller took to get to node
                        if ( pNode->version( memory_model::memory_order_relaxed ) != nVersion ) {
                            m_stat.onUpdateRetry();
                            return update_flags::retry;
                        }

                        // At this point we know that the traversal our parent took to get to node is still valid.
                        // The recursive implementation will validate the traversal from node to
                        // child, so just prior to the node nVersion validation both traversals were definitely okay.
                        // This means that we are no longer vulnerable to node shrinks, and we don't need
                        // to validate node version any more.
                        result = try_update( key, cmp, nFlags, funcUpdate, pNode, pChild, nChildVersion, disp );
                    }
                }
            } while ( result == update_flags::retry );
            return result;
        }

        template <typename K, typename Func>
        int try_insert_node( K const& key, Func funcUpdate, node_type * pNode, int nDir, version_type nVersion, rcu_disposer& disp )
        {
            node_type * pNew;

            auto fnCreateNode = [&funcUpdate]( node_type * pNew ) {
                mapped_type * pVal = funcUpdate( pNew );
                assert( pVal != nullptr );
                pNew->m_pValue.store( pVal, memory_model::memory_order_relaxed );
            };

            if ( c_bRelaxedInsert ) {
                if ( pNode->version( memory_model::memory_order_acquire ) != nVersion
                     || pNode->child( nDir ).load( memory_model::memory_order_relaxed ) != nullptr ) 
                {
                    m_stat.onInsertRetry();
                    return update_flags::retry;
                }

                fnCreateNode( pNew = alloc_node( key, 1, 0, pNode, nullptr, nullptr ));
            }

            node_type * pDamaged;
            {
                node_scoped_lock l( pNode );

                if ( pNode->version( memory_model::memory_order_relaxed ) != nVersion
                     || pNode->child( nDir ).load( memory_model::memory_order_relaxed ) != nullptr ) 
                {
                    if ( c_RelaxedInsert ) {
                        free_node( pNew );
                        m_stat.onRelaxedInsertFailed();
                    }

                    m_stat.onInsertRetry();
                    return update_flags::retry;
                }

                if ( !c_bRelaxedInsert )
                    fnCreateNode( pNew = alloc_node( key, 1, 0, pNode, nullptr, nullptr ));

                pNode->child( pNew, nDir, memory_model::memory_order_relaxed );
                pDamaged = fix_height_locked( pNode );
            }
            m_stat.onInsertSuccess();

            fix_height_and_rebalance( pDamaged, disp );

            return update_flags::result_insert;
        }

        template <typename Func>
        int try_update_node( Func funcUpdate, node_type * pNode )
        {
            mapped_type * pOld;
            {
                node_scoped_lock l( pNode );

                if ( pNode->version( memory_model::memory_order_relaxed ) == node_type::unlinked ) {
                    if ( c_RelaxedInsert )
                        disposer()(pVal);
                    m_stat.onUpdateUnlinked();
                    return update_flags::retry;
                }

                pOld = pNode->value( memory_model::memory_order_relaxed );
                mapped_type * pVal = funcUpdate( pNode );
                assert( pVal != nullptr );
                pNode->m_pValue.store( pVal, memory_model::memory_order_relaxed ));
            }

            if ( pOld ) {
                disposer()(pOld);
                m_stat.onDisposeValue();
            }

            m_stat.onUpdateSuccess();
            return update_flags::result_update;
        }

        bool try_unlink_locked( node_type * pParent, node_type * pNode, rcu_disposer& disp )
        {
            // pParent and pNode must be locked
            assert( !pParent->is_unlinked(memory_model::memory_order_relaxed) );

            node_type * pParentLeft = pParent->m_pLeft.load( memory_model::memory_order_relaxed );
            node_type * pParentRight = pParent->m_pRight.load( memory_model::memory_order_relaxed );
            if ( pNode != pParentLeft && pNode != pParentRight ) {
                // node is no longer a child of parent
                return false;
            }

            assert( !pNode->is_unlinked());
            assert( pParent == pNode->m_pParent.load(memory_model::memory_order_relaxed));

            node_type * pLeft = pNode->m_pLeft.load( memory_model::memory_order_relaxed );
            node_type * pRight = pNode->m_pRight.load( memory_model::memory_order_relaxed );
            if ( pLeft != nullptr && pRight != nullptr ) {
                // splicing is no longer possible
                return false;
            }
            node_type * pSplice = pLeft ? pLeft : pRight;

            if ( pParentLeft == pNode )
                pParent->m_pLeft.store( pSplice, memory_model::memory_order_relaxed );
            else
                pParent->m_pRight.store( pSplice, memory_model::memory_order_relaxed );

            if ( pSplice )
                pSplice->pParent.store( pParent, memory_model::memory_order_release );

            // Mark the node as unlinked
            pNode->version( node_type::unlinked, memory_model::memory_order_release );
            disp.dispose( pNode );

            return true;
        }

        //@endcond
    private: // rotations
        //@cond
        int estimate_node_condition( node_type * pNode )
        {
            node_type * pLeft = pNode->m_pLeft.load( memory_model::memory_order_relaxed );
            node_type * pRight = pNode->m_pRight.load( memory_model::memory_order_relaxed );

            if ( (pLeft == nullptr || pRight == nullptr) && pNode->value( memory_model::memory_order_relaxed ) == nullptr )
                return unlink_required;

            int h = pNode->height( memory_model::memory_order_relaxed );
            int hL = pLeft ? pLeft->height( memory_model::memory_order_relaxed ) : 0;
            int hR = pRight ? pRight->height( memory_model::memory_order_relaxed ) : 0;

            int hNew = 1 + std::max( hL, hR );
            int nBalance = hL - hR;

            if ( nBalance < -1 || nBalance > 1 )
                return rebalance_required;

            return h != hNew ? hNew : nothing_required;
        }

        node_type * fix_height( node_type * pNode )
        {
            node_scoped_lock l( pNode );
            return fix_height_locked( pNode );
        }

        node_type * fix_height_locked( node_type * pNode )
        {
            // pNode must be locked!!!
            int h = estimate_node_condition( pNode );
            switch ( h ) {
                case rebalance_required:
                case unlink_required:
                    return pNode;
                case nothing_required:
                    return nullptr;
                default:
                    pNode->height( h, memory_model::memory_order_relaxed );
                    return pNode->m_pParent.load( memory_model::memory_order_relaxed );
            }
        }

        void fix_height_and_rebalance( node_type * pNode, rcu_disposer& disp )
        {
            while ( pNode && pNode->m_pParent.load( memory_model::memory_order_relaxed ) ) {
                int nCond = estimate_node_condition( pNode );
                if ( nCond == nothing_required || pNode->is_unlinked( memory_model::memory_order_relaxed ) )
                    return;

                if ( nCond != unlink_required && nCond != rebalance_required )
                    pNode = fix_height( pNode );
                else {
                    node_type * pParent = pNode->m_pParent.load( memory_model::memry_order_relaxed );
                    assert( pParent != nullptr );
                    {
                        node_scoped_lock lp( pParent );
                        if ( !pParent->is_unlinked( memory_model::memory_order_relaxed )
                             && pNode->m_pParent.load( memory_model::memory_order_relaxed ) == pParent ) 
                        {
                            node_scoped_lock ln( pNode );
                            pNode = rebalance_locked( pParent, pNode, disp );
                        }
                    }
                }
            }
        }

        node_type * rebalance_locked( node_type * pParent, node_type * pNode, rcu_disposer& disp )
        {
            // pParent and pNode shlould be locked.
            // Returns a damaged node, or nullptr if no more rebalancing is necessary
            assert( pNode->m_pParent.load( memory_model::memry_order_relaxed ) == pNode );
            assert( m_pParent->m_pLeft.load( memory_model::memory_order_relaxed ) == pNode
                 || m_pParent->m_pRight.load( memory_model::memory_order_relaxed ) == pNode );

            node_type * pLeft = pNode->m_pLeft.load( memory_model::memory_order_relaxed );
            node_type * pRight = pNode->m_pRight.load( memory_model::memory_order_relaxed );

            if ( (pLeft == nullptr || pRight == nullptr) && pNode->value( memory_model::memory_order_relaxed ) == nullptr ) {
                if ( try_unlink_locked( pParent, pNode, disp ))
                    return fix_height_locked( pParent );
                else {
                    // retry needed for pNode
                    return pNode;
                }
            }

            int h = pNode->height( memory_model::memory_order_relaxed );
            int hL = pLeft ? pLeft->height( memory_model::memory_order_relaxed ) : 0;
            int hR = pRight ? pRight->height( memory_model::memory_order_relaxed ) : 0;
            int hNew = 1 + std::max( hL, hR );
            int balance = hL - hR;

            if ( balance > 1 )
                return rebalance_to_right_locked( pParent, pNode, pLeft, hR );
            else if ( balance < -1 )
                return rebalance_to_left_locked( pParent, pNode, pRight, hL );
            else if ( hNew != h ) {
                pNode->height( hNew, memory_model::memory_order_relaxed );

                // pParent is already locked
                return fix_height_locked( pParent );
            }
            else
                return nullptr;
        }

        node_type * rebalance_to_right_locked( node_type * pParent, node_type * pNode, node_type * pLeft, int hR )
        {
            assert( pNode->m_pParent.load( memory_model::memry_order_relaxed ) == pNode );
            assert( m_pParent->m_pLeft.load( memory_model::memory_order_relaxed ) == pNode
                 || m_pParent->m_pRight.load( memory_model::memory_order_relaxed ) == pNode );

            // pParent and pNode is locked yet
            // pNode->pLeft is too large, we will rotate-right.
            // If pLeft->pRight is taller than pLeft->pLeft, then we will first rotate-left pLeft.

            {
                node_scoped_lock l( pLeft );
                if ( pNode->m_pLeft.load( memory_model::memory_order_relaxed ) != pLeft )
                    return pNode; // retry for pNode

                int hL = pLeft->height( memory_model::memory_order_relaxed );
                if ( hL - hR <= 1 )
                    return pNode; // retry

                node_type * pLRight = pLeft->m_pRight.load( memory_model::memory_order_relaxed );
                int hLR = pLRight ? pLRight->height( memory_model::memory_order_relaxed ) : 0;
                node_type * pLLeft = pLeft->m_pLeft.load( memory_model::memory_order_relaxed );
                int hLL = pLLeft ? pLLeft->height( memory_model::memory_order_relaxed ) : 0;

                if ( hLL > hLR ) {
                    // rotate right
                    return rotate_right_locked( pParent, pNode, pLeft, hR, hLL, pLRight, hLR );
                }
                else {
                    assert( pLRight != nullptr );
                    {
                        node_scoped_lock lr( pLRight );
                        if ( pLeft->m_pRight.load( memory_model::memory_order_relaxed ) != pLRight )
                            return pNode; // retry

                        hLR = pLRight->height( memory_model::memory_order_relaxed );
                        if ( hLL > hLR )
                            return rotate_right_locked( pParent, pNode, pLeft, hR, hLL, pLRight, hLR );

                        node_type * pLRLeft = pLRight->m_pLeft.load( memory_model::memory_order_relaxed );
                        int hLRL = pLRLeft ? pLRLeft->height( memory_model::memory_order_relaxed  ) : 0;
                        int balance = hLL - hLRL;
                        if ( balance >= -1 && balance <= 1 && !((hLL == 0 || hLRL == 0) && pLeft->value(memory_model::memory_order_relaxed) == nullptr) ) {
                            // nParent.child.left won't be damaged after a double rotation
                            return rotate_right_over_left_locked( pParent, pNode, pLeft, hR, hLL, pLRight, hLRL );
                        }
                    }

                    // focus on pLeft, if necessary pNode will be balanced later
                    return rebalance_to_left_locked( pNode, pLeft, pLRight, hLL );
                }
            }
        }

        node_type * rebalance_to_left_locked( node_type * pParent, node_type * pNode, node_type * pRight, int hL )
        {
            assert( pNode->m_pParent.load( memory_model::memry_order_relaxed ) == pNode );
            assert( m_pParent->m_pLeft.load( memory_model::memory_order_relaxed ) == pNode
                    || m_pParent->m_pRight.load( memory_model::memory_order_relaxed ) == pNode );

            // pParent and pNode is locked yet
            {
                node_scoped_lock l( pRight );
                if ( pNode->m_pRight.load( memory_model::memory_order_relaxed ) != pRight )
                    return pNode; // retry for pNode

                int hR = pRight->height( memory_model::memory_order_relaxed );
                if ( hL - hR >= -1 )
                    return pNode; // retry

                node_type * pRLeft = pRight->m_pLeft.load( memory_model::memory_order_relaxed );
                int hRL = pRLeft > pRLeft->height( memory_model::memory_order_relaxed ) : 0;
                node_type * pRRight = pRight->m_pRight.load( memory_model::memory_order_relaxed );
                int hRR = pRRight ? pRRight->height( memory_model::memory_order_relaxed ) : 0;
                if ( hRR > hRL )
                    return rotate_left_locked( pParent, pNode, hL, pRight, pRLeft, hRL, hRR );

                {
                    assert( pRLeft != nullptr );
                    node_scoped_lock lrl( pRLeft );
                    if ( pRight->m_pLeft.load( memory_model::memory_order_relaxed ) != pRLeft )
                        return pNode; // retry

                    hRL = pRLeft->height( memory_model::memory_order_relaxed );
                    if ( hRR >= hRL )
                        return rotate_left_locked( pParent, pNode, hL, pRight, pRLeft, hRL, hRR );

                    node_type * pRLRight = pRLeft->m_pRight.load( memory_model::memory_order_relaxed );
                    int hRLR = pRLRight ? pRLRight->height( memory_model::memory_order_relaxed ) : 0;
                    int balance = hRR - hRLR;
                    if ( b >= -1 && b <= 1 && !((hRR == 0 || hRLR == 0) && pRight->value( memory_odel::memory_order_relaxed ) == nullptr)
                         return rotate_left_over_right_locked( pParent, pNode, hL, pRight, pRLeft, hRR, hRLR );
                }
                return rebalance_to_right_locked( pNode, pRight, pRLeft, hRR );
            }
        }

        static void begin_change( node_type * pNode, version_type version )
        {
            pNode->version( version | node_type::shrinking, memory_model::memory_order_release );
        }
        static void end_change( node_type * pNode, version_type version )
        {
            // Clear shrinking and unlinked flags and increment version
            pNode->version( (version | node_type::version_flags) + 1, memory_model::memory_order_release );
        }

        node_type * rotate_right_locked( node_type * pParent, node_type * pNode, node_type * pLeft, int hR, int hLL, node_type * pLRight, int hLR )
        {
            version_type nodeVersion = pNode->version( memory_model::memory_order_relaxed );
            node_type * pParentLeft = pParent->m_pLeft.load( memory_model::memory_order_relaxed );

            begin_change( pNode, nodeVersion );

            pNode->m_pLeft.store( pLRight, memory_model::memory_order_relaxed );
            if ( pLRight != nullptr )
                pLRight->m_pParent.store( pNode, memory_model::memory_order_relaxed  );

            pLeft->m_pRight.store( pNode, memory_model::memory_order_relaxed );
            pNode->m_pParent.store( pLeft, memory_model::memory_order_relaxed );

            if ( pParentLeft == pNode )
                pParent->m_pLeft.store( pLeft, memory_model::memory_order_relaxed );
            else {
                assert( pParent->m_pRight.load( memory_odel::memory_order_relaxed ) == pNode );
                pParent->m_pRight.store( pLeft, memory_mdel::memory_order_relaxed );
            }
            pLeft->m_pParent.store( pParent, memory_model::memory_order_relaxed );

            // fix up heights links
            int hNode = 1 + std::max( hLR, hR );
            pNode->height( hNode, memory_model::memory_order_relaxed );
            pLeft->height( 1 + std::max( hLL, hNode ), memory_model::memory_order_relaxed );

            end_change( pNode, nodeVersion );

            // We have damaged pParent, pNode (now parent.child.right), and pLeft (now
            // parent.child).  pNode is the deepest.  Perform as many fixes as we can
            // with the locks we've got.

            // We've already fixed the height for pNode, but it might still be outside
            // our allowable balance range.  In that case a simple fix_height_locked()
            // won't help.
            int nodeBalance = hLR - hR;
            if ( nodeBalance < -1 || nodeBalance > 1 ) {
                // we need another rotation at pNode
                return pNode;
            }

            // we've fixed balance and height damage for pNode, now handle
            // extra-routing node damage
            if ( (pLRight == nullptr || hR == 0) && pNode->value(memory_model::memory_order_relaxed) == nullptr ) {
                // we need to remove pNode and then repair
                return pNode;
            }

            // we've already fixed the height at pLeft, do we need a rotation here?
            int leftBalance = hLL - hNode;
            if ( leftBalance < -1 || leftBalance > 1 )
                return pLeft;

            // pLeft might also have routing node damage (if pLeft.left was null)
            if ( hLL == 0 && pLeft->value( memory_model::memory_order_relaxed ) == nullptr )
                return pLeft;

            // try to fix the parent height while we've still got the lock
            return fix_height_locked( pParent );
        }

        node_type * rotate_left_locked( node_type * pParent, node_type * pNode, int hL, node_type * pRight, node_type * pRLeft, int hRL, int hRR )
        {
            version_type nodeVersion = pNode->version( memory_model::memory_order_relaxed );
            node_type * pParentLeft = pParent->m_pLeft.load( memory_odel::memory_order_relaxed );

            begin_change( pNode, nodeVersion );

            // fix up pNode links, careful to be compatible with concurrent traversal for all but pNode
            pNode->m_pRight.store( pRLeft, memory_nodel::memory_order_relaxed );
            if ( pRLeft != nullptr )
                pRLeft->m_pParent.store( pNode, memory_model::memory_order_relaxed );

            pRight->m_pLeft.store( pNode, memory_model::memory_order_relaxed );
            pNode->m_pParent.store( pRight, memory_model::memory_order_relaxed );

            if ( pParentLeft == pNode )
                pParent->m_pLeft.store( pRight, memory_model::memory_order_relaxed );
            else {
                assert( pParent->m_pRight.load( memory_odel::memory_order_relaxed ) == pNode );
                pParent->m_pRight.store( pRight, memory_model::memory_model_relaxed );
            }
            pRight->m_pParent.store( pParent, memory_model::memory_order_relaxed );

            // fix up heights
            int hNode = 1 + std::max( hL, hRL );
            pNode->height( hNode, memory_model::memory_order_relaxed );
            pRight->height( 1 + std::max( hNode, hRR ), memory_model::memory_order_relaxed );

            end_change( pNode, nodeVersion );

            int nodeBalance = hRL - hL;
            if ( nodeBalance < -1 || nodeBalance > 1 )
                return pNode;

            if ( (pRLeft == nullptr || hL == 0) && pNode->value( memory_model::memory_order_relaxed ) == nullptr )
                return pNode;

            int rightBalance = hRR - hNode;
            if ( rightBalance < -1 || rightBalance > 1 )
                return pRight;

            if ( hRR == 0 && pRight->value( memory_model::memory_order_relaxed ) == nullptr )
                return pRight;

            return fix_height_locked( pParent );
        }

        node_type * rotate_right_over_left_locked( node_type * pParent, node_type * pNode, node_type * pLeft, int hR, int hLL, node_type * pLRight, int hLRL )
        {
            typename node_type::value_type nodeVersion = pNode->version( memory_model::memory_order_relaxed );
            typename node_type::value_type leftVersion = pLeft->version( memory_model::memory_order_relaxed );

            node_type * pPL = pParent->m_pLeft.load( memory_model::memory_order_relaxed );
            node_type * pLRL = pLRight->m_pLeft.load( memory_model::memory_order_relaxed );
            node_type * pLRR = pLRight->m_pRight.load( memory_model::memory_order_relaxed );
            int hLRR = pLRR ? pLRR->hight( memory_model::memory_order_relaxed ) : 0;

            begin_change( pNode, nodeVersion );
            begin_change( pLeft, leftVersion );

            // fix up pNode links, careful about the order!
            pNode->m_pLeft.store( pLRR, memory_model::memory_order_relaxed );
            if ( pLRR != nullptr )
                pLRR->m_pParent.store( pNode, memory_model::memory_order_relaxed );

            pLeft->m_pRight.store( pLRL, memory_model::memory_order_relaxed );
            if ( pLRL != nullptr )
                pLRL->m_pParent.store( pLeft, memory_model::memory_order_relaxed );

            pLRight->m_pLeft.store( pLeft, memory_model::memory_order_relaxed );
            pLeft->m_pParent.store( pLRight, memory_model::memory_order_relaxed );
            pLRight->m_pRight.store( pNode, memory_model::memory_order_relaxed );
            pNode->m_pParent.store( pLRight, memory_model::memory_order_relaxed );

            if ( pPL == pNode )
                pParent->m_pLeft.store( pLRight, memory_model::memory_order_relaxed );
            else {
                assert( Parent->m_pRight.load( memory_model::memory_order_relaxed ) == pNode );
                pParent->m_pRight.store( pLRight, memory_model::memory_order_relaxed );
            }
            pLRight->m_pParent.store( pParent, memory_model::memory_order_relaxed );

            // fix up heights
            int hNode = 1 + std::max( hLRR, hR );
            pNode->height( hNode, memory_model::memory_order_relaxed );
            int hLeft = 1 + std::max( hLL, hLRL );
            pLeft->height( hLeft, memory_model::memory_order_relaxed );
            pLRight->height( 1 + std::max( hLeft, hNode ), memory_model::memory_order_relaxed );

            end_change( pNode, nodeVersion );
            end_change( pLeft, leftVersion );

            // caller should have performed only a single rotation if pLeft was going
            // to end up damaged
            assert( hLL - hLRL <= 1 && hLRL - hLL <= 1 );
            assert( !((hLL == 0 || pLRL == nullptr) && pLeft->value( memory_model::memory_order_relaxed ) == nullptr ));

            // We have damaged pParent, pLR (now parent.child), and pNode (now
            // parent.child.right).  pNode is the deepest.  Perform as many fixes as we
            // can with the locks we've got.

            // We've already fixed the height for pNode, but it might still be outside
            // our allowable balance range.  In that case a simple fix_height_locked()
            // won't help.
            int nodeBalance = hLRR - hR;
            if ( nodeBalance < -1 || nodeBalance > 1 ) {
                // we need another rotation at pNode
                return pNode;
            }

            // pNode might also be damaged by being an unnecessary routing node
            if ( (pLRR == nullptr || hR == 0) && pNode->value( memory_model::memory_order_relaxed ) == nullptr ) {
                // repair involves splicing out pNode and maybe more rotations
                return pNode;
            }

            // we've already fixed the height at pLRight, do we need a rotation here?
            final int balanceLR = hLeft - hNode;
            if ( balanceLR < -1 || balanceLR > 1 )
                return pLR;

            // try to fix the parent height while we've still got the lock
            return fix_height_locked( pParent );
        }

        node_type * rotate_left_over_right_locked( node_type * pParent, node_type * pNode, int hL, node_type * pRight, node_type * pRLeft, int hRR, int hRLR )
        {
            version_type nodeVersion = pNode->version( memory_model::memory_order_relaxed );
            version_type rightVersion = pRight->version( memory_model::memory_order_relaxed );

            node_type * pPL = pParent->m_pLeft.load( memory_model::memory_order_relaxed );
            node_type * pRLL = pRLeft->m_pLeft.load( memory_model::memory_order_relaxed );
            node_type * pRLR = pRLeft->m_pRight.load( memory_model::memory_order_relaxed );
            int hRLL = pRLL ? pRLL->height( memory_model::memory_order_relaxed ) : 0;

            begin_change( pNode, nodeVersion );
            begin_change( pRight, ghtVersion );

            // fix up pNode links, careful about the order!
            pNode->m_pRight.store( pRLL, memory_model::memory_order_relaxed );
            if ( pRLL != nullptr )
                pRLL->m_pParent.store( pNode, memory_model::memory_order_relaxed );

            pRight->m_pLeft.store( pRLR, memory_model::memory_order_relaxed );
            if ( pRLR != nullptr )
                pRLR->m_pParent.store( pRight, memory_model::memory_order_relaxed );

            pRLeft->m_pRight.store( pRight, memory_model::memory_order_relaxed );
            pRight->m_pParent.store( pRLeft, memory_model::memory_order_relaxed );
            pRLeft->m_pLeft.store( pNode, memory_model::memory_order_relaxed );
            pNode->m_pParent.store( pRLeft, memory_model::memory_order_relaxed );

            if ( pPL == pNode )
                pParent->m_pLeft.store( pRLeft, memory_model::memory_order_relaxed );
            else {
                assert( pParent->m_pRight.load( memory_model::memory_order_relaxed ) == pNode );
                pParent->m_pRight.store( pRLeft, memory_model::memory_order_relaxed );
            }
            pRLeft->m_pParent.store( pParent, memory_model::memory_order_relaxed );

            // fix up heights
            int hNode = 1 + std::max( hL, hRLL );
            pNode->height( hNode, memory_model::memory_order_relaxed );
            int hRight = 1 + std::max( hRLR, hRR );
            pRight->height( hRight, memory_model::memory_order_relaxed );
            pRLeft->height( 1 + std::max( hNode, hRight ), memory_model::memory_order_relaxed );

            end_change( pNode, nodeVersion );
            end_change( pRight, rightVersion );

            assert( hRR - hRLR <= 1 && hRLR - hRR <= 1 );

            int nodeBalance = hRLL - hL;
            if ( nodeBalance < -1 || nodeBalance > 1 )
                return pNode;
            if ( (pRLL == nullptr || hL == 0) && pNode->value( memory_model::memory_order_relaxed ) == nullptr )
                return pNode;

            int balRL = hRight - hNode;
            if ( balRL < -1 || balRL > 1 )
                return pRLeft;

            return fix_height_locked( pParent );
        }

        //@endcond
    };
}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_IMPL_BRONSON_AVLTREE_MAP_RCU_H

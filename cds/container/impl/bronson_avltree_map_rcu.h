//$$CDS-header$$

#ifndef __CDS_CONTAINER_IMPL_BRONSON_AVLTREE_MAP_RCU_H
#define __CDS_CONTAINER_IMPL_BRONSON_AVLTREE_MAP_RCU_H

#include <cds/container/details/bronson_avltree_base.h>
#include <cds/urcu/details/check_deadlock.h>

namespace cds { namespace container {

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
        typedef typename traits::allocator              allocator_type;     ///< allocator for maintaining internal node
        typedef typename traits::stat                   stat;               ///< internal statistics
        typedef typename traits::rcu_check_deadlock     rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename traits::back_off               back_off;           ///< Back-off strategy
        typedef typename traits::disposer               disposer;           ///< Value disposer
        typedef typename traits::lock_type              lock_type;          ///< Node lock type

    protected:
        //@cond
        typedef bronson_avltree::node< key_type, mapped_type, lock_type > node_type;

        typedef typename std::conditional <
            std::is_same< typename traits::node_type, opt::none >::value,
            bronson_avltree::node< key_type, mapped_type, lock_type >,
            typename traits::node_type
        >::type alloc_node_type;

        typedef typename allocator_type::template rebind<alloc_node_type>::other memory_allocator;

        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock >   check_deadlock_policy;

        enum class find_result
        {
            not_found,
            found,
            retry,
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
        node_type * alloc_node( K&& key )
        {
            alloc_node_type * pNode = memory_allocator().allocate( 1 );
            return new (static_cast<node_type *>(pNode)) node_type( std::forward<K>( key ) );
        }

        template <typename K>
        node_type * alloc_node( K&& key, mapped_type * pVal )
        {
            alloc_node_type * pNode = memory_allocator().allocate( 1 );
            return new (static_cast<node_type *>(pNode)) node_type( std::forward<K>( key ), pVal );
        }

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
            //TODO
        }

        /// Ensures that the \p key exists in the map
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p key not found in the map, then the new item created from \p key
            is inserted into the map (note that in this case the \ref key_type should be
            constructible from type \p K).
            Otherwise, the value is changed to \p pVal.

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successfull,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the tree.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> ensure( K const& key, mapped_type * pVal )
        {
            //TODO
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
                if ( pNode->m_pValue ) {
                    f( *pNode->m_pValue );
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
                if ( pNode->m_pValue ) {
                    f( *pNode->m_pValue );
                    return true;
                }
                return false;
            } );
            
            //TODO
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

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type
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
            return m_Stat;
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

        template <typename Q, typename Compare, typename Func>
        find_result try_find( Q const& key, Compare cmp, Func f, node_type * pNode, int nDir, typename node_type::version_type nVersion ) const
        {
            assert( gc::is_locked() );
            assert( pNode );

            while ( true ) {
                node_type * pChild = pNode->child( nDir ).load( memory_model::memory_order_relaxed );
                if ( !pChild ) {
                    if ( pNode->version( memory_model::memory_order_relaxed ) != nVersion ) {
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

                typename node_type::version_type nChildVersion = pChild->version( memory_model::memory_order_relaxed );
                if ( nChildVersion & node_type::version_flags::shrinking ) {
                    m_stat.onFindWaitShrinking();
                    pChild->template wait_until_shrink_completed<back_off>( memory_model::memory_order_relaxed );

                    if ( pNode->version( memory_model::memory_order_relaxed ) != nVersion ) {
                        m_stat.onFindRetry();
                        return find_result::retry;
                    }
                }
                else if ( nChildVersion != node_type::version_flags::unlinked ) {

                    if ( pNode->version( memory_model::memory_order_relaxed ) != nVersion ) {
                        m_stat.onFindRetry();
                        return find_result::retry;
                    }

                    find_result found = try_find( key, cmp, f, pChild, nCmp, nChildVersion );
                    if ( found != find_result::retry )
                        return found;
                }
            }
        }
        //@endcond

    };
}} // namespace cds::container

#endif // #ifndef __CDS_CONTAINER_IMPL_BRONSON_AVLTREE_MAP_RCU_H

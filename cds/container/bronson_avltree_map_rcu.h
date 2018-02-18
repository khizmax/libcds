// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_BRONSON_AVLTREE_MAP_RCU_H
#define CDSLIB_CONTAINER_BRONSON_AVLTREE_MAP_RCU_H

#include <functional>
#include <cds/container/impl/bronson_avltree_map_rcu.h>

namespace cds { namespace container {

    namespace bronson_avltree {
        //@cond
        namespace details {
            template < class RCU, typename Key, typename T, typename Traits>
            struct make_map
            {
                typedef Key key_type;
                typedef T mapped_type;
                typedef Traits original_traits;

                typedef cds::details::Allocator< mapped_type, typename original_traits::allocator > cxx_allocator;

                struct traits : public original_traits
                {
                    struct disposer {
                        void operator()( mapped_type * p ) const
                        {
                            cxx_allocator().Delete( p );
                        }
                    };
                };

                // Metafunction result
                typedef BronsonAVLTreeMap< RCU, Key, mapped_type *, traits > type;
            };
        } // namespace details
        //@endcond
    } // namespace bronson_avltree

    /// Bronson et al AVL-tree (RCU specialization)
    /** @ingroup cds_nonintrusive_map
        @ingroup cds_nonintrusive_tree
        @anchor cds_container_BronsonAVLTreeMap_rcu

        Source:
            - [2010] N.Bronson, J.Casper, H.Chafi, K.Olukotun "A Practical Concurrent Binary Search Tree"
            - <a href="http://github.com/nbronson/snaptree">Java implementation</a>

        This is a concurrent AVL tree algorithm that uses hand-over-hand optimistic validation,
        a concurrency control mechanism for searching and navigating a binary search tree.
        This mechanism minimizes spurious retries when concurrent structural changes cannot
        affect the correctness of the search or navigation result.
        The algorithm is based on partially external trees, a simple scheme that simplifies deletions
        by leaving a routing node in the tree when deleting a node that has two children,
        then opportunistically unlinking routing nodes during rebalancing. As in external trees,
        which store values only in leaf nodes, deletions can be performed locally while holding
        a fixed number of locks. Partially external trees, however, require far fewer routing nodes
        than an external tree for most sequences of insertions and deletions.
        The algorithm uses optimistic concurrency control, but carefully manage the
        tree in such a way that all atomic regions have fixed read and write sets
        that are known ahead of time. This allows to reduce practical overheads by embedding
        the concurrency control directly. To perform tree operations using only fixed sized
        atomic regions the algo uses the following mechanisms: search operations overlap atomic blocks as
        in the hand-over-hand locking technique; mutations perform rebalancing separately;
        and deletions occasionally leave a routing node in the tree.

        <b>Template arguments</b>:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - key type
        - \p T - value type to be stored in tree's nodes.
        - \p Traits - tree traits, default is \p bronson_avltree::traits
            It is possible to declare option-based tree with \p bronson_avltree::make_traits metafunction
            instead of \p Traits template argument.

        There is \ref cds_container_BronsonAVLTreeMap_rcu_ptr "a specialization" for "key -> value pointer" map.

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
    class BronsonAVLTreeMap< cds::urcu::gc<RCU>, Key, T, Traits >
#ifdef CDS_DOXYGEN_INVOKED
        : private BronsonAVLTreeMap< cds::urcu::gc<RCU>, Key, T*, Traits >
#else
        : private bronson_avltree::details::make_map< cds::urcu::gc<RCU>, Key, T, Traits >::type
#endif
    {
        //@cond
        typedef bronson_avltree::details::make_map< cds::urcu::gc<RCU>, Key, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        typedef cds::urcu::gc<RCU>  gc;   ///< RCU Garbage collector
        typedef Key     key_type;    ///< type of a key stored in the map
        typedef T       mapped_type; ///< type of value stored in the map
        typedef Traits  traits;      ///< Traits template parameter

        typedef typename base_class::key_comparator     key_comparator;     ///< key compare functor based on \p Traits::compare and \p Traits::less
        typedef typename traits::item_counter           item_counter;       ///< Item counting policy
        typedef typename traits::memory_model           memory_model;       ///< Memory ordering, see \p cds::opt::memory_model option
        typedef typename traits::allocator              allocator_type;     ///< allocator for value
        typedef typename traits::node_allocator         node_allocator_type;///< allocator for maintaining internal nodes
        typedef typename traits::stat                   stat;               ///< internal statistics
        typedef typename traits::rcu_check_deadlock     rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename traits::back_off               back_off;           ///< Back-off strategy
        typedef typename traits::sync_monitor           sync_monitor;       ///< @ref cds_sync_monitor "Synchronization monitor" type for node-level locking

        /// Enabled or disabled @ref bronson_avltree::relaxed_insert "relaxed insertion"
        static bool const c_bRelaxedInsert = traits::relaxed_insert;

        /// Group of \p extract_xxx functions does not require external locking
        static constexpr const bool c_bExtractLockExternal = base_class::c_bExtractLockExternal;

        typedef typename base_class::rcu_lock   rcu_lock;  ///< RCU scoped lock

        /// Returned pointer to \p mapped_type of extracted node
        typedef typename base_class::exempt_ptr exempt_ptr;

    protected:
        //@cond
        typedef typename base_class::node_type        node_type;
        typedef typename base_class::node_scoped_lock node_scoped_lock;
        typedef typename maker::cxx_allocator         cxx_allocator;

        typedef typename base_class::update_flags update_flags;
        //@endcond

    public:
        /// Creates empty map
        BronsonAVLTreeMap()
        {}

        /// Destroys the map
        ~BronsonAVLTreeMap()
        {}

        /// Inserts new node with \p key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from a value of type \p K.
            - The \p mapped_type should be default-constructible.

            RCU \p synchronize() can be called. RCU should not be locked.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K const& key )
        {
            return base_class::do_update(key, key_comparator(),
                []( node_type * pNode ) -> mapped_type*
                {
                    assert( pNode->m_pValue.load( memory_model::memory_order_relaxed ) == nullptr );
                    CDS_UNUSED( pNode );
                    return cxx_allocator().New();
                },
                update_flags::allow_insert
            ) == update_flags::result_inserted;
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p mapped_type should be constructible from \p val of type \p V.

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns \p true if \p val is inserted into the map, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K const& key, V const& val )
        {
            return base_class::do_update( key, key_comparator(),
                [&val]( node_type * pNode ) -> mapped_type*
                {
                    assert( pNode->m_pValue.load( memory_model::memory_order_relaxed ) == nullptr );
                    CDS_UNUSED( pNode );
                    return cxx_allocator().New( val );
                },
                update_flags::allow_insert
            ) == update_flags::result_inserted;
        }

        /// Inserts new node and initialize it by a functor
        /**
            This function inserts new node with key \p key and if inserting is successful then it calls
            \p func functor with signature
            \code
                struct functor {
                    void operator()( key_type const& key, mapped_type& item );
                };
            \endcode

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.
            The functor is called under the node lock.

            RCU \p synchronize() method can be called. RCU should not be locked.
        */
        template <typename K, typename Func>
        bool insert_with( K const& key, Func func )
        {
            return base_class::do_update( key, key_comparator(),
                [&func]( node_type * pNode ) -> mapped_type*
                {
                    assert( pNode->m_pValue.load( memory_model::memory_order_relaxed ) == nullptr );
                    mapped_type * pVal = cxx_allocator().New();
                    func( pNode->m_key, *pVal );
                    return pVal;
                },
                update_flags::allow_insert
            ) == update_flags::result_inserted;
        }

        /// For \p key inserts data of type \p mapped_type created in-place from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.

            RCU \p synchronize() method can be called. RCU should not be locked.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            struct scoped_ptr
            {
                mapped_type * pVal;
                scoped_ptr( mapped_type * p ): pVal( p ) {}
                ~scoped_ptr() { if ( pVal ) cxx_allocator().Delete( pVal ); }
                void release() { pVal = nullptr; }
            };

            scoped_ptr p( cxx_allocator().MoveNew( std::forward<Args>( args )... ));
            if ( base_class::insert( std::forward<K>( key ), p.pVal )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Updates the value for \p key
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p key not found in the map, then the new item created from \p key
            will be inserted into the map iff \p bAllowInsert is \p true
            (note that in this case the \ref key_type should be constructible from type \p K).
            Otherwise, the functor \p func is called with item found.
            The functor \p Func signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, key_type const& key, mapped_type& item );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - value

            The functor may change any fields of the \p item. The functor is called under the node lock,
            the caller can change any field of \p item.

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already exists.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, Func func, bool bAllowInsert = true )
        {
            int result = base_class::do_update( key, key_comparator(),
                [&func]( node_type * pNode ) -> mapped_type*
                {
                    mapped_type * pVal = pNode->m_pValue.load( memory_model::memory_order_relaxed );
                    if ( !pVal ) {
                        pVal = cxx_allocator().New();
                        func( true, pNode->m_key, *pVal );
                    }
                    else
                        func( false, pNode->m_key, *pVal );
                    return pVal;
                },
                (bAllowInsert ? update_flags::allow_insert : 0) | update_flags::allow_update
            );
            return std::make_pair( result != 0, (result & update_flags::result_inserted) != 0 );
        }


        /// Delete \p key from the map
        /**
            RCU \p synchronize() method can be called. RCU should not be locked.

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return base_class::erase( key );
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
            return base_class::erase_with( key, pred );
        }

        /// Delete \p key from the map
        /** \anchor cds_nonintrusive_BronsonAVLTreeMap_rcu_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(key_type const& key, mapped_type& item) { ... }
            };
            \endcode

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return base_class::erase( key, f );
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_BronsonAVLTreeMap_rcu_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
            return base_class::erase_with( key, pred, f );
        }

        /// Extracts a value with minimal key from the map
        /**
            Returns \p exempt_ptr pointer to the leftmost item.
            If the set is empty, returns empty \p exempt_ptr.

            Note that the function returns only the value for minimal key.
            To retrieve its key use \p extract_min( Func ) member function.

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
            return base_class::extract_min();
        }

        /// Extracts minimal key and corresponding value
        /**
            Returns \p exempt_ptr to the leftmost item.
            If the tree is empty, returns empty \p exempt_ptr.

            \p Func functor is used to store minimal key.
            \p Func has the following signature:
            \code
                struct functor {
                    void operator()( key_type const& key );
                };
            \endcode
            If the tree is empty, \p f is not called.
            Otherwise, is it called with minimal key, the pointer to corresponding value is returned
            as \p exempt_ptr.

            @note Due the concurrent nature of the map, the function extracts <i>nearly</i> minimum key.
            It means that the function gets leftmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key less than leftmost item's key.
            So, the function returns the item with minimum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not free the item.
            The deallocator will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        template <typename Func>
        exempt_ptr extract_min( Func f )
        {
            return base_class::extract_min( f );
        }

        /// Extracts minimal key and corresponding value
        /**
            This function is a shortcut for the following call:
            \code
                key_type key;
                exempt_ptr xp = theTree.extract_min( [&key]( key_type const& k ) { key = k; } );
            \endcode
            \p key_type should be copy-assignable. The copy of minimal key
            is returned in \p min_key argument.
        */
        typename std::enable_if< std::is_copy_assignable<key_type>::value, exempt_ptr >::type
        extract_min_key( key_type& min_key )
        {
            return base_class::extract_min_key( min_key );
        }

        /// Extracts an item with maximal key from the map
        /**
            Returns \p exempt_ptr pointer to the rightmost item.
            If the set is empty, returns empty \p exempt_ptr.

            Note that the function returns only the value for maximal key.
            To retrieve its key use \p extract_max( Func ) or \p extract_max_key(key_type&) member function.

            @note Due the concurrent nature of the map, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key greater than rightmost item's key.
            So, the function returns the item with maximum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not free the item.
            The deallocator will be implicitly invoked when the returned object is destroyed or when
            its \p release() is called.
        */
        exempt_ptr extract_max()
        {
            return base_class::extract_max();
        }

        /// Extracts the maximal key and corresponding value
        /**
            Returns \p exempt_ptr pointer to the rightmost item.
            If the set is empty, returns empty \p exempt_ptr.

            \p Func functor is used to store maximal key.
            \p Func has the following signature:
            \code
                struct functor {
                    void operator()( key_type const& key );
                };
            \endcode
            If the tree is empty, \p f is not called.
            Otherwise, is it called with maximal key, the pointer to corresponding value is returned
            as \p exempt_ptr.

            @note Due the concurrent nature of the map, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key greater than rightmost item's key.
            So, the function returns the item with maximum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not free the item.
            The deallocator will be implicitly invoked when the returned object is destroyed or when
            its \p release() is called.
        */
        template <typename Func>
        exempt_ptr extract_max( Func f )
        {
            return base_class::extract_max( f );
        }

        /// Extracts the maximal key and corresponding value
        /**
            This function is a shortcut for the following call:
            \code
                key_type key;
                exempt_ptr xp = theTree.extract_max( [&key]( key_type const& k ) { key = k; } );
            \endcode
            \p key_type should be copy-assignable. The copy of maximal key
            is returned in \p max_key argument.
        */
        typename std::enable_if< std::is_copy_assignable<key_type>::value, exempt_ptr >::type
        extract_max_key( key_type& max_key )
        {
            return base_class::extract_max_key( max_key );
        }

        /// Extracts an item from the map
        /**
            The function searches an item with key equal to \p key in the tree,
            unlinks it, and returns \p exempt_ptr pointer to a value found.
            If \p key is not found the function returns an empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not destroy the value found.
            The dealloctor will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            return base_class::extract( key );
        }

        /// Extracts an item from the map using \p pred for searching
        /**
            The function is an analog of \p extract(Q const&)
            but \p pred is used for key compare.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            return base_class::extract_with( key, pred );
        }

        /// Find the key \p key
        /**
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( key_type const& key, mapped_type& val );
            };
            \endcode
            where \p val is the item found for \p key
            The functor is called under node-level lock.

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return base_class::find( key, f );
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
            return base_class::find_with( key, pred, f );
        }

        /// Checks whether the map contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename K>
        bool contains( K const& key )
        {
            return base_class::contains( key );
        }

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename K, typename Less>
        bool contains( K const& key, Less pred )
        {
            return base_class::contains( key, pred );
        }

        /// Clears the map
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the map is empty
        bool empty() const
        {
            return base_class::empty();
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
            return base_class::size();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }

        /// Returns reference to \p sync_monitor object
        sync_monitor& monitor()
        {
            return base_class::monitor();
        }
        //@cond
        sync_monitor const& monitor() const
        {
            return base_class::monitor();
        }
        //@endcond

        /// Checks internal consistency (not atomic, not thread-safe)
        /**
            The debugging function to check internal consistency of the tree.
        */
        bool check_consistency() const
        {
            return base_class::check_consistency();
        }

        /// Checks internal consistency (not atomic, not thread-safe)
        /**
            The debugging function to check internal consistency of the tree.
            The functor \p Func is called if a violation of internal tree structure
            is found:
            \code
            struct functor {
                void operator()( size_t nLevel, size_t hLeft, size_t hRight );
            };
            \endcode
            where
            - \p nLevel - the level where the violation is found
            - \p hLeft - the height of left subtree
            - \p hRight - the height of right subtree

            The functor is called for each violation found.
        */
        template <typename Func>
        bool check_consistency( Func f ) const
        {
            return base_class::check_consistency( f );
        }
    };
}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_IMPL_BRONSON_AVLTREE_MAP_RCU_H

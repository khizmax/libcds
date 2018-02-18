// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_ELLEN_BINTREE_MAP_RCU_H
#define CDSLIB_CONTAINER_ELLEN_BINTREE_MAP_RCU_H

#include <cds/container/details/ellen_bintree_base.h>
#include <cds/intrusive/ellen_bintree_rcu.h>

namespace cds { namespace container {

    /// Map based on Ellen's et al binary search tree (RCU specialization)
    /** @ingroup cds_nonintrusive_map
        @ingroup cds_nonintrusive_tree
        @anchor cds_container_EllenBinTreeMap_rcu

        Source:
            - [2010] F.Ellen, P.Fatourou, E.Ruppert, F.van Breugel "Non-blocking Binary Search Tree"

        %EllenBinTreeMap is an unbalanced leaf-oriented binary search tree that implements the <i>map</i>
        abstract data type. Nodes maintains child pointers but not parent pointers.
        Every internal node has exactly two children, and all data of type <tt>std::pair<Key const, T></tt>
        currently in the tree are stored in the leaves. Internal nodes of the tree are used to direct \p find
        operation along the path to the correct leaf. The keys (of \p Key type) stored in internal nodes
        may or may not be in the map.
        Unlike \ref cds_container_EllenBinTreeSet_rcu "EllenBinTreeSet" keys are not a part of \p T type.
        The map can be represented as a set containing <tt>std::pair< Key const, T> </tt> values.

        Due to \p extract_min and \p extract_max member functions the \p %EllenBinTreeMap can act as
        a <i>priority queue</i>. In this case you should provide unique compound key, for example,
        the priority value plus some uniformly distributed random value.

        @warning Recall the tree is <b>unbalanced</b>. The complexity of operations is <tt>O(log N)</tt>
        for uniformly distributed random keys, but in the worst case the complexity is <tt>O(N)</tt>.

        @note In the current implementation we do not use helping technique described in original paper.
        So, the current implementation is near to fine-grained lock-based tree.
        Helping will be implemented in future release

        <b>Template arguments</b> :
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - key type
        - \p T - value type to be stored in tree's leaf nodes.
        - \p Traits - map traits, default is \p ellen_bintree::traits.
            It is possible to declare option-based tree with \p ellen_bintree::make_map_traits metafunction
            instead of \p Traits template argument.

        @note Before including <tt><cds/container/ellen_bintree_map_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.
    */
    template <
        class RCU,
        typename Key,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = ellen_bintree::traits
#else
        class Traits
#endif
    >
    class EllenBinTreeMap< cds::urcu::gc<RCU>, Key, T, Traits >
#ifdef CDS_DOXYGEN_INVOKED
        : public cds::intrusive::EllenBinTree< cds::urcu::gc<RCU>, Key, T, Traits >
#else
        : public ellen_bintree::details::make_ellen_bintree_map< cds::urcu::gc<RCU>, Key, T, Traits >::type
#endif
    {
        //@cond
        typedef ellen_bintree::details::make_ellen_bintree_map< cds::urcu::gc<RCU>, Key, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond
    public:
        typedef cds::urcu::gc<RCU>  gc;   ///< RCU Garbage collector
        typedef Key     key_type;    ///< type of a key stored in the map
        typedef T       mapped_type; ///< type of value stored in the map
        typedef std::pair< key_type const, mapped_type >    value_type;   ///< Key-value pair stored in leaf node of the mp
        typedef Traits  traits;      ///< Traits template parameter

        static_assert( std::is_default_constructible<key_type>::value, "Key should be default constructible type" );

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key compare functor based on \p Traits::compare and \p Traits::less
#   else
        typedef typename maker::intrusive_traits::compare   key_comparator;
#   endif
        typedef typename base_class::item_counter           item_counter;       ///< Item counting policy
        typedef typename base_class::memory_model           memory_model;       ///< Memory ordering, see \p cds::opt::memory_model option
        typedef typename base_class::node_allocator         node_allocator_type; ///< allocator for maintaining internal node
        typedef typename base_class::stat                   stat;               ///< internal statistics
        typedef typename base_class::rcu_check_deadlock     rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename traits::copy_policy                copy_policy;        ///< key copy policy
        typedef typename traits::back_off                   back_off;           ///< Back-off strategy

        typedef typename traits::allocator                  allocator_type;        ///< Allocator for leaf nodes
        typedef typename base_class::node_allocator         node_allocator;        ///< Internal node allocator
        typedef typename base_class::update_desc_allocator  update_desc_allocator; ///< Update descriptor allocator

        static constexpr const bool c_bExtractLockExternal = base_class::c_bExtractLockExternal; ///< Group of \p extract_xxx functions do not require external locking

    protected:
        //@cond
        typedef typename base_class::value_type         leaf_node;
        typedef typename base_class::internal_node      internal_node;
        typedef typename base_class::update_desc        update_desc;

        typedef typename maker::cxx_leaf_node_allocator cxx_leaf_node_allocator;

        typedef std::unique_ptr< leaf_node, typename maker::leaf_deallocator >    scoped_node_ptr;
        //@endcond

    public:
        typedef typename gc::scoped_lock    rcu_lock ;  ///< RCU scoped lock

        /// pointer to extracted node
        using exempt_ptr = cds::urcu::exempt_ptr < gc, leaf_node, value_type, typename maker::intrusive_traits::disposer,
            cds::urcu::details::conventional_exempt_member_cast < leaf_node, value_type >
        >;

    public:
        /// Default constructor
        EllenBinTreeMap()
            : base_class()
        {}

        /// Clears the map
        ~EllenBinTreeMap()
        {}

        /// Inserts new node with key and default value
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
            return insert_with( key, [](value_type&){} );
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p value_type should be constructible from \p val of type \p V.

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns \p true if \p val is inserted into the map, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K const& key, V const& val )
        {
            scoped_node_ptr pNode( cxx_leaf_node_allocator().New( key, val ));
            if ( base_class::insert( *pNode ))
            {
                pNode.release();
                return true;
            }
            return false;
        }

        /// Inserts new node and initialize it by a functor
        /**
            This function inserts new node with key \p key and if inserting is successful then it calls
            \p func functor with signature
            \code
                struct functor {
                    void operator()( value_type& item );
                };
            \endcode

            The argument \p item of user-defined functor \p func is the reference
            to the map's item inserted:
                - <tt>item.first</tt> is a const reference to item's key that cannot be changed.
                - <tt>item.second</tt> is a reference to item's value that may be changed.

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            RCU \p synchronize() method can be called. RCU should not be locked.
        */
        template <typename K, typename Func>
        bool insert_with( K const& key, Func func )
        {
            scoped_node_ptr pNode( cxx_leaf_node_allocator().New( key ));
            if ( base_class::insert( *pNode, [&func]( leaf_node& item ) { func( item.m_Value ); } )) {
                pNode.release();
                return true;
            }
            return false;
        }

        /// For key \p key inserts data of type \p value_type created in-place from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.

            RCU \p synchronize() method can be called. RCU should not be locked.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            scoped_node_ptr pNode( cxx_leaf_node_allocator().MoveNew( key_type( std::forward<K>(key)), mapped_type( std::forward<Args>(args)... )));
            if ( base_class::insert( *pNode )) {
                pNode.release();
                return true;
            }
            return false;
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the map, then \p val is inserted iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor \p func signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the map

            The functor may change any fields of the \p item.second that is \p mapped_type;
            however, \p func must guarantee that during changing no any other modifications
            could be made on this item by concurrent threads.

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            i.e. the node has been inserted or updated,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, Func func, bool bAllowInsert = true )
        {
            scoped_node_ptr pNode( cxx_leaf_node_allocator().New( key ));
            std::pair<bool, bool> res = base_class::update( *pNode,
                [&func](bool bNew, leaf_node& item, leaf_node const& ){ func( bNew, item.m_Value ); },
                bAllowInsert
            );
            if ( res.first && res.second )
                pNode.release();
            return res;
        }
        //@cond
        template <typename K, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( K const& key, Func func )
        {
            return update( key, func, true );
        }
        //@endcond

        /// Delete \p key from the map
        /**\anchor cds_nonintrusive_EllenBinTreeMap_rcu_erase_val

            RCU \p synchronize() method can be called. RCU should not be locked.

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return base_class::erase(key);
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_rcu_erase_val "erase(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool erase_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >());
        }

        /// Delete \p key from the map
        /** \anchor cds_nonintrusive_EllenBinTreeMap_rcu_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return base_class::erase( key, [&f]( leaf_node& node) { f( node.m_Value ); } );
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_rcu_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >(),
                [&f]( leaf_node& node) { f( node.m_Value ); } );
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
            return exempt_ptr( base_class::extract_min_());
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
            return exempt_ptr( base_class::extract_max_());
        }

        /// Extracts an item from the map
        /** \anchor cds_nonintrusive_EllenBinTreeMap_rcu_extract
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
            return exempt_ptr( base_class::extract_( key, typename base_class::node_compare()));
        }

        /// Extracts an item from the map using \p pred for searching
        /**
            The function is an analog of \p extract(Q const&)
            but \p pred is used for key compare.
            \p Less has the interface like \p std::less and should meet \ref cds_container_EllenBinTreeSet_rcu_less
            "predicate requirements".
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return exempt_ptr( base_class::extract_with_( key,
                cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >()));
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_EllenBinTreeMap_rcu_find_cfunc

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change \p item.second.

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return base_class::find( key, [&f](leaf_node& item, K const& ) { f( item.m_Value );});
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_rcu_find_cfunc "find(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool find_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >(),
                [&f](leaf_node& item, K const& ) { f( item.m_Value );});
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
        //@cond
        template <typename K>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( K const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_rcu_less
            "Predicate requirements".
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename K, typename Less>
        bool contains( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >());
        }
        //@cond
        template <typename K, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( K const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_EllenBinTreeMap_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            RCU should be locked before call the function.
            Returned pointer is valid while RCU is locked.
        */
        template <typename Q>
        value_type * get( Q const& key ) const
        {
            leaf_node * pNode = base_class::get( key );
            return pNode ? &pNode->m_Value : nullptr;
        }

        /// Finds \p key with \p pred predicate and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \p key_type
            and \p Q in any order.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename Q, typename Less>
        value_type * get_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            leaf_node * pNode = base_class::get_with( key,
                cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >());
            return pNode ? &pNode->m_Value : nullptr;
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

        /// Checks internal consistency (not atomic, not thread-safe)
        /**
            The debugging function to check internal consistency of the tree.
        */
        bool check_consistency() const
        {
            return base_class::check_consistency();
        }
    };
}} // namespace cds::container

#endif //#ifndef CDSLIB_CONTAINER_ELLEN_BINTREE_MAP_RCU_H

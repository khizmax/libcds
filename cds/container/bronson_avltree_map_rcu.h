//$$CDS-header$$

#ifndef __CDS_CONTAINER_BRONSON_AVLTREE_MAP_RCU_H
#define __CDS_CONTAINER_BRONSON_AVLTREE_MAP_RCU_H

#include <cds/container/impl/bronson_avltree_map_rcu.h>

namespace cds { namespace container {

    namespace bronson_avltree { 
        //@cond
        namespace details {

            template <typename Key, typename T, typename Lock>
            struct value_node : public bronson_avltree::node< Key, T, Lock >
            {
                T   m_data; // placeholder for data
            };

            template <typename Key, typename T, typename Traits>
            struct pointer_oriented_traits: public Traits
            {
                struct disposer {
                    template <typename T>
                    void operator()( T * p )
                    {
                        std::allocator().destroy( p );
                    }
                };

                typedef value_node<Key, T, typename Traits::lock_type > node_type;
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

        bla-bla-bla

        <b>Template arguments</b>:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - key type
        - \p T - value type to be stored in tree's nodes.
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
    class BronsonAVLTreeMap< cds::urcu::gc<RCU>, Key, T, Traits >
        : private BronsonAVLTreeMap< cds::urcu::gc<RCU>, Key, T*, bronson_avltree::details::pointer_oriented_traits<Key, T, Traits>>
    {
        //@cond
        typedef BronsonAVLTreeMap< cds::urcu::gc<RCU>, Key, T*, bronson_avltree::details::pointer_oriented_traits<Traits>> base_class;
        //@endcond

    public:
        typedef cds::urcu::gc<RCU>  gc;   ///< RCU Garbage collector
        typedef Key     key_type;    ///< type of a key stored in the map
        typedef T       mapped_type; ///< type of value stored in the map
        typedef Traits  traits;      ///< Traits template parameter

        typedef typename base_class::key_comparator     key_comparator;     ///< key compare functor based on \p Traits::compare and \p Traits::less
        typedef typename traits::item_counter           item_counter;       ///< Item counting policy
        typedef typename traits::memory_model           memory_model;       ///< Memory ordering, see \p cds::opt::memory_model option
        typedef typename traits::allocator              allocator_type;     ///< allocator for maintaining internal node
        typedef typename traits::stat                   stat;               ///< internal statistics
        typedef typename traits::rcu_check_deadlock     rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename traits::back_off               back_off;           ///< Back-off strategy
        typedef typename traits::lock_type              lock_type;          ///< Node lock type

    protected:
        //@cond
        typedef typename base_class::alloc_node_type    node_type;
        typedef base_class::node_scoped_lock            node_scoped_lock;
        //@endcond

    public:
        /// Creates empty map
        BronsonAVLTreeMap()
        {}

        /// Destroys the map
        ~BronsonAVLTreeMap()
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
            //TODO
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
            //TODO
        }

        /// Inserts new node and initialize it by a functor
        /**
            This function inserts new node with key \p key and if inserting is successful then it calls
            \p func functor with signature
            \code
                struct functor {
                    void operator()( mapped_type& item );
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
            //TODO
        }

        /// For key \p key inserts data of type \p mapped_type created in-place from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.

            RCU \p synchronize() method can be called. RCU should not be locked.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            //TODO
        }

        /// Ensures that the \p key exists in the map
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p key not found in the map, then the new item created from \p key
            is inserted into the map (note that in this case the \ref key_type should be
            constructible from type \p K).
            Otherwise, the functor \p func is called with item found.
            The functor \p Func may be a functor:
            \code
                struct my_functor {
                    void operator()( bool bNew, mapped_type& item );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the tree

            The functor may change any fields of the \p item

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successfull,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the tree.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> ensure( K const& key, Func func )
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
        /** \anchor cds_nonintrusive_BronsonAVLTreeMap_rcu_erase_func

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
            The function is an analog of \ref cds_nonintrusive_BronsonAVLTreeMap_rcu_erase_func "erase(K const&, Func)"
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

        /// Find the key \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename K>
        bool find( K const& key )
        {
            return base_class::find( key );
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
            return base_class::find_with( key, pred );
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

#endif // #ifndef __CDS_CONTAINER_IMPL_BRONSON_AVLTREE_MAP_RCU_H

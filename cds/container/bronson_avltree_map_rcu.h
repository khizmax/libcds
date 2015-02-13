//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_BRONSON_AVLTREE_MAP_RCU_H
#define CDSLIB_CONTAINER_BRONSON_AVLTREE_MAP_RCU_H

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

                struct internal_mapped_type : public bronson_avltree::value
                {
                    mapped_type     m_val;

                    template <typename... Args>
                    internal_mapped_type( Args&&... args )
                        : m_val( std::forward<Args>(args)...)
                    {}
                };

                typedef cds::details::Allocator< internal_mapped_type, typename original_traits::allocator > cxx_allocator;

                struct cast_mapped_type
                {
                    mapped_type * operator()( internal_mapped_type * p ) const
                    {
                        return &(p->m_val);
                    }
                };

                struct traits : public original_traits
                {
                    struct disposer {
                        void operator()( internal_mapped_type * p ) const
                        {
                            cxx_allocator().Delete( p );
                        }
                    };
                };

                // Metafunction result
                typedef BronsonAVLTreeMap< RCU, Key, internal_mapped_type *, traits > type;
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
        typedef typename base_class::rcu_lock   rcu_lock;  ///< RCU scoped lock

    protected:
        //@cond
        typedef typename base_class::node_type        node_type;
        typedef typename maker::internal_mapped_type  internal_mapped_type;
        typedef typename base_class::node_scoped_lock node_scoped_lock;
        typedef typename maker::cxx_allocator         cxx_allocator;

        typedef typename base_class::update_flags update_flags;
        //@endcond

    public:
#   ifdef CDSDOXYGEN_INVOKED
        /// Returned pointer to \p mapped_type of extracted node
        typedef cds::urcu::exempt_ptr< gc, implementation_defined, mapped_type, implementation_defined, implementation_defined > exempt_ptr;
#   else
        typedef cds::urcu::exempt_ptr< gc,
            internal_mapped_type,
            mapped_type,
            typename maker::traits::disposer,
            typename maker::cast_mapped_type
        > exempt_ptr;
#   endif


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
                []( node_type * pNode ) -> internal_mapped_type*
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
                [&val]( node_type * pNode ) -> internal_mapped_type*
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
                [&func]( node_type * pNode ) -> internal_mapped_type*
                {
                    assert( pNode->m_pValue.load( memory_model::memory_order_relaxed ) == nullptr );
                    internal_mapped_type * pVal = cxx_allocator().New();
                    func( pNode->m_key, pVal->m_val );
                    return pVal;
                },
                update_flags::allow_insert
            ) == update_flags::result_inserted;
        }

        /// For key \p key inserts data of type \p mapped_type created in-place from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.

            RCU \p synchronize() method can be called. RCU should not be locked.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            return base_class::do_update( key, key_comparator(),
                [&]( node_type * pNode ) -> internal_mapped_type* 
                {
                    assert( pNode->m_pValue.load( memory_model::memory_order_relaxed ) == nullptr );
                    CDS_UNUSED( pNode );
                    return cxx_allocator().New( std::forward<Args>(args)...);
                },
                update_flags::allow_insert
            ) == update_flags::result_inserted;
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
                    void operator()( bool bNew, key_type const& key, mapped_type& item );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - value

            The functor may change any fields of the \p item. The functor is called under the node lock.

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successfull,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the tree.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, Func func )
        {
            int result = base_class::do_update( key, key_comparator(),
                [&func]( node_type * pNode ) -> internal_mapped_type* 
                {
                    internal_mapped_type * pVal = pNode->m_pValue.load( memory_model::memory_order_relaxed );
                    if ( !pVal ) {
                        pVal = cxx_allocator().New();
                        func( true, pNode->m_key, pVal->m_val );
                    }
                    else
                        func( false, pNode->m_key, pVal->m_val );
                    return pVal;
                },
                update_flags::allow_insert | update_flags::allow_update 
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
                void operator()(mapped_type& item) { ... }
            };
            \endcode

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return base_class::erase( key, [&f]( internal_mapped_type& v) { f( v.m_val ); });
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
            return base_class::erase_with( key, pred, [&f]( internal_mapped_type& v) { f( v.m_val ); } );
        }

        /// Extracts an item with minimal key from the map
        /**
            Returns \p exempt_ptr pointer to the leftmost item.
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
            return exempt_ptr( base_class::do_extract_min());
        }

        /// Extracts an item with maximal key from the map
        /**
            Returns \p exempt_ptr pointer to the rightmost item.
            If the set is empty, returns empty \p exempt_ptr.

            @note Due the concurrent nature of the map, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key great than leftmost item's key.
            So, the function returns the item with maximum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not free the item.
            The deallocator will be implicitly invoked when the returned object is destroyed or when
            its \p release() is called.
        */
        exempt_ptr extract_max()
        {
            return exempt_ptr( base_class::do_extract_min());
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
            return exempt_ptr( base_class::do_extract( key ));
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
            return exempt_ptr( base_class::do_extract_with( key, pred ));
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
            return base_class::find( key, [&f]( key_type const& key, internal_mapped_type& v) { f( key, v.m_val ); });
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
            return base_class::find_with( key, pred, [&f]( key_type const& key, internal_mapped_type& v) { f( key, v.m_val ); } );
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

#endif // #ifndef CDSLIB_CONTAINER_IMPL_BRONSON_AVLTREE_MAP_RCU_H

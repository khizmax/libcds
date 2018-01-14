//
// Created by pasha on 10.01.18.
//

#ifndef CDS_ELLEN_BINTREE_MAP_NOGC_H
#define CDS_ELLEN_BINTREE_MAP_NOGC_H
#include <cds/container/details/ellen_bintree_base.h>
#include <cds/intrusive/ellen_bintree_nogc.h>

namespace cds { namespace container {

        /*namespace details {

            template <typename Key, typename T, class Traits>
            struct make_ellen_bintree_map_nogc: public make_ellen_bintree_map<gc::nogc,Key, T, Traits>
            {
                typedef make_ellen_bintree_map<cds::gc::nogc, Key, T, Traits>  base_maker;
                typedef typename base_maker::node_type node_type;

                struct intrusive_traits: public base_maker::intrusive_traits
                {
                    typedef typename base_maker::node_deallocator    disposer;
                };

                typedef intrusive::MichaelList<cds::gc::nogc, node_type, intrusive_traits>  type;
            };

        }   // namespace details*/

        template <
                typename Key,
                typename T,
#ifdef CDS_DOXYGEN_INVOKED
                class Traits = ellen_bintree::traits
#else
                class Traits
#endif
        >
        class EllenBinTreeMap<gc::nogc, Key, T, Traits >
                : public ellen_bintree::details::make_ellen_bintree_map< gc::nogc, Key, T, Traits >::type
        {
            typedef ellen_bintree::details::make_ellen_bintree_map< gc::nogc,Key, T, Traits > maker;
            typedef typename maker::type base_class;
            //@endcond

        public:
            typedef cds::gc::nogc      gc;         ///< Garbage collector
            typedef Key     key_type;   ///< type of a key to be stored in internal nodes; key is a part of \p value_type
            typedef T       value_type; ///< type of value to be stored in the binary tree
            typedef Traits  traits;    ///< Traits template parameter

#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined key_comparator  ;    ///< key compare functor based on opt::compare and opt::less option setter.
#   else
            typedef typename maker::intrusive_traits::compare   key_comparator;
#   endif
            typedef typename base_class::item_counter           item_counter;  ///< Item counting policy used
            typedef typename base_class::memory_model           memory_model;  ///< Memory ordering. See cds::opt::memory_model option
            typedef typename traits::key_extractor              key_extractor; ///< key extracting functor
            typedef typename traits::back_off                   back_off;      ///< Back-off strategy

            typedef typename traits::allocator                  allocator_type;   ///< Allocator for leaf nodes
            typedef typename base_class::node_allocator         node_allocator;   ///< Internal node allocator
            typedef typename base_class::update_desc_allocator  update_desc_allocator; ///< Update descriptor allocator

        protected:
            //@cond

            typedef typename maker::cxx_leaf_node_allocator cxx_leaf_node_allocator;
            typedef typename base_class::value_type         leaf_node;
            typedef typename base_class::internal_node      internal_node;

            typedef std::unique_ptr< leaf_node, typename maker::leaf_deallocator > scoped_node_ptr;

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

            // Inserts new node and initialize it by a functor
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




        };
    }
}


#endif //CDS_ELLEN_BINTREE_MAP_NOGC_H

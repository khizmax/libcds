#ifndef CDS_ELLEN_BINTREE_MAP_NOGC_H
#define CDS_ELLEN_BINTREE_MAP_NOGC_H
#include <cds/container/details/ellen_bintree_base.h>
#include <cds/intrusive/ellen_bintree_nogc.h>

namespace cds { namespace container {

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
            template <typename K>
            bool insert( K const& key )
            {
                return insert_with( key, [](value_type&){} );
            }

            /// Inserts new node
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
            template <typename K, typename Func>
            bool find( K const& key, Func f )
            {
                return base_class::find( key, [&f](leaf_node& item, K const& ) { f( item.m_Value );});
            }

            /// Finds the key \p val using \p pred predicate for searching
            template <typename K, typename Less, typename Func>
            bool find_with( K const& key, Less pred, Func f )
            {
                CDS_UNUSED( pred );
                return base_class::find_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >(),
                                              [&f](leaf_node& item, K const& ) { f( item.m_Value );});
            }
            
                /// Clears the map
                void clear()
                {
                    base_class::clear();
                }

            /// Checks whether the map contains \p key
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
            template <typename Q>
            value_type * get( Q const& key ) const
            {
                leaf_node * pNode = base_class::get( key );
                return pNode ? &pNode->m_Value : nullptr;
            }

            /// Finds \p key with \p pred predicate and return the item found
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
            size_t size() const
            {
                return base_class::size();
            }




        };
    }
}


#endif //CDS_ELLEN_BINTREE_MAP_NOGC_H

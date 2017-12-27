//
// Created by pasha on 23.12.17.
//

#ifndef MDP_ELLEN_BEEN_TREE_ELLEN_BEENTREE_NOGC2_H
#define MDP_ELLEN_BEEN_TREE_ELLEN_BEENTREE_NOGC2_H
#include <cds/intrusive/details/ellen_bintree_base.h>
#include <cds/gc/nogc.h>
#include <cds/opt/compare.h>
namespace cds {
    namespace intrusive {
        template < //class GC,
                typename Key,
                typename T,
                class Traits
        >
        class EllenBinTreeNogc
        {
        public:
            typedef gc::nogc gc;
            typedef Key     key_type;
            typedef  T value_type;
            typedef Traits  traits;

            typedef typename traits::hook      hook;
            typedef typename traits::node_allocator        node_allocator;
            typedef typename traits::memory_model  memory_model;
            typedef typename traits::key_extractor key_extractor;
            typedef typename traits::back_off  back_off;
            typedef typename traits::update_desc_allocator update_desc_allocator;
            typedef typename hook::node_type   node_type;



        protected:
            typedef ellen_bintree::base_node< gc >            tree_node;
            typedef node_type                            leaf_node;
            typedef ellen_bintree::node_types< gc, key_type, typename leaf_node::tag > node_factory;
        public:
            typedef typename node_factory::internal_node_type internal_node;
        protected:
            typedef ellen_bintree::update_desc< leaf_node, internal_node> update_desc;
            typedef typename update_desc::update_ptr          update_ptr;

            typedef cds::details::Allocator< internal_node, node_allocator >        cxx_node_allocator;
        public:
            typedef typename traits::item_counter  item_counter;
            typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
            struct node_traits: public get_node_traits< value_type, node_type, hook>::type
            {
                static internal_node const& to_internal_node( tree_node const& n )
                {
                    assert( n.is_internal());
                    return static_cast<internal_node const&>( n );
                }

                static leaf_node const& to_leaf_node( tree_node const& n )
                {
                    assert( n.is_leaf());
                    return static_cast<leaf_node const&>( n );
                }
            };

        protected:
            //@cond
            internal_node       m_Root;     ///< Tree root node (key= Infinite2)
            leaf_node           m_LeafInf1; ///< Infinite leaf 1 (key= Infinite1)
            leaf_node           m_LeafInf2; ///< Infinite leaf 2 (key= Infinite2)

            item_counter        m_ItemCounter;

        public:
            typedef ellen_bintree::details::compare< key_type, value_type, key_comparator, node_traits > node_compare;
        protected:
            typedef cds::details::Allocator< update_desc, update_desc_allocator >   cxx_update_desc_allocator;
            struct search_result{
                internal_node      *pParent;
                leaf_node *         pLeaf;
                bool                bRightLeaf;


                update_ptr          updParent;
                search_result()
                        :pParent( nullptr )
                        ,pLeaf( nullptr )
                        ,bRightLeaf( false )
                {}
            };

            struct internal_node_deleter {
                void operator()( internal_node* p) const
                {
                    cxx_node_allocator().Delete( p );
                }
            };

            internal_node * alloc_internal_node() const
            {
                internal_node * pNode = cxx_node_allocator().New();
                return pNode;
            }
            update_desc * alloc_update_desc() const
            {
                return cxx_update_desc_allocator().New();
            }

            static void free_update_desc( void* pDesc )
            {
                cxx_update_desc_allocator().Delete( reinterpret_cast<update_desc*>( pDesc ));
            }

            typedef std::unique_ptr< internal_node, internal_node_deleter>  unique_internal_node_ptr;



        public:

            EllenBinTreeNogc()
            {
                make_empty_tree();
            }



            template <typename KeyValue, typename Compare>
            bool search(search_result& res, KeyValue const& key, Compare cmp) const
            {
                internal_node * pParent;
                tree_node *     pLeaf;
                update_ptr      updParent;

                bool bRightLeaf;
                bool bRightParent = false;

                int nCmp = 0;

                pParent = nullptr;
                pLeaf = const_cast<internal_node *>( &m_Root );
                updParent = nullptr;
                bRightLeaf = false;
                while ( pLeaf->is_internal())
                {
                    pParent = static_cast<internal_node *>( pLeaf );
                    bRightParent = bRightLeaf;
                    updParent = pParent->m_pUpdate.load( memory_model::memory_order_acquire );
                    nCmp = cmp( key, *pParent );
                    bRightLeaf = nCmp >= 0;
                    pLeaf = pParent->get_child( nCmp >= 0, memory_model::memory_order_acquire );
                };
                assert( pLeaf->is_leaf());

                leaf_node* keyValue =  static_cast<leaf_node *>( pLeaf );
                nCmp = cmp( key, *static_cast<leaf_node *>(pLeaf));

                res.pParent         = pParent;
                res.pLeaf           = static_cast<leaf_node *>( pLeaf );
                res.updParent       = updParent;
                res.bRightLeaf      = bRightLeaf;
                return nCmp == 0;
            }


            template <typename Func>
            bool insert( value_type& val, Func f  )
            {
                search_result res;
                back_off bkoff;
                unique_internal_node_ptr pNewInternal;

                while(true) {
                    if (search(res, val, node_compare())) {
                        return false; // uniq value
                    }

                    if (res.updParent.bits() == update_desc::Clean) {
                        if (!pNewInternal.get())
                        {
                            pNewInternal.reset(alloc_internal_node());
                        }

                        if (try_insert(val, pNewInternal.get(), res)) {
                            f( val );
                            pNewInternal.release();
                            break;
                        }
                    }
                    bkoff();
                }
                ++m_ItemCounter;
                return true;
              }

            bool insert( value_type& val )
            {
                return insert( val, []( value_type& ) {} );
            }


            bool empty() const
            {
                return m_Root.m_pLeft.load( memory_model::memory_order_relaxed )->is_leaf();
            }

            size_t size() const
            {
                return m_ItemCounter;
            }

            template <typename Q>
            bool contains( Q const& key ) const
            {
                search_result    res;
                if ( search( res, key, node_compare())) {
                    return true;
                }

                return false;
            }


            template <typename Q, typename Less>
            bool contains( Q const& key, Less pred ) const
            {
                typedef ellen_bintree::details::compare<
                        key_type,
                        value_type,
                        opt::details::make_comparator_from_less<Less>,
                        node_traits
                > compare_functor;

                search_result    res;
                if ( search( res, key, compare_functor())) {
                    return true;
                }
                return false;
            }

            template <typename Q>
            bool find( Q const& key )
            {
                return contains( key );
            }

            void my_printf_tree (internal_node nodePrint)
            {

                internal_node * pParent;
                tree_node *     left;
                tree_node *     right;
                tree_node *     pLeaf;
                leaf_node *     leaf;

                pLeaf = const_cast<internal_node *>( &nodePrint );
                while ( pLeaf->is_internal())
                {
                    pParent = static_cast<internal_node *>( pLeaf );
                    left = pParent->get_child( false, memory_model::memory_order_acquire );
                    right = pParent->get_child( true, memory_model::memory_order_acquire );
                    my_printf_tree(*(static_cast<internal_node *>( left )));
                    my_printf_tree(*(static_cast<internal_node *>( right )));
                };

                leaf = static_cast<leaf_node *>(pLeaf );
                //TUT LEAF
            }

            template <typename Q, typename Func>
            bool find( Q& key, Func f ) const
            {
                return find_( key, f );
            }
            //@cond
            template <typename Q, typename Func>
            bool find( Q const& key, Func f ) const
            {
                return find_( key, f );
            }

            template <typename Q, typename Less, typename Func>
            bool find_with( Q& key, Less pred, Func f ) const
            {
                return find_with_( key, pred, f );
            }

            template <typename Q, typename Less, typename Func>
            bool find_with( Q const& key, Less pred, Func f ) const
            {
                return find_with_( key, pred, f );
            }

            template <typename Func>
            std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
            {

                unique_internal_node_ptr pNewInternal;
                search_result res;
                back_off bkoff;

                for ( ;; ) {
                    if ( search( res, val, node_compare())) {
                        func( false, *node_traits::to_value_ptr( res.pLeaf ), val );
                        return std::make_pair( true, false );
                    }

                    if ( res.updParent.bits() == update_desc::Clean )  {
                        if ( !bAllowInsert )
                            return std::make_pair( false, false );

                        if ( !pNewInternal.get())
                            pNewInternal.reset( alloc_internal_node());

                        if ( try_insert( val, pNewInternal.get(), res )) {
                            func( true, val, val );
                            pNewInternal.release()  ;   // internal node has been linked into the tree and should not be deleted
                            break;
                        }
                    }
                    bkoff();
                }

                ++m_ItemCounter;
                return std::make_pair( true, true );
            }

        protected:
            bool try_insert( value_type& val,  internal_node * pNewInternal, search_result& res )
            {
                assert( res.updParent.bits() == update_desc::Clean );
                assert( res.pLeaf->is_leaf());

                if (  static_cast<leaf_node *>( res.pParent->get_child( res.bRightLeaf, memory_model::memory_order_relaxed )) == res.pLeaf)
                {
                    leaf_node * pNewLeaf = node_traits::to_node_ptr( val );

                    int nCmp = node_compare()(val, *res.pLeaf);
                    if ( nCmp < 0 )
                    {
                        if( res.pLeaf->infinite_key() )
                        {
                            pNewInternal->infinite_key( 1 );
                        }
                        else
                        {
                            pNewInternal->infinite_key( 0 );
                            key_extractor()(pNewInternal->m_Key, *node_traits::to_value_ptr( res.pLeaf ));
                        }
                        pNewInternal->m_pLeft.store( static_cast<tree_node *>(pNewLeaf), memory_model::memory_order_relaxed );
                        pNewInternal->m_pRight.store( static_cast<tree_node *>(res.pLeaf), memory_model::memory_order_relaxed );
                    }
                    else
                    {
                        pNewInternal->infinite_key( 0 );
                        key_extractor()(pNewInternal->m_Key, val);
                        pNewInternal->m_pLeft.store( static_cast<tree_node *>(res.pLeaf), memory_model::memory_order_relaxed );
                        pNewInternal->m_pRight.store( static_cast<tree_node *>(pNewLeaf), memory_model::memory_order_relaxed );
                    }
                    update_desc * pOp = alloc_update_desc();

                    pOp->iInfo.pParent = res.pParent;
                    pOp->iInfo.pNew = pNewInternal;
                    pOp->iInfo.pLeaf = res.pLeaf;
                    pOp->iInfo.bRightLeaf = res.bRightLeaf;

                    update_ptr updCur( res.updParent.ptr());

                    if ( res.pParent->m_pUpdate.compare_exchange_strong( updCur, update_ptr( pOp, update_desc::IFlag ),
                                                                         memory_model::memory_order_acq_rel, atomics::memory_order_acquire ))
                    {

                        // do insert
                        help_insert( pOp );
                        return true;
                    }
                    else
                    {
                        free_update_desc( pOp );
                    }

                    /*if ( res.pParent->m_pUpdate.compare_exchange_strong( updCur,
                                                                         update_ptr( nullptr, update_desc::IFlag ),
                                                                         memory_model::memory_order_acq_rel, atomics::memory_order_relaxed ))
                    {
                        tree_node * pLeaf = static_cast<tree_node *>( res.pLeaf );

                        if ( res.bRightLeaf )
                        {
                            res.pParent->m_pRight.compare_exchange_strong( pLeaf, static_cast<tree_node *>( pNewInternal ),
                                                                           memory_model::memory_order_release, atomics::memory_order_relaxed );
                        }
                        else
                        {
                            res.pParent->m_pLeft.compare_exchange_strong( pLeaf, static_cast<tree_node *>( pNewInternal ),
                                                                          memory_model::memory_order_release, atomics::memory_order_relaxed );
                        }
                        update_ptr cur( nullptr, update_desc::IFlag );

                        return true;
                    }*/
                }
                return false;
            }

            void help_insert( update_desc * pOp )
            {
                // pOp must be guarded

                tree_node * pLeaf = static_cast<tree_node *>( pOp->iInfo.pLeaf );
                if ( pOp->iInfo.bRightLeaf ) {
                    pOp->iInfo.pParent->m_pRight.compare_exchange_strong( pLeaf, static_cast<tree_node *>( pOp->iInfo.pNew ),
                                                                                      memory_model::memory_order_release, atomics::memory_order_relaxed );
                }
                else {
                    pOp->iInfo.pParent->m_pLeft.compare_exchange_strong( pLeaf, static_cast<tree_node *>( pOp->iInfo.pNew ),
                                                                                     memory_model::memory_order_release, atomics::memory_order_relaxed );
                }

                // Unflag parent
                update_ptr cur( pOp, update_desc::IFlag );
                CDS_VERIFY( pOp->iInfo.pParent->m_pUpdate.compare_exchange_strong( cur, pOp->iInfo.pParent->null_update_desc(),
                                                                                   memory_model::memory_order_release, atomics::memory_order_relaxed ));
            }

            void make_empty_tree()
            {
                m_Root.infinite_key( 2 );
                m_LeafInf1.infinite_key( 1 );
                m_LeafInf2.infinite_key( 2 );
                m_Root.m_pLeft.store( &m_LeafInf1, memory_model::memory_order_relaxed );
                m_Root.m_pRight.store( &m_LeafInf2, memory_model::memory_order_release );
            }

            template <typename Q, typename Less, typename Func>
            bool find_with_( Q& val, Less pred, Func f ) const
            {
                typedef ellen_bintree::details::compare<
                        key_type,
                        value_type,
                        opt::details::make_comparator_from_less<Less>,
                        node_traits
                > compare_functor;

                search_result    res;
                if ( search( res, val, compare_functor())) {
                    assert( res.pLeaf );
                    f( *node_traits::to_value_ptr( res.pLeaf ), val );

                    return true;
                }

                return false;
            }

            template <typename Q, typename Func>
            bool find_( Q& val, Func f ) const
            {
                search_result    res;
                if ( search( res, val, node_compare())) {
                    f( *node_traits::to_value_ptr( res.pLeaf ), val );
                    return true;
                }

                return false;
            }
        };
    }
}
#endif //MDP_ELLEN_BEEN_TREE_ELLEN_BEENTREE_NOGC2_H

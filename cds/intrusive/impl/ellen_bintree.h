// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_IMPL_ELLEN_BINTREE_H
#define CDSLIB_INTRUSIVE_IMPL_ELLEN_BINTREE_H

#include <memory>
#include <cds/intrusive/details/ellen_bintree_base.h>
#include <cds/opt/compare.h>
#include <cds/details/binary_functor_wrapper.h>
#include <cds/urcu/details/check_deadlock.h>

namespace cds { namespace intrusive {

    /// Ellen's et al binary search tree
    /** @ingroup cds_intrusive_map
        @ingroup cds_intrusive_tree
        @anchor cds_intrusive_EllenBinTree

        Source:
            - [2010] F.Ellen, P.Fatourou, E.Ruppert, F.van Breugel "Non-blocking Binary Search Tree"

        %EllenBinTree is an <i>unbalanced</i> leaf-oriented binary search tree that implements the <i>set</i>
        abstract data type. Nodes maintains child pointers but not parent pointers.
        Every internal node has exactly two children, and all data of type \p T currently in
        the tree are stored in the leaves. Internal nodes of the tree are used to direct \p find()
        operation along the path to the correct leaf. The keys (of \p Key type) stored in internal nodes
        may or may not be in the set. \p Key type is a subset of \p T type.
        There should be exactly defined a key extracting functor for converting object of type \p T to
        object of type \p Key.

        Due to \p extract_min() and \p extract_max() member functions the \p %EllenBinTree can act as
        a <i>priority queue</i>. In this case you should provide unique compound key, for example,
        the priority value plus some uniformly distributed random value.

        @note In the current implementation we do not use helping technique described in the original paper.
        In Hazard Pointer schema the helping is too complicated and does not give any observable benefits.
        Instead of helping, when a thread encounters a concurrent operation it just spins waiting for
        the operation done. Such solution allows greatly simplify implementation of the tree.

        @attention Recall the tree is <b>unbalanced</b>. The complexity of operations is <tt>O(log N)</tt>
        for uniformly distributed random keys, but in the worst case the complexity is <tt>O(N)</tt>.

        @note Do not include <tt><cds/intrusive/impl/ellen_bintree.h></tt> header file explicitly.
        There are header file for each GC type:
        - <tt><cds/intrusive/ellen_bintree_hp.h></tt> - for Hazard Pointer GC \p cds::gc::HP
        - <tt><cds/intrusive/ellen_bintree_dhp.h></tt> - for Dynamic Hazard Pointer GC \p cds::gc::DHP
        - <tt><cds/intrusive/ellen_bintree_rcu.h></tt> - for RCU (see \ref cds_intrusive_EllenBinTree_rcu "RCU-based EllenBinTree")

        <b>Template arguments</b> :
        - \p GC - garbage collector, possible types are cds::gc::HP, cds::gc::DHP.
        - \p Key - key type, a subset of \p T
        - \p T - type to be stored in tree's leaf nodes. The type must be based on \p ellen_bintree::node
            (for \p ellen_bintree::base_hook) or it must have a member of type \p ellen_bintree::node
            (for \p ellen_bintree::member_hook).
        - \p Traits - tree traits, default is \p ellen_bintree::traits
            It is possible to declare option-based tree with \p ellen_bintree::make_traits metafunction
            instead of \p Traits template argument.

        @anchor cds_intrusive_EllenBinTree_less
        <b>Predicate requirements</b>

        \p Traits::less, \p Traits::compare and other predicates using with member fuctions should accept at least parameters
        of type \p T and \p Key in any combination.
        For example, for \p Foo struct with \p std::string key field the appropiate \p less functor is:
        \code
        struct Foo: public cds::intrusive::ellen_bintree::node< ... >
        {
            std::string m_strKey;
            ...
        };

        struct less {
            bool operator()( Foo const& v1, Foo const& v2 ) const
            { return v1.m_strKey < v2.m_strKey ; }

            bool operator()( Foo const& v, std::string const& s ) const
            { return v.m_strKey < s ; }

            bool operator()( std::string const& s, Foo const& v ) const
            { return s < v.m_strKey ; }

            // Support comparing std::string and char const *
            bool operator()( std::string const& s, char const * p ) const
            { return s.compare(p) < 0 ; }

            bool operator()( Foo const& v, char const * p ) const
            { return v.m_strKey.compare(p) < 0 ; }

            bool operator()( char const * p, std::string const& s ) const
            { return s.compare(p) > 0; }

            bool operator()( char const * p, Foo const& v ) const
            { return v.m_strKey.compare(p) > 0; }
        };
        \endcode

        Usage examples see \ref cds_intrusive_EllenBinTree_usage "here"
    */
    template < class GC,
        typename Key,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = ellen_bintree::traits
#else
        class Traits
#endif
    >
    class EllenBinTree
    {
    public:
        typedef GC      gc;         ///< Garbage collector
        typedef Key     key_type;   ///< type of a key to be stored in internal nodes; key is a part of \p value_type
        typedef T       value_type; ///< type of value stored in the binary tree
        typedef Traits  traits;     ///< Traits template parameter

        typedef typename traits::hook      hook;        ///< hook type
        typedef typename hook::node_type   node_type;   ///< node type
        typedef typename traits::disposer  disposer;    ///< leaf node disposer
        typedef typename traits::back_off  back_off;    ///< back-off strategy

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

    protected:
        //@cond
        typedef ellen_bintree::base_node< gc >            tree_node; ///< Base type of tree node
        typedef node_type                                 leaf_node; ///< Leaf node type
        typedef ellen_bintree::node_types< gc, key_type, typename leaf_node::tag > node_factory;
        typedef typename node_factory::internal_node_type internal_node; ///< Internal node type
        typedef typename node_factory::update_desc_type   update_desc;   ///< Update descriptor
        typedef typename update_desc::update_ptr          update_ptr;    ///< Marked pointer to update descriptor
        //@endcond

    public:
#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator;    ///< key compare functor based on \p Traits::compare and \p Traits::less
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits; ///< Node traits
#   else
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
#   endif

        typedef typename traits::item_counter  item_counter;   ///< Item counting policy
        typedef typename traits::memory_model  memory_model;   ///< Memory ordering, see \p cds::opt::memory_model
        typedef typename traits::stat          stat;           ///< internal statistics type
        typedef typename traits::key_extractor key_extractor;  ///< key extracting functor

        typedef typename traits::node_allocator        node_allocator;        ///< Allocator for internal node
        typedef typename traits::update_desc_allocator update_desc_allocator; ///< Update descriptor allocator

        static constexpr const size_t c_nHazardPtrCount = 9; ///< Count of hazard pointer required for the algorithm

    protected:
        //@cond
        typedef ellen_bintree::details::compare< key_type, value_type, key_comparator, node_traits > node_compare;

        typedef cds::details::Allocator< internal_node, node_allocator >        cxx_node_allocator;
        typedef cds::details::Allocator< update_desc, update_desc_allocator >   cxx_update_desc_allocator;

        struct search_result {
            enum guard_index {
                Guard_GrandParent,
                Guard_Parent,
                Guard_Leaf,
                Guard_updGrandParent,
                Guard_updParent,
                Guard_temporary,

                // end of guard indices
                guard_count
            };

            typedef typename gc::template GuardArray< guard_count > guard_array;
            guard_array guards;

            internal_node *     pGrandParent;
            internal_node *     pParent;
            leaf_node *         pLeaf;
            update_ptr          updParent;
            update_ptr          updGrandParent;
            bool                bRightLeaf;   // true if pLeaf is right child of pParent, false otherwise
            bool                bRightParent; // true if pParent is right child of pGrandParent, false otherwise

            search_result()
                :pGrandParent( nullptr )
                ,pParent( nullptr )
                ,pLeaf( nullptr )
                ,bRightLeaf( false )
                ,bRightParent( false )
            {}
        };
        //@endcond

    protected:
        //@cond
        internal_node       m_Root;     ///< Tree root node (key= Infinite2)
        leaf_node           m_LeafInf1; ///< Infinite leaf 1 (key= Infinite1)
        leaf_node           m_LeafInf2; ///< Infinite leaf 2 (key= Infinite2)
        //@endcond

        item_counter        m_ItemCounter;  ///< item counter
        mutable stat        m_Stat;         ///< internal statistics

    protected:
        //@cond
        static void free_leaf_node( void* p )
        {
            disposer()( reinterpret_cast<value_type*>( p ));
        }

        internal_node * alloc_internal_node() const
        {
            m_Stat.onInternalNodeCreated();
            internal_node * pNode = cxx_node_allocator().New();
            return pNode;
        }

        static void free_internal_node( void* pNode )
        {
            cxx_node_allocator().Delete( reinterpret_cast<internal_node*>( pNode ));
        }

        struct internal_node_deleter {
            void operator()( internal_node* p) const
            {
                cxx_node_allocator().Delete( p );
            }
        };

        typedef std::unique_ptr< internal_node, internal_node_deleter>  unique_internal_node_ptr;

        update_desc * alloc_update_desc() const
        {
            m_Stat.onUpdateDescCreated();
            return cxx_update_desc_allocator().New();
        }

        static void free_update_desc( void* pDesc )
        {
            cxx_update_desc_allocator().Delete( reinterpret_cast<update_desc*>( pDesc ));
        }

        void retire_node( tree_node * pNode ) const
        {
            if ( pNode->is_leaf()) {
                assert( static_cast<leaf_node *>( pNode ) != &m_LeafInf1 );
                assert( static_cast<leaf_node *>( pNode ) != &m_LeafInf2 );

                gc::template retire( node_traits::to_value_ptr( static_cast<leaf_node *>( pNode )), free_leaf_node );
            }
            else {
                assert( static_cast<internal_node *>( pNode ) != &m_Root );
                m_Stat.onInternalNodeDeleted();

                gc::template retire( static_cast<internal_node *>( pNode ), free_internal_node );
            }
        }

        void retire_update_desc( update_desc * p ) const
        {
            m_Stat.onUpdateDescDeleted();
            gc::template retire( p, free_update_desc );
        }

        void make_empty_tree()
        {
            m_Root.infinite_key( 2 );
            m_LeafInf1.infinite_key( 1 );
            m_LeafInf2.infinite_key( 2 );
            m_Root.m_pLeft.store( &m_LeafInf1, memory_model::memory_order_relaxed );
            m_Root.m_pRight.store( &m_LeafInf2, memory_model::memory_order_release );
        }
        //@endcond

    public:
        /// Default constructor
        EllenBinTree()
        {
            static_assert( !std::is_same< key_extractor, opt::none >::value, "The key extractor option must be specified" );
            make_empty_tree();
        }

        /// Clears the tree
        ~EllenBinTree()
        {
            unsafe_clear();
        }

        /// Inserts new node
        /**
            The function inserts \p val in the tree if it does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is placed into the tree, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert( val, []( value_type& ) {} );
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the tree
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this tree's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            typename gc::Guard guardInsert;
            guardInsert.assign( &val );

            unique_internal_node_ptr pNewInternal;
            search_result res;
            back_off bkoff;

            for ( ;; ) {
                if ( search( res, val, node_compare())) {
                    if ( pNewInternal.get())
                        m_Stat.onInternalNodeDeleted() ;    // unique_internal_node_ptr deletes internal node
                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( res.updGrandParent.bits() == update_desc::Clean && res.updParent.bits() == update_desc::Clean ) {

                    if ( !pNewInternal.get())
                        pNewInternal.reset( alloc_internal_node());

                    if ( try_insert( val, pNewInternal.get(), res )) {
                        f( val );
                        pNewInternal.release(); // internal node is linked into the tree and should not be deleted
                        break;
                    }
                }

                bkoff();
                m_Stat.onInsertRetry();
            }

            ++m_ItemCounter;
            m_Stat.onInsertSuccess();
            return true;
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the set, then \p val is inserted into the set
            iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor \p func signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor can change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            i.e. the node has been inserted or updated,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            typename gc::Guard guardInsert;
            guardInsert.assign( &val );

            unique_internal_node_ptr pNewInternal;
            search_result res;
            back_off bkoff;

            for ( ;; ) {
                if ( search( res, val, node_compare())) {
                    func( false, *node_traits::to_value_ptr( res.pLeaf ), val );
                    if ( pNewInternal.get())
                        m_Stat.onInternalNodeDeleted() ;    // unique_internal_node_ptr deletes internal node
                    m_Stat.onUpdateExist();
                    return std::make_pair( true, false );
                }

                if ( res.updGrandParent.bits() == update_desc::Clean && res.updParent.bits() == update_desc::Clean )  {
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
                m_Stat.onUpdateRetry();
            }

            ++m_ItemCounter;
            m_Stat.onUpdateNew();
            return std::make_pair( true, true );
        }
        //@cond
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Unlinks the item \p val from the tree
        /**
            The function searches the item \p val in the tree and unlink it from the tree
            if it is found and is equal to \p val.

            Difference between \ref erase and \p unlink functions: \p erase finds <i>a key</i>
            and deletes the item found. \p unlink finds an item by key and deletes it
            only if \p val is a node, i.e. the pointer to item found is equal to <tt> &val </tt>.

            The \p disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            return erase_( val, node_compare(),
                []( value_type const& v, leaf_node const& n ) -> bool { return &v == node_traits::to_value_ptr( n ); },
                [](value_type const&) {} );
        }

        /// Deletes the item from the tree
        /** \anchor cds_intrusive_EllenBinTree_erase
            The function searches an item with key equal to \p key in the tree,
            unlinks it from the tree, and returns \p true.
            If the item with key equal to \p key is not found the function return \p false.

            Note the \p Traits::less and/or \p Traits::compare predicate should accept a parameter of type \p Q
            that can be not the same as \p value_type.
        */
        template <typename Q>
        bool erase( const Q& key )
        {
            return erase_( key, node_compare(),
                []( Q const&, leaf_node const& ) -> bool { return true; },
                [](value_type const&) {} );
        }

        /// Delete the item from the tree with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_EllenBinTree_erase "erase(Q const&)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_less
            "Predicate requirements".
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less>
        bool erase_with( const Q& key, Less pred )
        {
            CDS_UNUSED( pred );
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            return erase_( key, compare_functor(),
                []( Q const&, leaf_node const& ) -> bool { return true; },
                [](value_type const&) {} );
        }

        /// Deletes the item from the tree
        /** \anchor cds_intrusive_EllenBinTree_erase_func
            The function searches an item with key equal to \p key in the tree,
            call \p f functor with item found, unlinks it from the tree, and returns \p true.
            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode

            If the item with key equal to \p key is not found the function return \p false.

            Note the \p Traits::less and/or \p Traits::compare predicate should accept a parameter of type \p Q
            that can be not the same as \p value_type.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return erase_( key, node_compare(),
                []( Q const&, leaf_node const& ) -> bool { return true; },
                f );
        }

        /// Delete the item from the tree with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_EllenBinTree_erase_func "erase(Q const&, Func)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_less
            "Predicate requirements".
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            return erase_( key, compare_functor(),
                []( Q const&, leaf_node const& ) -> bool { return true; },
                f );
        }

        /// Extracts an item with minimal key from the tree
        /**
            The function searches an item with minimal key, unlinks it, and returns a guarded pointer to an item found.
            If the tree is empty the function returns an empty guarded pointer.

            @note Due the concurrent nature of the tree, the function extracts <i>nearly</i> minimum key.
            It means that the function gets leftmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key less than leftmost item's key.
            So, the function returns the item with minimum key at the moment of tree traversing.

            The returned \p guarded_ptr prevents disposer invocation for returned item,
            see \p cds::gc::guarded_ptr for explanation.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.
        */
        guarded_ptr extract_min()
        {
            return extract_min_();
        }

        /// Extracts an item with maximal key from the tree
        /**
            The function searches an item with maximal key, unlinks it, and returns a guarded pointer to an item found.
            If the tree is empty the function returns an empty \p guarded_ptr.

            @note Due the concurrent nature of the tree, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key great than rightmost item's key.
            So, the function returns the item with maximal key at the moment of tree traversing.

            The returned \p guarded_ptr prevents disposer invocation for returned item,
            see cds::gc::guarded_ptr for explanation.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.
        */
        guarded_ptr extract_max()
        {
            return extract_max_();
        }

        /// Extracts an item from the tree
        /** \anchor cds_intrusive_EllenBinTree_extract
            The function searches an item with key equal to \p key in the tree,
            unlinks it, and returns a guarded pointer to an item found.
            If the item  is not found the function returns an empty \p guarded_ptr.

            \p guarded_ptr prevents disposer invocation for returned item,
            see cds::gc::guarded_ptr for explanation.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.
        */
        template <typename Q>
        guarded_ptr extract( Q const& key )
        {
            return extract_( key );
        }

        /// Extracts an item from the tree using \p pred for searching
        /**
            The function is an analog of \ref cds_intrusive_EllenBinTree_extract "extract(Q const&)"
            but \p pred is used for key compare.
            \p Less has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_less
            "Predicate requirements".
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            return extract_with_( key, pred );
        }

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key ) const
        {
            search_result    res;
            if ( search( res, key, node_compare())) {
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key ) const
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            search_result    res;
            if ( search( res, key, compare_functor())) {
                m_Stat.onFindSuccess();
                return true;
            }
            m_Stat.onFindFailed();
            return false;
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred ) const
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds the key \p key
        /** @anchor cds_intrusive_EllenBinTree_find_func
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor can change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the tree \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The function returns \p true if \p key is found, \p false otherwise.
        */
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
        //@endcond

        /// Finds the key \p key with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_EllenBinTree_find_func "find(Q&, Func)"
            but \p pred is used for key comparison.
            \p Less functor has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_less
            "Predicate requirements".
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            return find_with_( key, pred, f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            return find_with_( key, pred, f );
        }
        //@endcond

        /// Finds \p key and returns the item found
        /** @anchor cds_intrusive_EllenBinTree_get
            The function searches the item with key equal to \p key and returns the item found as \p guarded_ptr object.
            The function returns an empty guarded pointer is \p key is not found.

            \p guarded_ptr prevents disposer invocation for returned item,
            see \p cds::gc::guarded_ptr for explanation.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.
        */
        template <typename Q>
        guarded_ptr get( Q const& key ) const
        {
            return get_( key );
        }

        /// Finds \p key with predicate \p pred and returns the item found
        /**
            The function is an analog of \ref cds_intrusive_EllenBinTree_get "get(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_less
            "Predicate requirements".
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred ) const
        {
            return get_with_( key, pred );
        }

        /// Checks if the tree is empty
        bool empty() const
        {
            return m_Root.m_pLeft.load( memory_model::memory_order_relaxed )->is_leaf();
        }

        /// Clears the tree (thread safe, not atomic)
        /**
            The function unlink all items from the tree.
            The function is thread safe but not atomic: in multi-threaded environment with parallel insertions
            this sequence
            \code
            tree.clear();
            assert( tree.empty());
            \endcode
            the assertion could be raised.

            For each leaf the \p disposer will be called after unlinking.
        */
        void clear()
        {
            guarded_ptr gp;
            do {
                gp = extract_min();
            }  while ( gp );
        }

        /// Clears the tree (not thread safe)
        /**
            This function is not thread safe and may be called only when no other thread deals with the tree.
            The function is used in the tree destructor.
        */
        void unsafe_clear()
        {
            while ( true ) {
                internal_node * pParent = nullptr;
                internal_node * pGrandParent = nullptr;
                tree_node *     pLeaf = const_cast<internal_node *>( &m_Root );

                // Get leftmost leaf
                while ( pLeaf->is_internal()) {
                    pGrandParent = pParent;
                    pParent = static_cast<internal_node *>( pLeaf );
                    pLeaf = pParent->m_pLeft.load( memory_model::memory_order_relaxed );
                }

                if ( pLeaf->infinite_key()) {
                    // The tree is empty
                    return;
                }

                // Remove leftmost leaf and its parent node
                assert( pGrandParent );
                assert( pParent );
                assert( pLeaf->is_leaf());

                pGrandParent->m_pLeft.store( pParent->m_pRight.load( memory_model::memory_order_relaxed ), memory_model::memory_order_relaxed );
                free_leaf_node( node_traits::to_value_ptr( static_cast<leaf_node *>( pLeaf )));
                free_internal_node( pParent );
            }
        }

        /// Returns item count in the tree
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
            return check_consistency( &m_Root );
        }

    protected:
        //@cond

        bool check_consistency( internal_node const * pRoot ) const
        {
            tree_node * pLeft  = pRoot->m_pLeft.load( atomics::memory_order_relaxed );
            tree_node * pRight = pRoot->m_pRight.load( atomics::memory_order_relaxed );
            assert( pLeft );
            assert( pRight );

            if ( node_compare()( *pLeft, *pRoot ) < 0
                && node_compare()( *pRoot, *pRight ) <= 0
                && node_compare()( *pLeft, *pRight ) < 0 )
            {
                bool bRet = true;
                if ( pLeft->is_internal())
                    bRet = check_consistency( static_cast<internal_node *>( pLeft ));
                assert( bRet );

                if ( bRet && pRight->is_internal())
                    bRet = bRet && check_consistency( static_cast<internal_node *>( pRight ));
                assert( bRet );

                return bRet;
            }
            return false;
        }

        tree_node * protect_child_node( search_result& res, internal_node * pParent, bool bRight, update_ptr updParent ) const
        {
        retry:
            tree_node * p = bRight
                ? res.guards.protect( search_result::Guard_Leaf, pParent->m_pRight,
                    []( tree_node * pn ) -> internal_node* { return static_cast<internal_node *>(pn);})
                : res.guards.protect( search_result::Guard_Leaf, pParent->m_pLeft,
                    []( tree_node * pn ) -> internal_node* { return static_cast<internal_node *>(pn);});

            // If we use member hook, data node pointer != internal node pointer
            // So, we need protect the child twice: as internal node and as data node
            // and then analyze what kind of node we have
            tree_node * pVal = bRight
                ? res.guards.protect( search_result::Guard_temporary, pParent->m_pRight,
                    []( tree_node * pn ) -> value_type* { return node_traits::to_value_ptr( static_cast<leaf_node *>(pn));} )
                : res.guards.protect( search_result::Guard_temporary, pParent->m_pLeft,
                    []( tree_node * pn ) -> value_type* { return node_traits::to_value_ptr( static_cast<leaf_node *>(pn));} );

            // child node is guarded
            // See whether pParent->m_pUpdate has not been changed
            if ( pParent->m_pUpdate.load( memory_model::memory_order_acquire ) != updParent ) {
                // update has been changed - returns nullptr as a flag to search retry
                return nullptr;
            }

            if ( p != pVal )
                goto retry;

            if ( p && p->is_leaf())
                res.guards.assign( search_result::Guard_Leaf, node_traits::to_value_ptr( static_cast<leaf_node *>( p )));

            res.guards.clear( search_result::Guard_temporary );

            return p;
        }

        static update_ptr search_protect_update( search_result& res, atomics::atomic<update_ptr> const& src )
        {
            return res.guards.protect( search_result::Guard_updParent, src, [](update_ptr p) -> update_desc* { return p.ptr(); });
        }

        template <typename KeyValue, typename Compare>
        bool search( search_result& res, KeyValue const& key, Compare cmp ) const
        {
            internal_node * pParent;
            internal_node * pGrandParent = nullptr;
            update_ptr      updParent;
            update_ptr      updGrandParent;
            bool bRightLeaf;
            bool bRightParent = false;

            int nCmp = 0;

        retry:
            pParent = nullptr;
            //pGrandParent = nullptr;
            updParent = nullptr;
            bRightLeaf = false;
            tree_node * pLeaf = const_cast<internal_node *>( &m_Root );
            while ( pLeaf->is_internal()) {
                res.guards.copy( search_result::Guard_GrandParent, search_result::Guard_Parent );
                pGrandParent = pParent;
                res.guards.copy( search_result::Guard_Parent, search_result::Guard_Leaf );
                pParent = static_cast<internal_node *>( pLeaf );
                bRightParent = bRightLeaf;
                res.guards.copy( search_result::Guard_updGrandParent, search_result::Guard_updParent );
                updGrandParent = updParent;

                updParent = search_protect_update( res, pParent->m_pUpdate );

                switch ( updParent.bits()) {
                    case update_desc::DFlag:
                    case update_desc::Mark:
                        m_Stat.onSearchRetry();
                        goto retry;
                }

                nCmp = cmp( key, *pParent );
                bRightLeaf = nCmp >= 0;

                pLeaf = protect_child_node( res, pParent, bRightLeaf, updParent );
                if ( !pLeaf ) {
                    m_Stat.onSearchRetry();
                    goto retry;
                }
            }

            assert( pLeaf->is_leaf());
            nCmp = cmp( key, *static_cast<leaf_node *>(pLeaf));

            res.pGrandParent    = pGrandParent;
            res.pParent         = pParent;
            res.pLeaf           = static_cast<leaf_node *>( pLeaf );
            res.updParent       = updParent;
            res.updGrandParent  = updGrandParent;
            res.bRightParent    = bRightParent;
            res.bRightLeaf      = bRightLeaf;

            return nCmp == 0;
        }

        bool search_min( search_result& res ) const
        {
            internal_node * pParent;
            internal_node * pGrandParent;
            update_ptr      updParent;
            update_ptr      updGrandParent;

        retry:
            pParent = nullptr;
            pGrandParent = nullptr;
            updParent = nullptr;
            tree_node *     pLeaf = const_cast<internal_node *>( &m_Root );
            while ( pLeaf->is_internal()) {
                res.guards.copy( search_result::Guard_GrandParent, search_result::Guard_Parent );
                pGrandParent = pParent;
                res.guards.copy( search_result::Guard_Parent, search_result::Guard_Leaf );
                pParent = static_cast<internal_node *>( pLeaf );
                res.guards.copy( search_result::Guard_updGrandParent, search_result::Guard_updParent );
                updGrandParent = updParent;

                updParent = search_protect_update( res, pParent->m_pUpdate );

                switch ( updParent.bits()) {
                    case update_desc::DFlag:
                    case update_desc::Mark:
                        m_Stat.onSearchRetry();
                        goto retry;
                }

                pLeaf = protect_child_node( res, pParent, false, updParent );
                if ( !pLeaf ) {
                    m_Stat.onSearchRetry();
                    goto retry;
                }
            }

            if ( pLeaf->infinite_key())
                return false;

            res.pGrandParent    = pGrandParent;
            res.pParent         = pParent;
            assert( pLeaf->is_leaf());
            res.pLeaf           = static_cast<leaf_node *>( pLeaf );
            res.updParent       = updParent;
            res.updGrandParent  = updGrandParent;
            res.bRightParent    = false;
            res.bRightLeaf      = false;

            return true;
        }

        bool search_max( search_result& res ) const
        {
            internal_node * pParent;
            internal_node * pGrandParent;
            update_ptr      updParent;
            update_ptr      updGrandParent;
            bool bRightLeaf;
            bool bRightParent = false;

        retry:
            pParent = nullptr;
            pGrandParent = nullptr;
            updParent = nullptr;
            bRightLeaf = false;
            tree_node * pLeaf = const_cast<internal_node *>( &m_Root );
            while ( pLeaf->is_internal()) {
                res.guards.copy( search_result::Guard_GrandParent, search_result::Guard_Parent );
                pGrandParent = pParent;
                res.guards.copy( search_result::Guard_Parent, search_result::Guard_Leaf );
                pParent = static_cast<internal_node *>( pLeaf );
                bRightParent = bRightLeaf;
                res.guards.copy( search_result::Guard_updGrandParent, search_result::Guard_updParent );
                updGrandParent = updParent;

                updParent = search_protect_update( res, pParent->m_pUpdate );

                switch ( updParent.bits()) {
                    case update_desc::DFlag:
                    case update_desc::Mark:
                        m_Stat.onSearchRetry();
                        goto retry;
                }

                bRightLeaf = !pParent->infinite_key();
                pLeaf = protect_child_node( res, pParent, bRightLeaf, updParent );
                if ( !pLeaf ) {
                    m_Stat.onSearchRetry();
                    goto retry;
                }
            }

            if ( pLeaf->infinite_key())
                return false;

            res.pGrandParent    = pGrandParent;
            res.pParent         = pParent;
            assert( pLeaf->is_leaf());
            res.pLeaf           = static_cast<leaf_node *>( pLeaf );
            res.updParent       = updParent;
            res.updGrandParent  = updGrandParent;
            res.bRightParent    = bRightParent;
            res.bRightLeaf      = bRightLeaf;

            return true;
        }

        /*
        void help( update_ptr pUpdate )
        {
            // pUpdate must be guarded!
            switch ( pUpdate.bits()) {
                case update_desc::IFlag:
                    help_insert( pUpdate.ptr());
                    m_Stat.onHelpInsert();
                    break;
                case update_desc::DFlag:
                    help_delete( pUpdate.ptr());
                    m_Stat.onHelpDelete();
                    break;
                case update_desc::Mark:
                    //m_Stat.onHelpMark();
                    //help_marked( pUpdate.ptr());
                    break;
            }
        }
        */

        void help_insert( update_desc * pOp )
        {
            // pOp must be guarded

            tree_node * pLeaf = static_cast<tree_node *>( pOp->iInfo.pLeaf );
            if ( pOp->iInfo.bRightLeaf ) {
                CDS_VERIFY( pOp->iInfo.pParent->m_pRight.compare_exchange_strong( pLeaf, static_cast<tree_node *>( pOp->iInfo.pNew ),
                    memory_model::memory_order_release, atomics::memory_order_relaxed ));
            }
            else {
                CDS_VERIFY( pOp->iInfo.pParent->m_pLeft.compare_exchange_strong( pLeaf, static_cast<tree_node *>( pOp->iInfo.pNew ),
                    memory_model::memory_order_release, atomics::memory_order_relaxed ));
            }

            // Unflag parent
            update_ptr cur( pOp, update_desc::IFlag );
            CDS_VERIFY( pOp->iInfo.pParent->m_pUpdate.compare_exchange_strong( cur, pOp->iInfo.pParent->null_update_desc(),
                memory_model::memory_order_release, atomics::memory_order_relaxed ));
        }

        bool check_delete_precondition( search_result& res ) const
        {
            // precondition: all member of res must be guarded

            assert( res.pGrandParent != nullptr );

            return static_cast<internal_node *>(res.pGrandParent->get_child( res.bRightParent, memory_model::memory_order_relaxed )) == res.pParent
                && static_cast<leaf_node *>( res.pParent->get_child( res.bRightLeaf, memory_model::memory_order_relaxed )) == res.pLeaf;
        }

        bool help_delete( update_desc * pOp )
        {
            // precondition: pOp must be guarded

            update_ptr pUpdate( pOp->dInfo.pUpdateParent );
            update_ptr pMark( pOp, update_desc::Mark );
            if ( pOp->dInfo.pParent->m_pUpdate.compare_exchange_strong( pUpdate, pMark, // *
                memory_model::memory_order_acquire, atomics::memory_order_relaxed ))
            {
                help_marked( pOp );

                retire_node( pOp->dInfo.pParent );
                retire_node( pOp->dInfo.pLeaf );
                retire_update_desc( pOp );
                return true;
            }
            else if ( pUpdate == pMark ) {
                // some other thread is processing help_marked()
                help_marked( pOp );
                m_Stat.onHelpMark();
                return true;
            }
            else {
                // Undo grandparent dInfo
                update_ptr pDel( pOp, update_desc::DFlag );
                if ( pOp->dInfo.pGrandParent->m_pUpdate.compare_exchange_strong( pDel, pOp->dInfo.pGrandParent->null_update_desc(),
                    memory_model::memory_order_release, atomics::memory_order_relaxed ))
                {
                    retire_update_desc( pOp );
                }
                return false;
            }
        }

        static tree_node * protect_sibling( typename gc::Guard& guard, atomics::atomic<tree_node *>& sibling )
        {
            tree_node * pSibling = guard.protect( sibling, [](tree_node * p) -> internal_node* { return static_cast<internal_node *>(p); } );
            if ( pSibling->is_leaf())
                guard.assign( node_traits::to_value_ptr( static_cast<leaf_node *>( pSibling )));
            return pSibling;
        }

        void help_marked( update_desc * pOp )
        {
            // precondition: pOp must be guarded

            tree_node * pParent = pOp->dInfo.pParent;

            typename gc::Guard guard;
            tree_node * pOpposite = protect_sibling( guard, pOp->dInfo.bRightLeaf ? pOp->dInfo.pParent->m_pLeft : pOp->dInfo.pParent->m_pRight );

            if ( pOp->dInfo.bRightParent ) {
                CDS_VERIFY( pOp->dInfo.pGrandParent->m_pRight.compare_exchange_strong( pParent, pOpposite,
                    memory_model::memory_order_release, atomics::memory_order_relaxed ));
            }
            else {
                CDS_VERIFY( pOp->dInfo.pGrandParent->m_pLeft.compare_exchange_strong( pParent, pOpposite,
                    memory_model::memory_order_release, atomics::memory_order_relaxed ));
            }

            update_ptr upd( pOp, update_desc::DFlag );
            CDS_VERIFY( pOp->dInfo.pGrandParent->m_pUpdate.compare_exchange_strong( upd, pOp->dInfo.pGrandParent->null_update_desc(),
                memory_model::memory_order_release, atomics::memory_order_relaxed ));
        }

        bool try_insert( value_type& val, internal_node * pNewInternal, search_result& res )
        {
            assert( res.updParent.bits() == update_desc::Clean );
            assert( res.pLeaf->is_leaf());

            // check search result
            if ( res.pParent->get_child( res.bRightLeaf, memory_model::memory_order_acquire ) == res.pLeaf ) {
                leaf_node * pNewLeaf = node_traits::to_node_ptr( val );

                int nCmp = node_compare()(val, *res.pLeaf);
                if ( nCmp < 0 ) {
                    if ( res.pGrandParent ) {
                        assert( !res.pLeaf->infinite_key());
                        pNewInternal->infinite_key( 0 );
                        key_extractor()(pNewInternal->m_Key, *node_traits::to_value_ptr( res.pLeaf ));
                    }
                    else {
                        assert( res.pLeaf->infinite_key() == tree_node::key_infinite1 );
                        pNewInternal->infinite_key( 1 );
                    }
                    pNewInternal->m_pLeft.store( static_cast<tree_node *>(pNewLeaf), memory_model::memory_order_relaxed );
                    pNewInternal->m_pRight.store( static_cast<tree_node *>(res.pLeaf), memory_model::memory_order_relaxed );
                }
                else {
                    assert( !res.pLeaf->is_internal());

                    pNewInternal->infinite_key( 0 );
                    key_extractor()(pNewInternal->m_Key, val);
                    pNewInternal->m_pLeft.store( static_cast<tree_node *>(res.pLeaf), memory_model::memory_order_relaxed );
                    pNewInternal->m_pRight.store( static_cast<tree_node *>(pNewLeaf), memory_model::memory_order_relaxed );
                    assert( !res.pLeaf->infinite_key());
                }

                typename gc::Guard guard;
                update_desc * pOp = alloc_update_desc();
                guard.assign( pOp );

                pOp->iInfo.pParent = res.pParent;
                pOp->iInfo.pNew = pNewInternal;
                pOp->iInfo.pLeaf = res.pLeaf;
                pOp->iInfo.bRightLeaf = res.bRightLeaf;

                update_ptr updCur( res.updParent.ptr());
                if ( res.pParent->m_pUpdate.compare_exchange_strong( updCur, update_ptr( pOp, update_desc::IFlag ),
                    memory_model::memory_order_acq_rel, atomics::memory_order_relaxed )) {
                    // do insert
                    help_insert( pOp );
                    retire_update_desc( pOp );
                    return true;
                }
                else {
                    m_Stat.onUpdateDescDeleted();
                    free_update_desc( pOp );
                }
            }

            return false;
        }

        template <typename Q, typename Compare, typename Equal, typename Func>
        bool erase_( Q const& val, Compare cmp, Equal eq, Func f )
        {
            update_desc * pOp = nullptr;
            search_result res;
            back_off bkoff;

            for ( ;; ) {
                if ( !search( res, val, cmp ) || !eq( val, *res.pLeaf )) {
                    if ( pOp )
                        retire_update_desc( pOp );
                    m_Stat.onEraseFailed();
                    return false;
                }

                if ( res.updGrandParent.bits() == update_desc::Clean && res.updParent.bits() == update_desc::Clean ) {
                    if ( !pOp )
                        pOp = alloc_update_desc();
                    if ( check_delete_precondition( res )) {
                        typename gc::Guard guard;
                        guard.assign( pOp );

                        pOp->dInfo.pGrandParent = res.pGrandParent;
                        pOp->dInfo.pParent = res.pParent;
                        pOp->dInfo.pLeaf = res.pLeaf;
                        pOp->dInfo.pUpdateParent = res.updParent.ptr();
                        pOp->dInfo.bRightParent = res.bRightParent;
                        pOp->dInfo.bRightLeaf = res.bRightLeaf;

                        update_ptr updGP( res.updGrandParent.ptr());
                        if ( res.pGrandParent->m_pUpdate.compare_exchange_strong( updGP, update_ptr( pOp, update_desc::DFlag ),
                            memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                            if ( help_delete( pOp )) {
                                // res.pLeaf is not deleted yet since it is guarded
                                f( *node_traits::to_value_ptr( res.pLeaf ));
                                break;
                            }
                            pOp = nullptr;
                        }
                    }
                }

                bkoff();
                m_Stat.onEraseRetry();
            }

            --m_ItemCounter;
            m_Stat.onEraseSuccess();
            return true;
        }

        template <typename Q, typename Compare>
        guarded_ptr extract_item( Q const& key, Compare cmp )
        {
            update_desc * pOp = nullptr;
            search_result res;
            back_off bkoff;

            for ( ;; ) {
                if ( !search( res, key, cmp )) {
                    if ( pOp )
                        retire_update_desc( pOp );
                    m_Stat.onEraseFailed();
                    return guarded_ptr();
                }

                if ( res.updGrandParent.bits() == update_desc::Clean && res.updParent.bits() == update_desc::Clean ) {
                    if ( !pOp )
                        pOp = alloc_update_desc();
                    if ( check_delete_precondition( res )) {
                        typename gc::Guard guard;
                        guard.assign( pOp );

                        pOp->dInfo.pGrandParent = res.pGrandParent;
                        pOp->dInfo.pParent = res.pParent;
                        pOp->dInfo.pLeaf = res.pLeaf;
                        pOp->dInfo.pUpdateParent = res.updParent.ptr();
                        pOp->dInfo.bRightParent = res.bRightParent;
                        pOp->dInfo.bRightLeaf = res.bRightLeaf;

                        update_ptr updGP( res.updGrandParent.ptr());
                        if ( res.pGrandParent->m_pUpdate.compare_exchange_strong( updGP, update_ptr( pOp, update_desc::DFlag ),
                            memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                            if ( help_delete( pOp ))
                                break;
                            pOp = nullptr;
                        }
                    }
                }

                bkoff();
                m_Stat.onEraseRetry();
            }

            --m_ItemCounter;
            m_Stat.onEraseSuccess();
            return guarded_ptr( res.guards.release( search_result::Guard_Leaf ));
        }

        template <typename Q>
        guarded_ptr extract_( Q const& key )
        {
            return extract_item( key, node_compare());
        }

        template <typename Q, typename Less>
        guarded_ptr extract_with_( Q const& key, Less /*pred*/ )
        {
            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            return extract_item( key, compare_functor());
        }

        guarded_ptr extract_max_()
        {
            update_desc * pOp = nullptr;
            search_result res;
            back_off bkoff;

            for ( ;; ) {
                if ( !search_max( res )) {
                    // Tree is empty
                    if ( pOp )
                        retire_update_desc( pOp );
                    m_Stat.onExtractMaxFailed();
                    return guarded_ptr();
                }

                if ( res.updGrandParent.bits() == update_desc::Clean && res.updParent.bits() == update_desc::Clean ) {
                    if ( !pOp )
                        pOp = alloc_update_desc();
                    if ( check_delete_precondition( res )) {
                        typename gc::Guard guard;
                        guard.assign( pOp );

                        pOp->dInfo.pGrandParent = res.pGrandParent;
                        pOp->dInfo.pParent = res.pParent;
                        pOp->dInfo.pLeaf = res.pLeaf;
                        pOp->dInfo.pUpdateParent = res.updParent.ptr();
                        pOp->dInfo.bRightParent = res.bRightParent;
                        pOp->dInfo.bRightLeaf = res.bRightLeaf;

                        update_ptr updGP( res.updGrandParent.ptr());
                        if ( res.pGrandParent->m_pUpdate.compare_exchange_strong( updGP, update_ptr( pOp, update_desc::DFlag ),
                                memory_model::memory_order_acquire, atomics::memory_order_relaxed ))
                        {
                            if ( help_delete( pOp ))
                                break;
                            pOp = nullptr;
                        }
                    }
                }

                bkoff();
                m_Stat.onExtractMaxRetry();
            }

            --m_ItemCounter;
            m_Stat.onExtractMaxSuccess();
            return guarded_ptr( res.guards.release( search_result::Guard_Leaf ));
        }

        guarded_ptr extract_min_()
        {
            update_desc * pOp = nullptr;
            search_result res;
            back_off bkoff;

            for ( ;; ) {
                if ( !search_min( res )) {
                    // Tree is empty
                    if ( pOp )
                        retire_update_desc( pOp );
                    m_Stat.onExtractMinFailed();
                    return guarded_ptr();
                }

                if ( res.updGrandParent.bits() == update_desc::Clean && res.updParent.bits() == update_desc::Clean ) {
                    if ( !pOp )
                        pOp = alloc_update_desc();
                    if ( check_delete_precondition( res )) {
                        typename gc::Guard guard;
                        guard.assign( pOp );

                        pOp->dInfo.pGrandParent = res.pGrandParent;
                        pOp->dInfo.pParent = res.pParent;
                        pOp->dInfo.pLeaf = res.pLeaf;
                        pOp->dInfo.pUpdateParent = res.updParent.ptr();
                        pOp->dInfo.bRightParent = res.bRightParent;
                        pOp->dInfo.bRightLeaf = res.bRightLeaf;

                        update_ptr updGP( res.updGrandParent.ptr());
                        if ( res.pGrandParent->m_pUpdate.compare_exchange_strong( updGP, update_ptr( pOp, update_desc::DFlag ),
                            memory_model::memory_order_acquire, atomics::memory_order_relaxed ))
                        {
                            if ( help_delete( pOp ))
                                break;
                            pOp = nullptr;
                        }
                    }
                }

                bkoff();
                m_Stat.onExtractMinRetry();
            }

            --m_ItemCounter;
            m_Stat.onExtractMinSuccess();
            return guarded_ptr( res.guards.release( search_result::Guard_Leaf ));
        }

        template <typename Q, typename Func>
        bool find_( Q& val, Func f ) const
        {
            search_result    res;
            if ( search( res, val, node_compare())) {
                assert( res.pLeaf );
                f( *node_traits::to_value_ptr( res.pLeaf ), val );

                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Less, typename Func>
        bool find_with_( Q& val, Less /*pred*/, Func f ) const
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

                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q>
        guarded_ptr get_( Q const& val ) const
        {
            search_result    res;
            if ( search( res, val, node_compare())) {
                assert( res.pLeaf );
                m_Stat.onFindSuccess();
                return guarded_ptr( res.guards.release( search_result::Guard_Leaf ));
            }

            m_Stat.onFindFailed();
            return guarded_ptr();
        }

        template <typename Q, typename Less>
        guarded_ptr get_with_( Q const& val, Less pred ) const
        {
            CDS_UNUSED( pred );

            typedef ellen_bintree::details::compare<
                key_type,
                value_type,
                opt::details::make_comparator_from_less<Less>,
                node_traits
            > compare_functor;

            search_result    res;
            if ( search( res, val, compare_functor())) {
                assert( res.pLeaf );
                m_Stat.onFindSuccess();
                return guarded_ptr( res.guards.release( search_result::Guard_Leaf ));
            }

            m_Stat.onFindFailed();
            return guarded_ptr();

        }

        //@endcond
    };

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_ELLEN_BINTREE_H

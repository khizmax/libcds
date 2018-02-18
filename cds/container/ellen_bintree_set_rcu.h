// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_ELLEN_BINTREE_SET_RCU_H
#define CDSLIB_CONTAINER_ELLEN_BINTREE_SET_RCU_H

#include <cds/container/details/ellen_bintree_base.h>
#include <cds/intrusive/ellen_bintree_rcu.h>

namespace cds { namespace container {

    /// Set based on Ellen's et al binary search tree (RCU specialization)
    /** @ingroup cds_nonintrusive_set
        @ingroup cds_nonintrusive_tree
        @anchor cds_container_EllenBinTreeSet_rcu

        Source:
            - [2010] F.Ellen, P.Fatourou, E.Ruppert, F.van Breugel "Non-blocking Binary Search Tree"

        %EllenBinTreeSet is an unbalanced leaf-oriented binary search tree that implements the <i>set</i>
        abstract data type. Nodes maintains child pointers but not parent pointers.
        Every internal node has exactly two children, and all data of type \p T currently in
        the tree are stored in the leaves. Internal nodes of the tree are used to direct \p find
        operation along the path to the correct leaf. The keys (of \p Key type) stored in internal nodes
        may or may not be in the set. \p Key type is a subset of \p T type.
        There should be exactly defined a key extracting functor for converting object of type \p T to
        object of type \p Key.

        Due to \p extract_min and \p extract_max member functions the \p %EllenBinTreeSet can act as
        a <i>priority queue</i>. In this case you should provide unique compound key, for example,
        the priority value plus some uniformly distributed random value.

        @warning Recall the tree is <b>unbalanced</b>. The complexity of operations is <tt>O(log N)</tt>
        for uniformly distributed random keys, but in the worst case the complexity is <tt>O(N)</tt>.

        @note In the current implementation we do not use helping technique described in original paper.
        So, the current implementation is near to fine-grained lock-based tree.
        Helping will be implemented in future release

        <b>Template arguments</b> :
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - key type, a subset of \p T
        - \p T - type to be stored in tree's leaf nodes.
        - \p Traits - set traits, default is \p ellen_bintree::traits.
            It is possible to declare option-based tree with \p ellen_bintree::make_set_traits metafunction
            instead of \p Traits template argument.

        @note Before including <tt><cds/container/ellen_bintree_set_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.

        @anchor cds_container_EllenBinTreeSet_rcu_less
        <b>Predicate requirements</b>

        opt::less, opt::compare and other predicates using with member fuctions should accept at least parameters
        of type \p T and \p Key in any combination.
        For example, for \p Foo struct with \p std::string key field the appropiate \p less functor is:
        \code
        struct Foo
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
    class EllenBinTreeSet< cds::urcu::gc<RCU>, Key, T, Traits >
#ifdef CDS_DOXYGEN_INVOKED
        : public cds::intrusive::EllenBinTree< cds::urcu::gc<RCU>, Key, T, Traits >
#else
        : public ellen_bintree::details::make_ellen_bintree_set< cds::urcu::gc<RCU>, Key, T, Traits >::type
#endif
    {
        //@cond
        typedef ellen_bintree::details::make_ellen_bintree_set< cds::urcu::gc<RCU>, Key, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        typedef cds::urcu::gc<RCU>  gc;   ///< RCU Garbage collector
        typedef Key     key_type;   ///< type of a key stored in internal nodes; key is a part of \p value_type
        typedef T       value_type; ///< type of value stored in the binary tree
        typedef Traits  traits;     ///< Traits template parameter

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator;    ///< key compare functor based on \p Traits::compare and \p Traits::less
#   else
        typedef typename maker::intrusive_traits::compare   key_comparator;
#   endif
        typedef typename base_class::item_counter           item_counter;       ///< Item counting policy
        typedef typename base_class::memory_model           memory_model;       ///< Memory ordering, see \p cds::opt::memory_model
        typedef typename base_class::stat                   stat;               ///< internal statistics type
        typedef typename base_class::rcu_check_deadlock     rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename traits::key_extractor              key_extractor;      ///< key extracting functor
        typedef typename traits::back_off                   back_off;           ///< Back-off strategy


        typedef typename traits::allocator                  allocator_type;     ///< Allocator for leaf nodes
        typedef typename base_class::node_allocator         node_allocator;     ///< Internal node allocator
        typedef typename base_class::update_desc_allocator  update_desc_allocator; ///< Update descriptor allocator

        static constexpr const bool c_bExtractLockExternal = base_class::c_bExtractLockExternal; ///< Group of \p extract_xxx functions do not require external locking

    protected:
        //@cond
        typedef typename maker::cxx_leaf_node_allocator cxx_leaf_node_allocator;
        typedef typename base_class::value_type         leaf_node;
        typedef typename base_class::internal_node      internal_node;
        typedef std::unique_ptr< leaf_node, typename maker::intrusive_traits::disposer >    scoped_node_ptr;
        //@endcond

    public:
        typedef typename gc::scoped_lock    rcu_lock;  ///< RCU scoped lock

        /// pointer to extracted node
        using exempt_ptr = cds::urcu::exempt_ptr < gc, leaf_node, value_type, typename maker::intrusive_traits::disposer,
            cds::urcu::details::conventional_exempt_member_cast < leaf_node, value_type >
        >;

    public:
        /// Default constructor
        EllenBinTreeSet()
            : base_class()
        {}

        /// Clears the set
        ~EllenBinTreeSet()
        {}

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the set.

            The type \p Q should contain at least the complete key for the node.
            The object of \ref value_type should be constructible from a value of type \p Q.
            In trivial case, \p Q is equal to \ref value_type.

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q const& val )
        {
            scoped_node_ptr sp( cxx_leaf_node_allocator().New( val ));
            if ( base_class::insert( *sp.get())) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Inserts new node
        /**
            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls  \p f functor to initialize value-fields of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this set's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.

            RCU \p synchronize() can be called. RCU should not be locked.
        */
        template <typename Q, typename Func>
        bool insert( Q const& val, Func f )
        {
            scoped_node_ptr sp( cxx_leaf_node_allocator().New( val ));
            if ( base_class::insert( *sp.get(), [&f]( leaf_node& v ) { f( v.m_Value ); } )) {
                sp.release();
                return true;
            }
            return false;
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

            The functor can change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            RCU \p synchronize method can be called. RCU should not be locked.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            i.e. the node has been inserted or updated,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> update( Q const& val, Func func, bool bAllowInsert = true )
        {
            scoped_node_ptr sp( cxx_leaf_node_allocator().New( val ));
            std::pair<bool, bool> bRes = base_class::update( *sp,
                [&func, &val](bool bNew, leaf_node& node, leaf_node&){ func( bNew, node.m_Value, val ); },
                bAllowInsert );
            if ( bRes.first && bRes.second )
                sp.release();
            return bRes;
        }
        //@cond
        template <typename Q, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( const Q& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Inserts data of type \p value_type created in-place from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr sp( cxx_leaf_node_allocator().MoveNew( std::forward<Args>(args)... ));
            if ( base_class::insert( *sp.get())) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Delete \p key from the set
        /** \anchor cds_nonintrusive_EllenBinTreeSet_rcu_erase_val

            The item comparator should be able to compare the type \p value_type
            and the type \p Q.

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return base_class::erase( key );
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeSet_rcu_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >());
        }

        /// Delete \p key from the set
        /** \anchor cds_nonintrusive_EllenBinTreeSet_rcu_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type const& val);
            };
            \endcode

            Since the key of MichaelHashSet's \p value_type is not explicitly specified,
            template parameter \p Q defines the key type searching in the list.
            The list item comparator should be able to compare the type \p T of list item
            and the type \p Q.

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise

            See also: \ref erase
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return base_class::erase( key, [&f]( leaf_node const& node) { f( node.m_Value ); } );
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeSet_rcu_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                [&f]( leaf_node const& node) { f( node.m_Value ); } );
        }

        /// Extracts an item with minimal key from the set
        /**
            Returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the leftmost item.
            If the set is empty, returns empty \p exempt_ptr.

            @note Due the concurrent nature of the set, the function extracts <i>nearly</i> minimum key.
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

        /// Extracts an item with maximal key from the set
        /**
            Returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the rightmost item.
            If the set is empty, returns empty \p exempt_ptr.

            @note Due the concurrent nature of the set, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key great than leftmost item's key.
            So, the function returns the item with maximum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not free the item.
            The deallocator will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        exempt_ptr extract_max()
        {
            return exempt_ptr( base_class::extract_max_());
        }

        /// Extracts an item from the set
        /** \anchor cds_nonintrusive_EllenBinTreeSet_rcu_extract
            The function searches an item with key equal to \p key in the tree,
            unlinks it, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to an item found.
            If \p key is not found the function returns an empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not destroy the item found.
            The dealloctor will be implicitly invoked when the returned object is destroyed or when
            its release() member function is called.
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            return exempt_ptr( base_class::extract_( key, typename base_class::node_compare()));
        }

        /// Extracts an item from the set using \p pred for searching
        /**
            The function is an analog of \p extract(Q const&) but \p pred is used for key compare.
            \p Less has the interface like \p std::less and should meet \ref cds_container_EllenBinTreeSet_rcu_less
            "predicate requirements".
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return exempt_ptr( base_class::extract_with_( key,
                cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >()));
        }

        /// Find the key \p key
        /**
            @anchor cds_nonintrusive_EllenBinTreeSet_rcu_find_func

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p key argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that may be not the same as \p value_type.

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f ) const
        {
            return base_class::find( key, [&f]( leaf_node& node, Q& v ) { f( node.m_Value, v ); });
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return base_class::find( key, [&f]( leaf_node& node, Q const& v ) { f( node.m_Value, v ); } );
        }
        //@endcond

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeSet_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                [&f]( leaf_node& node, Q& v ) { f( node.m_Value, v ); } );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                                          [&f]( leaf_node& node, Q const& v ) { f( node.m_Value, v ); } );
        }
        //@endcond

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename Q>
        bool contains( Q const& key ) const
        {
            return base_class::contains( key );
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
            \p Less functor has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_rcu_less
            "Predicate requirements".
            \p Less must imply the same element order as the comparator used for building the set.
            \p pred should accept arguments of type \p Q, \p key_type, \p value_type in any combination.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >());
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred ) const
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_EllenBinTreeSet_rcu_get
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
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeSet_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type
            and \p Q in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        value_type * get_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            leaf_node * pNode = base_class::get_with( key,
                cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >());
            return pNode ? &pNode->m_Value : nullptr;
        }

        /// Clears the set (non-atomic)
        /**
            The function unlink all items from the tree.
            The function is not atomic, thus, in multi-threaded environment with parallel insertions
            this sequence
            \code
            set.clear();
            assert( set.empty());
            \endcode
            the assertion could be raised.

            For each leaf the \ref disposer will be called after unlinking.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the set is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the set
        /**
            Only leaf nodes containing user data are counted.

            The value returned depends on item counter type provided by \p Traits template parameter.
            If it is \p atomicity::empty_item_counter \p %size() always returns 0.
            Therefore, the function is not suitable for checking the tree emptiness, use \p empty()
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
}}  // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_ELLEN_BINTREE_SET_RCU_H

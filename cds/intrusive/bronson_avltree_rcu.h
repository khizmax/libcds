//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_BRONSON_AVLTREE_RCU_H
#define __CDS_INTRUSIVE_BRONSON_AVLTREE_RCU_H

#include <cds/intrusive/details/bronson_avltree_base.h>
#include <cds/opt/compare.h>
#include <cds/urcu/details/check_deadlock.h>
#include <cds/urcu/exempt_ptr.h>

namespace cds { namespace intrusive {

    /// Bronson et al AVL-tree (RCU specialization)
    /** @ingroup cds_intrusive_map
        @ingroup cds_intrusive_tree
        @anchor cds_intrusive_BronsonAVLTree_rcu

        Source:
            - [2010] N.Bronson, J.Casper, H.Chafi, K.Olukotun "A Practical Concurrent Binary Search Tree"

        bla-bla-bla

        <b>Template arguments</b>:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p T - type to be stored in tree's nodes. The type must be based on \p bronson_avltree::node
            (for \p bronson_avltree::base_hook) or it must have a member of type \p bronson_avltree::node
            (for \p bronson_avltree::member_hook).
        - \p Traits - tree traits, default is \p bronson_avltree::traits
            It is possible to declare option-based tree with \p bronson_avltree::make_traits metafunction
            instead of \p Traits template argument.

        @note Before including <tt><cds/intrusive/bronson_avltree_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.
    */
    template < 
        class RCU,
        typename T,
#   ifdef CDS_DOXYGEN_INVOKED
        class Traits = bronson_avltree::traits
#   else
        class Traits
#   endif
    >
    class BronsonAVLTree < cds::urcu::gc<RCU>, T, Traits >
    {
    public:
        typedef cds::urcu::gc<RCU>  gc;   ///< RCU Garbage collector
        typedef T       value_type;       ///< type of value stored in the tree
        typedef Traits  traits;           ///< Traits template parameter

        typedef typename traits::hook    hook;      ///< hook type
        typedef typename hook::node_type node_type; ///< node type

        typedef typename traits::disposer       disposer;       ///< node disposer
        typedef typename traits::value_cleaner  value_cleaner;  ///< the functor to clean node's field except key field, see bronson_avltree::traits::value_cleaner
        typedef typename traits::back_off       back_off;       ///< back-off strategy
        typedef typename traits::item_counter   item_counter;   ///< Item counting policy used
        typedef typename traits::memory_model   memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename traits::stat           stat;           ///< internal statistics type
        typedef typename traits::rcu_check_deadlock rcu_check_deadlock; ///< Deadlock checking policy

    public:
        using exempt_ptr = cds::urcu::exempt_ptr< gc, value_type, value_type, disposer, void >; ///< pointer to extracted node
        typedef typename gc::scoped_lock    rcu_lock;   ///< RCU scoped lock
#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator;    ///< key compare functor based on \p Traits::compare and \p Traits::less
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
#endif

    protected:
        //@cond
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits; ///< Node traits
        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock >   check_deadlock_policy;
        //@endcond

    protected:
        //@cond
        bronson_avltree::base_node< node_type > m_Root; ///< Tree root
        node_type * m_pRoot;    ///< pointer to root node

        item_counter        m_ItemCounter;  ///< item counter
        mutable stat        m_Stat;         ///< internal statistics
        //@endcond

    public:
        /// Default ctor creates empty tree
        BronsonAVLTree()
            : m_pRoot(static_cast<node_type *>( &m_Root ))
        {}

        /// Cleans the tree
        ~BronsonAVLTree()
        {
            unsafe_clean();
        }

        /// Inserts new node
        /**
            The function inserts \p val in the tree if it does not contain
            an item with key equal to \p val.

            The function applies RCU lock internally.

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
            where \p val is the item inserted. User-defined functor \p f is called under node-level spin-lock.
            The user-defined functor is called only if the inserting is success.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            //TODO
        }

        /// Ensures that the \p val exists in the tree
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the tree, then \p val is inserted into the tree.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the tree
            - \p val - argument \p val passed into the \p ensure function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor can change non-key fields of the \p item.
            The functor is called under node-level spin-lock.

            RCU \p synchronize method can be called. RCU should not be locked.

            Returns <tt>std::pair<bool, bool> </tt> where \p first is \p true if operation is successfull,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the tree.
        */
        template <typename Func>
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            //TODO
        }

        /// Unlinks the item \p val from the tree
        /**
            The function searches the item \p val in the tree and unlink it from the tree
            if it is found and is equal to \p val.

            Difference between \p erase() and \p %unlink() functions: \p %erase() finds <i>a key</i>
            and deletes the item found. \p %unlink() finds an item by key and deletes it
            only if \p val is an item of the tree, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            RCU \p synchronize method can be called. RCU should not be locked.

            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            //TODO
        }

        /// Deletes the item from the tree
        /** \anchor cds_intrusive_BronsonAVLTree_rcu_erase
            The function searches an item with key equal to \p key in the tree,
            unlinks it from the tree, and returns \p true.
            If the item with key equal to \p key is not found the function return \p false.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Q>
        bool erase( const Q& key )
        {
            //TODO
        }

        /// Delete the item from the tree with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_BronsonAVLTree_rcu_erase "erase(Q const&)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less>
        bool erase_with( const Q& key, Less pred )
        {
            CDS_UNUSED( pred );
            //TODO
        }

        /// Deletes the item from the tree
        /** \anchor cds_intrusive_BronsonAVLTree_rcu_erase_func
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

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            //TODO
        }

        /// Delete the item from the tree with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_BronsonAVLTree_rcu_erase_func "erase(Q const&, Func)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            //TODO
        }

        /// Extracts an item with minimal key from the tree
        /**
            The function searches an item with minimal key, unlinks it, and returns
            \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the leftmost item.
            If the tree is empty the function returns empty \p exempt_ptr.

            @note Due the concurrent nature of the tree, the function extracts <i>nearly</i> minimum key.
            It means that the function gets leftmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key less than leftmost item's key.
            So, the function returns the item with minimum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        exempt_ptr extract_min()
        {
            return exempt_ptr( extract_min_() );
        }

        /// Extracts an item with maximal key from the tree
        /**
            The function searches an item with maximal key, unlinks it, and returns
            \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the rightmost item.
            If the tree is empty the function returns empty \p exempt_ptr.

            @note Due the concurrent nature of the tree, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key great than rightmost item's key.
            So, the function returns the item with maximum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        exempt_ptr extract_max()
        {
            return exempt_ptr( extract_max_() );
        }

        /// Extracts an item from the tree
        /** \anchor cds_intrusive_BronsonAVLTree_rcu_extract
            The function searches an item with key equal to \p key in the tree,
            unlinks it, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to an item found.
            If the item with the key equal to \p key is not found the function returns empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            return exempt_ptr( extract_( key, key_comparator() ) );
        }

        /// Extracts an item from the set using \p pred for searching
        /**
            The function is an analog of \ref cds_intrusive_BronsonAVLTree_rcu_extract "extract(exempt_ptr&, Q const&)"
            but \p pred is used for key compare.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            return exempt_ptr( extract_with_( key, pred ));
        }

        /// Finds the key \p key
        /** @anchor cds_intrusive_BronsonAVLTree_rcu_find_val
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function applies RCU lock internally.
        */
        template <typename Q>
        bool find( Q const& key ) const
        {
            //TODO
        }

        /// Finds the key \p key with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_BronsonAVLTree_rcu_find_val "find(Q const&)"
            but \p pred is used for key compare.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the tree.
            \p pred should accept arguments of type \p Q, \p key_type, \p value_type in any combination.
        */
        template <typename Q, typename Less>
        bool find_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            //TODO
        }

        /// Finds the key \p key
        /** @anchor cds_intrusive_BronsonAVLTree_rcu_find_func
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

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f ) const
        {
            //TODO
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            //TODO
        }
        //@endcond

        /// Finds the key \p key with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_BronsonAVLTree_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparison.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            //TODO
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            //TODO
        }
        //@endcond

        /// Finds \p key and return the item found
        /** \anchor cds_intrusive_BronsonAVLTree_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            RCU should be locked before call the function.
            Returned pointer is valid while RCU is locked.
        */
        template <typename Q>
        value_type * get( Q const& key ) const
        {
            return get_( key, node_compare() );
        }

        /// Finds \p key with \p pred predicate and return the item found
        /**
            The function is an analog of \ref cds_intrusive_BronsonAVLTree_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the tree.
        */
        template <typename Q, typename Less>
        value_type * get_with( Q const& key, Less pred ) const
        {
            //TODO
        }

        /// Checks if the tree is empty
        bool empty() const
        {
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

        /// Returns item count in the tree
        /**
            The value returned depends on item counter type provided by \p Traits template parameter.
            If it is \p atomicity::empty_item_counter this function always returns 0.

            The function is not suitable for checking the tree emptiness, use \p empty()
            member function for that.
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
        //@endcond
    };

}} // nsmespace cds::intrusive

#endif // #ifndef __CDS_INTRUSIVE_BRONSON_AVLTREE_RCU_H

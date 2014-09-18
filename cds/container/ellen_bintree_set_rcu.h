//$$CDS-header$$

#ifndef __CDS_CONTAINER_ELLEN_BINTREE_SET_RCU_H
#define __CDS_CONTAINER_ELLEN_BINTREE_SET_RCU_H

#include <cds/container/ellen_bintree_base.h>
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
        for uniformly distributed random keys, but in worst case the complexity is <tt>O(N)</tt>.

        @note In the current implementation we do not use helping technique described in original paper.
        So, the current implementation is near to fine-grained lock-based tree.
        Helping will be implemented in future release

        <b>Template arguments</b> :
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - key type, a subset of \p T
        - \p T - type to be stored in tree's leaf nodes.
        - \p Traits - type traits. See ellen_bintree::type_traits for explanation.

        It is possible to declare option-based tree with ellen_bintree::make_set_traits metafunction
        instead of \p Traits template argument.
        Template argument list \p Options of ellen_bintree::make_set_traits metafunction are:
        - ellen_bintree::key_extractor - key extracting functor, mandatory option. The functor has the following prototype:
            \code
                struct key_extractor {
                    void operator ()( Key& dest, T const& src );
                };
            \endcode
            It should initialize \p dest key from \p src data. The functor is used to initialize internal nodes.
        - opt::compare - key compare functor. No default functor is provided.
            If the option is not specified, \p %opt::less is used.
        - opt::less - specifies binary predicate used for key compare. At least \p %opt::compare or \p %opt::less should be defined.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter that is no item counting.
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).
        - opt::allocator - the allocator used for \ref ellen_bintree::node "leaf nodes" which contains data.
            Default is \ref CDS_DEFAULT_ALLOCATOR.
        - opt::node_allocator - the allocator used for internal nodes. Default is \ref CDS_DEFAULT_ALLOCATOR.
        - ellen_bintree::update_desc_allocator - an allocator of \ref ellen_bintree::update_desc "update descriptors",
            default is \ref CDS_DEFAULT_ALLOCATOR.
            Note that update descriptor is helping data structure with short lifetime and it is good candidate for pooling.
            The number of simultaneously existing descriptors is a relatively small number limited the number of threads
            working with the tree and RCU buffer size.
            Therefore, a bounded lock-free container like \p cds::container::VyukovMPMCCycleQueue is good choice for the free-list
            of update descriptors, see cds::memory::vyukov_queue_pool free-list implementation.
            Also notice that size of update descriptor is not dependent on the type of data
            stored in the tree so single free-list object can be used for several EllenBinTree-based object.
        - opt::stat - internal statistics. Available types: ellen_bintree::stat, ellen_bintree::empty_stat (the default)
        - opt::rcu_check_deadlock - a deadlock checking policy. Default is opt::v::rcu_throw_deadlock

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
        class Traits = ellen_bintree::type_traits
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
        typedef cds::urcu::gc<RCU>  gc  ;   ///< RCU Garbage collector
        typedef Key     key_type        ;   ///< type of a key stored in internal nodes; key is a part of \p value_type
        typedef T       value_type      ;   ///< type of value stored in the binary tree
        typedef Traits  options         ;   ///< Traits template parameter

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key compare functor based on opt::compare and opt::less option setter.
#   else
        typedef typename maker::intrusive_type_traits::compare   key_comparator;
#   endif
        typedef typename base_class::item_counter           item_counter        ; ///< Item counting policy used
        typedef typename base_class::memory_model           memory_model        ; ///< Memory ordering. See cds::opt::memory_model option
        typedef typename base_class::stat                   stat                ; ///< internal statistics type
        typedef typename base_class::rcu_check_deadlock     rcu_check_deadlock  ; ///< Deadlock checking policy
        typedef typename options::key_extractor             key_extractor       ; ///< key extracting functor

        typedef typename options::allocator                 allocator_type      ;   ///< Allocator for leaf nodes
        typedef typename base_class::node_allocator         node_allocator      ;   ///< Internal node allocator
        typedef typename base_class::update_desc_allocator  update_desc_allocator ; ///< Update descriptor allocator

        static CDS_CONSTEXPR_CONST bool c_bExtractLockExternal = base_class::c_bExtractLockExternal; ///< Group of \p extract_xxx functions do not require external locking

    protected:
        //@cond
        typedef typename maker::cxx_leaf_node_allocator cxx_leaf_node_allocator;
        typedef typename base_class::value_type         leaf_node;
        typedef typename base_class::internal_node      internal_node;
        typedef std::unique_ptr< leaf_node, typename maker::intrusive_type_traits::disposer >    scoped_node_ptr;
        //@endcond

    public:
        typedef typename gc::scoped_lock    rcu_lock ;  ///< RCU scoped lock

        /// pointer to extracted node
        typedef cds::urcu::exempt_ptr< gc, leaf_node, value_type, typename maker::intrusive_type_traits::disposer,
            cds::urcu::details::conventional_exempt_member_cast<leaf_node, value_type>
        > exempt_ptr;

    protected:
        //@cond
#   ifndef CDS_CXX11_LAMBDA_SUPPORT
        template <typename Func>
        struct insert_functor
        {
            Func        m_func;

            insert_functor ( Func f )
                : m_func(f)
            {}

            void operator()( leaf_node& node )
            {
                cds::unref(m_func)( node.m_Value );
            }
        };

        template <typename Q, typename Func>
        struct ensure_functor
        {
            Func        m_func;
            Q const&    m_arg;

            ensure_functor( Q const& arg, Func f )
                : m_func(f)
                , m_arg( arg )
            {}

            void operator ()( bool bNew, leaf_node& node, leaf_node& )
            {
                cds::unref(m_func)( bNew, node.m_Value, m_arg );
            }
        };

        template <typename Func>
        struct erase_functor
        {
            Func        m_func;

            erase_functor( Func f )
                : m_func(f)
            {}

            void operator()( leaf_node const& node )
            {
                cds::unref(m_func)( node.m_Value );
            }
        };

        template <typename Func>
        struct find_functor
        {
            Func    m_func;

            find_functor( Func f )
                : m_func(f)
            {}

            template <typename Q>
            void operator ()( leaf_node& node, Q& val )
            {
                cds::unref(m_func)( node.m_Value, val );
            }
        };
#endif
        //@endcond

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

            RCU \p synchronize method can be called. RCU should not be locked.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q const& val )
        {
            scoped_node_ptr sp( cxx_leaf_node_allocator().New( val ));
            if ( base_class::insert( *sp.get() )) {
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
            The user-defined functor is called only if the inserting is success. It may be passed by reference
            using <tt>boost::ref</tt>

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Q, typename Func>
        bool insert( Q const& val, Func f )
        {
            scoped_node_ptr sp( cxx_leaf_node_allocator().New( val ));
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            if ( base_class::insert( *sp.get(), [&f]( leaf_node& val ) { cds::unref(f)( val.m_Value ); } ))
#       else
            insert_functor<Func> wrapper(f);
            if ( base_class::insert( *sp, cds::ref(wrapper) ))
#       endif
            {
                sp.release();
                return true;
            }
            return false;
        }

        /// Ensures that the item exists in the set
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p val key not found in the set, then the new item created from \p val
            is inserted into the set. Otherwise, the functor \p func is called with the item found.
            The functor \p Func should be a function with signature:
            \code
                void func( bool bNew, value_type& item, const Q& val );
            \endcode
            or a functor:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item, const Q& val );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p key passed into the \p ensure function

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            You may pass \p func argument by reference using <tt>boost::ref</tt>.

            RCU \p synchronize method can be called. RCU should not be locked.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successfull,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the set.
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> ensure( const Q& val, Func func )
        {
            scoped_node_ptr sp( cxx_leaf_node_allocator().New( val ));
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            std::pair<bool, bool> bRes = base_class::ensure( *sp,
                [&func, &val](bool bNew, leaf_node& node, leaf_node&){ cds::unref(func)( bNew, node.m_Value, val ); });
#       else
            ensure_functor<Q, Func> wrapper( val, func );
            std::pair<bool, bool> bRes = base_class::ensure( *sp, cds::ref(wrapper));
#       endif
            if ( bRes.first && bRes.second )
                sp.release();
            return bRes;
        }

#   ifdef CDS_EMPLACE_SUPPORT
        /// Inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.

            RCU \p synchronize method can be called. RCU should not be locked.

            @note This function is available only for compiler that supports
            variadic template and move semantics
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr sp( cxx_leaf_node_allocator().New( std::forward<Args>(args)... ));
            if ( base_class::insert( *sp.get() )) {
                sp.release();
                return true;
            }
            return false;
        }
#   endif

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
            The functor may be passed by reference using <tt>boost:ref</tt>

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
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::erase( key, [&f]( leaf_node const& node) { cds::unref(f)( node.m_Value ); } );
#       else
            erase_functor<Func> wrapper(f);
            return base_class::erase( key, cds::ref(wrapper));
#       endif
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
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::erase_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                [&f]( leaf_node const& node) { cds::unref(f)( node.m_Value ); } );
#       else
            erase_functor<Func> wrapper(f);
            return base_class::erase_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(), cds::ref(wrapper));
#       endif
        }

        /// Extracts an item with minimal key from the set
        /**
            If the set is not empty, the function returns \p true, \p result contains a pointer to value.
            If the set is empty, the function returns \p false, \p result is left unchanged.

            @note Due the concurrent nature of the set, the function extracts <i>nearly</i> minimum key.
            It means that the function gets leftmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key less than leftmost item's key.
            So, the function returns the item with minimum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not free the item.
            The deallocator will be implicitly invoked when \p result object is destroyed or when
            <tt>result.release()</tt> is called, see cds::urcu::exempt_ptr for explanation.
            @note Before reusing \p result object you should call its \p release() method.
        */
        bool extract_min( exempt_ptr& result )
        {
            return base_class::extract_min_( result );
        }

        /// Extracts an item with maximal key from the set
        /**
            If the set is not empty, the function returns \p true, \p result contains a pointer to extracted item.
            If the set is empty, the function returns \p false, \p result is left unchanged.

            @note Due the concurrent nature of the set, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key great than leftmost item's key.
            So, the function returns the item with maximum key at the moment of tree traversing.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not free the item.
            The deallocator will be implicitly invoked when \p result object is destroyed or when
            <tt>result.release()</tt> is called, see cds::urcu::exempt_ptr for explanation.
            @note Before reusing \p result object you should call its \p release() method.
        */
        bool extract_max( exempt_ptr& result )
        {
            return base_class::extract_max_( result );
        }

        /// Extracts an item from the set
        /** \anchor cds_nonintrusive_EllenBinTreeSet_rcu_extract
            The function searches an item with key equal to \p key in the tree,
            unlinks it, and returns pointer to an item found in \p result parameter.
            If \p key is not found the function returns \p false.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not destroy the item found.
            The dealloctor will be implicitly invoked when \p result object is destroyed or when
            <tt>result.release()</tt> is called, see cds::urcu::exempt_ptr for explanation.
            @note Before reusing \p result object you should call its \p release() method.
        */
        template <typename Q>
        bool extract( exempt_ptr& result, Q const& key )
        {
            return base_class::extract_( result, key, typename base_class::node_compare());
        }

        /// Extracts an item from the set using \p pred for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeSet_rcu_extract "extract(exempt_ptr&, Q const&)"
            but \p pred is used for key compare.
            \p Less has the interface like \p std::less and should meet \ref cds_container_EllenBinTreeSet_rcu_less
            "predicate requirements".
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool extract_with( exempt_ptr& result,  Q const& val, Less pred )
        {
            return base_class::extract_with_( result, val,
                cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >() );
        }

        /// Find the key \p val
        /**
            @anchor cds_nonintrusive_EllenBinTreeSet_rcu_find_func

            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You may pass \p f argument by reference using <tt>boost::ref</tt> or cds::ref.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p val argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that may be not the same as \p value_type.

            The function applies RCU lock internally.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& val, Func f ) const
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find( val, [&f]( leaf_node& node, Q& v ) { cds::unref(f)( node.m_Value, v ); });
#       else
            find_functor<Func> wrapper(f);
            return base_class::find( val, cds::ref(wrapper));
#       endif
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeSet_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& val, Less pred, Func f ) const
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find_with( val, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                [&f]( leaf_node& node, Q& v ) { cds::unref(f)( node.m_Value, v ); } );
#       else
            find_functor<Func> wrapper(f);
            return base_class::find_with( val, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                cds::ref(wrapper));
#       endif
        }

        /// Find the key \p val
        /** @anchor cds_nonintrusive_EllenBinTreeSet_rcu_find_cfunc

            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q const& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You may pass \p f argument by reference using <tt>boost::ref</tt> or cds::ref.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that may be not the same as \p value_type.

            The function applies RCU lock internally.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& val, Func f ) const
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find( val, [&f]( leaf_node& node, Q const& v ) { cds::unref(f)( node.m_Value, v ); });
#       else
            find_functor<Func> wrapper(f);
            return base_class::find( val, cds::ref(wrapper));
#       endif
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeSet_rcu_find_cfunc "find(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& val, Less pred, Func f ) const
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find_with( val, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                [&f]( leaf_node& node, Q const& v ) { cds::unref(f)( node.m_Value, v ); } );
#       else
            find_functor<Func> wrapper(f);
            return base_class::find_with( val, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >(),
                cds::ref(wrapper));
#       endif
        }

        /// Find the key \p val
        /** @anchor cds_nonintrusive_EllenBinTreeSet_rcu_find_val

            The function searches the item with key equal to \p val
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that may be not the same as \ref value_type.

            The function applies RCU lock internally.
        */
        template <typename Q>
        bool find( Q const & val ) const
        {
            return base_class::find( val );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeSet_rcu_find_val "find(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool find_with( Q const& val, Less pred ) const
        {
            return base_class::find_with( val, cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >());
        }

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_EllenBinTreeSet_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p NULL.

            RCU should be locked before call the function.
            Returned pointer is valid while RCU is locked.
        */
        template <typename Q>
        value_type * get( Q const& key ) const
        {
            leaf_node * pNode = base_class::get( key );
            return pNode ? &pNode->m_Value : null_ptr<value_type *>();
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
            leaf_node * pNode = base_class::get_with( key,
                cds::details::predicate_wrapper< leaf_node, Less, typename maker::value_accessor >());
            return pNode ? &pNode->m_Value : null_ptr<value_type *>();
        }

        /// Clears the set (non-atomic)
        /**
            The function unlink all items from the tree.
            The function is not atomic, thus, in multi-threaded environment with parallel insertions
            this sequence
            \code
            set.clear();
            assert( set.empty() );
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
            If it is atomicity::empty_item_counter this function always returns 0.
            Therefore, the function is not suitable for checking the tree emptiness, use \ref empty
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

#endif // #ifndef __CDS_CONTAINER_ELLEN_BINTREE_SET_RCU_H

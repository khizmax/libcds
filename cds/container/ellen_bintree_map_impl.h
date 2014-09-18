//$$CDS-header$$

#ifndef __CDS_CONTAINER_ELLEN_BINTREE_MAP_IMPL_H
#define __CDS_CONTAINER_ELLEN_BINTREE_MAP_IMPL_H

#include <cds/container/ellen_bintree_base.h>
#include <cds/intrusive/ellen_bintree_impl.h>
#include <cds/details/functor_wrapper.h>
#include <cds/details/std/type_traits.h>
#include <cds/container/details/guarded_ptr_cast.h>

namespace cds { namespace container {

    /// Map based on Ellen's et al binary search tree
    /** @ingroup cds_nonintrusive_map
        @ingroup cds_nonintrusive_tree
        @anchor cds_container_EllenBinTreeMap

        Source:
            - [2010] F.Ellen, P.Fatourou, E.Ruppert, F.van Breugel "Non-blocking Binary Search Tree"

        %EllenBinTreeMap is an unbalanced leaf-oriented binary search tree that implements the <i>map</i>
        abstract data type. Nodes maintains child pointers but not parent pointers.
        Every internal node has exactly two children, and all data of type <tt>std::pair<Key const, T></tt>
        currently in the tree are stored in the leaves. Internal nodes of the tree are used to direct \p find
        operation along the path to the correct leaf. The keys (of \p Key type) stored in internal nodes
        may or may not be in the map.
        Unlike \ref cds_container_EllenBinTreeSet "EllenBinTreeSet" keys are not a part of \p T type.
        The map can be represented as a set containing <tt>std::pair< Key const, T> </tt> values.

        Due to \p extract_min and \p extract_max member functions the \p %EllenBinTreeMap can act as
        a <i>priority queue</i>. In this case you should provide unique compound key, for example,
        the priority value plus some uniformly distributed random value.

        @warning Recall the tree is <b>unbalanced</b>. The complexity of operations is <tt>O(log N)</tt>
        for uniformly distributed random keys, but in worst case the complexity is <tt>O(N)</tt>.

        @note In the current implementation we do not use helping technique described in original paper.
        So, the current implementation is near to fine-grained lock-based tree.
        Helping will be implemented in future release

        <b>Template arguments</b> :
        - \p GC - safe memory reclamation (i.e. light-weight garbage collector) type, like cds::gc::HP, cds::gc::PTB
            Note that cds::gc::HRC is not supported.
        - \p Key - key type
        - \p T - value type to be stored in tree's leaf nodes.
        - \p Traits - type traits. See ellen_bintree::type_traits for explanation.

        It is possible to declare option-based tree with ellen_bintree::make_map_traits metafunction
        instead of \p Traits template argument.
        Template argument list \p Options of ellen_bintree::make_map_traits metafunction are:
        - opt::compare - key compare functor. No default functor is provided.
            If the option is not specified, \p %opt::less is used.
        - opt::less - specifies binary predicate used for key compare. At least \p %opt::compare or \p %opt::less should be defined.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter that is no item counting.
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).
        - opt::allocator - the allocator used for \ref ellen_bintree::map_node "leaf nodes" which contains data.
            Default is \ref CDS_DEFAULT_ALLOCATOR.
        - opt::node_allocator - the allocator used for \ref ellen_bintree::internal_node "internal nodes".
            Default is \ref CDS_DEFAULT_ALLOCATOR.
        - ellen_bintree::update_desc_allocator - an allocator of \ref ellen_bintree::update_desc "update descriptors",
            default is \ref CDS_DEFAULT_ALLOCATOR.
            Note that update descriptor is helping data structure with short lifetime and it is good candidate for pooling.
            The number of simultaneously existing descriptors is a relatively small number limited the number of threads
            working with the tree and GC buffer size.
            Therefore, a bounded lock-free container like \p cds::container::VyukovMPMCCycleQueue is good choice for the free-list
            of update descriptors, see cds::memory::vyukov_queue_pool free-list implementation.
            Also notice that size of update descriptor is not dependent on the type of data
            stored in the tree so single free-list object can be used for several EllenBinTree-based object.
        - opt::stat - internal statistics. Available types: ellen_bintree::stat, ellen_bintree::empty_stat (the default)
        - opt::copy_policy - key copy policy defines a functor to copy leaf node's key to internal node.
            By default, assignment operator is used.
            The copy functor interface is:
            \code
            struct copy_functor {
                void operator()( Key& dest, Key const& src );
            };
            \endcode

        @note Do not include <tt><cds/container/ellen_bintree_map_impl.h></tt> header file directly.
        There are header file for each GC type:
        - <tt><cds/container/ellen_bintree_map_hp.h></tt> - for Hazard Pointer GC cds::gc::HP
        - <tt><cds/container/ellen_bintree_map_ptb.h></tt> - for Pass-the-Buck GC cds::gc::PTB
        - <tt><cds/container/ellen_bintree_map_rcu.h></tt> - for RCU GC
            (see \ref cds_container_EllenBinTreeMap_rcu "RCU-based EllenBinTreeMap")
    */
    template <
        class GC,
        typename Key,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = ellen_bintree::type_traits
#else
        class Traits
#endif
    >
    class EllenBinTreeMap
#ifdef CDS_DOXYGEN_INVOKED
        : public cds::intrusive::EllenBinTree< GC, Key, T, Traits >
#else
        : public ellen_bintree::details::make_ellen_bintree_map< GC, Key, T, Traits >::type
#endif
    {
        //@cond
        typedef ellen_bintree::details::make_ellen_bintree_map< GC, Key, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond
    public:
        typedef GC      gc              ;   ///< Garbage collector
        typedef Key     key_type        ;   ///< type of a key stored in the map
        typedef T       mapped_type      ;  ///< type of value stored in the map
        typedef std::pair< key_type const, mapped_type >    value_type  ;   ///< Key-value pair stored in leaf node of the mp
        typedef Traits  options         ;   ///< Traits template parameter

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key compare functor based on opt::compare and opt::less option setter.
#   else
        typedef typename maker::intrusive_type_traits::compare   key_comparator;
#   endif
        typedef typename base_class::item_counter           item_counter        ; ///< Item counting policy used
        typedef typename base_class::memory_model           memory_model        ; ///< Memory ordering. See cds::opt::memory_model option
        typedef typename base_class::node_allocator         node_allocator_type ; ///< allocator for maintaining internal node
        typedef typename base_class::stat                   stat                ; ///< internal statistics type
        typedef typename options::copy_policy               copy_policy         ; ///< key copy policy

        typedef typename options::allocator                 allocator_type      ;   ///< Allocator for leaf nodes
        typedef typename base_class::node_allocator         node_allocator      ;   ///< Internal node allocator
        typedef typename base_class::update_desc_allocator  update_desc_allocator ; ///< Update descriptor allocator

    protected:
        //@cond
        typedef typename base_class::value_type         leaf_node;
        typedef typename base_class::internal_node      internal_node;
        typedef typename base_class::update_desc        update_desc;

        typedef typename maker::cxx_leaf_node_allocator cxx_leaf_node_allocator;

        typedef std::unique_ptr< leaf_node, typename maker::leaf_deallocator >    scoped_node_ptr;
        //@endcond

    public:
        /// Guarded pointer
        typedef cds::gc::guarded_ptr< gc, leaf_node, value_type, details::guarded_ptr_cast_set<leaf_node, value_type> > guarded_ptr;

    protected:
        //@cond
#   ifndef CDS_CXX11_LAMBDA_SUPPORT
        struct empty_insert_functor
        {
            void operator()( value_type& ) const
            {}
        };

        template <typename Q>
        class insert_value_functor
        {
            Q const&    m_val;
        public:
            insert_value_functor( Q const& v)
                : m_val(v)
            {}

            void operator()( value_type& item )
            {
                item.second = m_val;
            }
        };

        template <typename Func>
        class insert_key_wrapper: protected cds::details::functor_wrapper<Func>
        {
            typedef cds::details::functor_wrapper<Func> base_class;
        public:
            insert_key_wrapper( Func f ): base_class(f) {}

            void operator()( leaf_node& item )
            {
                base_class::get()( item.m_Value );
            }
        };

        template <typename Func>
        class ensure_wrapper: protected cds::details::functor_wrapper<Func>
        {
            typedef cds::details::functor_wrapper<Func> base_class;
        public:
            ensure_wrapper( Func f) : base_class(f) {}

            void operator()( bool bNew, leaf_node& item, leaf_node const& )
            {
                base_class::get()( bNew, item.m_Value );
            }
        };

        template <typename Func>
        struct erase_functor
        {
            Func        m_func;

            erase_functor( Func f )
                : m_func(f)
            {}

            void operator()( leaf_node& node )
            {
                cds::unref(m_func)( node.m_Value );
            }
        };

        template <typename Func>
        class find_wrapper: protected cds::details::functor_wrapper<Func>
        {
            typedef cds::details::functor_wrapper<Func> base_class;
        public:
            find_wrapper( Func f )
                : base_class(f)
            {}

            template <typename Q>
            void operator()( leaf_node& item, Q& val )
            {
                base_class::get()( item.m_Value, val );
            }
        };
#   endif
        //@endcond

    public:
        /// Default constructor
        EllenBinTreeMap()
            : base_class()
        {
            //static_assert( (std::is_same<gc, cds::gc::HP>::value || std::is_same<gc, cds::gc::PTB>::value), "GC must be cds::gc::HP or cds:gc::PTB" );
        }

        /// Clears the map
        ~EllenBinTreeMap()
        {}

        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \ref key_type should be constructible from a value of type \p K.
                In trivial case, \p K is equal to \ref key_type.
            - The \ref mapped_type should be default-constructible.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K const& key )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return insert_key( key, [](value_type&){} );
#       else
            return insert_key( key, empty_insert_functor() );
#       endif
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \ref key_type should be constructible from \p key of type \p K.
            - The \ref value_type should be constructible from \p val of type \p V.

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

            The user-defined functor can be passed by reference using <tt>boost::ref</tt>
            and it is called only if inserting is successful.

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.
        */
        template <typename K, typename Func>
        bool insert_key( const K& key, Func func )
        {
            scoped_node_ptr pNode( cxx_leaf_node_allocator().New( key ));
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            if ( base_class::insert( *pNode, [&func]( leaf_node& item ) { cds::unref(func)( item.m_Value ); } ))
#       else
            insert_key_wrapper<Func> wrapper(func);
            if ( base_class::insert( *pNode, cds::ref(wrapper) ))
#endif
            {
                pNode.release();
                return true;
            }
            return false;
        }

#   ifdef CDS_EMPLACE_SUPPORT
        /// For key \p key inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.

            @note This function is available only for compiler that supports
            variadic template and move semantics
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            scoped_node_ptr pNode( cxx_leaf_node_allocator().New( std::forward<K>(key), std::forward<Args>(args)... ));
            if ( base_class::insert( *pNode )) {
                pNode.release();
                return true;
            }
            return false;
        }
#   endif

        /// Ensures that the \p key exists in the map
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p key not found in the map, then the new item created from \p key
            is inserted into the map (note that in this case the \ref key_type should be
            constructible from type \p K).
            Otherwise, the functor \p func is called with item found.
            The functor \p Func may be a function with signature:
            \code
                void func( bool bNew, value_type& item );
            \endcode
            or a functor:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the list

            The functor may change any fields of the \p item.second that is \ref value_type.

            You may pass \p func argument by reference using <tt>boost::ref</tt>.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successfull,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the list.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> ensure( K const& key, Func func )
        {
            scoped_node_ptr pNode( cxx_leaf_node_allocator().New( key ));
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            std::pair<bool, bool> res = base_class::ensure( *pNode,
                [&func](bool bNew, leaf_node& item, leaf_node const& ){ cds::unref(func)( bNew, item.m_Value ); }
            );
#       else
            ensure_wrapper<Func> wrapper( func );
            std::pair<bool, bool> res = base_class::ensure( *pNode, cds::ref(wrapper) );
#       endif
            if ( res.first && res.second )
                pNode.release();
            return res;
        }

        /// Delete \p key from the map
        /**\anchor cds_nonintrusive_EllenBinTreeMap_erase_val

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return base_class::erase(key);
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_erase_val "erase(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool erase_with( K const& key, Less pred )
        {
            return base_class::erase_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >());
        }

        /// Delete \p key from the map
        /** \anchor cds_nonintrusive_EllenBinTreeMap_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode
            The functor may be passed by reference using <tt>boost:ref</tt>

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::erase( key, [&f]( leaf_node& node) { cds::unref(f)( node.m_Value ); } );
#       else
            erase_functor<Func> wrapper(f);
            return base_class::erase( key, cds::ref(wrapper));
#       endif
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::erase_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >(),
                [&f]( leaf_node& node) { cds::unref(f)( node.m_Value ); } );
#       else
            erase_functor<Func> wrapper(f);
            return base_class::erase_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >(), cds::ref(wrapper));
#       endif
        }

        /// Extracts an item with minimal key from the map
        /**
            If the map is not empty, the function returns \p true, \p result contains a pointer to minimum value.
            If the map is empty, the function returns \p false, \p result is left unchanged.

            @note Due the concurrent nature of the map, the function extracts <i>nearly</i> minimum key.
            It means that the function gets leftmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key less than leftmost item's key.
            So, the function returns the item with minimum key at the moment of tree traversing.

            The guarded pointer \p result prevents deallocation of returned item,
            see cds::gc::guarded_ptr for explanation.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.
        */
        bool extract_min( guarded_ptr& result )
        {
            return base_class::extract_min_( result.guard() );
        }

        /// Extracts an item with maximal key from the map
        /**
            If the map is not empty, the function returns \p true, \p result contains a pointer to maximal value.
            If the map is empty, the function returns \p false, \p result is left unchanged.

            @note Due the concurrent nature of the map, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost leaf of the tree and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key great than leftmost item's key.
            So, the function returns the item with maximum key at the moment of tree traversing.

            The guarded pointer \p result prevents deallocation of returned item,
            see cds::gc::guarded_ptr for explanation.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.
        */
        bool extract_max( guarded_ptr& result )
        {
            return base_class::extract_max_( result.guard() );
        }

        /// Extracts an item from the tree
        /** \anchor cds_nonintrusive_EllenBinTreeMap_extract
            The function searches an item with key equal to \p key in the tree,
            unlinks it, and returns pointer to an item found in \p result parameter.
            If the item  is not found the function returns \p false.

            The guarded pointer \p result prevents deallocation of returned item,
            see cds::gc::guarded_ptr for explanation.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.
        */
        template <typename Q>
        bool extract( guarded_ptr& result, Q const& key )
        {
            return base_class::extract_( result.guard(), key );
        }

        /// Extracts an item from the map using \p pred for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_extract "extract(guarded_ptr&, Q const&)"
            but \p pred is used for key compare.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename Q, typename Less>
        bool extract_with( guarded_ptr& result, Q const& key, Less pred )
        {
            return base_class::extract_with_( result.guard(), key,
                cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >());
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_EllenBinTreeMap_find_cfunc

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            You can pass \p f argument by reference using <tt>boost::ref</tt> or cds::ref.

            The functor may change \p item.second.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find( key, [&f](leaf_node& item, K const& ) { cds::unref(f)( item.m_Value );});
#       else
            find_wrapper<Func> wrapper(f);
            return base_class::find( key, cds::ref(wrapper) );
#       endif
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_find_cfunc "find(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool find_with( K const& key, Less pred, Func f )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >(),
                [&f](leaf_node& item, K const& ) { cds::unref(f)( item.m_Value );});
#       else
            find_wrapper<Func> wrapper(f);
            return base_class::find_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >(), cds::ref(wrapper) );
#       endif
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_EllenBinTreeMap_find_val

            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename K>
        bool find( K const& key )
        {
            return base_class::find( key );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_find_val "find(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool find_with( K const& key, Less pred )
        {
            return base_class::find_with( key, cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >() );
        }

        /// Finds \p key and returns the item found
        /** @anchor cds_nonintrusive_EllenBinTreeMap_get
            The function searches the item with key equal to \p key and returns the item found in \p result parameter.
            The function returns \p true if \p key is found, \p false otherwise.

            The guarded pointer \p result prevents deallocation of returned item,
            see cds::gc::guarded_ptr for explanation.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.
        */
        template <typename Q>
        bool get( guarded_ptr& result, Q const& key )
        {
            return base_class::get_( result.guard(), key );
        }

        /// Finds \p key with predicate \p pred and returns the item found
        /**
            The function is an analog of \ref cds_nonintrusive_EllenBinTreeMap_get "get(guarded_ptr&, Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename Q, typename Less>
        bool get_with( guarded_ptr& result, Q const& key, Less pred )
        {
            return base_class::get_with_( result.guard(), key,
                cds::details::predicate_wrapper< leaf_node, Less, typename maker::key_accessor >() );
        }

        /// Clears the map
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the map is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the map is empty.
        */
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the map
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

#endif //#ifndef __CDS_CONTAINER_ELLEN_BINTREE_MAP_IMPL_H

//$$CDS-header$$

#ifndef __CDS_CONTAINER_ELLEN_BINTREE_BASE_H
#define __CDS_CONTAINER_ELLEN_BINTREE_BASE_H

#include <cds/intrusive/details/ellen_bintree_base.h>
#include <cds/container/base.h>
#include <cds/opt/compare.h>
#include <cds/details/binary_functor_wrapper.h>


namespace cds { namespace container {
    /// EllenBinTree related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace ellen_bintree {

#ifdef CDS_DOXYGEN_INVOKED
        /// Typedef for cds::intrusive::ellen_bintree::update_desc
        typedef cds::intrusive::ellen_bintree::update_desc update_desc;

        /// Typedef for cds::intrusive::ellen_bintree::internal_node
        typedef cds::intrusive::ellen_bintree::internal_node internal_node;

        /// Typedef for cds::intrusive::ellen_bintree::key_extractor
        typedef cds::intrusive::ellen_bintree::key_extractor key_extractor;

        /// Typedef for cds::intrusive::ellen_bintree::update_desc_allocator
        typedef cds::intrusive::ellen_bintree::update_desc_allocator update_desc_allocator;

        /// Typedef for cds::intrusive::ellen_bintree::stat
        typedef cds::intrusive::ellen_bintree::stat stat;

        /// Typedef for cds::intrusive::ellen_bintree::empty_stat
        typedef cds::intrusive::ellen_bintree::empty_stat empty_stat;
#else
        using cds::intrusive::ellen_bintree::update_desc;
        using cds::intrusive::ellen_bintree::internal_node;
        using cds::intrusive::ellen_bintree::key_extractor;
        using cds::intrusive::ellen_bintree::update_desc_allocator;
        using cds::intrusive::ellen_bintree::stat;
        using cds::intrusive::ellen_bintree::empty_stat;
        using cds::intrusive::ellen_bintree::node_types;
#endif

        /// EllenBinTree leaf node
        template <typename GC, typename T>
        struct node: public cds::intrusive::ellen_bintree::node<GC>
        {
            typedef T   value_type  ;   ///< Value type

            T   m_Value ;   ///< Value

            /// Default ctor
            node()
            {}

            /// Initializing ctor
            template <typename Q>
            node(Q const& v)
                : m_Value(v)
            {}

#ifdef CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
            /// Copy constructor
            template <typename... Args>
            node( Args const&... args)
                : m_Value( args... )
            {}

#ifdef CDS_RVALUE_SUPPORT
            /// Move constructor
            template <typename... Args>
            node( Args&&... args)
                : m_Value( std::forward<Args>(args)... )
            {}
#endif  // CDS_RVALUE_SUPPORT
#endif  // CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
        };

        /// EllenBinTreeMap leaf node
        template <typename GC, typename Key, typename T>
        struct map_node: public cds::intrusive::ellen_bintree::node< GC >
        {
            typedef Key     key_type    ;   ///< key type
            typedef T       mapped_type ;   ///< value type
            typedef std::pair<key_type const, mapped_type> value_type  ;   ///< key-value pair stored in the map

            value_type  m_Value     ;   ///< Key-value pair stored in map leaf node

            /// Initializes key field, value if default-constructed
            template <typename K>
            map_node( K const& key )
                : m_Value( std::make_pair( key_type(key), mapped_type() ))
            {}

            /// Initializes key and value fields
            template <typename K, typename Q>
            map_node( K const& key, Q const& v )
                : m_Value( std::make_pair(key_type(key), mapped_type(v) ))
            {}
        };

        /// Type traits for EllenBinTreeSet, EllenBinTreeMap and EllenBinTreePriorityQueue
        struct type_traits
        {
            /// Key extracting functor (only for EllenBinTreeSet)
            /**
                You should explicit define a valid functor.
                The functor has the following prototype:
                \code
                struct key_extractor {
                    void operator ()( Key& dest, T const& src );
                };
                \endcode
                It should initialize \p dest key from \p src data.
                The functor is used to initialize internal nodes.
            */
            typedef opt::none           key_extractor;

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.

                See cds::opt::compare option description for functor interface.

                You should provide \p compare or \p less functor.
                See \ref cds_container_EllenBinTreeSet_rcu_less "predicate requirements".
            */
            typedef opt::none                       compare;

            /// Specifies binary predicate used for key compare.
            /**
                See cds::opt::less option description for predicate interface.

                You should provide \p compare or \p less functor.
                See \ref cds_container_EllenBinTreeSet_rcu_less "predicate requirements".
            */
            typedef opt::none                       less;

            /// Item counter
            /**
                The type for item counting feature (see cds::opt::item_counter).
                Default is no item counter (\ref atomicity::empty_item_counter)
            */
            typedef atomicity::empty_item_counter     item_counter;

            /// C++ memory ordering model
            /**
                List of available memory ordering see opt::memory_model
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// Allocator for update descriptors
            /**
                The allocator type is used for \ref update_desc.

                Update descriptor is helping data structure with short lifetime and it is good candidate
                for pooling. The number of simultaneously existing descriptors is a small number
                limited the number of threads working with the tree.
                Therefore, a bounded lock-free container like \p cds::container::VyukovMPMCCycleQueue
                is good choice for the free-list of update descriptors,
                see cds::memory::vyukov_queue_pool free-list implementation.

                Also notice that size of update descriptor is not dependent on the type of data
                stored in the tree so single free-list object can be used for several \p EllenBinTree object.
            */
            typedef CDS_DEFAULT_ALLOCATOR           update_desc_allocator;

            /// Allocator for internal nodes
            /**
                The allocator type is used for \ref internal_node.
            */
            typedef CDS_DEFAULT_ALLOCATOR           node_allocator;

            /// Allocator for leaf nodes
            /**
                Each leaf node contains data stored in the container.
            */
            typedef CDS_DEFAULT_ALLOCATOR           allocator;

            /// Internal statistics
            /**
                Possible types: ellen_bintree::empty_stat (the default), ellen_bintree::stat or any
                other with interface like \p %stat.
            */
            typedef empty_stat                      stat;

            /// RCU deadlock checking policy (only for RCU-based EllenBinTree<i>XXX</i> classes)
            /**
                List of available options see opt::rcu_check_deadlock
            */
            typedef cds::opt::v::rcu_throw_deadlock      rcu_check_deadlock;

            /// Key copy policy (for EllenBinTreeMap)
            /**
                The key copy policy defines a functor to copy leaf node's key to internal node.
                This policy is used only in EllenBinTreeMap. By default, assignment operator is used.

                The copy functor interface is:
                \code
                struct copy_functor {
                    void operator()( Key& dest, Key const& src );
                };
                \endcode
            */
            typedef opt::none                           copy_policy;
        };


        /// Metafunction converting option list to EllenBinTreeSet traits
        /**
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>
            \p Options list see \ref cds_container_EllenBinTreeSet "EllenBinTreeSet".
        */
        template <CDS_DECL_OPTIONS11>
        struct make_set_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< type_traits, CDS_OPTIONS11 >::type
                ,CDS_OPTIONS11
            >::type   type;
#   endif
        };

        /// Metafunction converting option list to EllenBinTreeMap traits
        /**
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>
            \p Options list see \ref cds_container_EllenBinTreeMap "EllenBinTreeMap".
        */
        template <CDS_DECL_OPTIONS11>
        struct make_map_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< type_traits, CDS_OPTIONS11 >::type
                ,CDS_OPTIONS11
            >::type   type;
#   endif
        };

        //@cond
        namespace details {

            template < class GC, typename Key, typename T, class Traits>
            struct make_ellen_bintree_set
            {
                typedef GC      gc;
                typedef Key     key_type;
                typedef T       value_type;
                typedef Traits  original_type_traits;

                typedef node< gc, value_type >  leaf_node;

                struct intrusive_key_extractor
                {
                    void operator()( key_type& dest, leaf_node const& src ) const
                    {
                        typename original_type_traits::key_extractor()( dest, src.m_Value );
                    }
                };

                struct value_accessor
                {
                    value_type const& operator()( leaf_node const& node ) const
                    {
                        return node.m_Value;
                    }
                };

                typedef typename cds::opt::details::make_comparator< value_type, original_type_traits, false >::type key_comparator;

                typedef cds::details::Allocator< leaf_node, typename original_type_traits::allocator>    cxx_leaf_node_allocator;
                struct leaf_deallocator
                {
                    void operator()( leaf_node * p ) const
                    {
                        cxx_leaf_node_allocator().Delete( p );
                    }
                };

                struct intrusive_type_traits: public original_type_traits
                {
                    typedef cds::intrusive::ellen_bintree::base_hook< cds::opt::gc< gc > >  hook;
                    typedef intrusive_key_extractor key_extractor;
                    typedef leaf_deallocator        disposer;
                    typedef cds::details::compare_wrapper< leaf_node, key_comparator, value_accessor > compare;
                };

                // Metafunction result
                typedef cds::intrusive::EllenBinTree< gc, key_type, leaf_node, intrusive_type_traits >    type;
            };

            template < class GC, typename Key, typename T, class Traits>
            struct make_ellen_bintree_map
            {
                typedef GC      gc;
                typedef Key     key_type;
                typedef T       mapped_type;
                typedef map_node< gc, key_type, mapped_type >   leaf_node;
                typedef typename leaf_node::value_type          value_type;

                typedef Traits  original_type_traits;

                struct assignment_copy_policy {
                    void operator()( key_type& dest, key_type const& src )
                    {
                        dest = src;
                    }
                };
                typedef typename std::conditional<
                    std::is_same< typename original_type_traits::copy_policy, opt::none >::value,
                    assignment_copy_policy,
                    typename original_type_traits::copy_policy
                >::type copy_policy;

                struct intrusive_key_extractor
                {
                    void operator()( key_type& dest, leaf_node const& src ) const
                    {
                        copy_policy()( dest, src.m_Value.first );
                    }
                };

                struct key_accessor
                {
                    key_type const& operator()( leaf_node const& node ) const
                    {
                        return node.m_Value.first;
                    }
                };

                typedef typename cds::opt::details::make_comparator< key_type, original_type_traits, false >::type key_comparator;

                typedef cds::details::Allocator< leaf_node, typename original_type_traits::allocator>    cxx_leaf_node_allocator;
                struct leaf_deallocator
                {
                    void operator()( leaf_node * p ) const
                    {
                        cxx_leaf_node_allocator().Delete( p );
                    }
                };

                struct intrusive_type_traits: public original_type_traits
                {
                    typedef cds::intrusive::ellen_bintree::base_hook< cds::opt::gc< gc > >  hook;
                    typedef intrusive_key_extractor key_extractor;
                    typedef leaf_deallocator        disposer;
                    typedef cds::details::compare_wrapper< leaf_node, key_comparator, key_accessor >    compare;
                };

                // Metafunction result
                typedef cds::intrusive::EllenBinTree< gc, key_type, leaf_node, intrusive_type_traits >    type;
            };

        } // namespace details
        //@endcond
    } // namespace ellen_bintree

    // Forward declarations
    //@cond
    template < class GC, typename Key, typename T, class Traits = ellen_bintree::type_traits >
    class EllenBinTreeSet;

    template < class GC, typename Key, typename T, class Traits = ellen_bintree::type_traits >
    class EllenBinTreeMap;
    //@endcond

}} // namespace cds::container

#endif // #ifndef __CDS_CONTAINER_ELLEN_BINTREE_BASE_H

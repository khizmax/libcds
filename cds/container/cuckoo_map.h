// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_CUCKOO_MAP_H
#define CDSLIB_CONTAINER_CUCKOO_MAP_H

#include <cds/container/details/cuckoo_base.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace container {

    //@cond
    namespace details {
        template <typename Key, typename T, typename Traits>
        struct make_cuckoo_map
        {
            typedef Key key_type;    ///< key type
            typedef T   mapped_type; ///< type of value stored in the map
            typedef std::pair<key_type const, mapped_type>   value_type;   ///< Pair type

            typedef Traits original_traits;
            typedef typename original_traits::probeset_type probeset_type;
            static bool const store_hash = original_traits::store_hash;
            static unsigned int const store_hash_count = store_hash ? ((unsigned int) std::tuple_size< typename original_traits::hash::hash_tuple_type >::value) : 0;

            struct node_type: public intrusive::cuckoo::node<probeset_type, store_hash_count>
            {
                value_type  m_val;

                template <typename K>
                node_type( K const& key )
                    : m_val( std::make_pair( key_type(key), mapped_type()))
                {}

                template <typename K, typename Q>
                node_type( K const& key, Q const& v )
                    : m_val( std::make_pair( key_type(key), mapped_type(v)))
                {}

                template <typename K, typename... Args>
                node_type( K&& key, Args&&... args )
                    : m_val( std::forward<K>(key), std::move( mapped_type(std::forward<Args>(args)...)))
                {}
            };

            struct key_accessor {
                key_type const& operator()( node_type const& node ) const
                {
                    return node.m_val.first;
                }
            };

            struct intrusive_traits: public original_traits
            {
                typedef intrusive::cuckoo::base_hook<
                    cds::intrusive::cuckoo::probeset_type< probeset_type >
                    ,cds::intrusive::cuckoo::store_hash< store_hash_count >
                >  hook;

                typedef cds::intrusive::cuckoo::traits::disposer   disposer;

                typedef typename std::conditional<
                    std::is_same< typename original_traits::equal_to, opt::none >::value
                    , opt::none
                    , cds::details::predicate_wrapper< node_type, typename original_traits::equal_to, key_accessor >
                >::type equal_to;

                typedef typename std::conditional<
                    std::is_same< typename original_traits::compare, opt::none >::value
                    , opt::none
                    , cds::details::compare_wrapper< node_type, typename original_traits::compare, key_accessor >
                >::type compare;

                typedef typename std::conditional<
                    std::is_same< typename original_traits::less, opt::none >::value
                    ,opt::none
                    ,cds::details::predicate_wrapper< node_type, typename original_traits::less, key_accessor >
                >::type less;

                typedef opt::details::hash_list_wrapper< typename original_traits::hash, node_type, key_accessor >    hash;
            };

            typedef intrusive::CuckooSet< node_type, intrusive_traits > type;
        };
    }   // namespace details
    //@endcond

    /// Cuckoo hash map
    /** @ingroup cds_nonintrusive_map

        Source
            - [2007] M.Herlihy, N.Shavit, M.Tzafrir "Concurrent Cuckoo Hashing. Technical report"
            - [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"

        <b>About Cuckoo hashing</b>

            [From "The Art of Multiprocessor Programming"]
            <a href="https://en.wikipedia.org/wiki/Cuckoo_hashing">Cuckoo hashing</a> is a hashing algorithm in which a newly added item displaces any earlier item
            occupying the same slot. For brevity, a table is a k-entry array of items. For a hash set f size
            N = 2k we use a two-entry array of tables, and two independent hash functions,
            <tt> h0, h1: KeyRange -> 0,...,k-1</tt>
            mapping the set of possible keys to entries in he array. To test whether a value \p x is in the set,
            <tt>find(x)</tt> tests whether either <tt>table[0][h0(x)]</tt> or <tt>table[1][h1(x)]</tt> is
            equal to \p x. Similarly, <tt>erase(x)</tt>checks whether \p x is in either <tt>table[0][h0(x)]</tt>
            or <tt>table[1][h1(x)]</tt>, ad removes it if found.

            The <tt>insert(x)</tt> successively "kicks out" conflicting items until every key has a slot.
            To add \p x, the method swaps \p x with \p y, the current occupant of <tt>table[0][h0(x)]</tt>.
            If the prior value was \p nullptr, it is done. Otherwise, it swaps the newly nest-less value \p y
            for the current occupant of <tt>table[1][h1(y)]</tt> in the same way. As before, if the prior value
            was \p nullptr, it is done. Otherwise, the method continues swapping entries (alternating tables)
            until it finds an empty slot. We might not find an empty slot, either because the table is full,
            or because the sequence of displacement forms a cycle. We therefore need an upper limit on the
            number of successive displacements we are willing to undertake. When this limit is exceeded,
            we resize the hash table, choose new hash functions and start over.

            For concurrent cuckoo hashing, rather than organizing the set as a two-dimensional table of
            items, we use two-dimensional table of probe sets, where a probe set is a constant-sized set
            of items with the same hash code. Each probe set holds at most \p PROBE_SIZE items, but the algorithm
            tries to ensure that when the set is quiescent (i.e no method call in progress) each probe set
            holds no more than <tt>THRESHOLD < PROBE_SET</tt> items. While method calls are in-flight, a probe
            set may temporarily hold more than \p THRESHOLD but never more than \p PROBE_SET items.

            In current implementation, a probe set can be defined either as a (single-linked) list
            or as a fixed-sized vector, optionally ordered.

            In description above two-table cuckoo hashing (<tt>k = 2</tt>) has been considered.
            We can generalize this approach for <tt>k >= 2</tt> when we have \p k hash functions
            <tt>h[0], ... h[k-1]</tt> and \p k tables <tt>table[0], ... table[k-1]</tt>.

            The search in probe set is linear, the complexity is <tt> O(PROBE_SET) </tt>.
            The probe set may be ordered or not. Ordered probe set can be a little better since
            the average search complexity is <tt>O(PROBE_SET/2)</tt>.
            However, the overhead of sorting can eliminate a gain of ordered search.

            The probe set is ordered if \p compare or \p less is specified in \p Traits
            template parameter. Otherwise, the probe set is unordered and \p Traits must contain
            \p equal_to predicate.

        Template arguments:
        - \p Key - key type
        - \p T - the type stored in the map.
        - \p Traits - map traits, default is \p cuckoo::traits.
            It is possible to declare option-based set with \p cuckoo::make_traits metafunction
            result as \p Traits template argument.

       <b>Examples</b>

       Declares cuckoo mapping from \p std::string to struct \p foo.
       For cuckoo hashing we should provide at least two hash functions:
       \code
        struct hash1 {
            size_t operator()(std::string const& s) const
            {
                return cds::opt::v::hash<std::string>( s );
            }
        };

        struct hash2: private hash1 {
            size_t operator()(std::string const& s) const
            {
                size_t h = ~( hash1::operator()(s));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
        };
       \endcode

        Cuckoo-map with list-based unordered probe set and storing hash values
        \code
        #include <cds/container/cuckoo_map.h>

        // Declare type traits
        struct my_traits: public cds::container::cuckoo::traits
        {
            typedef std::equal_to< std::string > equal_to;
            typedef std::tuple< hash1, hash2 >  hash;

            static bool const store_hash = true;
        };

        // Declare CuckooMap type
        typedef cds::container::CuckooMap< std::string, foo, my_traits > my_cuckoo_map;

        // Equal option-based declaration
        typedef cds::container::CuckooMap< std::string, foo,
            cds::container::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::equal_to< std::equal_to< std::string > >
                ,cds::container::cuckoo::store_hash< true >
            >::type
        > opt_cuckoo_map;
        \endcode

        If we provide \p less functor instead of \p equal_to
        we get as a result a cuckoo map with ordered probe set that may improve
        performance.
        Example for ordered vector-based probe-set:

        \code
        #include <cds/container/cuckoo_map.h>

        // Declare type traits
        // We use a vector of capacity 4 as probe-set container and store hash values in the node
        struct my_traits: public cds::container::cuckoo::traits
        {
            typedef std::less< std::string > less;
            typedef std::tuple< hash1, hash2 >  hash;
            typedef cds::container::cuckoo::vector<4> probeset_type;

            static bool const store_hash = true;
        };

        // Declare CuckooMap type
        typedef cds::container::CuckooMap< std::string, foo, my_traits > my_cuckoo_map;

        // Equal option-based declaration
        typedef cds::container::CuckooMap< std::string, foo,
            cds::container::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< std::less< std::string > >
                ,cds::container::cuckoo::probeset_type< cds::container::cuckoo::vector<4> >
                ,cds::container::cuckoo::store_hash< true >
            >::type
        > opt_cuckoo_map;
        \endcode

    */
    template <typename Key, typename T, typename Traits = cuckoo::traits>
    class CuckooMap:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::CuckooSet< std::pair< Key const, T>, Traits>
#else
        protected details::make_cuckoo_map<Key, T, Traits>::type
#endif
    {
        //@cond
        typedef details::make_cuckoo_map<Key, T, Traits>    maker;
        typedef typename maker::type  base_class;
        //@endcond
    public:
        typedef Key     key_type;    ///< key type
        typedef T       mapped_type; ///< value type stored in the container
        typedef std::pair<key_type const, mapped_type> value_type;   ///< Key-value pair type stored in the map
        typedef Traits  traits;     ///< Map traits

        typedef typename traits::hash                 hash;            ///< hash functor tuple wrapped for internal use
        typedef typename base_class::hash_tuple_type  hash_tuple_type; ///< hash tuple type

        typedef typename base_class::mutex_policy mutex_policy; ///< Concurrent access policy, see \p cuckoo::traits::mutex_policy
        typedef typename base_class::stat         stat;         ///< internal statistics type

        static bool const c_isSorted = base_class::c_isSorted; ///< whether the probe set should be ordered
        static size_t const c_nArity = base_class::c_nArity;   ///< the arity of cuckoo hashing: the number of hash functors provided; minimum 2.

        typedef typename base_class::key_equal_to key_equal_to; ///< Key equality functor; used only for unordered probe-set

        typedef typename base_class::key_comparator  key_comparator; ///< key comparing functor based on opt::compare and opt::less option setter. Used only for ordered probe set

        typedef typename base_class::allocator     allocator; ///< allocator type used for internal bucket table allocations

        /// Node allocator type
        typedef typename std::conditional<
            std::is_same< typename traits::node_allocator, opt::none >::value,
            allocator,
            typename traits::node_allocator
        >::type node_allocator;

        /// item counter type
        typedef typename traits::item_counter  item_counter;

    protected:
        //@cond
        typedef typename base_class::scoped_cell_lock   scoped_cell_lock;
        typedef typename base_class::scoped_full_lock   scoped_full_lock;
        typedef typename base_class::scoped_resize_lock scoped_resize_lock;
        typedef typename maker::key_accessor            key_accessor;

        typedef typename base_class::value_type         node_type;
        typedef cds::details::Allocator< node_type, node_allocator >    cxx_node_allocator;
        //@endcond

    public:
        static unsigned int const   c_nDefaultProbesetSize = base_class::c_nDefaultProbesetSize; ///< default probeset size
        static size_t const         c_nDefaultInitialSize = base_class::c_nDefaultInitialSize;   ///< default initial size
        static unsigned int const   c_nRelocateLimit = base_class::c_nRelocateLimit;             ///< Count of attempts to relocate before giving up

    protected:
        //@cond
        template <typename K>
        static node_type * alloc_node( K const& key )
        {
            return cxx_node_allocator().New( key );
        }
        template <typename K, typename... Args>
        static node_type * alloc_node( K&& key, Args&&... args )
        {
            return cxx_node_allocator().MoveNew( std::forward<K>( key ), std::forward<Args>(args)... );
        }

        static void free_node( node_type * pNode )
        {
            cxx_node_allocator().Delete( pNode );
        }
        //@endcond

    protected:
        //@cond
        struct node_disposer {
            void operator()( node_type * pNode )
            {
                free_node( pNode );
            }
        };

        typedef std::unique_ptr< node_type, node_disposer >     scoped_node_ptr;

        //@endcond

    public:
        /// Default constructor
        /**
            Initial size = \ref c_nDefaultInitialSize

            Probe set size:
            - \ref c_nDefaultProbesetSize if \p probeset_type is \p cuckoo::list
            - \p Capacity if \p probeset_type is <tt> cuckoo::vector<Capacity> </tt>

            Probe set threshold = probe set size - 1
        */
        CuckooMap()
        {}

        /// Constructs an object with given probe set size and threshold
        /**
            If probe set type is <tt> cuckoo::vector<Capacity> </tt> vector
            then \p nProbesetSize should be equal to vector's \p Capacity.
        */
        CuckooMap(
            size_t nInitialSize                     ///< Initial map size; if 0 - use default initial size \ref c_nDefaultInitialSize
            , unsigned int nProbesetSize            ///< probe set size
            , unsigned int nProbesetThreshold = 0   ///< probe set threshold, <tt>nProbesetThreshold < nProbesetSize</tt>. If 0, nProbesetThreshold = nProbesetSize - 1
        )
        : base_class( nInitialSize, nProbesetSize, nProbesetThreshold )
        {}

        /// Constructs an object with given hash functor tuple
        /**
            The probe set size and threshold are set as default, see CuckooSet()
        */
        CuckooMap(
            hash_tuple_type const& h    ///< hash functor tuple of type <tt>std::tuple<H1, H2, ... Hn></tt> where <tt> n == \ref c_nArity </tt>
        )
        : base_class( h )
        {}

        /// Constructs a map with given probe set properties and hash functor tuple
        /**
            If probe set type is <tt> cuckoo::vector<Capacity> </tt> vector
            then \p nProbesetSize should be equal to vector's \p Capacity.
        */
        CuckooMap(
            size_t nInitialSize                 ///< Initial map size; if 0 - use default initial size \ref c_nDefaultInitialSize
            , unsigned int nProbesetSize        ///< probe set size
            , unsigned int nProbesetThreshold   ///< probe set threshold, <tt>nProbesetThreshold < nProbesetSize</tt>. If 0, nProbesetThreshold = nProbesetSize - 1
            , hash_tuple_type const& h    ///< hash functor tuple of type <tt>std::tuple<H1, H2, ... Hn></tt> where <tt> n == \ref c_nArity </tt>
        )
        : base_class( nInitialSize, nProbesetSize, nProbesetThreshold, h )
        {}

        /// Constructs a map with given hash functor tuple (move semantics)
        /**
            The probe set size and threshold are set as default, see CuckooSet()
        */
        CuckooMap(
            hash_tuple_type&& h     ///< hash functor tuple of type <tt>std::tuple<H1, H2, ... Hn></tt> where <tt> n == \ref c_nArity </tt>
        )
        : base_class( std::forward<hash_tuple_type>(h))
        {}

        /// Constructs a map with given probe set properties and hash functor tuple (move semantics)
        /**
            If probe set type is <tt> cuckoo::vector<Capacity> </tt> vector
            then \p nProbesetSize should be equal to vector's \p Capacity.
        */
        CuckooMap(
            size_t nInitialSize                 ///< Initial map size; if 0 - use default initial size \ref c_nDefaultInitialSize
            , unsigned int nProbesetSize        ///< probe set size
            , unsigned int nProbesetThreshold   ///< probe set threshold, <tt>nProbesetThreshold < nProbesetSize</tt>. If 0, nProbesetThreshold = nProbesetSize - 1
            , hash_tuple_type&& h    ///< hash functor tuple of type <tt>std::tuple<H1, H2, ... Hn></tt> where <tt> n == \ref c_nArity </tt>
        )
        : base_class( nInitialSize, nProbesetSize, nProbesetThreshold, std::forward<hash_tuple_type>(h))
        {}

        /// Destructor clears the map
        ~CuckooMap()
        {
            clear();
        }

    public:
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
            return insert_with( key, [](value_type&){} );
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \ref key_type should be constructible from \p key of type \p K.
            - The \ref value_type should be constructible from \p val of type \p V.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K const& key, V const& val )
        {
            return insert_with( key, [&val](value_type& item) { item.second = val ; } );
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

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.
        */
        template <typename K, typename Func>
        bool insert_with( const K& key, Func func )
        {
            scoped_node_ptr pNode( alloc_node( key ));
            if ( base_class::insert( *pNode, [&func]( node_type& item ) { func( item.m_val ); } )) {
                pNode.release();
                return true;
            }
            return false;
        }

        /// For key \p key inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            scoped_node_ptr pNode( alloc_node( std::forward<K>(key), std::forward<Args>(args)... ));
            if ( base_class::insert( *pNode )) {
                pNode.release();
                return true;
            }
            return false;
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If \p key is not found in the map, then \p key is inserted iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor \p func signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - an item of the map for \p key

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            i.e. the node has been inserted or updated,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already exists.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, Func func, bool bAllowInsert = true )
        {
            scoped_node_ptr pNode( alloc_node( key ));
            std::pair<bool, bool> res = base_class::update( *pNode,
                [&func](bool bNew, node_type& item, node_type const& ){ func( bNew, item.m_val ); },
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
        /** \anchor cds_nonintrusive_CuckooMap_erase_val

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            node_type * pNode = base_class::erase(key);
            if ( pNode ) {
                free_node( pNode );
                return true;
            }
            return false;
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_CuckooMap_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            If cuckoo map is ordered, then \p Predicate should have the interface and semantics like \p std::less.
            If cuckoo map is unordered, then \p Predicate should have the interface and semantics like \p std::equal_to.
            \p Predicate must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Predicate>
        bool erase_with( K const& key, Predicate pred )
        {
            CDS_UNUSED( pred );
            node_type * pNode = base_class::erase_with(key, cds::details::predicate_wrapper<node_type, Predicate, key_accessor>());
            if ( pNode ) {
                free_node( pNode );
                return true;
            }
            return false;
        }

        /// Delete \p key from the map
        /** \anchor cds_nonintrusive_CuckooMap_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode

            Return \p true if key is found and deleted, \p false otherwise

            See also: \ref erase
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            node_type * pNode = base_class::erase( key );
            if ( pNode ) {
                f( pNode->m_val );
                free_node( pNode );
                return true;
            }
            return false;
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_CuckooMap_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            If cuckoo map is ordered, then \p Predicate should have the interface and semantics like \p std::less.
            If cuckoo map is unordered, then \p Predicate should have the interface and semantics like \p std::equal_to.
            \p Predicate must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Predicate, typename Func>
        bool erase_with( K const& key, Predicate pred, Func f )
        {
            CDS_UNUSED( pred );
            node_type * pNode = base_class::erase_with( key, cds::details::predicate_wrapper<node_type, Predicate, key_accessor>());
            if ( pNode ) {
                f( pNode->m_val );
                free_node( pNode );
                return true;
            }
            return false;
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_CuckooMap_find_func

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change \p item.second.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return base_class::find( key, [&f](node_type& item, K const& ) { f( item.m_val );});
        }

        /// Find the key \p val using \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_nonintrusive_CuckooMap_find_func "find(K const&, Func)"
            but \p pred is used for key comparison.
            If you use ordered cuckoo map, then \p Predicate should have the interface and semantics like \p std::less.
            If you use unordered cuckoo map, then \p Predicate should have the interface and semantics like \p std::equal_to.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Predicate, typename Func>
        bool find_with( K const& key, Predicate pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper<node_type, Predicate, key_accessor>(),
                [&f](node_type& item, K const& ) { f( item.m_val );});
        }

        /// Checks whether the map contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename K>
        bool contains( K const& key )
        {
            return base_class::contains( key );
        }
        //@cond
        template <typename K>
        CDS_DEPRECATED("the function is deprecated, use contains()")
        bool find( K const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Predicate>
        bool contains( K const& key, Predicate pred )
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, cds::details::predicate_wrapper<node_type, Predicate, key_accessor>());
        }
        //@cond
        template <typename K, typename Predicate>
        CDS_DEPRECATED("the function is deprecated, use contains()")
        bool find_with( K const& key, Predicate pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Clears the map
        void clear()
        {
            base_class::clear_and_dispose( node_disposer());
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

        /// Returns the size of hash table
        /**
            The hash table size is non-constant and can be increased via resizing.
        */
        size_t bucket_count() const
        {
            return base_class::bucket_count();
        }

        /// Returns lock array size
        /**
            The lock array size is constant.
        */
        size_t lock_count() const
        {
            return base_class::lock_count();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }

        /// Returns const reference to mutex policy internal statistics
        typename mutex_policy::statistics_type const& mutex_policy_statistics() const
        {
            return base_class::mutex_policy_statistics();
        }
    };
}}  // namespace cds::container

#endif //#ifndef CDSLIB_CONTAINER_CUCKOO_MAP_H

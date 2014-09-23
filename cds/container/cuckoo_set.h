//$$CDS-header$$

#ifndef __CDS_CONTAINER_CUCKOO_SET_H
#define __CDS_CONTAINER_CUCKOO_SET_H

#include <cds/container/cuckoo_base.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace container {

    //@cond
    namespace details {
        template <typename T, typename Traits>
        struct make_cuckoo_set
        {
            typedef T value_type;
            typedef Traits original_type_traits;
            typedef typename original_type_traits::probeset_type probeset_type;
            static bool const store_hash = original_type_traits::store_hash;
            static unsigned int const store_hash_count = store_hash ? ((unsigned int) std::tuple_size< typename original_type_traits::hash::hash_tuple_type >::value) : 0;

            struct node_type: public intrusive::cuckoo::node<probeset_type, store_hash_count>
            {
                value_type  m_val;

                template <typename Q>
                node_type( Q const& v )
                    : m_val(v)
                {}

                template <typename... Args>
                node_type( Args&&... args )
                    : m_val( std::forward<Args>(args)...)
                {}
            };

            struct value_accessor {
                value_type const& operator()( node_type const& node ) const
                {
                    return node.m_val;
                }
            };

            template <typename Pred, typename ReturnValue>
            using predicate_wrapper = cds::details::binary_functor_wrapper< ReturnValue, Pred, node_type, value_accessor >;

            struct intrusive_traits: public original_type_traits
            {
                typedef intrusive::cuckoo::base_hook<
                    cds::intrusive::cuckoo::probeset_type< probeset_type >
                    ,cds::intrusive::cuckoo::store_hash< store_hash_count >
                >  hook;

                typedef cds::intrusive::cuckoo::type_traits::disposer   disposer;

                typedef typename std::conditional<
                    std::is_same< typename original_type_traits::equal_to, opt::none >::value
                    , opt::none
                    , predicate_wrapper< typename original_type_traits::equal_to, bool >
                >::type equal_to;

                typedef typename std::conditional<
                    std::is_same< typename original_type_traits::compare, opt::none >::value
                    , opt::none
                    , predicate_wrapper< typename original_type_traits::compare, int >
                >::type compare;

                typedef typename std::conditional<
                    std::is_same< typename original_type_traits::less, opt::none >::value
                    ,opt::none
                    ,predicate_wrapper< typename original_type_traits::less, bool >
                >::type less;

                typedef opt::details::hash_list_wrapper< typename original_type_traits::hash, node_type, value_accessor >    hash;
            };

            typedef intrusive::CuckooSet< node_type, intrusive_traits > type;
        };
    } // namespace details
    //@endcond

    /// Cuckoo hash set
    /** @ingroup cds_nonintrusive_set

        Source
            - [2007] M.Herlihy, N.Shavit, M.Tzafrir "Concurrent Cuckoo Hashing. Technical report"
            - [2008] Maurice Herlihy, Nir Shavit "The Art of Multiprocessor Programming"

        <b>About Cuckoo hashing</b>

            [From "The Art of Multiprocessor Programming"]
            Cuckoo hashing is a hashing algorithm in which a newly added item displaces any earlier item
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

            The probe set is ordered if opt::compare or opt::less is specified in \p %CuckooSet
            declaration. Otherwise, the probe set is unordered and \p %CuckooSet must contain
            opt::equal_to option.

        Template arguments:
        - \p T - the type stored in the set.
        - \p Traits - type traits. See cuckoo::type_traits for explanation.
            It is possible to declare option-based set with cuckoo::make_traits metafunction result as \p Traits template argument.

        Template argument list \p Options... of cuckoo::make_traits metafunction are:
        - opt::hash - hash functor tuple, mandatory option. At least, two hash functors should be provided. All hash functor
            should be orthogonal (different): for each <tt> i,j: i != j => h[i](x) != h[j](x) </tt>.
            The hash functors are passed as <tt> std::tuple< H1, H2, ... Hn > </tt>. The number of hash functors specifies
            the number \p k - the count of hash tables in cuckoo hashing. If the compiler supports variadic templates
            then k is unlimited, otherwise up to 10 different hash functors are supported.
        - opt::mutex_policy - concurrent access policy.
            Available policies: cuckoo::striping, cuckoo::refinable.
            Default is cuckoo::striping.
        - opt::equal_to - key equality functor like \p std::equal_to.
            If this functor is defined then the probe-set will be unordered.
            If opt::compare or opt::less option is specified too, then the probe-set will be ordered
            and opt::equal_to will be ignored.
        - opt::compare - key comparison functor. No default functor is provided.
            If the option is not specified, the opt::less is used.
            If opt::compare or opt::less option is specified, then the probe-set will be ordered.
        - opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
            If opt::compare or opt::less option is specified, then the probe-set will be ordered.
        - opt::item_counter - the type of item counting feature. Default is \ref opt::v::sequential_item_counter.
        - opt::allocator - the allocator type using for allocating bucket tables.
            Default is \p CDS_DEFAULT_ALLOCATOR
        - opt::node_allocator - the allocator type using for allocating set's items. If this option
            is not specified then the type defined in opt::allocator option is used.
        - cuckoo::store_hash - this option reserves additional space in the node to store the hash value
            of the object once it's introduced in the container. When this option is used,
            the unordered container will store the calculated hash value in the node and rehashing operations won't need
            to recalculate the hash of the value. This option will improve the performance of unordered containers
            when rehashing is frequent or hashing the value is a slow operation. Default value is \p false.
        - \ref intrusive::cuckoo::probeset_type "cuckoo::probeset_type" - type of probe set, may be \p cuckoo::list or <tt>cuckoo::vector<Capacity></tt>,
            Default is \p cuckoo::list.
        - opt::stat - internal statistics. Possibly types: cuckoo::stat, cuckoo::empty_stat.
            Default is cuckoo::empty_stat

        <b>Examples</b>

        Cuckoo-set with list-based unordered probe set and storing hash values
        \code
        #include <cds/container/cuckoo_set.h>

        // Data stored in cuckoo set
        struct my_data
        {
            // key field
            std::string     strKey;

            // other data
            // ...
        };

        // Provide equal_to functor for my_data since we will use unordered probe-set
        struct my_data_equal_to {
            bool operator()( const my_data& d1, const my_data& d2 ) const
            {
                return d1.strKey.compare( d2.strKey ) == 0;
            }

            bool operator()( const my_data& d, const std::string& s ) const
            {
                return d.strKey.compare(s) == 0;
            }

            bool operator()( const std::string& s, const my_data& d ) const
            {
                return s.compare( d.strKey ) == 0;
            }
        };

        // Provide two hash functor for my_data
        struct hash1 {
            size_t operator()(std::string const& s) const
            {
                return cds::opt::v::hash<std::string>( s );
            }
            size_t operator()( my_data const& d ) const
            {
                return (*this)( d.strKey );
            }
        };

        struct hash2: private hash1 {
            size_t operator()(std::string const& s) const
            {
                size_t h = ~( hash1::operator()(s));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            size_t operator()( my_data const& d ) const
            {
                return (*this)( d.strKey );
            }
        };

        // Declare type traits
        struct my_traits: public cds::container::cuckoo::type_traits
        {
            typedef my_data_equa_to equal_to;
            typedef std::tuple< hash1, hash2 >  hash;

            static bool const store_hash = true;
        };

        // Declare CuckooSet type
        typedef cds::container::CuckooSet< my_data, my_traits > my_cuckoo_set;

        // Equal option-based declaration
        typedef cds::container::CuckooSet< my_data,
            cds::container::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::equal_to< my_data_equal_to >
                ,cds::container::cuckoo::store_hash< true >
            >::type
        > opt_cuckoo_set;
        \endcode

        If we provide \p compare function instead of \p equal_to for \p my_data
        we get as a result a cuckoo set with ordered probe set that may improve
        performance.
        Example for ordered vector-based probe-set:

        \code
        #include <cds/container/cuckoo_set.h>

        // Data stored in cuckoo set
        struct my_data
        {
            // key field
            std::string     strKey;

            // other data
            // ...
        };

        // Provide compare functor for my_data since we want to use ordered probe-set
        struct my_data_compare {
            int operator()( const my_data& d1, const my_data& d2 ) const
            {
                return d1.strKey.compare( d2.strKey );
            }

            int operator()( const my_data& d, const std::string& s ) const
            {
                return d.strKey.compare(s);
            }

            int operator()( const std::string& s, const my_data& d ) const
            {
                return s.compare( d.strKey );
            }
        };

        // Provide two hash functor for my_data
        struct hash1 {
            size_t operator()(std::string const& s) const
            {
                return cds::opt::v::hash<std::string>( s );
            }
            size_t operator()( my_data const& d ) const
            {
                return (*this)( d.strKey );
            }
        };

        struct hash2: private hash1 {
            size_t operator()(std::string const& s) const
            {
                size_t h = ~( hash1::operator()(s));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            size_t operator()( my_data const& d ) const
            {
                return (*this)( d.strKey );
            }
        };

        // Declare type traits
        // We use a vector of capacity 4 as probe-set container and store hash values in the node
        struct my_traits: public cds::container::cuckoo::type_traits
        {
            typedef my_data_compare compare;
            typedef std::tuple< hash1, hash2 >  hash;
            typedef cds::container::cuckoo::vector<4> probeset_type;

            static bool const store_hash = true;
        };

        // Declare CuckooSet type
        typedef cds::container::CuckooSet< my_data, my_traits > my_cuckoo_set;

        // Equal option-based declaration
        typedef cds::container::CuckooSet< my_data,
            cds::container::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::compare< my_data_compare >
                ,cds::container::cuckoo::probeset_type< cds::container::cuckoo::vector<4> >
                ,cds::container::cuckoo::store_hash< true >
            >::type
        > opt_cuckoo_set;
        \endcode

    */
    template <typename T, typename Traits = cuckoo::type_traits>
    class CuckooSet:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::CuckooSet<T, Traits>
#else
        protected details::make_cuckoo_set<T, Traits>::type
#endif
    {
        //@cond
        typedef details::make_cuckoo_set<T, Traits> maker;
        typedef typename maker::type  base_class;
        //@endcond

    public:
        typedef T       value_type  ;   ///< value type stored in the container
        typedef Traits  options     ;   ///< traits

        typedef typename options::hash                  hash            ;   ///< hash functor tuple wrapped for internal use
        typedef typename base_class::hash_tuple_type    hash_tuple_type ;   ///< Type of hash tuple

        typedef typename base_class::mutex_policy       mutex_policy    ;   ///< Concurrent access policy, see cuckoo::type_traits::mutex_policy
        typedef typename base_class::stat               stat            ;   ///< internal statistics type


        static bool const c_isSorted = base_class::c_isSorted   ; ///< whether the probe set should be ordered
        static size_t const c_nArity = base_class::c_nArity     ; ///< the arity of cuckoo hashing: the number of hash functors provided; minimum 2.

        typedef typename base_class::key_equal_to key_equal_to  ; ///< Key equality functor; used only for unordered probe-set

        typedef typename base_class::key_comparator  key_comparator ; ///< key comparing functor based on opt::compare and opt::less option setter. Used only for ordered probe set

        typedef typename base_class::allocator     allocator   ; ///< allocator type used for internal bucket table allocations

        /// Node allocator type
        typedef typename std::conditional<
            std::is_same< typename options::node_allocator, opt::none >::value,
            allocator,
            typename options::node_allocator
        >::type node_allocator;

        /// item counter type
        typedef typename options::item_counter  item_counter;

    protected:
        //@cond
        typedef typename base_class::value_type         node_type;
        typedef cds::details::Allocator< node_type, node_allocator >    cxx_node_allocator;
        //@endcond

    public:
        static unsigned int const   c_nDefaultProbesetSize = base_class::c_nDefaultProbesetSize ;   ///< default probeset size
        static size_t const         c_nDefaultInitialSize = base_class::c_nDefaultInitialSize   ;   ///< default initial size
        static unsigned int const   c_nRelocateLimit = base_class::c_nRelocateLimit             ;   ///< Count of attempts to relocate before giving up

    protected:
        //@cond
        template <typename Q>
        static node_type * alloc_node( Q const& v )
        {
            return cxx_node_allocator().New( v );
        }
        template <typename... Args>
        static node_type * alloc_node( Args&&... args )
        {
            return cxx_node_allocator().MoveNew( std::forward<Args>(args)... );
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

#   ifndef CDS_CXX11_LAMBDA_SUPPORT
        struct empty_insert_functor {
            void operator()( value_type& ) const
            {}
        };

        struct empty_find_functor {
            template <typename Q>
            void operator()( node_type& item, Q& val ) const
            {}
        };

        template <typename Func>
        class insert_wrapper: protected cds::details::functor_wrapper<Func>
        {
            typedef cds::details::functor_wrapper<Func> base_class;
        public:
            insert_wrapper( Func f ): base_class(f) {}

            void operator()( node_type& node )
            {
                base_class::get()( node.m_val );
            }
        };

        template <typename Q, typename Func>
        class ensure_wrapper: protected cds::details::functor_wrapper<Func>
        {
            typedef cds::details::functor_wrapper<Func> base_class;
        public:
            Q const&    val;

            ensure_wrapper( Q const& v, Func f) : base_class(f), val(v) {}

            void operator()( bool bNew, node_type& item, node_type const& )
            {
                base_class::get()( bNew, item.m_val, val );
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
            void operator()( node_type& item, Q& val )
            {
                base_class::get()( item.m_val, val );
            }
        };
#   endif
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
        CuckooSet()
        {}

        /// Constructs the set object with given probe set size and threshold
        /**
            If probe set type is <tt> cuckoo::vector<Capacity> </tt> vector
            then \p nProbesetSize should be equal to vector's \p Capacity.
        */
        CuckooSet(
            size_t nInitialSize                 ///< Initial set size; if 0 - use default initial size \ref c_nDefaultInitialSize
            , unsigned int nProbesetSize        ///< probe set size
            , unsigned int nProbesetThreshold = 0  ///< probe set threshold, <tt>nProbesetThreshold < nProbesetSize</tt>. If 0, nProbesetThreshold = nProbesetSize - 1
        )
        : base_class( nInitialSize, nProbesetSize, nProbesetThreshold )
        {}

        /// Constructs the set object with given hash functor tuple
        /**
            The probe set size and threshold are set as default, see CuckooSet()
        */
        CuckooSet(
            hash_tuple_type const& h    ///< hash functor tuple of type <tt>std::tuple<H1, H2, ... Hn></tt> where <tt> n == \ref c_nArity </tt>
        )
        : base_class( h )
        {}

        /// Constructs the set object with given probe set properties and hash functor tuple
        /**
            If probe set type is <tt> cuckoo::vector<Capacity> </tt> vector
            then \p nProbesetSize should be equal to vector's \p Capacity.
        */
        CuckooSet(
            size_t nInitialSize                 ///< Initial set size; if 0 - use default initial size \ref c_nDefaultInitialSize
            , unsigned int nProbesetSize        ///< probe set size
            , unsigned int nProbesetThreshold   ///< probe set threshold, <tt>nProbesetThreshold < nProbesetSize</tt>. If 0, nProbesetThreshold = nProbesetSize - 1
            , hash_tuple_type const& h    ///< hash functor tuple of type <tt>std::tuple<H1, H2, ... Hn></tt> where <tt> n == \ref c_nArity </tt>
        )
        : base_class( nInitialSize, nProbesetSize, nProbesetThreshold, h )
        {}

        /// Constructs the set object with given hash functor tuple (move semantics)
        /**
            The probe set size and threshold are set as default, see CuckooSet()
        */
        CuckooSet(
            hash_tuple_type&& h     ///< hash functor tuple of type <tt>std::tuple<H1, H2, ... Hn></tt> where <tt> n == \ref c_nArity </tt>
        )
        : base_class( std::forward<hash_tuple_type>(h) )
        {}

        /// Constructs the set object with given probe set properties and hash functor tuple (move semantics)
        /**
            If probe set type is <tt> cuckoo::vector<Capacity> </tt> vector
            then \p nProbesetSize should be equal to vector's \p Capacity.
        */
        CuckooSet(
            size_t nInitialSize                 ///< Initial set size; if 0 - use default initial size \ref c_nDefaultInitialSize
            , unsigned int nProbesetSize        ///< probe set size
            , unsigned int nProbesetThreshold   ///< probe set threshold, <tt>nProbesetThreshold < nProbesetSize</tt>. If 0, nProbesetThreshold = nProbesetSize - 1
            , hash_tuple_type&& h    ///< hash functor tuple of type <tt>std::tuple<H1, H2, ... Hn></tt> where <tt> n == \ref c_nArity </tt>
        )
        : base_class( nInitialSize, nProbesetSize, nProbesetThreshold, std::forward<hash_tuple_type>(h) )
        {}

        /// Destructor clears the set
        ~CuckooSet()
        {
            clear();
        }

    public:
        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the set.

            The type \p Q should contain as minimum the complete key for the node.
            The object of \ref value_type should be constructible from a value of type \p Q.
            In trivial case, \p Q is equal to \ref value_type.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q const& val )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return insert( val, []( value_type& ) {} );
#       else
            return insert( val, empty_insert_functor() );
#       endif
        }

        /// Inserts new node
        /**
            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls \p f functor to initialize value-field of new item .

            The functor signature is:
            \code
                void func( value_type& item );
            \endcode
            where \p item is the item inserted.

            The type \p Q can differ from \ref value_type of items storing in the set.
            Therefore, the \p value_type should be constructible from type \p Q.

            The user-defined functor is called only if the inserting is success. It can be passed by reference
            using <tt>boost::ref</tt>
        */
        template <typename Q, typename Func>
        bool insert( Q const& val, Func f )
        {
            scoped_node_ptr pNode( alloc_node( val ));
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            if ( base_class::insert( *pNode, [&f]( node_type& node ) { cds::unref(f)( node.m_val ); } ))
#       else
            insert_wrapper<Func> wrapper( f );
            if ( base_class::insert( *pNode, cds::ref(wrapper) ))
#       endif
            {
                pNode.release();
                return true;
            }
            return false;
        }

        /// Inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr pNode( alloc_node( std::forward<Args>(args)... ));
            if ( base_class::insert( *pNode )) {
                pNode.release();
                return true;
            }
            return false;
        }

        /// Ensures that the \p val exists in the set
        /**
            The operation performs inserting or changing data.

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
            - \p val - argument \p val passed into the \p ensure function

            The functor can change non-key fields of the \p item.

            You can pass \p func argument by value or by reference using <tt>boost::ref</tt>.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successfull,
            \p second is true if new item has been added or \p false if the item with \p val key
            already exists.
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> ensure( Q const& val, Func func )
        {
            scoped_node_ptr pNode( alloc_node( val ));
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            std::pair<bool, bool> res = base_class::ensure( *pNode,
                [&val,&func](bool bNew, node_type& item, node_type const& ){ cds::unref(func)( bNew, item.m_val, val ); }
            );
#       else
            ensure_wrapper<Q, Func> wrapper( val, func );
            std::pair<bool, bool> res = base_class::ensure( *pNode, cds::ref(wrapper) );
#       endif
            if ( res.first && res.second )
                pNode.release();
            return res;
        }

        /// Delete \p key from the set
        /** \anchor cds_nonintrusive_CuckooSet_erase

            Since the key of set's item type \ref value_type is not explicitly specified,
            template parameter \p Q defines the key type searching in the list.
            The set item comparator should be able to compare the type \p value_type
            and the type \p Q.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            node_type * pNode = base_class::erase( key );
            if ( pNode ) {
                free_node( pNode );
                return true;
            }
            return false;
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_CuckooSet_erase "erase(Q const&)"
            but \p pred is used for key comparing.
            If cuckoo set is ordered, then \p Predicate should have the interface and semantics like \p std::less.
            If cuckoo set is unordered, then \p Predicate should have the interface and semantics like \p std::equal_to.
            \p Predicate must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Predicate>
        bool erase_with( Q const& key, Predicate pred )
        {
            node_type * pNode = base_class::erase_with( key, typename maker::template predicate_wrapper<Predicate, bool>() );
            if ( pNode ) {
                free_node( pNode );
                return true;
            }
            return false;
        }

        /// Delete \p key from the set
        /** \anchor cds_nonintrusive_CuckooSet_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface is:
            \code
            struct functor {
                void operator()(value_type const& val);
            };
            \endcode
            The functor can be passed by value or by reference using <tt>boost:ref</tt>

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            node_type * pNode = base_class::erase( key );
            if ( pNode ) {
                cds::unref(f)( pNode->m_val );
                free_node( pNode );
                return true;
            }
            return false;
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_CuckooSet_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            If you use ordered cuckoo set, then \p Predicate should have the interface and semantics like \p std::less.
            If you use unordered cuckoo set, then \p Predicate should have the interface and semantics like \p std::equal_to.
            \p Predicate must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Predicate, typename Func>
        bool erase_with( Q const& key, Predicate pred, Func f )
        {
            node_type * pNode = base_class::erase_with( key, typename maker::template predicate_wrapper<Predicate, bool>() );
            if ( pNode ) {
                cds::unref(f)( pNode->m_val );
                free_node( pNode );
                return true;
            }
            return false;
        }

        /// Find the key \p val
        /** \anchor cds_nonintrusive_CuckooSet_find_func

            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You can pass \p f argument by value or by reference using <tt>boost::ref</tt> or cds::ref.

            The functor can change non-key fields of \p item.
            The \p val argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            The type \p Q can differ from \ref value_type of items storing in the container.
            Therefore, the \p value_type should be comparable with type \p Q.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& val, Func f )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find( val, [&f](node_type& item, Q& v) { cds::unref(f)( item.m_val, v );});
#       else
            find_wrapper<Func> wrapper(f);
            return base_class::find( val, cds::ref(wrapper) );
#       endif
        }

        /// Find the key \p val using \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_nonintrusive_CuckooSet_find_func "find(Q&, Func)"
            but \p pred is used for key comparison.
            If you use ordered cuckoo set, then \p Predicate should have the interface and semantics like \p std::less.
            If you use unordered cuckoo set, then \p Predicate should have the interface and semantics like \p std::equal_to.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Predicate, typename Func>
        bool find_with( Q& val, Predicate pred, Func f )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find_with( val, typename maker::template predicate_wrapper<Predicate, bool>(),
                [&f](node_type& item, Q& v) { cds::unref(f)( item.m_val, v );});
#       else
            find_wrapper<Func> wrapper(f);
            return base_class::find_with( val, typename maker::template predicate_wrapper<Predicate, bool>(), cds::ref(wrapper) );
#       endif
        }

        /// Find the key \p val
        /** \anchor cds_nonintrusive_CuckooSet_find_cfunc

            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q const& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You can pass \p f argument by value or by reference using <tt>boost::ref</tt> or cds::ref.

            The functor can change non-key fields of \p item.

            The type \p Q can differ from \ref value_type of items storing in the container.
            Therefore, the \p value_type should be comparable with type \p Q.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& val, Func f )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find( val, [&f](node_type& item, Q const& v) { cds::unref(f)( item.m_val, v );});
#       else
            find_wrapper<Func> wrapper(f);
            return base_class::find( val, cds::ref(wrapper) );
#       endif
        }

        /// Find the key \p val using \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_nonintrusive_CuckooSet_find_cfunc "find(Q const&, Func)"
            but \p pred is used for key comparison.
            If you use ordered cuckoo set, then \p Predicate should have the interface and semantics like \p std::less.
            If you use unordered cuckoo set, then \p Predicate should have the interface and semantics like \p std::equal_to.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Predicate, typename Func>
        bool find_with( Q const& val, Predicate pred, Func f )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find_with( val, typename maker::template predicate_wrapper<Predicate, bool>(),
                [&f](node_type& item, Q const& v) { cds::unref(f)( item.m_val, v );});
#       else
            find_wrapper<Func> wrapper(f);
            return base_class::find_with( val, typename maker::template predicate_wrapper<Predicate, bool>(), cds::ref(wrapper) );
#       endif
        }

        /// Find the key \p val
        /** \anchor cds_nonintrusive_CuckooSet_find_val

            The function searches the item with key equal to \p val
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \ref value_type.
        */
        template <typename Q>
        bool find( Q const& val )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find( val, [](node_type&, Q const&) {});
#       else
            return base_class::find( val, empty_find_functor());
#       endif
        }

        /// Find the key \p val using \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_nonintrusive_CuckooSet_find_val "find(Q const&)"
            but \p pred is used for key comparison.
            If you use ordered cuckoo set, then \p Predicate should have the interface and semantics like \p std::less.
            If you use unordered cuckoo set, then \p Predicate should have the interface and semantics like \p std::equal_to.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Predicate>
        bool find_with( Q const& val, Predicate pred )
        {
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            return base_class::find_with( val, typename maker::template predicate_wrapper<Predicate, bool>(), [](node_type&, Q const&) {});
#       else
            return base_class::find_with( val, typename maker::template predicate_wrapper<Predicate, bool>(), empty_find_functor());
#       endif
        }

        /// Clears the set
        /**
            The function erases all items from the set.
        */
        void clear()
        {
            return base_class::clear_and_dispose( node_disposer() );
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
        */
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the set
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

}} // namespace cds::container

#endif //#ifndef __CDS_CONTAINER_CUCKOO_SET_H

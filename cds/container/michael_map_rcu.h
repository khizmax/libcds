// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_MICHAEL_MAP_RCU_H
#define CDSLIB_CONTAINER_MICHAEL_MAP_RCU_H

#include <cds/container/details/michael_map_base.h>
#include <cds/details/allocator.h>

namespace cds { namespace container {

    /// Michael's hash map (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_nonintrusive_map
        \anchor cds_nonintrusive_MichaelHashMap_rcu

        Source:
            - [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

        Michael's hash table algorithm is based on lock-free ordered list and it is very simple.
        The main structure is an array \p T of size \p M. Each element in \p T is basically a pointer
        to a hash bucket, implemented as a singly linked list. The array of buckets cannot be dynamically expanded.
        However, each bucket may contain unbounded number of items.

        Template parameters are:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p OrderedList - ordered key-value list implementation used as bucket for hash map, for example, \p MichaelKVList.
            The ordered list implementation specifies the \p Key and \p Value types stored in the hash-map, the reclamation
            schema \p GC used by hash-map, the comparison functor for the type \p Key and other features specific for
            the ordered list.
        - \p Traits - map traits, default is \p michael_map::traits.
            Instead of defining \p Traits struct you may use option-based syntax with \p michael_map::make_traits metafunction

        Many of the class function take a key argument of type \p K that in general is not \p key_type.
        \p key_type and an argument of template type \p K must meet the following requirements:
        - \p key_type should be constructible from value of type \p K;
        - the hash functor should be able to calculate correct hash value from argument \p key of type \p K:
            <tt> hash( key_type(key)) == hash( key ) </tt>
        - values of type \p key_type and \p K should be comparable

        <b>How to use</b>

        The tips about how to use Michael's map see \ref cds_nonintrusive_MichaelHashMap_how_touse "MichaelHashMap".
        Remember, that you should include RCU-related header file (for example, <tt>cds/urcu/general_buffered.h</tt>)
        before including <tt>cds/container/michael_map_rcu.h</tt>.
    */
    template <
        class RCU,
        class OrderedList,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = michael_map::traits
#else
        class Traits
#endif
    >
    class MichaelHashMap< cds::urcu::gc< RCU >, OrderedList, Traits >
    {
    public:
        typedef cds::urcu::gc< RCU > gc;   ///< RCU used as garbage collector
        typedef OrderedList ordered_list;   ///< type of ordered list used as a bucket implementation
        typedef Traits      traits;        ///< Map traits

        typedef typename ordered_list::key_type          key_type;      ///< key type
        typedef typename ordered_list::mapped_type       mapped_type;   ///< value type
        typedef typename ordered_list::value_type        value_type;    ///< key/value pair stored in the list
        typedef typename ordered_list::key_comparator    key_comparator;///< key comparison functor
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename ordered_list::stat         stat;       ///< Internal statistics
        typedef typename ordered_list::exempt_ptr   exempt_ptr; ///< pointer to extracted node
        /// Type of \p get() member function return value
        typedef typename ordered_list::raw_ptr      raw_ptr;
        typedef typename ordered_list::rcu_lock     rcu_lock;   ///< RCU scoped lock
#endif

        /// Hash functor for \ref key_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type   hash;
        typedef typename traits::item_counter  item_counter;   ///< Item counter type
        typedef typename traits::allocator     allocator;    ///< Bucket table allocator

        /// Group of \p extract_xxx functions require external locking if underlying ordered list requires that
        static constexpr const bool c_bExtractLockExternal = ordered_list::c_bExtractLockExternal;

        // GC and OrderedList::gc must be the same
        static_assert(std::is_same<gc, typename ordered_list::gc>::value, "GC and OrderedList::gc must be the same");

    protected:
        //@cond
        typedef typename ordered_list::template select_stat_wrapper< typename ordered_list::stat > bucket_stat;

        typedef typename ordered_list::template rebind_traits<
            cds::opt::item_counter< cds::atomicity::empty_item_counter >
            , cds::opt::stat< typename bucket_stat::wrapped_stat >
        >::type internal_bucket_type;

        /// Bucket table allocator
        typedef typename std::allocator_traits<
            allocator
        >::template rebind_alloc< internal_bucket_type > bucket_table_allocator;
        //@endcond

    public:
        //@cond
        typedef typename bucket_stat::stat stat;
        typedef typename internal_bucket_type::exempt_ptr   exempt_ptr;
        typedef typename internal_bucket_type::raw_ptr      raw_ptr;
        typedef typename internal_bucket_type::rcu_lock     rcu_lock;
        //@endcond

    protected:
        //@cond
        const size_t    m_nHashBitmask;
        hash            m_HashFunctor;      ///< Hash functor
        internal_bucket_type*  m_Buckets;   ///< bucket table
        item_counter    m_ItemCounter;      ///< Item counter
        stat            m_Stat;             ///< Internal statistics
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type: private cds::intrusive::michael_set::details::iterator< internal_bucket_type, IsConst >
        {
            typedef cds::intrusive::michael_set::details::iterator< internal_bucket_type, IsConst >  base_class;
            friend class MichaelHashMap;

        protected:
            typedef typename base_class::bucket_ptr     bucket_ptr;
            typedef typename base_class::list_iterator  list_iterator;

        public:
            /// Value pointer type (const for const_iterator)
            typedef typename cds::details::make_const_type<typename MichaelHashMap::mapped_type, IsConst>::pointer   value_ptr;
            /// Value reference type (const for const_iterator)
            typedef typename cds::details::make_const_type<typename MichaelHashMap::mapped_type, IsConst>::reference value_ref;

            /// Key-value pair pointer type (const for const_iterator)
            typedef typename cds::details::make_const_type<typename MichaelHashMap::value_type, IsConst>::pointer   pair_ptr;
            /// Key-value pair reference type (const for const_iterator)
            typedef typename cds::details::make_const_type<typename MichaelHashMap::value_type, IsConst>::reference pair_ref;

        protected:
            iterator_type( list_iterator const& it, bucket_ptr pFirst, bucket_ptr pLast )
                : base_class( it, pFirst, pLast )
            {}

        public:
            /// Default ctor
            iterator_type()
                : base_class()
            {}

            /// Copy ctor
            iterator_type( const iterator_type& src )
                : base_class( src )
            {}

            /// Dereference operator
            pair_ptr operator ->() const
            {
                assert( base_class::m_pCurBucket != nullptr );
                return base_class::m_itList.operator ->();
            }

            /// Dereference operator
            pair_ref operator *() const
            {
                assert( base_class::m_pCurBucket != nullptr );
                return base_class::m_itList.operator *();
            }

            /// Pre-increment
            iterator_type& operator ++()
            {
                base_class::operator++();
                return *this;
            }

            /// Assignment operator
            iterator_type& operator = (const iterator_type& src)
            {
                base_class::operator =(src);
                return *this;
            }

            /// Returns current bucket (debug function)
            bucket_ptr bucket() const
            {
                return base_class::bucket();
            }

            /// Equality operator
            template <bool C>
            bool operator ==(iterator_type<C> const& i )
            {
                return base_class::operator ==( i );
            }
            /// Equality operator
            template <bool C>
            bool operator !=(iterator_type<C> const& i )
            {
                return !( *this == i );
            }
        };
        //@endcond

    public:
    ///@name Forward iterators (thread-safe under RCU lock)
    //@{

        /// Forward iterator
        /**
            The forward iterator for Michael's map is based on \p OrderedList forward iterator and has some features:
            - it has no post-increment operator
            - it iterates items in unordered fashion

            You may safely use iterators in multi-threaded environment only under RCU lock.
            Otherwise, a crash is possible if another thread deletes the element the iterator points to.

            The iterator interface:
            \code
            class iterator {
            public:
                // Default constructor
                iterator();

                // Copy construtor
                iterator( iterator const& src );

                // Dereference operator
                value_type * operator ->() const;

                // Dereference operator
                value_type& operator *() const;

                // Preincrement operator
                iterator& operator ++();

                // Assignment operator
                iterator& operator = (iterator const& src);

                // Equality operators
                bool operator ==(iterator const& i ) const;
                bool operator !=(iterator const& i ) const;
            };
            \endcode
        */
        typedef iterator_type< false >    iterator;

        /// Const forward iterator
        typedef iterator_type< true >     const_iterator;

        /// Returns a forward iterator addressing the first element in a map
        /**
            For empty map \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( m_Buckets[0].begin(), m_Buckets, m_Buckets + bucket_count());
        }

        /// Returns an iterator that addresses the location succeeding the last element in a map
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the map.
            For empty map \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( m_Buckets[bucket_count() - 1].end(), m_Buckets + bucket_count() - 1, m_Buckets + bucket_count());
        }

        /// Returns a forward const iterator addressing the first element in a map
        const_iterator begin() const
        {
            return get_const_begin();
        }

        /// Returns a forward const iterator addressing the first element in a map
        const_iterator cbegin() const
        {
            return get_const_begin();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a map
        const_iterator end() const
        {
            return get_const_end();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a map
        const_iterator cend() const
        {
            return get_const_end();
        }
    //@}

    public:
        /// Initializes the map
        /**
            The Michael's hash map is non-expandable container. You should point the average count of items \p nMaxItemCount
            when you create an object.
            \p nLoadFactor parameter defines average count of items per bucket and it should be small number between 1 and 10.
            Remember, since the bucket implementation is an ordered list, searching in the bucket is linear [<tt>O(nLoadFactor)</tt>].
            Note, that many popular STL hash map implementation uses load factor 1.

            The ctor defines hash table size as rounding <tt>nMacItemCount / nLoadFactor</tt> up to nearest power of two.
        */
        MichaelHashMap(
            size_t nMaxItemCount,   ///< estimation of max item count in the hash map
            size_t nLoadFactor      ///< load factor: estimation of max number of items in the bucket
        ) : m_nHashBitmask( michael_map::details::init_hash_bitmask( nMaxItemCount, nLoadFactor ))
          , m_Buckets( bucket_table_allocator().allocate( bucket_count()))
        {
            for ( auto it = m_Buckets, itEnd = m_Buckets + bucket_count(); it != itEnd; ++it )
                construct_bucket<bucket_stat>( it );
        }

        /// Clears hash map and destroys it
        ~MichaelHashMap()
        {
            clear();
            for ( auto it = m_Buckets, itEnd = m_Buckets + bucket_count(); it != itEnd; ++it )
                it->~internal_bucket_type();
            bucket_table_allocator().deallocate( m_Buckets, bucket_count());
        }

        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from value of type \p K.
                In trivial case, \p K is equal to \ref key_type.
            - The \p mapped_type should be default-constructible.

            The function applies RCU lock internally.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( const K& key )
        {
            const bool bRet = bucket( key ).insert( key );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p mapped_type should be constructible from \p val of type \p V.

            The function applies RCU lock internally.

            Returns \p true if \p val is inserted into the map, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K const& key, V const& val )
        {
            const bool bRet = bucket( key ).insert( key, val );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
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

            The user-defined functor is called only if inserting is successful.

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            The function applies RCU lock internally.

            @warning For \ref cds_nonintrusive_MichaelKVList_rcu "MichaelKVList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_rcu "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K, typename Func>
        bool insert_with( const K& key, Func func )
        {
            const bool bRet = bucket( key ).insert_with( key, func );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Updates data by \p key
        /**
            The operation performs inserting or replacing the element with lock-free manner.

            If the \p key not found in the map, then the new item created from \p key
            will be inserted into the map iff \p bAllowInsert is \p true.
            (note that in this case the \ref key_type should be constructible from type \p K).
            Otherwise, if \p key is found, the functor \p func is called with item found.

            The functor \p Func signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - the item found or inserted

            The functor may change any fields of the \p item.second that is \p mapped_type.

            The function applies RCU lock internally.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already exists.

            @warning For \ref cds_nonintrusive_MichaelKVList_rcu "MichaelKVList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_rcu "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, Func func, bool bAllowInsert = true )
        {
            std::pair<bool, bool> bRet = bucket( key ).update( key, func, bAllowInsert );
            if ( bRet.second )
                ++m_ItemCounter;
            return bRet;
        }
        //@cond
        template <typename K, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( K const& key, Func func )
        {
            return update( key, func, true );
        }
        //@endcond

        /// For key \p key inserts data of type \p mapped_type created from \p args
        /**
            \p key_type should be constructible from type \p K

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            const bool bRet = bucket( key ).emplace( std::forward<K>(key), std::forward<Args>(args)... );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Deletes \p key from the map
        /** \anchor cds_nonintrusive_MichaelMap_rcu_erase_val

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( const K& key )
        {
            const bool bRet = bucket( key ).erase( key );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelMap_rcu_erase_val "erase(K const&)"
            but \p pred is used for key comparing.
            \p Less predicate has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool erase_with( const K& key, Less pred )
        {
            const bool bRet = bucket( key ).erase_with( key, pred );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes \p key from the map
        /** \anchor cds_nonintrusive_MichaelMap_rcu_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( const K& key, Func f )
        {
            const bool bRet = bucket( key ).erase( key, f );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelMap_rcu_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( const K& key, Less pred, Func f )
        {
            const bool bRet = bucket( key ).erase_with( key, pred, f );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Extracts an item from the map
        /** \anchor cds_nonintrusive_MichaelHashMap_rcu_extract
            The function searches an item with key equal to \p key,
            unlinks it from the map, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item is not found the function return an empty \p exempt_ptr.

            The function just excludes the key from the map and returns a pointer to item found.
            Depends on \p ordered_list you should or should not lock RCU before calling of this function:
            - for the set based on \ref cds_nonintrusive_MichaelList_rcu "MichaelList" RCU should not be locked
            - for the set based on \ref cds_nonintrusive_LazyList_rcu "LazyList" RCU should be locked
            See ordered list implementation for details.

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/container/michael_kvlist_rcu.h>
            #include <cds/container/michael_map_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::container::MichaelKVList< rcu, int, Foo > rcu_michael_list;
            typedef cds::container::MichaelHashMap< rcu, rcu_michael_list, foo_traits > rcu_michael_map;

            rcu_michael_map theMap;
            // ...

            rcu_michael_map::exempt_ptr p;

            // For MichaelList we should not lock RCU

            // Note that you must not delete the item found inside the RCU lock
            p = theMap.extract( 10 );
            if ( p ) {
                // do something with p
                ...
            }

            // We may safely release p here
            // release() passes the pointer to RCU reclamation cycle
            p.release();
            \endcode
        */
        template <typename K>
        exempt_ptr extract( K const& key )
        {
            exempt_ptr p = bucket( key ).extract( key );
            if ( p )
                --m_ItemCounter;
            return p;
        }

        /// Extracts an item from the map using \p pred predicate for searching
        /**
            The function is an analog of \p extract(K const&) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        exempt_ptr extract_with( K const& key, Less pred )
        {
            exempt_ptr p = bucket( key ).extract_with( key, pred );
            if ( p )
                --m_ItemCounter;
            return p;
        }

        /// Finds the key \p key
        /** \anchor cds_nonintrusive_MichaelMap_rcu_find_cfunc

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change \p item.second. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the map's \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return bucket( key ).find( key, f );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelMap_rcu_find_cfunc "find(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool find_with( K const& key, Less pred, Func f )
        {
            return bucket( key ).find_with( key, pred, f );
        }

        /// Checks whether the map contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename K>
        bool contains( K const& key )
        {
            return bucket( key ).contains( key );
        }
        //@cond
        template <typename K>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( K const& key )
        {
            return bucket( key ).contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool contains( K const& key, Less pred )
        {
            return bucket( key ).contains( key, pred );
        }
        //@cond
        template <typename K, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( K const& key, Less pred )
        {
            return bucket( key ).contains( key, pred );
        }
        //@endcond

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_MichaelHashMap_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.
            Note the type of returned value depends on underlying \p ordered_list.
            For details, see documentation of ordered list you use.

            Note the compare functor should accept a parameter of type \p K that can be not the same as \p key_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::container::MichaelHashMap< your_template_parameters > hash_map;
            hash_map theMap;
            // ...
            typename hash_map::raw_ptr gp;
            {
                // Lock RCU
                hash_map::rcu_lock lock;

                gp = theMap.get( 5 );
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // gp can be reclaimed at any time after RCU has been unlocked
            }
            \endcode
        */
        template <typename K>
        raw_ptr get( K const& key )
        {
            return bucket( key ).get( key );
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelHashMap_rcu_get "get(K const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref key_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        raw_ptr get_with( K const& key, Less pred )
        {
            return bucket( key ).get_with( key, pred );
        }

        /// Clears the map (not atomic)
        /**
            The function erases all items from the map.

            The function is not atomic. It cleans up each bucket and then resets the item counter to zero.
            If there are a thread that performs insertion while \p clear is working the result is undefined in general case:
            <tt> empty() </tt> may return \p true but the map may contain item(s).
            Therefore, \p clear may be used only for debugging purposes.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        void clear()
        {
            for ( size_t i = 0; i < bucket_count(); ++i )
                m_Buckets[i].clear();
            m_ItemCounter.reset();
        }

        /// Checks if the map is empty
        /**
            @warning If you use \p atomicity::empty_item_counter in \p traits::item_counter,
            the function always returns \p true.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Returns item count in the map
        /**
            @warning If you use \p atomicity::empty_item_counter in \p traits::item_counter,
            the function always returns 0.
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

        /// Returns the size of hash table
        /**
            Since \p %MichaelHashMap cannot dynamically extend the hash table size,
            the value returned is an constant depending on object initialization parameters;
            see \p MichaelHashMap::MichaelHashMap for explanation.
        */
        size_t bucket_count() const
        {
            return m_nHashBitmask + 1;
        }

    protected:
        //@cond
        /// Calculates hash value of \p key
        template <typename Q>
        size_t hash_value( Q const& key ) const
        {
            return m_HashFunctor( key ) & m_nHashBitmask;
        }

        /// Returns the bucket (ordered list) for \p key
        template <typename Q>
        internal_bucket_type&    bucket( Q const& key )
        {
            return m_Buckets[hash_value( key )];
        }
        template <typename Q>
        internal_bucket_type const&    bucket( Q const& key ) const
        {
            return m_Buckets[hash_value( key )];
        }
        //@endcond
    private:
        //@cond
        const_iterator get_const_begin() const
        {
            return const_iterator( const_cast<internal_bucket_type const&>(m_Buckets[0]).begin(), m_Buckets, m_Buckets + bucket_count());
        }
        const_iterator get_const_end() const
        {
            return const_iterator( const_cast<internal_bucket_type const&>(m_Buckets[bucket_count() - 1]).end(), m_Buckets + bucket_count() - 1, m_Buckets + bucket_count());
        }

        template <typename Stat>
        typename std::enable_if< Stat::empty >::type construct_bucket( internal_bucket_type* bkt )
        {
            new (bkt) internal_bucket_type;
        }

        template <typename Stat>
        typename std::enable_if< !Stat::empty >::type construct_bucket( internal_bucket_type* bkt )
        {
            new (bkt) internal_bucket_type( m_Stat );
        }
        //@endcond
    };
}}  // namespace cds::container

#endif // ifndef CDSLIB_CONTAINER_MICHAEL_MAP_RCU_H

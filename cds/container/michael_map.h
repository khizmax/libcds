// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_MICHAEL_MAP_H
#define CDSLIB_CONTAINER_MICHAEL_MAP_H

#include <cds/container/details/michael_map_base.h>
#include <cds/container/details/iterable_list_base.h>
#include <cds/details/allocator.h>

namespace cds { namespace container {

    /// Michael's hash map
    /** @ingroup cds_nonintrusive_map
        \anchor cds_nonintrusive_MichaelHashMap_hp

        Source:
            - [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

        Michael's hash table algorithm is based on lock-free ordered list and it is very simple.
        The main structure is an array \p T of size \p M. Each element in \p T is basically a pointer
        to a hash bucket, implemented as a singly linked list. The array of buckets cannot be dynamically expanded.
        However, each bucket may contain unbounded number of items.

        Template parameters are:
        - \p GC - Garbage collector used. You may use any \ref cds_garbage_collector "Garbage collector"
            from the \p libcds library.
            Note the \p GC must be the same as the GC used for \p OrderedList
        - \p OrderedList - ordered key-value list implementation used as bucket for hash map, for example, \p MichaelKVList,
            \p LazyKVList, \p IterableKVList. The ordered list implementation specifies the \p Key and \p Value types
            stored in the hash-map, the reclamation schema \p GC used by hash-map, the comparison functor for the type \p Key
            and other features specific for the ordered list.
        - \p Traits - map traits, default is \p michael_map::traits.
            Instead of defining \p Traits struct you may use option-based syntax with \p michael_map::make_traits metafunction.

        Many of the class function take a key argument of type \p K that in general is not \p key_type.
        \p key_type and an argument of template type \p K must meet the following requirements:
        - \p key_type should be constructible from value of type \p K;
        - the hash functor should be able to calculate correct hash value from argument \p key of type \p K:
            <tt> hash( key_type(key)) == hash( key ) </tt>
        - values of type \p key_type and \p K should be comparable

        There are the specializations:
        - for \ref cds_urcu_desc "RCU" - declared in <tt>cds/container/michael_map_rcu.h</tt>,
            see \ref cds_nonintrusive_MichaelHashMap_rcu "MichaelHashMap<RCU>".
        - for \p cds::gc::nogc declared in <tt>cds/container/michael_map_nogc.h</tt>,
            see \ref cds_nonintrusive_MichaelHashMap_nogc "MichaelHashMap<gc::nogc>".

        \anchor cds_nonintrusive_MichaelHashMap_how_touse
        <b>How to use</b>

        Suppose, you want to make \p int to \p int map for Hazard Pointer garbage collector. You should
        choose suitable ordered list class that will be used as a bucket for the map; it may be \p MichaelKVList.
        \code
        #include <cds/container/michael_kvlist_hp.h>    // MichaelKVList for gc::HP
        #include <cds/container/michael_map.h>          // MichaelHashMap

        // List traits based on std::less predicate
        struct list_traits: public cds::container::michael_list::traits
        {
            typedef std::less<int>      less;
        };

        // Ordered list
        typedef cds::container::MichaelKVList< cds::gc::HP, int, int, list_traits> int2int_list;

        // Map traits
        struct map_traits: public cds::container::michael_map::traits
        {
            struct hash {
                size_t operator()( int i ) const
                {
                    return cds::opt::v::hash<int>()( i );
                }
            }
        };

        // Your map
        typedef cds::container::MichaelHashMap< cds::gc::HP, int2int_list, map_traits > int2int_map;

        // Now you can use int2int_map class

        int main()
        {
            int2int_map theMap;

            theMap.insert( 100 );
            ...
        }
        \endcode

        You may use option-based declaration:
        \code
        #include <cds/container/michael_kvlist_hp.h>    // MichaelKVList for gc::HP
        #include <cds/container/michael_map.h>          // MichaelHashMap

        // Ordered list
        typedef cds::container::MichaelKVList< cds::gc::HP, int, int,
            typename cds::container::michael_list::make_traits<
                cds::container::opt::less< std::less<int> >     // item comparator option
            >::type
        >  int2int_list;

        // Map
        typedef cds::container::MichaelHashMap< cds::gc::HP, int2int_list,
            cds::container::michael_map::make_traits<
                cc::opt::hash< cds::opt::v::hash<int> >
            >
        > int2int_map;
        \endcode
    */
    template <
        class GC,
        class OrderedList,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = michael_map::traits
#else
        class Traits
#endif
    >
    class MichaelHashMap
    {
    public:
        typedef GC          gc;             ///< Garbage collector
        typedef OrderedList ordered_list;   ///< type of ordered list to be used as a bucket
        typedef Traits      traits;         ///< Map traits

        typedef typename ordered_list::key_type    key_type;    ///< key type
        typedef typename ordered_list::mapped_type mapped_type; ///< value type
        typedef typename ordered_list::value_type  value_type;  ///< key/value pair stored in the map
        typedef typename traits::allocator         allocator;   ///< Bucket table allocator

        typedef typename ordered_list::key_comparator key_comparator;  ///< key compare functor
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename ordered_list::stat           stat;           ///< Internal statistics
        /// Guarded pointer - a result of \p get() and \p extract() functions
        typedef typename ordered_list::guarded_ptr    guarded_ptr;
#endif

        /// Hash functor for \ref key_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;
        typedef typename traits::item_counter  item_counter;   ///< Item counter type

        // GC and OrderedList::gc must be the same
        static_assert( std::is_same<gc, typename ordered_list::gc>::value, "GC and OrderedList::gc must be the same");

        static constexpr const size_t c_nHazardPtrCount = ordered_list::c_nHazardPtrCount; ///< Count of hazard pointer required

        //@cond
        typedef typename ordered_list::template select_stat_wrapper< typename ordered_list::stat > bucket_stat;

        typedef typename ordered_list::template rebind_traits<
            cds::opt::item_counter< cds::atomicity::empty_item_counter >
            , cds::opt::stat< typename bucket_stat::wrapped_stat >
        >::type internal_bucket_type;

        typedef typename internal_bucket_type::guarded_ptr guarded_ptr;

        typedef typename std::allocator_traits<
            allocator
        >::template rebind_alloc<internal_bucket_type> bucket_table_allocator;
        typedef typename bucket_stat::stat stat;
        //@endcond

    protected:
        //@cond
        const size_t            m_nHashBitmask;
        internal_bucket_type*   m_Buckets;     ///< bucket table
        hash                    m_HashFunctor; ///< Hash functor
        item_counter            m_ItemCounter; ///< Item counter
        stat                    m_Stat;        ///< Internal statistics
        //@endcond

    protected:
        //@cond
        /// Forward iterator
        template <bool IsConst>
        class iterator_type: protected cds::intrusive::michael_set::details::iterator< internal_bucket_type, IsConst >
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
            bool operator ==(iterator_type<C> const& i ) const
            {
                return base_class::operator ==( i );
            }
            /// Equality operator
            template <bool C>
            bool operator !=(iterator_type<C> const& i ) const
            {
                return !( *this == i );
            }
        };
        //@endcond

    public:
    ///@name Forward iterators (only for debugging purpose)
    //@{
        /// Forward iterator
        /**
            The forward iterator for Michael's map has some features:
            - it has no post-increment operator
            - to protect the value, the iterator contains a GC-specific guard + another guard is required locally for increment operator.
              For some GC (like as \p gc::HP), a guard is a limited resource per thread, so an exception (or assertion) "no free guard"
              may be thrown if the limit of guard count per thread is exceeded.
            - The iterator cannot be moved across thread boundary because it contains thread-private GC's guard.

            Iterator thread safety depends on type of \p OrderedList:
            - for \p MichaelKVList and \p LazyKVList: iterator guarantees safety even if you delete the item that iterator points to
              because that item is guarded by hazard pointer.
              However, in case of concurrent deleting operations it is no guarantee that you iterate all item in the map.
              Moreover, a crash is possible when you try to iterate the next element that has been deleted by concurrent thread.
              Use this iterator on the concurrent container for debugging purpose only.
            - for \p IterableList: iterator is thread-safe. You may use it freely in concurrent environment.

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

            @note The iterator object returned by \p end(), \p cend() member functions points to \p nullptr and should not be dereferenced.
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
            return iterator( bucket_begin()->begin(), bucket_begin(), bucket_end());
        }

        /// Returns an iterator that addresses the location succeeding the last element in a map
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the map.
            For empty map \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( bucket_end()[-1].end(), bucket_end() - 1, bucket_end());
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
        /** @anchor cds_nonintrusive_MichaelHashMap_hp_ctor
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
            )
            : m_nHashBitmask( michael_map::details::init_hash_bitmask( nMaxItemCount, nLoadFactor ))
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
                In trivial case, \p K is equal to \p key_type.
            - The \p mapped_type should be default-constructible.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K&& key )
        {
            const bool bRet = bucket( key ).insert( std::forward<K>( key ));
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

            Returns \p true if \p val is inserted into the map, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K&& key, V&& val )
        {
            const bool bRet = bucket( key ).insert( std::forward<K>( key ), std::forward<V>( val ));
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

            The \p key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            @warning For \ref cds_nonintrusive_MichaelKVList_gc "MichaelKVList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_gc "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K, typename Func>
        bool insert_with( K&& key, Func func )
        {
            const bool bRet = bucket( key ).insert_with( std::forward<K>( key ), func );
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

            The functor \p func signature depends on \p OrderedList:

            <b>for \p MichaelKVList, \p LazyKVList</b>
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - the item found or inserted

            The functor may change any fields of the \p item.second that is \p mapped_type.

            <b>for \p IterableKVList</b>
            \code
                void func( value_type& val, value_type * old );
            \endcode
            where
            - \p val - a new data constructed from \p key
            - \p old - old value that will be retired. If new item has been inserted then \p old is \p nullptr.

            The functor may change non-key fields of \p val; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            @return <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already exists.

            @warning For \ref cds_nonintrusive_MichaelKVList_gc "MichaelKVList" and \ref cds_nonintrusive_IterableKVList_gc "IterableKVList"
            as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_gc "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K, typename Func >
        std::pair<bool, bool> update( K&& key, Func func, bool bAllowInsert = true )
        {
            std::pair<bool, bool> bRet = bucket( key ).update( std::forward<K>( key ), func, bAllowInsert );
            if ( bRet.first && bRet.second )
                ++m_ItemCounter;
            return bRet;
        }
        //@cond
        template <typename K, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( K const& key, Func func )
        {
            std::pair<bool, bool> bRet = bucket( key ).update( key, func, true );
            if ( bRet.first && bRet.second )
                ++m_ItemCounter;
            return bRet;
        }
        //@endcond

        /// Inserts or updates the node (only for \p IterableKVList)
        /**
            The operation performs inserting or changing data with lock-free manner.

            If \p key is not found in the map, then \p key is inserted iff \p bAllowInsert is \p true.
            Otherwise, the current element is changed to \p val, the old element will be retired later.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if \p val has been added or \p false if the item with that key
            already in the map.
        */
        template <typename Q, typename V>
#ifdef CDS_DOXYGEN_INVOKED
        std::pair<bool, bool>
#else
        typename std::enable_if<
            std::is_same< Q, Q>::value && is_iterable_list< ordered_list >::value,
            std::pair<bool, bool>
        >::type
#endif
        upsert( Q&& key, V&& val, bool bAllowInsert = true )
        {
            std::pair<bool, bool> bRet = bucket( val ).upsert( std::forward<Q>( key ), std::forward<V>( val ), bAllowInsert );
            if ( bRet.second )
                ++m_ItemCounter;
            return bRet;
        }

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
        /** \anchor cds_nonintrusive_MichaelMap_erase_val

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            const bool bRet = bucket( key ).erase( key );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelMap_erase_val "erase(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool erase_with( K const& key, Less pred )
        {
            const bool bRet = bucket( key ).erase_with( key, pred );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes \p key from the map
        /** \anchor cds_nonintrusive_MichaelMap_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            const bool bRet = bucket( key ).erase( key, f );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelMap_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
            const bool bRet = bucket( key ).erase_with( key, pred, f );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes the item pointed by iterator \p iter (only for \p IterableList based map)
        /**
            Returns \p true if the operation is successful, \p false otherwise.
            The function can return \p false if the node the iterator points to has already been deleted
            by other thread.

            The function does not invalidate the iterator, it remains valid and can be used for further traversing.

            @note \p %erase_at() is supported only for \p %MichaelHashMap based on \p IterableList.
        */
#ifdef CDS_DOXYGEN_INVOKED
        bool erase_at( iterator const& iter )
#else
        template <typename Iterator>
        typename std::enable_if< std::is_same<Iterator, iterator>::value && is_iterable_list< ordered_list >::value, bool >::type
        erase_at( Iterator const& iter )
#endif
        {
            assert( iter != end());
            assert( iter.bucket() != nullptr );

            if ( iter.bucket()->erase_at( iter.underlying_iterator())) {
                --m_ItemCounter;
                return true;
            }
            return false;
        }

        /// Extracts the item with specified \p key
        /** \anchor cds_nonintrusive_MichaelHashMap_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the map, and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p K that may be not the same as \p key_type.

            The extracted item is freed automatically when returned \p guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::MichaelHashMap< your_template_args > michael_map;
            michael_map theMap;
            // ...
            {
                michael_map::guarded_ptr gp( theMap.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        template <typename K>
        guarded_ptr extract( K const& key )
        {
            guarded_ptr gp( bucket( key ).extract( key ));
            if ( gp )
                --m_ItemCounter;
            return gp;
        }

        /// Extracts the item using compare functor \p pred
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelHashMap_hp_extract "extract(K const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \p key_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        guarded_ptr extract_with( K const& key, Less pred )
        {
            guarded_ptr gp( bucket( key ).extract_with( key, pred ));
            if ( gp )
                --m_ItemCounter;
            return gp;
        }

        /// Finds the key \p key
        /** \anchor cds_nonintrusive_MichaelMap_find_cfunc

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

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return bucket( key ).find( key, f );
        }

        /// Finds \p key and returns iterator pointed to the item found (only for \p IterableList)
        /**
            If \p key is not found the function returns \p end().

            @note This function is supported only for map based on \p IterableList
        */
        template <typename K>
#ifdef CDS_DOXYGEN_INVOKED
        iterator
#else
        typename std::enable_if< std::is_same<K,K>::value && is_iterable_list< ordered_list >::value, iterator >::type
#endif
        find( K const& key )
        {
            auto& b = bucket( key );
            auto it = b.find( key );
            if ( it == b.end())
                return end();
            return iterator( it, &b, bucket_end());
        }


        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelMap_find_cfunc "find(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool find_with( K const& key, Less pred, Func f )
        {
            return bucket( key ).find_with( key, pred, f );
        }

        /// Finds \p key using \p pred predicate and returns iterator pointed to the item found (only for \p IterableList)
        /**
            The function is an analog of \p find(K&) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.

            If \p key is not found the function returns \p end().

            @note This function is supported only for map based on \p IterableList
        */
        template <typename K, typename Less>
#ifdef CDS_DOXYGEN_INVOKED
        iterator
#else
        typename std::enable_if< std::is_same<K, K>::value && is_iterable_list< ordered_list >::value, iterator >::type
#endif
        find_with( K const& key, Less pred )
        {
            auto& b = bucket( key );
            auto it = b.find_with( key, pred );
            if ( it == b.end())
                return end();
            return iterator( it, &b, bucket_end());
        }

        /// Checks whether the map contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename K>
        bool contains( K const& key )
        {
            return bucket( key ).contains( key );
        }

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

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_MichaelHashMap_hp_get
            The function searches the item with key equal to \p key
            and returns the guarded pointer to the item found.
            If \p key is not found the function returns an empty guarded pointer,

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::MichaeHashMap< your_template_params >  michael_map;
            michael_map theMap;
            // ...
            {
                michael_map::guarded_ptr gp( theMap.get( 5 ));
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode

            Note the compare functor specified for \p OrderedList template parameter
            should accept a parameter of type \p K that can be not the same as \p key_type.
        */
        template <typename K>
        guarded_ptr get( K const& key )
        {
            return bucket( key ).get( key );
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelHashMap_hp_get "get( K const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \p key_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        guarded_ptr get_with( K const& key, Less pred )
        {
            return bucket( key ).get_with( key, pred );
        }

        /// Clears the map (not atomic)
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
            If you use \p atomicity::empty_item_counter in \p traits::item_counter,
            the function always returns 0.
        */
        size_t size() const
        {
            return m_ItemCounter;
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

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
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
        internal_bucket_type& bucket( Q const& key )
        {
            return m_Buckets[hash_value( key )];
        }
        //@endcond

    private:
        //@cond
        internal_bucket_type* bucket_begin() const
        {
            return m_Buckets;
        }

        internal_bucket_type* bucket_end() const
        {
            return m_Buckets + bucket_count();
        }

        const_iterator get_const_begin() const
        {
            return const_iterator( bucket_begin()->cbegin(), bucket_begin(), bucket_end());
        }
        const_iterator get_const_end() const
        {
            return const_iterator( (bucket_end() - 1)->cend(), bucket_end() - 1, bucket_end());
        }

        template <typename Stat>
        typename std::enable_if< Stat::empty >::type construct_bucket( internal_bucket_type* b )
        {
            new (b) internal_bucket_type;
        }

        template <typename Stat>
        typename std::enable_if< !Stat::empty >::type construct_bucket( internal_bucket_type* b )
        {
            new (b) internal_bucket_type( m_Stat );
        }
        //@endcond
    };
}}  // namespace cds::container

#endif // ifndef CDSLIB_CONTAINER_MICHAEL_MAP_H

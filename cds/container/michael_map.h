/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#ifndef CDSLIB_CONTAINER_MICHAEL_MAP_H
#define CDSLIB_CONTAINER_MICHAEL_MAP_H

#include <cds/container/details/michael_map_base.h>
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
        - \p OrderedList - ordered key-value list implementation used as bucket for hash map, for example, \p MichaelKVList
            or \p LazyKVList. The ordered list implementation specifies the \p Key and \p Value types stored in the hash-map,
            the reclamation schema \p GC used by hash-map, the comparison functor for the type \p Key and other features
            specific for the ordered list.
        - \p Traits - map traits, default is \p michael_map::traits.
            Instead of defining \p Traits struct you may use option-based syntax with \p michael_map::make_traits metafunction.

        Many of the class function take a key argument of type \p K that in general is not \p key_type.
        \p key_type and an argument of template type \p K must meet the following requirements:
        - \p key_type should be constructible from value of type \p K;
        - the hash functor should be able to calculate correct hash value from argument \p key of type \p K:
            <tt> hash( key_type(key) ) == hash( key ) </tt>
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
        typedef GC          gc;          ///< Garbage collector
        typedef OrderedList bucket_type; ///< type of ordered list to be used as a bucket
        typedef Traits      traits;      ///< Map traits

        typedef typename bucket_type::key_type    key_type;    ///< key type
        typedef typename bucket_type::mapped_type mapped_type; ///< value type
        typedef typename bucket_type::value_type  value_type;  ///< key/value pair stored in the map

        typedef typename bucket_type::key_comparator key_comparator;  ///< key compare functor

        /// Hash functor for \ref key_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;
        typedef typename traits::item_counter  item_counter;   ///< Item counter type

        /// Bucket table allocator
        typedef cds::details::Allocator< bucket_type, typename traits::allocator >  bucket_table_allocator;
        typedef typename bucket_type::guarded_ptr  guarded_ptr; ///< Guarded pointer

        static CDS_CONSTEXPR const size_t c_nHazardPtrCount = bucket_type::c_nHazardPtrCount; ///< Count of hazard pointer required

    protected:
        item_counter    m_ItemCounter; ///< Item counter
        hash            m_HashFunctor; ///< Hash functor
        bucket_type *   m_Buckets;     ///< bucket table

    private:
        //@cond
        const size_t    m_nHashBitmask;
        //@endcond

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
        bucket_type&    bucket( Q const& key )
        {
            return m_Buckets[ hash_value( key ) ];
        }
        //@endcond

    protected:
        //@cond
        /// Forward iterator
        template <bool IsConst>
        class iterator_type: private cds::intrusive::michael_set::details::iterator< bucket_type, IsConst >
        {
            typedef cds::intrusive::michael_set::details::iterator< bucket_type, IsConst >  base_class;
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
    ///@name Forward iterators (only for debugging purpose)
    //@{
        /// Forward iterator
        /**
            The iteration is unordered.
            The iterator object is thread-safe: the element pointed by the iterator object is guarded,
            so, the element cannot be reclaimed while the iterator object is alive.
            However, passing an iterator object between threads is dangerous.

            @warning Due to concurrent nature of Michael's map it is not guarantee that you can iterate
            all elements in the map: any concurrent deletion can exclude the element
            pointed by the iterator from the map, and your iteration can be terminated
            before end of the map. Therefore, such iteration is more suitable for debugging purpose only.

            Remember, each iterator object requires an additional hazard pointer, that may be
            a limited resource for \p GC like \p gc::HP (for \p gc::DHP the count of
            guards is unlimited).

            The iterator class supports the following minimalistic interface:
            \code
            struct iterator {
                // Default ctor
                iterator();

                // Copy ctor
                iterator( iterator const& s);

                value_type * operator ->() const;
                value_type& operator *() const;

                // Pre-increment
                iterator& operator ++();

                // Copy assignment
                iterator& operator = (iterator const& src);

                bool operator ==(iterator const& i ) const;
                bool operator !=(iterator const& i ) const;
            };
            \endcode

            Note, the iterator object returned by \p end(), \p cend() member functions points to \p nullptr and should not be dereferenced.
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
            return iterator( m_Buckets[0].begin(), m_Buckets, m_Buckets + bucket_count() );
        }

        /// Returns an iterator that addresses the location succeeding the last element in a map
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the map.
            For empty map \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( m_Buckets[bucket_count() - 1].end(), m_Buckets + bucket_count() - 1, m_Buckets + bucket_count() );
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

    private:
        //@cond
        const_iterator get_const_begin() const
        {
            return const_iterator( const_cast<bucket_type const&>(m_Buckets[0]).begin(), m_Buckets, m_Buckets + bucket_count() );
        }
        const_iterator get_const_end() const
        {
            return const_iterator( const_cast<bucket_type const&>(m_Buckets[bucket_count() - 1]).end(), m_Buckets + bucket_count() - 1, m_Buckets + bucket_count() );
        }
        //@endcond

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
        ) : m_nHashBitmask( michael_map::details::init_hash_bitmask( nMaxItemCount, nLoadFactor ))
        {
            // GC and OrderedList::gc must be the same
            static_assert( std::is_same<gc, typename bucket_type::gc>::value, "GC and OrderedList::gc must be the same");

            // atomicity::empty_item_counter is not allowed as a item counter
            static_assert( !std::is_same<item_counter, atomicity::empty_item_counter>::value,
                           "atomicity::empty_item_counter is not allowed as a item counter");

            m_Buckets = bucket_table_allocator().NewArray( bucket_count() );
        }

        /// Clears hash map and destroys it
        ~MichaelHashMap()
        {
            clear();
            bucket_table_allocator().Delete( m_Buckets, bucket_count() );
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

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successfull,
            \p second is true if new item has been added or \p false if the item with \p key
            already exists.

            @warning For \ref cds_nonintrusive_MichaelKVList_gc "MichaelKVList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_gc "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K, typename Func >
        std::pair<bool, bool> update( K const& key, Func func, bool bAllowInsert = true )
        {
            std::pair<bool, bool> bRet = bucket( key ).update( key, func, bAllowInsert );
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
            Emptiness is checked by item counting: if item count is zero then the map is empty.
            Thus, the correct item counting is an important part of the map implementation.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Returns item count in the map
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
    };
}}  // namespace cds::container

#endif // ifndef CDSLIB_CONTAINER_MICHAEL_MAP_H

//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_MICHAEL_MAP_NOGC_H
#define CDSLIB_CONTAINER_MICHAEL_MAP_NOGC_H

#include <cds/container/details/michael_map_base.h>
#include <cds/gc/nogc.h>
#include <cds/details/allocator.h>

namespace cds { namespace container {

    /// Michael's hash map (template specialization for \p cds::gc::nogc)
    /** @ingroup cds_nonintrusive_map
        \anchor cds_nonintrusive_MichaelHashMap_nogc

        This specialization is so-called append-only when no item
        reclamation may be performed. The class does not support deleting of map item.

        See \ref cds_nonintrusive_MichaelHashMap_hp "MichaelHashMap" for description of template parameters.
    */
    template <
        class OrderedList,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = michael_map::traits
#else
        class Traits
#endif
    >
    class MichaelHashMap<cds::gc::nogc, OrderedList, Traits>
    {
    public:
        typedef cds::gc::nogc gc;        ///< No garbage collector
        typedef OrderedList bucket_type; ///< type of ordered list used as a bucket implementation
        typedef Traits      traits;      ///< Map traits

        typedef typename bucket_type::key_type    key_type;    ///< key type
        typedef typename bucket_type::mapped_type mapped_type; ///< type of value to be stored in the map
        typedef typename bucket_type::value_type  value_type;  ///< Pair used as the some functor's argument

        typedef typename bucket_type::key_comparator key_comparator;   ///< key comparing functor

        /// Hash functor for \ref key_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;
        typedef typename traits::item_counter item_counter;   ///< Item counter type

        /// Bucket table allocator
        typedef cds::details::Allocator< bucket_type, typename traits::allocator >  bucket_table_allocator;

        //@cond
        typedef cds::container::michael_map::implementation_tag implementation_tag;
        //@endcond

    protected:
        //@cond
        typedef typename bucket_type::iterator       bucket_iterator;
        typedef typename bucket_type::const_iterator bucket_const_iterator;
        //@endcond

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
        size_t hash_value( key_type const & key ) const
        {
            return m_HashFunctor( key ) & m_nHashBitmask;
        }

        /// Returns the bucket (ordered list) for \p key
        bucket_type&    bucket( key_type const& key )
        {
            return m_Buckets[ hash_value( key ) ];
        }
        //@endcond

    protected:
        /// Forward iterator
        /**
            \p IsConst - constness boolean flag

            The forward iterator for Michael's map is based on \p OrderedList forward iterator and has some features:
            - it has no post-increment operator, only pre-increment
            - it iterates items in unordered fashion
        */
        template <bool IsConst>
        class iterator_type: private cds::intrusive::michael_set::details::iterator< bucket_type, IsConst >
        {
            //@cond
            typedef cds::intrusive::michael_set::details::iterator< bucket_type, IsConst >  base_class;
            friend class MichaelHashMap;
            //@endcond

        protected:
            //@cond
            //typedef typename base_class::bucket_type    bucket_type;
            typedef typename base_class::bucket_ptr     bucket_ptr;
            typedef typename base_class::list_iterator  list_iterator;

            //typedef typename bucket_type::key_type      key_type;
            //@endcond

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
            //@cond
            iterator_type( list_iterator const& it, bucket_ptr pFirst, bucket_ptr pLast )
                : base_class( it, pFirst, pLast )
            {}
            //@endcond

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


    public:
        /// Forward iterator
        typedef iterator_type< false >    iterator;

        /// Const forward iterator
        typedef iterator_type< true >     const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        /**
            For empty set \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( m_Buckets[0].begin(), m_Buckets, m_Buckets + bucket_count() );
        }

        /// Returns an iterator that addresses the location succeeding the last element in a set
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the set.
            For empty set \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( m_Buckets[bucket_count() - 1].end(), m_Buckets + bucket_count() - 1, m_Buckets + bucket_count() );
        }

        /// Returns a forward const iterator addressing the first element in a set
        //@{
        const_iterator begin() const
        {
            return get_const_begin();
        }
        const_iterator cbegin() const
        {
            return get_const_begin();
        }
        //@}

        /// Returns an const iterator that addresses the location succeeding the last element in a set
        //@{
        const_iterator end() const
        {
            return get_const_end();
        }
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
        /// Initialize the map
        /** @copydetails cds_nonintrusive_MichaelHashMap_hp_ctor
        */
        MichaelHashMap(
            size_t nMaxItemCount,   ///< estimation of max item count in the hash set
            size_t nLoadFactor      ///< load factor: estimation of max number of items in the bucket
        ) : m_nHashBitmask( michael_map::details::init_hash_bitmask( nMaxItemCount, nLoadFactor ))
        {
            // GC and OrderedList::gc must be the same
            static_assert( std::is_same<gc, typename bucket_type::gc>::value, "GC and OrderedList::gc must be the same");

            // atomicity::empty_item_counter is not allowed as a item counter
            static_assert( !std::is_same<item_counter, atomicity::empty_item_counter>::value,
                           "cds::atomicity::empty_item_counter is not allowed as a item counter");

            m_Buckets = bucket_table_allocator().NewArray( bucket_count() );
        }

        /// Clears hash set and destroys it
        ~MichaelHashMap()
        {
            clear();
            bucket_table_allocator().Delete( m_Buckets, bucket_count() );
        }

        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \ref key_type should be constructible from value of type \p K.
                In trivial case, \p K is equal to \ref key_type.
            - The \ref mapped_type should be default-constructible.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K>
        iterator insert( const K& key )
        {
            bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.insert( key );

            if ( it != refBucket.end() ) {
                ++m_ItemCounter;
                return iterator( it, &refBucket, m_Buckets + bucket_count() );
            }

            return end();
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \ref key_type should be constructible from \p key of type \p K.
            - The \ref mapped_type should be constructible from \p val of type \p V.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename V>
        iterator insert( K const& key, V const& val )
        {
            bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.insert( key, val );

            if ( it != refBucket.end() ) {
                ++m_ItemCounter;
                return iterator( it, &refBucket, m_Buckets + bucket_count() );
            }

            return end();
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
            to the map's item inserted. <tt>item.second</tt> is a reference to item's value that may be changed.

            The user-defined functor it is called only if the inserting is successful.
            The \p key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p f functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed

            @warning For \ref cds_nonintrusive_MichaelKVList_nogc "MichaelKVList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_nogc "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K, typename Func>
        iterator insert_with( const K& key, Func func )
        {
            bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.insert_with( key, func );

            if ( it != refBucket.end() ) {
                ++m_ItemCounter;
                return iterator( it, &refBucket, m_Buckets + bucket_count() );
            }

            return end();
        }

        /// For key \p key inserts data of type \p mapped_type created from \p args
        /**
            \p key_type should be constructible from type \p K

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename... Args>
        iterator emplace( K&& key, Args&&... args )
        {
            bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.emplace( std::forward<K>(key), std::forward<Args>(args)... );

            if ( it != refBucket.end() ) {
                ++m_ItemCounter;
                return iterator( it, &refBucket, m_Buckets + bucket_count() );
            }

            return end();
        }

        /// Ensures that the key \p key exists in the map
        /**
            The operation inserts new item if the key \p key is not found in the map.
            Otherwise, the function returns an iterator that points to item found.

            Returns <tt> std::pair<iterator, bool>  </tt> where \p first is an iterator pointing to
            item found or inserted, \p second is true if new item has been added or \p false if the item
            already is in the list.

            @warning For \ref cds_nonintrusive_MichaelKVList_nogc "MichaelKVList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_nogc "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K>
        std::pair<iterator, bool> ensure( const K& key )
        {
            bucket_type& refBucket = bucket( key );
            std::pair<bucket_iterator, bool> ret = refBucket.ensure( key );

            if ( ret.second  )
                ++m_ItemCounter;

            return std::make_pair( iterator( ret.first, &refBucket, m_Buckets + bucket_count() ), ret.second );
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_MichaelMap_nogc_find

            The function searches the item with key equal to \p key
            and returns an iterator pointed to item found if the key is found,
            and \ref end() otherwise
        */
        template <typename K>
        iterator find( const K& key )
        {
            bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.find( key );

            if ( it != refBucket.end() )
                return iterator( it, &refBucket, m_Buckets + bucket_count() );

            return end();
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelMap_nogc_find "find(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        iterator find_with( const K& key, Less pred )
        {
            bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.find_with( key, pred );

            if ( it != refBucket.end() )
                return iterator( it, &refBucket, m_Buckets + bucket_count() );

            return end();
        }

        /// Clears the map (not atomic)
        void clear()
        {
            for ( size_t i = 0; i < bucket_count(); ++i )
                m_Buckets[i].clear();
            m_ItemCounter.reset();
        }

        /// Checks whether the map is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the map is empty.
            Thus, the correct item counting feature is an important part of Michael's map implementation.
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
}} // namespace cds::container

#endif // ifndef CDSLIB_CONTAINER_MICHAEL_MAP_NOGC_H

// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_FELDMAN_HASHMAP_RCU_H
#define CDSLIB_CONTAINER_FELDMAN_HASHMAP_RCU_H

#include <cds/intrusive/feldman_hashset_rcu.h>
#include <cds/container/details/feldman_hashmap_base.h>

namespace cds { namespace container {

    /// Hash map based on multi-level array
    /** @ingroup cds_nonintrusive_map
        @anchor cds_container_FeldmanHashMap_rcu

        Source:
        - [2013] Steven Feldman, Pierre LaBorde, Damian Dechev "Concurrent Multi-level Arrays:
                 Wait-free Extensible Hash Maps"

        See algorithm short description @ref cds_container_FeldmanHashMap_hp "here"

        @note Two important things you should keep in mind when you're using \p %FeldmanHashMap:
        - all keys is converted to fixed-size bit-string by hash functor provided.
          You can use variable-length keys, for example, \p std::string as a key for \p %FeldmanHashMap,
          but real key in the map will be fixed-size hash values of your keys.
          For the strings you may use well-known hashing algorithms like <a href="https://en.wikipedia.org/wiki/Secure_Hash_Algorithm">SHA1, SHA2</a>,
          <a href="https://en.wikipedia.org/wiki/MurmurHash">MurmurHash</a>, <a href="https://en.wikipedia.org/wiki/CityHash">CityHash</a>
          or its successor <a href="https://code.google.com/p/farmhash/">FarmHash</a> and so on, which
          converts variable-length strings to fixed-length bit-strings, and such hash values will be the keys in \p %FeldmanHashMap.
          If your key is fixed-sized the hash functor is optional, see \p feldman_hashmap::traits::hash for explanation and examples.
        - \p %FeldmanHashMap uses a perfect hashing. It means that if two different keys, for example, of type \p std::string,
          have identical hash then you cannot insert both that keys in the map. \p %FeldmanHashMap does not maintain the key,
          it maintains its fixed-size hash value.

        The map supports @ref cds_container_FeldmanHashMap_rcu_iterators "bidirectional thread-safe iterators".

        Template parameters:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - a key type to be stored in the map
        - \p T - a value type to be stored in the map
        - \p Traits - type traits, the structure based on \p feldman_hashmap::traits or result of \p feldman_hashmap::make_traits metafunction.

        @note Before including <tt><cds/intrusive/feldman_hashset_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.
    */
    template <
        class RCU
        ,typename Key
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
        ,class Traits = feldman_hashmap::traits
#else
        ,class Traits
#endif
    >
    class FeldmanHashMap< cds::urcu::gc< RCU >, Key, T, Traits >
#ifdef CDS_DOXYGEN_INVOKED
        : protected cds::intrusive::FeldmanHashSet< cds::urcu::gc< RCU >, std::pair<Key const, T>, Traits >
#else
        : protected cds::container::details::make_feldman_hashmap< cds::urcu::gc< RCU >, Key, T, Traits >::type
#endif
    {
        //@cond
        typedef cds::container::details::make_feldman_hashmap< cds::urcu::gc< RCU >, Key, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond
    public:
        typedef cds::urcu::gc< RCU > gc; ///< RCU garbage collector
        typedef Key     key_type;    ///< Key type
        typedef T       mapped_type; ///< Mapped type
        typedef std::pair< key_type const, mapped_type> value_type;   ///< Key-value pair to be stored in the map
        typedef Traits  traits;      ///< Map traits
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename traits::hash hasher; ///< Hash functor, see \p feldman_hashmap::traits::hash
#else
        typedef typename maker::hasher hasher;
#endif

        typedef typename maker::hash_type hash_type; ///< Hash type deduced from \p hasher return type
        typedef typename base_class::hash_comparator hash_comparator; ///< hash compare functor based on \p Traits::compare and \p Traits::less
        typedef typename traits::item_counter   item_counter;   ///< Item counter type
        typedef typename traits::allocator      allocator;      ///< Element allocator
        typedef typename traits::node_allocator node_allocator; ///< Array node allocator
        typedef typename traits::memory_model   memory_model;   ///< Memory model
        typedef typename traits::back_off       back_off;       ///< Back-off strategy
        typedef typename traits::stat           stat;           ///< Internal statistics type
        typedef typename traits::rcu_check_deadlock rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename gc::scoped_lock       rcu_lock;        ///< RCU scoped lock
        static constexpr const bool c_bExtractLockExternal = false; ///< Group of \p extract_xxx functions does not require external locking

        /// Level statistics
        typedef feldman_hashmap::level_statistics level_statistics;

    protected:
        //@cond
        typedef typename maker::node_type node_type;
        typedef typename maker::cxx_node_allocator cxx_node_allocator;
        typedef std::unique_ptr< node_type, typename maker::node_disposer > scoped_node_ptr;
        typedef typename base_class::check_deadlock_policy check_deadlock_policy;

        struct node_cast
        {
            value_type * operator()(node_type * p) const
            {
                return p ? &p->m_Value : nullptr;
            }
        };

    public:
        /// pointer to extracted node
        using exempt_ptr = cds::urcu::exempt_ptr< gc, node_type, value_type, typename base_class::disposer, node_cast >;

    protected:
        template <bool IsConst>
        class bidirectional_iterator: public base_class::iterator_base
        {
            friend class FeldmanHashMap;
            typedef typename base_class::iterator_base iterator_base;

        protected:
            static constexpr bool const c_bConstantIterator = IsConst;

        public:
            typedef typename std::conditional< IsConst, value_type const*, value_type*>::type value_ptr; ///< Value pointer
            typedef typename std::conditional< IsConst, value_type const&, value_type&>::type value_ref; ///< Value reference

        public:
            bidirectional_iterator() noexcept
            {}

            bidirectional_iterator( bidirectional_iterator const& rhs ) noexcept
                : iterator_base( rhs )
            {}

            bidirectional_iterator& operator=(bidirectional_iterator const& rhs) noexcept
            {
                iterator_base::operator=( rhs );
                return *this;
            }

            bidirectional_iterator& operator++()
            {
                iterator_base::operator++();
                return *this;
            }

            bidirectional_iterator& operator--()
            {
                iterator_base::operator--();
                return *this;
            }

            value_ptr operator ->() const noexcept
            {
                node_type * p = iterator_base::pointer();
                return p ? &p->m_Value : nullptr;
            }

            value_ref operator *() const noexcept
            {
                node_type * p = iterator_base::pointer();
                assert( p );
                return p->m_Value;
            }

            void release()
            {
                iterator_base::release();
            }

            template <bool IsConst2>
            bool operator ==(bidirectional_iterator<IsConst2> const& rhs) const noexcept
            {
                return iterator_base::operator==( rhs );
            }

            template <bool IsConst2>
            bool operator !=(bidirectional_iterator<IsConst2> const& rhs) const noexcept
            {
                return !( *this == rhs );
            }

        public: // for internal use only!
            bidirectional_iterator( base_class const& set, typename base_class::array_node * pNode, size_t idx, bool )
                : iterator_base( set, pNode, idx, false )
            {}

            bidirectional_iterator( base_class const& set, typename base_class::array_node * pNode, size_t idx )
                : iterator_base( set, pNode, idx )
            {}
        };

        /// Reverse bidirectional iterator
        template <bool IsConst>
        class reverse_bidirectional_iterator : public base_class::iterator_base
        {
            friend class FeldmanHashMap;
            typedef typename base_class::iterator_base iterator_base;

        public:
            typedef typename std::conditional< IsConst, value_type const*, value_type*>::type value_ptr; ///< Value pointer
            typedef typename std::conditional< IsConst, value_type const&, value_type&>::type value_ref; ///< Value reference

        public:
            reverse_bidirectional_iterator() noexcept
                : iterator_base()
            {}

            reverse_bidirectional_iterator( reverse_bidirectional_iterator const& rhs ) noexcept
                : iterator_base( rhs )
            {}

            reverse_bidirectional_iterator& operator=( reverse_bidirectional_iterator const& rhs) noexcept
            {
                iterator_base::operator=( rhs );
                return *this;
            }

            reverse_bidirectional_iterator& operator++()
            {
                iterator_base::operator--();
                return *this;
            }

            reverse_bidirectional_iterator& operator--()
            {
                iterator_base::operator++();
                return *this;
            }

            value_ptr operator ->() const noexcept
            {
                node_type * p = iterator_base::pointer();
                return p ? &p->m_Value : nullptr;
            }

            value_ref operator *() const noexcept
            {
                node_type * p = iterator_base::pointer();
                assert( p );
                return p->m_Value;
            }

            void release()
            {
                iterator_base::release();
            }

            template <bool IsConst2>
            bool operator ==(reverse_bidirectional_iterator<IsConst2> const& rhs) const
            {
                return iterator_base::operator==( rhs );
            }

            template <bool IsConst2>
            bool operator !=(reverse_bidirectional_iterator<IsConst2> const& rhs)
            {
                return !( *this == rhs );
            }

        public: // for internal use only!
            reverse_bidirectional_iterator( base_class const& set, typename base_class::array_node * pNode, size_t idx, bool )
                : iterator_base( set, pNode, idx, false )
            {}

            reverse_bidirectional_iterator( base_class const& set, typename base_class::array_node * pNode, size_t idx )
                : iterator_base( set, pNode, idx, false )
            {
                iterator_base::backward();
            }
        };
        //@endcond

    public:
#ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined iterator;            ///< @ref cds_container_FeldmanHashMap_rcu_iterators "bidirectional iterator" type
        typedef implementation_defined const_iterator;      ///< @ref cds_container_FeldmanHashMap_rcu_iterators "bidirectional const iterator" type
        typedef implementation_defined reverse_iterator;    ///< @ref cds_container_FeldmanHashMap_rcu_iterators "bidirectional reverse iterator" type
        typedef implementation_defined const_reverse_iterator; ///< @ref cds_container_FeldmanHashMap_rcu_iterators "bidirectional reverse const iterator" type
#else
        typedef bidirectional_iterator<false> iterator;
        typedef bidirectional_iterator<true>  const_iterator;
        typedef reverse_bidirectional_iterator<false> reverse_iterator;
        typedef reverse_bidirectional_iterator<true>  const_reverse_iterator;
#endif

    protected:
        //@cond
        hasher  m_Hasher;
        //@endcond

    public:
        /// Creates empty map
        /**
            @param head_bits - 2<sup>head_bits</sup> specifies the size of head array, minimum is 4.
            @param array_bits - 2<sup>array_bits</sup> specifies the size of array node, minimum is 2.

            Equation for \p head_bits and \p array_bits:
            \code
            sizeof(hash_type) * 8 == head_bits + N * array_bits
            \endcode
            where \p N is multi-level array depth.
        */
        FeldmanHashMap( size_t head_bits = 8, size_t array_bits = 4 )
            : base_class( head_bits, array_bits )
        {}

        /// Destructs the map and frees all data
        ~FeldmanHashMap()
        {}

        /// Inserts new element with key and default value
        /**
            The function creates an element with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from a value of type \p K.
                In trivial case, \p K is equal to \p key_type.
            - The \p mapped_type should be default-constructible.

            Returns \p true if inserting successful, \p false otherwise.

            The function locks RCU internally.
        */
        template <typename K>
        bool insert( K&& key )
        {
            scoped_node_ptr sp( cxx_node_allocator().MoveNew( m_Hasher, std::forward<K>(key)));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Inserts new element
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p value_type should be constructible from \p val of type \p V.

            Returns \p true if \p val is inserted into the map, \p false otherwise.

            The function locks RCU internally.
        */
        template <typename K, typename V>
        bool insert( K&& key, V&& val )
        {
            scoped_node_ptr sp( cxx_node_allocator().MoveNew( m_Hasher, std::forward<K>(key), std::forward<V>(val)));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Inserts new element and initialize it by a functor
        /**
            This function inserts new element with key \p key and if inserting is successful then it calls
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

            \p key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            The function locks RCU internally.
        */
        template <typename K, typename Func>
        bool insert_with( K&& key, Func func )
        {
            scoped_node_ptr sp( cxx_node_allocator().MoveNew( m_Hasher, std::forward<K>(key)));
            if ( base_class::insert( *sp, [&func]( node_type& item ) { func( item.m_Value ); } )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// For key \p key inserts data of type \p value_type created in-place from <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.

            The function locks RCU internally.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            scoped_node_ptr sp( cxx_node_allocator().MoveNew( m_Hasher, std::forward<K>(key), std::forward<Args>(args)... ));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Updates data by \p key
        /**
            The operation performs inserting or replacing the element with lock-free manner.

            If the \p key not found in the map, then the new item created from \p key
            will be inserted into the map iff \p bInsert is \p true
            (note that in this case the \ref key_type should be constructible from type \p K).
            Otherwise, if \p key is found, it is replaced with a new item created from
            \p key.
            The functor \p Func signature:
            \code
                struct my_functor {
                    void operator()( value_type& item, value_type * old );
                };
            \endcode
            where:
            - \p item - item of the map
            - \p old - old item of the map, if \p nullptr - the new item was inserted

            The functor may change any fields of the \p item.second.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if \p key already exists.

            The function locks RCU internally.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K&& key, Func func, bool bInsert = true )
        {
            scoped_node_ptr sp( cxx_node_allocator().MoveNew( m_Hasher, std::forward<K>(key)));
            std::pair<bool, bool> result = base_class::do_update( *sp,
                [&func]( node_type& node, node_type * old ) { func( node.m_Value, old ? &old->m_Value : nullptr );},
                bInsert );
            if ( result.first )
                sp.release();
            return result;
        }

        /// Delete \p key from the map
        /**
            \p key_type must be constructible from value of type \p K.
            The function deletes the element with hash value equal to <tt>hash( key_type( key ))</tt>

            Return \p true if \p key is found and deleted, \p false otherwise.

            RCU should not be locked. The function locks RCU internally.
        */
        template <typename K>
        bool erase( K const& key )
        {
            return base_class::erase(m_Hasher(key_type(key)));
        }

        /// Delete \p key from the map
        /**
            The function searches an item with hash value equal to <tt>hash( key_type( key ))</tt>,
            calls \p f functor and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode
            where \p item is the element found.

            \p key_type must be constructible from value of type \p K.

            Return \p true if key is found and deleted, \p false otherwise

            RCU should not be locked. The function locks RCU internally.
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return base_class::erase(m_Hasher(key_type(key)), [&f]( node_type& node) { f( node.m_Value ); });
        }

        /// Extracts the item from the map with specified \p key
        /**
            The function searches an item with key equal to <tt>hash( key_type( key ))</tt> in the map,
            unlinks it from the map, and returns a guarded pointer to the item found.
            If \p key is not found the function returns an empty guarded pointer.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
            Example:
            \code
            typedef cds::container::FeldmanHashMap< cds::urcu::gc< cds::urcu::general_buffered<>>, int, foo, my_traits > map_type;
            map_type theMap;
            // ...

            typename map_type::exempt_ptr ep( theMap.extract( 5 ));
            if ( ep ) {
                // Deal with ep
                //...

                // Dispose returned item.
                ep.release();
            }
            \endcode
        */
        template <typename K>
        exempt_ptr extract( K const& key )
        {
            check_deadlock_policy::check();

            node_type * p;
            {
                rcu_lock rcuLock;
                p = base_class::do_erase( m_Hasher( key_type(key)), [](node_type const&) -> bool {return true;});
            }
            return exempt_ptr(p);
        }

        /// Checks whether the map contains \p key
        /**
            The function searches the item by its hash that is equal to <tt>hash( key_type( key ))</tt>
            and returns \p true if it is found, or \p false otherwise.
        */
        template <typename K>
        bool contains( K const& key )
        {
            return base_class::contains( m_Hasher( key_type( key )));
        }

        /// Find the key \p key
        /**

            The function searches the item by its hash that is equal to <tt>hash( key_type( key ))</tt>
            and calls the functor \p f for item found.
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
            return base_class::find( m_Hasher( key_type( key )), [&f](node_type& node) { f( node.m_Value );});
        }

        /// Finds the key \p key and return the item found
        /**
            The function searches the item by its \p hash
            and returns the pointer to the item found.
            If \p hash is not found the function returns \p nullptr.

            RCU should be locked before the function invocation.
            Returned pointer is valid only while RCU is locked.

            Usage:
            \code
            typedef cds::container::FeldmanHashMap< your_template_params >  my_map;
            my_map theMap;
            // ...
            {
                // lock RCU
                my_map::rcu_lock;

                foo * p = theMap.get( 5 );
                if ( p ) {
                    // Deal with p
                    //...
                }
            }
            \endcode
        */
        template <typename K>
        value_type * get( K const& key )
        {
            node_type * p = base_class::get( m_Hasher( key_type( key )));
            return p ? &p->m_Value : nullptr;
        }

        /// Clears the map (non-atomic)
        /**
            The function unlink all data node from the map.
            The function is not atomic but is thread-safe.
            After \p %clear() the map may not be empty because another threads may insert items.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the map is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the map is empty.
            Thus, the correct item counting feature is an important part of the map implementation.
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

        /// Returns the size of head node
        size_t head_size() const
        {
            return base_class::head_size();
        }

        /// Returns the size of the array node
        size_t array_node_size() const
        {
            return base_class::array_node_size();
        }

        /// Collects tree level statistics into \p stat
        /**
            The function traverses the set and collects statistics for each level of the tree
            into \p feldman_hashset::level_statistics struct. The element of \p stat[i]
            represents statistics for level \p i, level 0 is head array.
            The function is thread-safe and may be called in multi-threaded environment.

            Result can be useful for estimating efficiency of hash functor you use.
        */
        void get_level_statistics(std::vector< feldman_hashmap::level_statistics>& stat) const
        {
            base_class::get_level_statistics(stat);
        }

    public:
    ///@name Thread-safe iterators
        /** @anchor cds_container_FeldmanHashMap_rcu_iterators
            The map supports thread-safe iterators: you may iterate over the map in multi-threaded environment
            under explicit RCU lock.
            RCU lock requirement means that inserting or searching is allowed but you must not erase the items from the map
            since erasing under RCU lock can lead to a deadlock. However, another thread can call \p erase() safely
            while your thread is iterating.

            A typical example is:
            \code
            struct foo {
                // ... other fields
                uint32_t    payload; // only for example
            };
            typedef cds::urcu::gc< cds::urcu::general_buffered<>> rcu;
            typedef cds::container::FeldmanHashMap< rcu, std::string, foo> map_type;

            map_type m;

            // ...

            // iterate over the map
            {
                // lock the RCU.
                typename set_type::rcu_lock l; // scoped RCU lock

                // traverse the map
                for ( auto i = m.begin(); i != s.end(); ++i ) {
                    // deal with i. Remember, erasing is prohibited here!
                    i->second.payload++;
                }
            } // at this point RCU lock is released
            \endcode

            Each iterator object supports the common interface:
            - dereference operators:
                @code
                value_type [const] * operator ->() noexcept
                value_type [const] & operator *() noexcept
                @endcode
            - pre-increment and pre-decrement. Post-operators is not supported
            - equality operators <tt>==</tt> and <tt>!=</tt>.
                Iterators are equal iff they point to the same cell of the same array node.
                Note that for two iterators \p it1 and \p it2 the condition <tt> it1 == it2 </tt>
                does not entail <tt> &(*it1) == &(*it2) </tt>: welcome to concurrent containers

            @note It is possible the item can be iterated more that once, for example, if an iterator points to the item
            in an array node that is being splitted.
        */
    ///@{
        /// Returns an iterator to the beginning of the map
        iterator begin()
        {
            return base_class::template init_begin<iterator>();
        }

        /// Returns an const iterator to the beginning of the map
        const_iterator begin() const
        {
            return base_class::template init_begin<const_iterator>();
        }

        /// Returns an const iterator to the beginning of the map
        const_iterator cbegin()
        {
            return base_class::template init_begin<const_iterator>();
        }

        /// Returns an iterator to the element following the last element of the map. This element acts as a placeholder; attempting to access it results in undefined behavior.
        iterator end()
        {
            return base_class::template init_end<iterator>();
        }

        /// Returns a const iterator to the element following the last element of the map. This element acts as a placeholder; attempting to access it results in undefined behavior.
        const_iterator end() const
        {
            return base_class::template init_end<const_iterator>();
        }

        /// Returns a const iterator to the element following the last element of the map. This element acts as a placeholder; attempting to access it results in undefined behavior.
        const_iterator cend()
        {
            return base_class::template init_end<const_iterator>();
        }

        /// Returns a reverse iterator to the first element of the reversed map
        reverse_iterator rbegin()
        {
            return base_class::template init_rbegin<reverse_iterator>();
        }

        /// Returns a const reverse iterator to the first element of the reversed map
        const_reverse_iterator rbegin() const
        {
            return base_class::template init_rbegin<const_reverse_iterator>();
        }

        /// Returns a const reverse iterator to the first element of the reversed map
        const_reverse_iterator crbegin()
        {
            return base_class::template init_rbegin<const_reverse_iterator>();
        }

        /// Returns a reverse iterator to the element following the last element of the reversed map
        /**
            It corresponds to the element preceding the first element of the non-reversed container.
            This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        reverse_iterator rend()
        {
            return base_class::template init_rend<reverse_iterator>();
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed map
        /**
            It corresponds to the element preceding the first element of the non-reversed container.
            This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        const_reverse_iterator rend() const
        {
            return base_class::template init_rend<const_reverse_iterator>();
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed map
        /**
            It corresponds to the element preceding the first element of the non-reversed container.
            This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        const_reverse_iterator crend()
        {
            return base_class::template init_rend<const_reverse_iterator>();
        }
    ///@}
    };
}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_FELDMAN_HASHMAP_RCU_H

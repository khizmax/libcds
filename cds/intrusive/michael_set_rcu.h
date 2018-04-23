// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_MICHAEL_SET_RCU_H
#define CDSLIB_INTRUSIVE_MICHAEL_SET_RCU_H

#include <cds/intrusive/details/michael_set_base.h>
#include <memory>

namespace cds { namespace intrusive {

    /// Michael's hash set, \ref cds_urcu_desc "RCU" specialization
    /** @ingroup cds_intrusive_map
        \anchor cds_intrusive_MichaelHashSet_rcu

        Source:
            - [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

        Michael's hash table algorithm is based on lock-free ordered list and it is very simple.
        The main structure is an array \p T of size \p M. Each element in \p T is basically a pointer
        to a hash bucket, implemented as a singly linked list. The array of buckets cannot be dynamically expanded.
        However, each bucket may contain unbounded number of items.

        Template parameters are:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p OrderedList - ordered list implementation used as bucket for hash set, for example, MichaelList, LazyList.
            The intrusive ordered list implementation specifies the type \p T stored in the hash-set, the reclamation
            schema \p GC used by hash-set, the comparison functor for the type \p T and other features specific for
            the ordered list.
        - \p Traits - type traits, default is \p michael_set::traits.
            Instead of defining \p Traits struct you can use option-based syntax with \p michael_set::make_traits metafunction.

        \par Usage
            Before including <tt><cds/intrusive/michael_set_rcu.h></tt> you should include appropriate RCU header file,
            see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.
            For example, for \ref cds_urcu_general_buffered_gc "general-purpose buffered RCU" you should include:
            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/intrusive/michael_list_rcu.h>
            #include <cds/intrusive/michael_set_rcu.h>

            struct Foo { ... };
            // Hash functor for struct Foo
            struct foo_hash {
                size_t operator()( Foo const& foo ) const { return ... }
            };

            // Now, you can declare Michael's list for type Foo and default traits:
            typedef cds::intrusive::MichaelList<cds::urcu::gc< general_buffered<> >, Foo > rcu_michael_list;

            // Declare Michael's set with MichaelList as bucket type
            typedef cds::intrusive::MichaelSet<
                cds::urcu::gc< general_buffered<> >,
                rcu_michael_list,
                cds::intrusive::michael_set::make_traits<
                    cds::opt::::hash< foo_hash >
                >::type
            > rcu_michael_set;

            // Declares hash set for 1000000 items with load factor 2
            rcu_michael_set theSet( 1000000, 2 );

            // Now you can use theSet object in many threads without any synchronization.
            \endcode
    */
    template <
        class RCU,
        class OrderedList,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = michael_set::traits
#else
        class Traits
#endif
    >
    class MichaelHashSet< cds::urcu::gc< RCU >, OrderedList, Traits >
    {
    public:
        typedef cds::urcu::gc< RCU > gc;            ///< RCU schema
        typedef OrderedList          ordered_list;  ///< type of ordered list used as a bucket implementation
        typedef Traits               traits;        ///< Set traits

        typedef typename ordered_list::value_type     value_type;       ///< type of value stored in the list
        typedef typename ordered_list::key_comparator key_comparator;   ///< key comparing functor
        typedef typename ordered_list::disposer       disposer;         ///< Node disposer functor
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename ordered_list::stat           stat;             ///< Internal statistics
#endif

        /// Hash functor for \ref value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;
        typedef typename traits::item_counter item_counter;   ///< Item counter type
        typedef typename traits::allocator    allocator;      ///< Bucket table allocator

        typedef typename ordered_list::rcu_lock    rcu_lock;   ///< RCU scoped lock
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

        typedef typename std::allocator_traits<allocator>::template rebind_alloc< internal_bucket_type > bucket_table_allocator;
        //@endcond

    public:
        typedef typename internal_bucket_type::exempt_ptr  exempt_ptr; ///< pointer to extracted node
        typedef typename internal_bucket_type::raw_ptr     raw_ptr;    ///< Return type of \p get() member function and its derivatives

        //@cond
        typedef typename bucket_stat::stat stat;
        //@endcond

    private:
        //@cond
        hash                    m_HashFunctor;   ///< Hash functor
        size_t const            m_nHashBitmask;
        internal_bucket_type*   m_Buckets;       ///< bucket table
        item_counter            m_ItemCounter;   ///< Item counter
        stat                    m_Stat;          ///< Internal statistics
        //@endcond

    public:
    ///@name Forward iterators (thread-safe under RCU lock)
    //@{
        /// Forward iterator
        /**
            The forward iterator for Michael's set is based on \p OrderedList forward iterator and has some features:
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
        typedef michael_set::details::iterator< internal_bucket_type, false >    iterator;

        /// Const forward iterator
        /**
            For iterator's features and requirements see \ref iterator
        */
        typedef michael_set::details::iterator< internal_bucket_type, true >     const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        /**
            For empty set \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( m_Buckets[0].begin(), m_Buckets, m_Buckets + bucket_count());
        }

        /// Returns an iterator that addresses the location succeeding the last element in a set
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the set.
            For empty set \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( m_Buckets[bucket_count() - 1].end(), m_Buckets + bucket_count() - 1, m_Buckets + bucket_count());
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator begin() const
        {
            return cbegin();
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator cbegin() const
        {
            return const_iterator( m_Buckets[0].cbegin(), m_Buckets, m_Buckets + bucket_count());
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a set
        const_iterator end() const
        {
            return cend();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a set
        const_iterator cend() const
        {
            return const_iterator( m_Buckets[bucket_count() - 1].cend(), m_Buckets + bucket_count() - 1, m_Buckets + bucket_count());
        }
    //@}

    public:
        /// Initialize hash set
        /**
            The Michael's hash set is an unbounded container, but its hash table is non-expandable.
            At construction time you should pass estimated maximum item count and a load factor.
            The load factor is average size of one bucket - a small number between 1 and 10.
            The bucket is an ordered single-linked list, the complexity of searching in the bucket is linear <tt>O(nLoadFactor)</tt>.
            The constructor defines hash table size as rounding <tt>nMaxItemCount / nLoadFactor</tt> up to nearest power of two.
        */
        MichaelHashSet(
            size_t nMaxItemCount,   ///< estimation of max item count in the hash set
            size_t nLoadFactor      ///< load factor: average size of the bucket
        ) : m_nHashBitmask( michael_set::details::init_hash_bitmask( nMaxItemCount, nLoadFactor ))
          , m_Buckets( bucket_table_allocator().allocate( bucket_count()))
        {
            for ( auto it = m_Buckets, itEnd = m_Buckets + bucket_count(); it != itEnd; ++it )
                construct_bucket<bucket_stat>( it );
        }

        /// Clear hash set and destroy it
        ~MichaelHashSet()
        {
            clear();

            for ( auto it = m_Buckets, itEnd = m_Buckets + bucket_count(); it != itEnd; ++it )
                it->~internal_bucket_type();
            bucket_table_allocator().deallocate( m_Buckets, bucket_count());
        }

        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            bool bRet = bucket( val ).insert( val );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted.
            The user-defined functor is called only if the inserting is success.

            @warning For \ref cds_intrusive_MichaelList_rcu "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_rcu "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            bool bRet = bucket( val ).insert( val, f );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Updates the element
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val not found in the set, then \p val is inserted iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
                struct functor {
                    void operator()( bool bNew, value_type& item, value_type& val );
                };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refers to the same thing.

            The functor may change non-key fields of the \p item.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the set.

            @warning For \ref cds_intrusive_MichaelList_hp "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_hp "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            std::pair<bool, bool> bRet = bucket( val ).update( val, func, bAllowInsert );
            if ( bRet.second )
                ++m_ItemCounter;
            return bRet;
        }
        //@cond
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Unlinks the item \p val from the set
        /**
            The function searches the item \p val in the set and unlink it from the set
            if it is found and is equal to \p val.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            bool bRet = bucket( val ).unlink( val );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_MichaelHashSet_rcu_erase
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns \p true.
            If the item with key equal to \p key is not found the function return \p false.

            Note the hash functor should accept a parameter of type \p Q that may be not the same as \p value_type.
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            if ( bucket( key ).erase( key )) {
                --m_ItemCounter;
                return true;
            }
            return false;
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelHashSet_rcu_erase "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            if ( bucket( key ).erase_with( key, pred )) {
                --m_ItemCounter;
                return true;
            }
            return false;
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_MichaelHashSet_rcu_erase_func
            The function searches an item with key equal to \p key in the set,
            call \p f functor with item found, and unlinks it from the set.
            The \ref disposer specified in \p OrderedList class template parameter is called
            by garbage collector \p GC asynchronously.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode

            If the item with key equal to \p key is not found the function return \p false.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q, typename Func>
        bool erase( const Q& key, Func f )
        {
            if ( bucket( key ).erase( key, f )) {
                --m_ItemCounter;
                return true;
            }
            return false;
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelHashSet_rcu_erase_func "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( const Q& key, Less pred, Func f )
        {
            if ( bucket( key ).erase_with( key, pred, f )) {
                --m_ItemCounter;
                return true;
            }
            return false;
        }

        /// Extracts an item from the set
        /** \anchor cds_intrusive_MichaelHashSet_rcu_extract
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item with the key equal to \p key is not found the function returns an empty \p exempt_ptr.

            Depends on \p ordered_list you should or should not lock RCU before calling of this function:
            - for the set based on \ref cds_intrusive_MichaelList_rcu "MichaelList" RCU should not be locked
            - for the set based on \ref cds_intrusive_LazyList_rcu "LazyList" RCU should be locked

            See ordered list implementation for details.

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/intrusive/michael_list_rcu.h>
            #include <cds/intrusive/michael_set_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::intrusive::MichaelList< rcu, Foo > rcu_michael_list;
            typedef cds::intrusive::MichaelHashSet< rcu, rcu_michael_list, foo_traits > rcu_michael_set;

            rcu_michael_set theSet;
            // ...

            typename rcu_michael_set::exempt_ptr p;

            // For MichaelList we should not lock RCU

            // Now, you can apply extract function
            // Note that you must not delete the item found inside the RCU lock
            p = theSet.extract( 10 )
            if ( p ) {
                // do something with p
                ...
            }

            // We may safely release p here
            // release() passes the pointer to RCU reclamation cycle:
            // it invokes RCU retire_ptr function with the disposer you provided for rcu_michael_list.
            p.release();
            \endcode
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            exempt_ptr p( bucket( key ).extract( key ));
            if ( p )
                --m_ItemCounter;
            return p;
        }

        /// Extracts an item from the set using \p pred predicate for searching
        /**
            The function is an analog of \p extract(Q const&) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            exempt_ptr p( bucket( key ).extract_with( key, pred ));
            if ( p )
                --m_ItemCounter;
            return p;
        }

        /// Checks whether the set contains \p key
        /**

            The function searches the item with key equal to \p key
            and returns \p true if the key is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        bool contains( Q const& key )
        {
            return bucket( key ).contains( key );
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            return bucket( key ).contains( key, pred );
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Find the key \p key
        /** \anchor cds_intrusive_MichaelHashSet_rcu_find_func
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor can change non-key fields of \p item.
            The functor does not serialize simultaneous access to the set \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p key argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return bucket( key ).find( key, f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return bucket( key ).find( key, f );
        }
        //@endcond

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelHashSet_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            return bucket( key ).find_with( key, pred, f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            return bucket( key ).find_with( key, pred, f );
        }
        //@endcond

        /// Finds the key \p key and return the item found
        /** \anchor cds_intrusive_MichaelHashSet_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.
            Note the type of returned value depends on underlying \p ordered_list.
            For details, see documentation of ordered list you use.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::intrusive::MichaelHashSet< your_template_parameters > hash_set;
            hash_set theSet;
            // ...
            // Result of get() call
            typename hash_set::raw_ptr ptr;
            {
                // Lock RCU
                hash_set::rcu_lock lock;

                ptr = theSet.get( 5 );
                if ( ptr ) {
                    // Deal with ptr
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // ptr can be reclaimed by disposer at any time after RCU has been unlocked
            }
            \endcode
        */
        template <typename Q>
        raw_ptr get( Q const& key )
        {
            return bucket( key ).get( key );
        }

        /// Finds the key \p key and return the item found
        /**
            The function is an analog of \ref cds_intrusive_MichaelHashSet_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        raw_ptr get_with( Q const& key, Less pred )
        {
            return bucket( key ).get_with( key, pred );
        }

        /// Clears the set (non-atomic)
        /**
            The function unlink all items from the set.
            The function is not atomic. It cleans up each bucket and then resets the item counter to zero.
            If there are a thread that performs insertion while \p clear is working the result is undefined in general case:
            <tt> empty() </tt> may return \p true but the set may contain item(s).
            Therefore, \p clear may be used only for debugging purposes.

            For each item the \p disposer is called after unlinking.
        */
        void clear()
        {
            for ( size_t i = 0; i < bucket_count(); ++i )
                m_Buckets[i].clear();
            m_ItemCounter.reset();
        }


        /// Checks if the set is empty
        /**
            @warning If you use \p atomicity::empty_item_counter in \p traits::item_counter,
            the function always returns \p true.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Returns item count in the set
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
            Since %MichaelHashSet cannot dynamically extend the hash table size,
            the value returned is an constant depending on object initialization parameters;
            see \ref cds_intrusive_MichaelHashSet_hp "MichaelHashSet" for explanation.
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

    private:
        //@cond
        template <typename Stat>
        typename std::enable_if< Stat::empty >::type construct_bucket( internal_bucket_type * bkt )
        {
            new (bkt) internal_bucket_type;
        }

        template <typename Stat>
        typename std::enable_if< !Stat::empty >::type construct_bucket( internal_bucket_type * bkt )
        {
            new (bkt) internal_bucket_type( m_Stat );
        }

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
        template <typename Q>
        internal_bucket_type const& bucket( Q const& key ) const
        {
            return m_Buckets[hash_value( key )];
        }
        //@endcond
    };

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_MICHAEL_SET_NOGC_H

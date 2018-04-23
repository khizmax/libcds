// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_MICHAEL_SET_RCU_H
#define CDSLIB_CONTAINER_MICHAEL_SET_RCU_H

#include <cds/container/details/michael_set_base.h>
#include <cds/details/allocator.h>

namespace cds { namespace container {

    /// Michael's hash set (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_nonintrusive_set
        \anchor cds_nonintrusive_MichaelHashSet_rcu

        Source:
            - [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

        Michael's hash table algorithm is based on lock-free ordered list and it is very simple.
        The main structure is an array \p T of size \p M. Each element in \p T is basically a pointer
        to a hash bucket, implemented as a singly linked list. The array of buckets cannot be dynamically expanded.
        However, each bucket may contain unbounded number of items.

        Template parameters are:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p OrderedList - ordered list implementation used as the bucket for hash set, for example,
            \ref cds_nonintrusive_MichaelList_rcu "MichaelList".
            The ordered list implementation specifies the type \p T stored in the hash-set,
            the comparison functor for the type \p T and other features specific for
            the ordered list.
        - \p Traits - set traits, default is michael_set::traits.
            Instead of defining \p Traits struct you may use option-based syntax with michael_set::make_traits metafunction.

        About hash functor see \ref cds_nonintrusive_MichaelHashSet_hash_functor "MichaelSet hash functor".

        <b>How to use</b>

        Suppose, we have the following type \p Foo that we want to store in your \p %MichaelHashSet:
        \code
        struct Foo {
            int     nKey    ;   // key field
            int     nVal    ;   // value field
        };
        \endcode

        To use \p %MichaelHashSet for \p Foo values, you should first choose suitable ordered list class
        that will be used as a bucket for the set. We will cds::urcu::general_buffered<> RCU type and
        MichaelList as a bucket type.
        You should include RCU-related header file (<tt>cds/urcu/general_buffered.h</tt> in this example)
        before including <tt>cds/container/michael_set_rcu.h</tt>.
        Also, for ordered list we should develop a comparator for our \p Foo struct.
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/container/michael_list_rcu.h>
        #include <cds/container/michael_set_rcu.h>

        namespace cc = cds::container;

        // Foo comparator
        struct Foo_cmp {
            int operator ()(Foo const& v1, Foo const& v2 ) const
            {
                if ( std::less( v1.nKey, v2.nKey ))
                    return -1;
                return std::less(v2.nKey, v1.nKey) ? 1 : 0;
            }
        };

        // Ordered list
        typedef cc::MichaelList< cds::urcu::gc< cds::urcu::general_buffered<> >, Foo,
            typename cc::michael_list::make_traits<
                cc::opt::compare< Foo_cmp >     // item comparator option
            >::type
        > bucket_list;

        // Hash functor for Foo
        struct foo_hash {
            size_t operator ()( int i ) const
            {
                return std::hash( i );
            }
            size_t operator()( Foo const& i ) const
            {
                return std::hash( i.nKey );
            }
        };

        // Declare the set
        // Note that \p RCU template parameter of ordered list must be equal \p RCU for the set.
        typedef cc::MichaelHashSet< cds::urcu::gc< cds::urcu::general_buffered<> >, bucket_list,
            cc::michael_set::make_traits<
                cc::opt::hash< foo_hash >
            >::type
        > foo_set;

        foo_set fooSet;
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
        typedef cds::urcu::gc< RCU > gc; ///< RCU used as garbage collector
        typedef OrderedList ordered_list; ///< type of ordered list to be used as a bucket implementation
        typedef Traits      traits;      ///< Set traits

        typedef typename ordered_list::value_type     value_type;     ///< type of value to be stored in the list
        typedef typename ordered_list::key_comparator key_comparator; ///< key comparing functor
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename ordered_list::stat       stat;       ///< Internal statistics
        typedef typename ordered_list::exempt_ptr exempt_ptr; ///< pointer to extracted node
        typedef typename ordered_list::raw_ptr    raw_ptr;    ///< Return type of \p get() member function and its derivatives
#endif

        /// Hash functor for \ref value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;
        typedef typename traits::item_counter item_counter; ///< Item counter type
        typedef typename traits::allocator    allocator;    ///< Bucket table allocator

        typedef typename ordered_list::rcu_lock   rcu_lock;   ///< RCU scoped lock
        /// Group of \p extract_xxx functions require external locking if underlying ordered list requires that
        static constexpr const bool c_bExtractLockExternal = ordered_list::c_bExtractLockExternal;

        // GC and OrderedList::gc must be the same
        static_assert(std::is_same<gc, typename ordered_list::gc>::value, "GC and OrderedList::gc must be the same");

        //@cond
        typedef typename ordered_list::template select_stat_wrapper< typename ordered_list::stat > bucket_stat;

        typedef typename ordered_list::template rebind_traits<
            cds::opt::item_counter< cds::atomicity::empty_item_counter >
            , cds::opt::stat< typename bucket_stat::wrapped_stat >
        >::type internal_bucket_type_;

        class internal_bucket_type: public internal_bucket_type_
        {
            typedef internal_bucket_type_ base_class;
        public:
            using base_class::base_class;
            using typename base_class::node_type;
            using base_class::alloc_node;
            using base_class::insert_node;
            using base_class::node_to_value;
        };

        typedef typename internal_bucket_type::exempt_ptr exempt_ptr;
        typedef typename internal_bucket_type::raw_ptr    raw_ptr;
        typedef typename bucket_stat::stat stat;
        //@endcond

    protected:
        //@cond
        /// Bucket table allocator
        typedef typename std::allocator_traits< allocator >::template rebind_alloc< internal_bucket_type > bucket_table_allocator;

        const size_t            m_nHashBitmask;
        hash                    m_HashFunctor; ///< Hash functor
        internal_bucket_type*   m_Buckets;     ///< bucket table
        item_counter            m_ItemCounter; ///< Item counter
        stat                    m_Stat;        ///< Internal statistics
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
            return get_const_begin();
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator cbegin() const
        {
            return get_const_begin();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a set
        const_iterator end() const
        {
            return get_const_end();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a set
        const_iterator cend() const
        {
            return get_const_end();
        }
    //@}

    public:
        /// Initialize hash set
        /**
            The Michael's hash set is non-expandable container. You should point the average count of items \p nMaxItemCount
            when you create an object.
            \p nLoadFactor parameter defines average count of items per bucket and it should be small number between 1 and 10.
            Remember, since the bucket implementation is an ordered list, searching in the bucket is linear [<tt>O(nLoadFactor)</tt>].

            The ctor defines hash table size as rounding <tt>nMaxItemCount / nLoadFactor</tt> up to nearest power of two.
        */
        MichaelHashSet(
            size_t nMaxItemCount,   ///< estimation of max item count in the hash set
            size_t nLoadFactor      ///< load factor: estimation of max number of items in the bucket
        ) : m_nHashBitmask( michael_set::details::init_hash_bitmask( nMaxItemCount, nLoadFactor ))
          , m_Buckets( bucket_table_allocator().allocate( bucket_count()))
        {
            for ( auto it = m_Buckets, itEnd = m_Buckets + bucket_count(); it != itEnd; ++it )
                construct_bucket<bucket_stat>( it );
        }

        /// Clears hash set and destroys it
        ~MichaelHashSet()
        {
            clear();

            for ( auto it = m_Buckets, itEnd = m_Buckets + bucket_count(); it != itEnd; ++it )
                it->~internal_bucket_type();
            bucket_table_allocator().deallocate( m_Buckets, bucket_count());

        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the set.

            The type \p Q should contain as minimum the complete key for the node.
            The object of \ref value_type should be constructible from a value of type \p Q.
            In trivial case, \p Q is equal to \ref value_type.

            The function applies RCU lock internally.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q&& val )
        {
            const bool bRet = bucket( val ).insert( std::forward<Q>( val ));
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Inserts new node
        /**
            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls  \p f functor to initialize value-fields of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted.
            The user-defined functor is called only if the inserting is success.

            The function applies RCU lock internally.

            @warning For \ref cds_nonintrusive_MichaelList_rcu "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyList_rcu "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
            */
        template <typename Q, typename Func>
        bool insert( Q&& val, Func f )
        {
            const bool bRet = bucket( val ).insert( std::forward<Q>( val ), f );
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
                    void operator()( bool bNew, value_type& item, Q const& val );
                };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %update() function

            The functor may change non-key fields of the \p item.

            The function applies RCU lock internally.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the set.

            @warning For \ref cds_intrusive_MichaelList_hp "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_hp "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> update( Q const& val, Func func, bool bAllowInsert = true )
        {
            std::pair<bool, bool> bRet = bucket( val ).update( val, func, bAllowInsert );
            if ( bRet.second )
                ++m_ItemCounter;
            return bRet;
        }//@cond
        template <typename Q, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( const Q& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Inserts data of type \p value_type created from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            typename internal_bucket_type::node_type * pNode = internal_bucket_type::alloc_node( std::forward<Args>( args )... );
            bool bRet = bucket( internal_bucket_type::node_to_value( *pNode )).insert_node( pNode );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Deletes \p key from the set
        /** \anchor cds_nonintrusive_MichealSet_rcu_erase_val

            Since the key of MichaelHashSet's item type \p value_type is not explicitly specified,
            template parameter \p Q defines the key type searching in the list.
            The set item comparator should be able to compare the type \p value_type
            and the type \p Q.

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            const bool bRet = bucket( key ).erase( key );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichealSet_rcu_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            const bool bRet = bucket( key ).erase_with( key, pred );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes \p key from the set
        /** \anchor cds_nonintrusive_MichealSet_rcu_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type const& val);
            };
            \endcode

            Since the key of %MichaelHashSet's \p value_type is not explicitly specified,
            template parameter \p Q defines the key type searching in the list.
            The list item comparator should be able to compare the type \p T of list item
            and the type \p Q.

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            const bool bRet = bucket( key ).erase( key, f );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichealSet_rcu_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            const bool bRet = bucket( key ).erase_with( key, pred, f );
            if ( bRet )
                --m_ItemCounter;
            return bRet;
        }

        /// Extracts an item from the set
        /** \anchor cds_nonintrusive_MichaelHashSet_rcu_extract
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item with the key equal to \p key is not found the function return an empty \p exempt_ptr.

            The function just excludes the item from the set and returns a pointer to item found.
            Depends on \p ordered_list you should or should not lock RCU before calling of this function:
            - for the set based on \ref cds_nonintrusive_MichaelList_rcu "MichaelList" RCU should not be locked
            - for the set based on \ref cds_nonintrusive_LazyList_rcu "LazyList" RCU should be locked
            See ordered list implementation for details.

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/container/michael_list_rcu.h>
            #include <cds/container/michael_set_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::container::MichaelList< rcu, Foo > rcu_michael_list;
            typedef cds::container::MichaelHashSet< rcu, rcu_michael_list, foo_traits > rcu_michael_set;

            rcu_michael_set theSet;
            // ...

            typename rcu_michael_set::exempt_ptr p;

            // For MichaelList we should not lock RCU

            // Note that you must not delete the item found inside the RCU lock
            p = theSet.extract( 10 );
            if ( p ) {
                // do something with p
                ...
            }

            // We may safely release p here
            // release() passes the pointer to RCU reclamation cycle
            p.release();
            \endcode
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            exempt_ptr p = bucket( key ).extract( key );
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
            exempt_ptr p = bucket( key ).extract_with( key, pred );
            if ( p )
                --m_ItemCounter;
            return p;
        }

        /// Finds the key \p key
        /** \anchor cds_nonintrusive_MichealSet_rcu_find_func

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p key argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that may be not the same as \p value_type.

            The function applies RCU lock internally.

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
            The function is an analog of \ref cds_nonintrusive_MichealSet_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
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

        /// Finds the key \p key and return the item found
        /** \anchor cds_nonintrusive_MichaelHashSet_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.
            Note the type of returned value depends on underlying \p ordered_list.
            For details, see documentation of ordered list you use.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::container::MichaelHashSet< your_template_parameters > hash_set;
            hash_set theSet;
            typename hash_set::raw_ptr gp;
            // ...
            {
                // Lock RCU
                hash_set::rcu_lock lock;

                gp = theSet.get( 5 );
                if ( gp ) {
                    // Deal with pVal
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // gp can be reclaimed at any time after RCU has been unlocked
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
            The function is an analog of \ref cds_nonintrusive_MichaelHashSet_rcu_get "get(Q const&)"
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

        /// Clears the set (not atomic)
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
            Since \p %MichaelHashSet cannot dynamically extend the hash table size,
            the value returned is an constant depending on object initialization parameters;
            see MichaelHashSet::MichaelHashSet for explanation.
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
        internal_bucket_type& bucket( Q const& key )
        {
            return m_Buckets[hash_value( key )];
        }
        template <typename Q>
        internal_bucket_type const& bucket( Q const& key ) const
        {
            return m_Buckets[hash_value( key )];
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

        const_iterator get_const_begin() const
        {
            return const_iterator( const_cast<internal_bucket_type const&>(m_Buckets[0]).begin(), m_Buckets, m_Buckets + bucket_count());
        }
        const_iterator get_const_end() const
        {
            return const_iterator( const_cast<internal_bucket_type const&>(m_Buckets[bucket_count() - 1]).end(), m_Buckets + bucket_count() - 1, m_Buckets + bucket_count());
        }
        //@endcond
    };

}} // namespace cds::container

#endif // ifndef CDSLIB_CONTAINER_MICHAEL_SET_RCU_H

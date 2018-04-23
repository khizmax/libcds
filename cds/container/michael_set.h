// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_MICHAEL_SET_H
#define CDSLIB_CONTAINER_MICHAEL_SET_H

#include <cds/container/details/michael_set_base.h>
#include <cds/container/details/iterable_list_base.h>
#include <cds/details/allocator.h>

namespace cds { namespace container {

    /// Michael's hash set
    /** @ingroup cds_nonintrusive_set
        \anchor cds_nonintrusive_MichaelHashSet_hp

        Source:
            - [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

        Michael's hash table algorithm is based on lock-free ordered list and it is very simple.
        The main structure is an array \p T of size \p M. Each element in \p T is basically a pointer
        to a hash bucket, implemented as a singly linked list. The array of buckets cannot be dynamically expanded.
        However, each bucket may contain unbounded number of items.

        Template parameters are:
        - \p GC - Garbage collector used. You may use any \ref cds_garbage_collector "Garbage collector"
            from the \p libcds library.
            Note the \p GC must be the same as the \p GC used for \p OrderedList
        - \p OrderedList - ordered list implementation used as bucket for hash set, possible implementations:
            \p MichaelList, \p LazyList, \p IterableList.
            The ordered list implementation specifies the type \p T to be stored in the hash-set,
            the comparing functor for the type \p T and other features specific for the ordered list.
        - \p Traits - set traits, default is \p michael_set::traits.
            Instead of defining \p Traits struct you may use option-based syntax with \p michael_set::make_traits metafunction.

        There are the specializations:
        - for \ref cds_urcu_desc "RCU" - declared in <tt>cd/container/michael_set_rcu.h</tt>,
            see \ref cds_nonintrusive_MichaelHashSet_rcu "MichaelHashSet<RCU>".
        - for \ref cds::gc::nogc declared in <tt>cds/container/michael_set_nogc.h</tt>,
            see \ref cds_nonintrusive_MichaelHashSet_nogc "MichaelHashSet<gc::nogc>".

        \anchor cds_nonintrusive_MichaelHashSet_hash_functor
        <b>Hash functor</b>

        Some member functions of Michael's hash set accept the key parameter of type \p Q which differs from node type \p value_type.
        It is expected that type \p Q contains full key of node type \p value_type, and if keys of type \p Q and \p value_type
        are equal the hash values of these keys must be equal too.

        The hash functor \p Traits::hash should accept parameters of both type:
        \code
        // Our node type
        struct Foo {
            std::string     key_;   // key field
            // ... other fields
        };

        // Hash functor
        struct fooHash {
            size_t operator()( const std::string& s ) const
            {
                return std::hash( s );
            }

            size_t operator()( const Foo& f ) const
            {
                return (*this)( f.key_ );
            }
        };
        \endcode

        <b>How to use</b>

        Suppose, we have the following type \p Foo that we want to store in our \p %MichaelHashSet:
        \code
        struct Foo {
            int     nKey;   // key field
            int     nVal;   // value field
        };
        \endcode

        To use \p %MichaelHashSet for \p Foo values, you should first choose suitable ordered list class
        that will be used as a bucket for the set. We will use \p gc::DHP reclamation schema and
        \p MichaelList as a bucket type. Also, for ordered list we should develop a comparator for our \p Foo
        struct.
        \code
        #include <cds/container/michael_list_dhp.h>
        #include <cds/container/michael_set.h>

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

        // Our ordered list
        typedef cc::MichaelList< cds::gc::DHP, Foo,
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

        // Declare set type.
        // Note that \p GC template parameter of ordered list must be equal \p GC for the set.
        typedef cc::MichaelHashSet< cds::gc::DHP, bucket_list,
            cc::michael_set::make_traits<
                cc::opt::hash< foo_hash >
            >::type
        > foo_set;

        // Set variable
        foo_set fooSet;
        \endcode
    */
    template <
        class GC,
        class OrderedList,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = michael_set::traits
#else
        class Traits
#endif
    >
    class MichaelHashSet
    {
    public:
        typedef GC          gc;           ///< Garbage collector
        typedef OrderedList ordered_list; ///< type of ordered list used as a bucket implementation
        typedef Traits      traits;       ///< Set traits

        typedef typename ordered_list::value_type     value_type;     ///< type of value to be stored in the list
        typedef typename ordered_list::key_comparator key_comparator; ///< key comparison functor
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename ordered_list::stat           stat;           ///< Internal statistics
#endif

        /// Hash functor for \ref value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;
        typedef typename traits::item_counter item_counter; ///< Item counter type
        typedef typename traits::allocator    allocator;    ///< Bucket table allocator

        static constexpr const size_t c_nHazardPtrCount = ordered_list::c_nHazardPtrCount; ///< Count of hazard pointer required

        // GC and OrderedList::gc must be the same
        static_assert( std::is_same<gc, typename ordered_list::gc>::value, "GC and OrderedList::gc must be the same");

        //@cond
        typedef typename ordered_list::template select_stat_wrapper< typename ordered_list::stat > bucket_stat;

        typedef typename ordered_list::template rebind_traits<
            cds::opt::item_counter< cds::atomicity::empty_item_counter >
            , cds::opt::stat< typename bucket_stat::wrapped_stat >
        >::type internal_bucket_type;

        /// Bucket table allocator
        typedef typename std::allocator_traits<allocator>::template rebind_alloc< internal_bucket_type > bucket_table_allocator;

        typedef typename bucket_stat::stat stat;
        //@endcond

        /// Guarded pointer - a result of \p get() and \p extract() functions
        typedef typename internal_bucket_type::guarded_ptr guarded_ptr;

    protected:
        //@cond
        size_t const           m_nHashBitmask;
        internal_bucket_type * m_Buckets;     ///< bucket table
        hash                   m_HashFunctor; ///< Hash functor
        item_counter           m_ItemCounter; ///< Item counter
        stat                   m_Stat;        ///< Internal statistics
        //@endcond

    public:
    ///@name Forward iterators
    //@{
        /// Forward iterator
        /**
            The forward iterator for Michael's set has some features:
            - it has no post-increment operator
            - to protect the value, the iterator contains a GC-specific guard + another guard is required locally for increment operator.
              For some GC (like as \p gc::HP), a guard is a limited resource per thread, so an exception (or assertion) "no free guard"
              may be thrown if the limit of guard count per thread is exceeded.
            - The iterator cannot be moved across thread boundary because it contains thread-private GC's guard.

            Iterator thread safety depends on type of \p OrderedList:
            - for \p MichaelList and \p LazyList: iterator guarantees safety even if you delete the item that iterator points to
              because that item is guarded by hazard pointer.
              However, in case of concurrent deleting operations it is no guarantee that you iterate all item in the set.
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
        */

        /// Forward iterator
        typedef michael_set::details::iterator< internal_bucket_type, false > iterator;

        /// Const forward iterator
        typedef michael_set::details::iterator< internal_bucket_type, true > const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        /**
            For empty set \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( bucket_begin()->begin(), bucket_begin(), bucket_end());
        }

        /// Returns an iterator that addresses the location succeeding the last element in a set
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the set.
            For empty set \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( bucket_end()[-1].end(), bucket_end() - 1, bucket_end());
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

            @warning For \ref cds_nonintrusive_MichaelList_gc "MichaelList" and \ref cds_nonintrusive_IterableList_gc "IterableList"
            as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            @ref cds_nonintrusive_LazyList_gc "LazyList" provides exclusive access to inserted item and does not require any node-level
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

            The functor \p func signature depends of \p OrderedList:

            <b>for \p MichaelList, \p LazyList</b>
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

            <b>for \p IterableList</b>
            \code
                void func( value_type& val, value_type * old );
            \endcode
            where
            - \p val - a new data constructed from \p key
            - \p old - old value that will be retired. If new item has been inserted then \p old is \p nullptr.

            @return <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the set.

            @warning For \ref cds_intrusive_MichaelList_hp "MichaelList" and \ref cds_nonintrusive_IterableList_gc "IterableList"
            as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_hp "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> update( Q&& val, Func func, bool bAllowUpdate = true )
        {
            std::pair<bool, bool> bRet = bucket( val ).update( std::forward<Q>( val ), func, bAllowUpdate );
            if ( bRet.second )
                ++m_ItemCounter;
            return bRet;
        }
        //@cond
        template <typename Q, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( const Q& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Inserts or updates the node (only for \p IterableList)
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the set, then \p val is inserted iff \p bAllowInsert is \p true.
            Otherwise, the current element is changed to \p val, the old element will be retired later.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if \p val has been added or \p false if the item with that key
            already in the set.
        */
        template <typename Q>
#ifdef CDS_DOXYGEN_INVOKED
        std::pair<bool, bool>
#else
        typename std::enable_if<
            std::is_same< Q, Q>::value && is_iterable_list< ordered_list >::value,
            std::pair<bool, bool>
        >::type
#endif
        upsert( Q&& val, bool bAllowInsert = true )
        {
            std::pair<bool, bool> bRet = bucket( val ).upsert( std::forward<Q>( val ), bAllowInsert );
            if ( bRet.second )
                ++m_ItemCounter;
            return bRet;
        }

        /// Inserts data of type \p value_type constructed from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            bool bRet = bucket_emplace<internal_bucket_type>( std::forward<Args>(args)... );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Deletes \p key from the set
        /**
            Since the key of MichaelHashSet's item type \p value_type is not explicitly specified,
            template parameter \p Q defines the key type searching in the list.
            The set item comparator should be able to compare the type \p value_type
            and the type \p Q.

            Return \p true if key is found and deleted, \p false otherwise.
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
            The function is an analog of \p erase(Q const&) but \p pred is used for key comparing.
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
        /**
            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& item);
            };
            \endcode
            where \p item - the item found.

            Since the key of %MichaelHashSet's \p value_type is not explicitly specified,
            template parameter \p Q defines the key type searching in the list.
            The list item comparator should be able to compare the type \p T of list item
            and the type \p Q.

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
            The function is an analog of \p erase(Q const&, Func) but \p pred is used for key comparing.
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

        /// Deletes the item pointed by iterator \p iter (only for \p IterableList based set)
        /**
            Returns \p true if the operation is successful, \p false otherwise.
            The function can return \p false if the node the iterator points to has already been deleted
            by other thread.

            The function does not invalidate the iterator, it remains valid and can be used for further traversing.

            @note \p %erase_at() is supported only for \p %MichaelHashSet based on \p IterableList.
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
        /** \anchor cds_nonintrusive_MichaelHashSet_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the set, and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guadd pointer.

            Note the compare functor should accept a parameter of type \p Q that may be not the same as \p value_type.

            The extracted item is freed automatically when returned \p guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::MichaelHashSet< your_template_args > michael_set;
            michael_set theSet;
            // ...
            {
                typename michael_set::guarded_ptr gp( theSet.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        template <typename Q>
        guarded_ptr extract( Q const& key )
        {
            guarded_ptr gp( bucket( key ).extract( key ));
            if ( gp )
                --m_ItemCounter;
            return gp;
        }

        /// Extracts the item using compare functor \p pred
        /**
            The function is an analog of \p extract(Q const&)
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments
            of type \p value_type and \p Q in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            guarded_ptr gp( bucket( key ).extract_with( key, pred ));
            if ( gp )
                --m_ItemCounter;
            return gp;
        }

        /// Finds the key \p key
        /**
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

        /// Finds \p key and returns iterator pointed to the item found (only for \p IterableList)
        /**
            If \p key is not found the function returns \p end().

            @note This function is supported only for the set based on \p IterableList
        */
        template <typename Q>
#ifdef CDS_DOXYGEN_INVOKED
        iterator
#else
        typename std::enable_if< std::is_same<Q,Q>::value && is_iterable_list< ordered_list >::value, iterator >::type
#endif
        find( Q& key )
        {
            internal_bucket_type& b = bucket( key );
            typename internal_bucket_type::iterator it = b.find( key );
            if ( it == b.end())
                return end();
            return iterator( it, &b, bucket_end());
        }
        //@cond
        template <typename Q>
        typename std::enable_if< std::is_same<Q, Q>::value && is_iterable_list< ordered_list >::value, iterator >::type
        find( Q const& key )
        {
            internal_bucket_type& b = bucket( key );
            typename internal_bucket_type::iterator it = b.find( key );
            if ( it == b.end())
                return end();
            return iterator( it, &b, bucket_end());
        }
        //@endcond

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \p find(Q&, Func) but \p pred is used for key comparing.
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

        /// Finds \p key using \p pred predicate and returns iterator pointed to the item found (only for \p IterableList)
        /**
            The function is an analog of \p find(Q&) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.

            If \p key is not found the function returns \p end().

            @note This function is supported only for the set based on \p IterableList
        */
        template <typename Q, typename Less>
#ifdef CDS_DOXYGEN_INVOKED
        iterator
#else
        typename std::enable_if< std::is_same<Q, Q>::value && is_iterable_list< ordered_list >::value, iterator >::type
#endif
        find_with( Q& key, Less pred )
        {
            internal_bucket_type& b = bucket( key );
            typename internal_bucket_type::iterator it = b.find_with( key, pred );
            if ( it == b.end())
                return end();
            return iterator( it, &b, bucket_end());
        }
        //@cond
        template <typename Q, typename Less>
        typename std::enable_if< std::is_same<Q, Q>::value && is_iterable_list< ordered_list >::value, iterator >::type
        find_with( Q const& key, Less pred )
        {
            internal_bucket_type& b = bucket( key );
            typename internal_bucket_type::iterator it = b.find_with( key, pred );
            if ( it == b.end())
                return end();
            return iterator( it, &b, bucket_end());
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

        /// Finds the key \p key and return the item found
        /** \anchor cds_nonintrusive_MichaelHashSet_hp_get
            The function searches the item with key equal to \p key
            and returns the guarded pointer to the item found.
            If \p key is not found the functin returns an empty guarded pointer.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::MichaeHashSet< your_template_params >  michael_set;
            michael_set theSet;
            // ...
            {
                typename michael_set::guarded_ptr gp( theSet.get( 5 ));
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode

            Note the compare functor specified for \p OrderedList template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        guarded_ptr get( Q const& key )
        {
            return bucket( key ).get( key );
        }

        /// Finds the key \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelHashSet_hp_get "get( Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred )
        {
            return bucket( key ).get_with( key, pred );
        }

        /// Clears the set (non-atomic)
        /**
            The function erases all items from the set.

            The function is not atomic. It cleans up each bucket and then resets the item counter to zero.
            If there are a thread that performs insertion while \p clear is working the result is undefined in general case:
            <tt> empty() </tt> may return \p true but the set may contain item(s).
            Therefore, \p clear may be used only for debugging purposes.
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
            Since MichaelHashSet cannot dynamically extend the hash table size,
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
            return m_Buckets[ hash_value( key ) ];
        }
        template <typename Q>
        internal_bucket_type const& bucket( Q const& key ) const
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
            return const_iterator(( bucket_end() -1 )->cend(), bucket_end() - 1, bucket_end());
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

        template <typename List, typename... Args>
        typename std::enable_if< !is_iterable_list<List>::value, bool>::type
        bucket_emplace( Args&&... args )
        {
            class list_accessor: public List
            {
            public:
                using List::alloc_node;
                using List::node_to_value;
                using List::insert_node;
            };

            auto pNode = list_accessor::alloc_node( std::forward<Args>( args )... );
            assert( pNode != nullptr );
            return static_cast<list_accessor&>( bucket( list_accessor::node_to_value( *pNode ))).insert_node( pNode );
        }

        template <typename List, typename... Args>
        typename std::enable_if< is_iterable_list<List>::value, bool>::type
        bucket_emplace( Args&&... args )
        {
            class list_accessor: public List
            {
            public:
                using List::alloc_data;
                using List::insert_node;
            };

            auto pData = list_accessor::alloc_data( std::forward<Args>( args )... );
            assert( pData != nullptr );
            return static_cast<list_accessor&>( bucket( *pData )).insert_node( pData );
        }
        //@endcond
    };

}} // namespace cds::container

#endif // ifndef CDSLIB_CONTAINER_MICHAEL_SET_H

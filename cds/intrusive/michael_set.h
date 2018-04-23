// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_MICHAEL_SET_H
#define CDSLIB_INTRUSIVE_MICHAEL_SET_H

#include <cds/intrusive/details/michael_set_base.h>
#include <cds/intrusive/details/iterable_list_base.h>
#include <memory>

namespace cds { namespace intrusive {

    /// Michael's hash set
    /** @ingroup cds_intrusive_map
        \anchor cds_intrusive_MichaelHashSet_hp

        Source:
            - [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

        Michael's hash table algorithm is based on lock-free ordered list and it is very simple.
        The main structure is an array \p T of size \p M. Each element in \p T is basically a pointer
        to a hash bucket, implemented as a singly linked list. The array of buckets cannot be dynamically expanded.
        However, each bucket may contain unbounded number of items.

        Template parameters are:
        - \p GC - Garbage collector used. Note the \p GC must be the same as the GC used for \p OrderedList
        - \p OrderedList - ordered list implementation used as bucket for hash set, possible implementations:
            \p MichaelList, \p LazyList, \p IterableList.
            The intrusive ordered list implementation specifies the type \p T stored in the hash-set, the reclamation
            schema \p GC used by hash-set, the comparison functor for the type \p T and other features specific for
            the ordered list.
        - \p Traits - type traits. See \p michael_set::traits for explanation.
            Instead of defining \p Traits struct you can use option-based syntax with \p michael_set::make_traits metafunction.

        There are several specializations of \p %MichaelHashSet for each GC. You should include:
        - <tt><cds/intrusive/michael_set_rcu.h></tt> for \ref cds_intrusive_MichaelHashSet_rcu "RCU type"
        - <tt><cds/intrusive/michael_set_nogc.h></tt> for \ref cds_intrusive_MichaelHashSet_nogc for append-only set
        - <tt><cds/intrusive/michael_set.h></tt> for \p gc::HP, \p gc::DHP

        <b>Hash functor</b>

        Some member functions of Michael's hash set accept the key parameter of type \p Q which differs from \p value_type.
        It is expected that type \p Q contains full key of \p value_type, and for equal keys of type \p Q and \p value_type
        the hash values of these keys must be equal.
        The hash functor \p Traits::hash should accept parameters of both type:
        \code
        // Our node type
        struct Foo {
            std::string key_; // key field
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

        First, you should define ordered list type to use in your hash set:
        \code
        // For gc::HP-based MichaelList implementation
        #include <cds/intrusive/michael_list_hp.h>

        // cds::intrusive::MichaelHashSet declaration
        #include <cds/intrusive/michael_set.h>

        // Type of hash-set items
        struct Foo: public cds::intrusive::michael_list::node< cds::gc::HP >
        {
            std::string     key_    ;   // key field
            unsigned        val_    ;   // value field
            // ...  other value fields
        };

        // Declare comparator for the item
        struct FooCmp
        {
            int operator()( const Foo& f1, const Foo& f2 ) const
            {
                return f1.key_.compare( f2.key_ );
            }
        };

        // Declare bucket type for Michael's hash set
        // The bucket type is any ordered list type like MichaelList, LazyList
        typedef cds::intrusive::MichaelList< cds::gc::HP, Foo,
            typename cds::intrusive::michael_list::make_traits<
                // hook option
                cds::intrusive::opt::hook< cds::intrusive::michael_list::base_hook< cds::opt::gc< cds::gc::HP > > >
                // item comparator option
                ,cds::opt::compare< FooCmp >
            >::type
        >  Foo_bucket;
        \endcode

        Second, you should declare Michael's hash set container:
        \code

        // Declare hash functor
        // Note, the hash functor accepts parameter type Foo and std::string
        struct FooHash {
            size_t operator()( const Foo& f ) const
            {
                return cds::opt::v::hash<std::string>()( f.key_ );
            }
            size_t operator()( const std::string& f ) const
            {
                return cds::opt::v::hash<std::string>()( f );
            }
        };

        // Michael's set typedef
        typedef cds::intrusive::MichaelHashSet<
            cds::gc::HP
            ,Foo_bucket
            ,typename cds::intrusive::michael_set::make_traits<
                cds::opt::hash< FooHash >
            >::type
        > Foo_set;
        \endcode

        Now, you can use \p Foo_set in your application.

        Like other intrusive containers, you may build several containers on single item structure:
        \code
        #include <cds/intrusive/michael_list_hp.h>
        #include <cds/intrusive/michael_list_dhp.h>
        #include <cds/intrusive/michael_set.h>

        struct tag_key1_idx;
        struct tag_key2_idx;

        // Your two-key data
        // The first key is maintained by gc::HP, second key is maintained by gc::DHP garbage collectors
        // (I don't know what is needed for, but it is correct)
        struct Foo
            : public cds::intrusive::michael_list::node< cds::gc::HP, tag_key1_idx >
            , public cds::intrusive::michael_list::node< cds::gc::DHP, tag_key2_idx >
        {
            std::string     key1_   ;   // first key field
            unsigned int    key2_   ;   // second key field

            // ... value fields and fields for controlling item's lifetime
        };

        // Declare comparators for the item
        struct Key1Cmp
        {
            int operator()( const Foo& f1, const Foo& f2 ) const { return f1.key1_.compare( f2.key1_ ) ; }
        };
        struct Key2Less
        {
            bool operator()( const Foo& f1, const Foo& f2 ) const { return f1.key2_ < f2.key1_ ; }
        };

        // Declare bucket type for Michael's hash set indexed by key1_ field and maintained by gc::HP
        typedef cds::intrusive::MichaelList< cds::gc::HP, Foo,
            typename cds::intrusive::michael_list::make_traits<
                // hook option
                cds::intrusive::opt::hook< cds::intrusive::michael_list::base_hook< cds::opt::gc< cds::gc::HP >, tag_key1_idx > >
                // item comparator option
                ,cds::opt::compare< Key1Cmp >
            >::type
        >  Key1_bucket;

        // Declare bucket type for Michael's hash set indexed by key2_ field and maintained by gc::DHP
        typedef cds::intrusive::MichaelList< cds::gc::DHP, Foo,
            typename cds::intrusive::michael_list::make_traits<
                // hook option
                cds::intrusive::opt::hook< cds::intrusive::michael_list::base_hook< cds::opt::gc< cds::gc::DHP >, tag_key2_idx > >
                // item comparator option
                ,cds::opt::less< Key2Less >
            >::type
        >  Key2_bucket;

        // Declare hash functor
        struct Key1Hash {
            size_t operator()( const Foo& f ) const { return cds::opt::v::hash<std::string>()( f.key1_ ) ; }
            size_t operator()( const std::string& s ) const { return cds::opt::v::hash<std::string>()( s ) ; }
        };
        inline size_t Key2Hash( const Foo& f ) { return (size_t) f.key2_  ; }

        // Michael's set indexed by key1_ field
        typedef cds::intrusive::MichaelHashSet<
            cds::gc::HP
            ,Key1_bucket
            ,typename cds::intrusive::michael_set::make_traits<
                cds::opt::hash< Key1Hash >
            >::type
        > key1_set;

        // Michael's set indexed by key2_ field
        typedef cds::intrusive::MichaelHashSet<
            cds::gc::DHP
            ,Key2_bucket
            ,typename cds::intrusive::michael_set::make_traits<
                cds::opt::hash< Key2Hash >
            >::type
        > key2_set;
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
        typedef GC           gc;            ///< Garbage collector
        typedef OrderedList  ordered_list;  ///< type of ordered list used as a bucket implementation
        typedef Traits       traits;        ///< Set traits

        typedef typename ordered_list::value_type       value_type      ; ///< type of value to be stored in the set
        typedef typename ordered_list::key_comparator   key_comparator  ; ///< key comparing functor
        typedef typename ordered_list::disposer         disposer        ; ///< Node disposer functor
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename ordered_list::stat             stat            ; ///< Internal statistics
#endif

        /// Hash functor for \p value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;
        typedef typename traits::item_counter item_counter;   ///< Item counter type
        typedef typename traits::allocator    allocator;      ///< Bucket table allocator

        typedef typename ordered_list::guarded_ptr guarded_ptr; ///< Guarded pointer

        /// Count of hazard pointer required for the algorithm
        static constexpr const size_t c_nHazardPtrCount = ordered_list::c_nHazardPtrCount;

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
        //@cond
        typedef typename bucket_stat::stat stat;
        //@endcond

    protected:
        //@cond
        hash                    m_HashFunctor;   ///< Hash functor
        size_t const            m_nHashBitmask;
        internal_bucket_type*   m_Buckets;       ///< bucket table
        item_counter            m_ItemCounter;   ///< Item counter
        stat                    m_Stat;          ///< Internal statistics
        //@endcond

    public:
    ///@name Forward iterators
    //@{
        /// Forward iterator
        /**
            The forward iterator for Michael's set is based on \p OrderedList forward iterator and has some features:
            - it has no post-increment operator
            - it iterates items in unordered fashion
            - The iterator cannot be moved across thread boundary because it may contain GC's guard that is thread-private GC data.

            Iterator thread safety depends on type of \p OrderedList:
            - for \p MichaelList and \p LazyList: iterator guarantees safety even if you delete the item that iterator points to
              because that item is guarded by hazard pointer.
              However, in case of concurrent deleting operations it is no guarantee that you iterate all item in the set.
              Moreover, a crash is possible when you try to iterate the next element that has been deleted by concurrent thread.
              Use this iterator on the concurrent container for debugging purpose only.
            - for \p IterableList: iterator is thread-safe. You may use it freely in concurrent environment.
        */
        typedef michael_set::details::iterator< internal_bucket_type, false > iterator;

        /// Const forward iterator
        /**
            For iterator's features and requirements see \ref iterator
        */
        typedef michael_set::details::iterator< internal_bucket_type, true > const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        /**
            For empty set \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( m_Buckets[0].begin(), bucket_begin(), bucket_end());
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
        /// Initializes hash set
        /**
            The Michael's hash set is an unbounded container, but its hash table is non-expandable.
            At construction time you should pass estimated maximum item count and a load factor.
            The load factor is average size of one bucket - a small number between 1 and 10.
            The bucket is an ordered single-linked list, searching in the bucket has linear complexity <tt>O(nLoadFactor)</tt>.
            The constructor defines hash table size as rounding <tt>nMaxItemCount / nLoadFactor</tt> up to nearest power of two.
        */
        MichaelHashSet(
            size_t nMaxItemCount,   ///< estimation of max item count in the hash set
            size_t nLoadFactor      ///< load factor: estimation of max number of items in the bucket. Small integer up to 10.
        ) : m_nHashBitmask( michael_set::details::init_hash_bitmask( nMaxItemCount, nLoadFactor ))
          , m_Buckets( bucket_table_allocator().allocate( bucket_count()))
        {
            for ( auto it = m_Buckets, itEnd = m_Buckets + bucket_count(); it != itEnd; ++it )
                construct_bucket<bucket_stat>( it );
        }

        /// Clears hash set object and destroys it
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

            @warning For \ref cds_intrusive_MichaelList_hp "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_hp "LazyList" provides exclusive access to inserted item and does not require any node-level
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

            The functor signature depends of the type of \p OrderedList:

            <b>for \p MichaelList, \p LazyList</b>
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
                @warning For \ref cds_intrusive_MichaelList_hp "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
                \ref cds_intrusive_LazyList_hp "LazyList" provides exclusive access to inserted item and does not require any node-level
                synchronization.

            <b>for \p IterableList</b>
                \code
                void func( value_type& val, value_type * old );
                \endcode
                where
                - \p val - argument \p val passed into the \p %update() function
                - \p old - old value that will be retired. If new item has been inserted then \p old is \p nullptr.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the set.
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

        /// Inserts or updates the node (only for \p IterableList)
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the set, then \p val is inserted iff \p bAllowInsert is \p true.
            Otherwise, the current element is changed to \p val, the old element will be retired later
            by call \p Traits::disposer.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if \p val has been added or \p false if the item with that key
            already in the set.
        */
#ifdef CDS_DOXYGEN_INVOKED
        std::pair<bool, bool> upsert( value_type& val, bool bAllowInsert = true )
#else
        template <typename Q>
        typename std::enable_if<
            std::is_same< Q, value_type>::value && is_iterable_list< ordered_list >::value,
            std::pair<bool, bool>
        >::type
        upsert( Q& val, bool bAllowInsert = true )
#endif
        {
            std::pair<bool, bool> bRet = bucket( val ).upsert( val, bAllowInsert );
            if ( bRet.second )
                ++m_ItemCounter;
            return bRet;
        }

        /// Unlinks the item \p val from the set
        /**
            The function searches the item \p val in the set and unlink it
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
        /** \anchor cds_intrusive_MichaelHashSet_hp_erase
            The function searches an item with key equal to \p key in the set,
            unlinks it, and returns \p true.
            If the item with key equal to \p key is not found the function return \p false.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.
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
            The function is an analog of \ref cds_intrusive_MichaelHashSet_hp_erase "erase(Q const&)"
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
        /** \anchor cds_intrusive_MichaelHashSet_hp_erase_func
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
        bool erase( Q const& key, Func f )
        {
            if ( bucket( key ).erase( key, f )) {
                --m_ItemCounter;
                return true;
            }
            return false;
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelHashSet_hp_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            if ( bucket( key ).erase_with( key, pred, f )) {
                --m_ItemCounter;
                return true;
            }
            return false;
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
        /** \anchor cds_intrusive_MichaelHashSet_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the set, and returns an guarded pointer to the item extracted.
            If \p key is not found the function returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p Q that may be not the same as \p value_type.

            The \p disposer specified in \p OrderedList class' template parameter is called automatically
            by garbage collector \p GC when returned \ref guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::MichaelHashSet< your_template_args > michael_set;
            michael_set theSet;
            // ...
            {
                michael_set::guarded_ptr gp( theSet.extract( 5 ));
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
            guarded_ptr gp = bucket( key ).extract( key );
            if ( gp )
                --m_ItemCounter;
            return gp;
        }

        /// Extracts the item using compare functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_MichaelHashSet_hp_extract "extract(Q const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            guarded_ptr gp = bucket( key ).extract_with( key, pred );
            if ( gp )
                --m_ItemCounter;
            return gp;
        }

        /// Finds the key \p key
        /** \anchor cds_intrusive_MichaelHashSet_hp_find_func
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
            The functor does not serialize simultaneous access to the set \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p key argument is non-const since it can be used as \p f functor destination i.e., the functor
            may modify both arguments.

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
            The function is an analog of \ref cds_intrusive_MichaelHashSet_hp_find_func "find(Q&, Func)"
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
        /** \anchor cds_intrusive_MichaelHashSet_hp_get
            The function searches the item with key equal to \p key
            and returns the guarded pointer to the item found.
            If \p key is not found the function returns an empty \p guarded_ptr.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::MichaelHashSet< your_template_params >  michael_set;
            michael_set theSet;
            // ...
            {
                michael_set::guarded_ptr gp( theSet.get( 5 ));
                if ( theSet.get( 5 )) {
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
            The function is an analog of \ref cds_intrusive_MichaelHashSet_hp_get "get( Q const&)"
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
            The function unlink all items from the set.
            The function is not atomic. It cleans up each bucket and then resets the item counter to zero.
            If there are a thread that performs insertion while \p %clear() is working the result is undefined in general case:
            \p empty() may return \p true but the set may contain item(s).
            Therefore, \p %clear() may be used only for debugging purposes.

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

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }

        /// Returns the size of hash table
        /**
            Since \p %MichaelHashSet cannot dynamically extend the hash table size,
            the value returned is an constant depending on object initialization parameters,
            see \p MichaelHashSet::MichaelHashSet.
        */
        size_t bucket_count() const
        {
            return m_nHashBitmask + 1;
        }

    private:
        //@cond
        internal_bucket_type * bucket_begin() const
        {
            return m_Buckets;
        }

        internal_bucket_type * bucket_end() const
        {
            return m_Buckets + bucket_count();
        }

        const_iterator get_const_begin() const
        {
            return const_iterator( m_Buckets[0].cbegin(), bucket_begin(), bucket_end());
        }
        const_iterator get_const_end() const
        {
            return const_iterator( bucket_end()[-1].cend(), bucket_end() - 1, bucket_end());
        }

        template <typename Stat>
        typename std::enable_if< Stat::empty >::type construct_bucket( internal_bucket_type * b )
        {
            new (b) internal_bucket_type;
        }

        template <typename Stat>
        typename std::enable_if< !Stat::empty >::type construct_bucket( internal_bucket_type * b )
        {
            new (b) internal_bucket_type( m_Stat );
        }

        /// Calculates hash value of \p key
        template <typename Q>
        size_t hash_value( const Q& key ) const
        {
            return m_HashFunctor( key ) & m_nHashBitmask;
        }

        /// Returns the bucket (ordered list) for \p key
        template <typename Q>
        internal_bucket_type& bucket( const Q& key )
        {
            return m_Buckets[hash_value( key )];
        }
        //@endcond
    };

}}  // namespace cds::intrusive

#endif // ifndef CDSLIB_INTRUSIVE_MICHAEL_SET_H

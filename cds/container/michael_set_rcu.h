//$$CDS-header$$

#ifndef __CDS_CONTAINER_MICHAEL_SET_RCU_H
#define __CDS_CONTAINER_MICHAEL_SET_RCU_H

#include <cds/container/michael_set_base.h>
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
        - \p OrderedList - ordered list implementation used as bucket for hash set, for example, MichaelList.
            The ordered list implementation specifies the type \p T stored in the hash-set,
            the comparison functor for the type \p T and other features specific for
            the ordered list.
        - \p Traits - type traits. See michael_set::type_traits for explanation.

        Instead of defining \p Traits struct you may use option-based syntax with michael_set::make_traits metafunction.
        For michael_set::make_traits the following option may be used:
        - opt::hash - mandatory option, specifies hash functor.
        - opt::item_counter - optional, specifies item counting policy. See michael_set::type_traits for explanation.
        - opt::allocator - optional, bucket table allocator. Default is \ref CDS_DEFAULT_ALLOCATOR.

        \note About hash functor see \ref cds_nonintrusive_MichaelHashSet_hash_functor "MichaelSet".

        <b>How to use</b>

        Suppose, we have the following type \p Foo that we want to store in our MichaelHashSet:
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

        // Our ordered list
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

        // Declare set type.
        // Note that \p RCU template parameter of ordered list must be equal \p RCU for the set.
        typedef cc::MichaelHashSet< cds::urcu::gc< cds::urcu::general_buffered<> >, bucket_list,
            cc::michael_set::make_traits<
                cc::opt::hash< foo_hash >
            >::type
        > foo_set;

        // Set variable
        foo_set fooSet;
        \endcode
    */
    template <
        class RCU,
        class OrderedList,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = michael_set::type_traits
#else
        class Traits
#endif
    >
    class MichaelHashSet< cds::urcu::gc< RCU >, OrderedList, Traits >
    {
    public:
        typedef OrderedList bucket_type     ;   ///< type of ordered list used as a bucket implementation
        typedef Traits      options         ;   ///< Traits template parameters

        typedef typename bucket_type::value_type        value_type      ;   ///< type of value stored in the list
        typedef cds::urcu::gc< RCU >                    gc              ;   ///< RCU used as garbage collector
        typedef typename bucket_type::key_comparator    key_comparator  ;   ///< key comparing functor

        /// Hash functor for \ref value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename options::hash >::type   hash;
        typedef typename options::item_counter          item_counter    ;   ///< Item counter type

        /// Bucket table allocator
        typedef cds::details::Allocator< bucket_type, typename options::allocator >  bucket_table_allocator;

        typedef typename bucket_type::rcu_lock      rcu_lock   ; ///< RCU scoped lock
        typedef typename bucket_type::exempt_ptr    exempt_ptr ; ///< pointer to extracted node
        /// Group of \p extract_xxx functions require external locking if underlying ordered list requires that
        static CDS_CONSTEXPR_CONST bool c_bExtractLockExternal = bucket_type::c_bExtractLockExternal;

    protected:
        item_counter    m_ItemCounter   ;   ///< Item counter
        hash            m_HashFunctor   ;   ///< Hash functor

        bucket_type *   m_Buckets       ;   ///< bucket table

    private:
        //@cond
        const size_t    m_nHashBitmask;
        //@endcond

    protected:
        /// Calculates hash value of \p key
        template <typename Q>
        size_t hash_value( Q const& key ) const
        {
            return m_HashFunctor( key ) & m_nHashBitmask;
        }

        /// Returns the bucket (ordered list) for \p key
        //@{
        template <typename Q>
        bucket_type&    bucket( Q const& key )
        {
            return m_Buckets[ hash_value( key ) ];
        }
        template <typename Q>
        bucket_type const&    bucket( Q const& key ) const
        {
            return m_Buckets[ hash_value( key ) ];
        }
        //@}
    public:
        /// Forward iterator
        /**
            The forward iterator for Michael's set is based on \p OrderedList forward iterator and has some features:
            - it has no post-increment operator
            - it iterates items in unordered fashion
            - The iterator cannot be moved across thread boundary since it may contain GC's guard that is thread-private GC data.
            - Iterator ensures thread-safety even if you delete the item that iterator points to. However, in case of concurrent
              deleting operations it is no guarantee that you iterate all item in the set.

            Therefore, the use of iterators in concurrent environment is not good idea. Use the iterator for the concurrent container
            for debug purpose only.
        */
        typedef michael_set::details::iterator< bucket_type, false >    iterator;

        /// Const forward iterator
        typedef michael_set::details::iterator< bucket_type, true >     const_iterator;

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
        const_iterator cbegin()
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
        const_iterator cend()
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
        /// Initialize hash set
        /**
            The Michael's hash set is non-expandable container. You should point the average count of items \p nMaxItemCount
            when you create an object.
            \p nLoadFactor parameter defines average count of items per bucket and it should be small number between 1 and 10.
            Remember, since the bucket implementation is an ordered list, searching in the bucket is linear [<tt>O(nLoadFactor)</tt>].
            Note, that many popular STL hash map implementation uses load factor 1.

            The ctor defines hash table size as rounding <tt>nMacItemCount / nLoadFactor</tt> up to nearest power of two.
        */
        MichaelHashSet(
            size_t nMaxItemCount,   ///< estimation of max item count in the hash set
            size_t nLoadFactor      ///< load factor: estimation of max number of items in the bucket
        ) : m_nHashBitmask( michael_set::details::init_hash_bitmask( nMaxItemCount, nLoadFactor ))
        {
            // GC and OrderedList::gc must be the same
            static_assert(( std::is_same<gc, typename bucket_type::gc>::value ), "GC and OrderedList::gc must be the same");

            // atomicity::empty_item_counter is not allowed as a item counter
            static_assert(( !std::is_same<item_counter, atomicity::empty_item_counter>::value ), "atomicity::empty_item_counter is not allowed as a item counter");

            m_Buckets = bucket_table_allocator().NewArray( bucket_count() );
        }

        /// Clear hash set and destroy it
        ~MichaelHashSet()
        {
            clear();
            bucket_table_allocator().Delete( m_Buckets, bucket_count() );
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
        bool insert( Q const& val )
        {
            const bool bRet = bucket( val ).insert( val );
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
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this set's item by concurrent threads.
            The user-defined functor is called only if the inserting is success. It may be passed by reference
            using <tt>boost::ref</tt>

            The function applies RCU lock internally.
        */
        template <typename Q, typename Func>
        bool insert( Q const& val, Func f )
        {
            const bool bRet = bucket( val ).insert( val, f );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Ensures that the item exists in the set
        /**
            The operation performs inserting or changing data with lock-free manner.

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
            - \p val - argument \p key passed into the \p ensure function

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            You may pass \p func argument by reference using <tt>boost::ref</tt>.

            The function applies RCU lock internally.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successfull,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the set.
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> ensure( const Q& val, Func func )
        {
            std::pair<bool, bool> bRet = bucket( val ).ensure( val, func );
            if ( bRet.first && bRet.second )
                ++m_ItemCounter;
            return bRet;
        }

        /// Inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            bool bRet = bucket( value_type(std::forward<Args>(args)...) ).emplace( std::forward<Args>(args)... );
            if ( bRet )
                ++m_ItemCounter;
            return bRet;
        }

        /// Deletes \p key from the set
        /** \anchor cds_nonintrusive_MichealSet_rcu_erase_val

            Since the key of MichaelHashSet's item type \ref value_type is not explicitly specified,
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
            The functor may be passed by reference using <tt>boost:ref</tt>

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
            The function searches an item with key equal to \p val in the set,
            unlinks it from the set, places item pointer into \p dest argument, and returns \p true.
            If the item with the key equal to \p val is not found the function return \p false.

            @note The function does NOT call RCU read-side lock or synchronization,
            and does NOT dispose the item found. It just excludes the item from the set
            and returns a pointer to item found.
            You should lock RCU before calling of the function, and you should synchronize RCU
            outside the RCU lock to free extracted item

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/container/michael_list_rcu.h>
            #include <cds/container/michael_set_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::container::MichaelList< rcu, Foo > rcu_michael_list;
            typedef cds::container::MichaelHashSet< rcu, rcu_michael_list, foo_traits > rcu_michael_set;

            rcu_michael_set theSet;
            // ...

            rcu_michael_set::exempt_ptr p;
            {
                // first, we should lock RCU
                rcu_michael_set::rcu_lock lock;

                // Now, you can apply extract function
                // Note that you must not delete the item found inside the RCU lock
                if ( theSet.extract( p, 10 )) {
                    // do something with p
                    ...
                }
            }

            // We may safely release p here
            // release() passes the pointer to RCU reclamation cycle
            p.release();
            \endcode
        */
        template <typename Q>
        bool extract( exempt_ptr& dest, Q const& val )
        {
            if ( bucket( val ).extract( dest, val )) {
                --m_ItemCounter;
                return true;
            }
            return false;
        }

        /// Extracts an item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelHashSet_rcu_extract "extract(exempt_ptr&, Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool extract_with( exempt_ptr& dest, Q const& val, Less pred )
        {
            if ( bucket( val ).extract_with( dest, val, pred )) {
                --m_ItemCounter;
                return true;
            }
            return false;
        }

        /// Finds the key \p val
        /** \anchor cds_nonintrusive_MichealSet_rcu_find_func

            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You may pass \p f argument by reference using <tt>boost::ref</tt> or cds::ref.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p val argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that may be not the same as \p value_type.

            The function applies RCU lock internally.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& val, Func f ) const
        {
            return bucket( val ).find( val, f );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichealSet_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& val, Less pred, Func f ) const
        {
            return bucket( val ).find_with( val, pred, f );
        }

        /// Finds the key \p val
        /** \anchor cds_nonintrusive_MichealSet_rcu_find_cfunc

            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q const& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You may pass \p f argument by reference using <tt>boost::ref</tt> or cds::ref.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that may be not the same as \p value_type.

            The function applies RCU lock internally.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& val, Func f ) const
        {
            return bucket( val ).find( val, f );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichealSet_rcu_find_cfunc "find(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& val, Less pred, Func f ) const
        {
            return bucket( val ).find_with( val, pred, f );
        }

        /// Finds the key \p val
        /** \anchor cds_nonintrusive_MichealSet_rcu_find_val

            The function searches the item with key equal to \p val
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that may be not the same as \ref value_type.
        */
        template <typename Q>
        bool find( Q const & val ) const
        {
            return bucket( val ).find( val );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichealSet_rcu_find_val "find(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool find_with( Q const & val, Less pred ) const
        {
            return bucket( val ).find_with( val, pred );
        }

        /// Finds the key \p val and return the item found
        /** \anchor cds_nonintrusive_MichaelHashSet_rcu_get
            The function searches the item with key equal to \p val and returns the pointer to item found.
            If \p val is not found it returns \p nullptr.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::container::MichaelHashSet< your_template_parameters > hash_set;
            hash_set theSet;
            // ...
            {
                // Lock RCU
                hash_set::rcu_lock lock;

                foo * pVal = theSet.get( 5 );
                if ( pVal ) {
                    // Deal with pVal
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // pVal can be freed at any time after RCU has been unlocked
            }
            \endcode
        */
        template <typename Q>
        value_type * get( Q const& val ) const
        {
            return bucket( val ).get( val );
        }

        /// Finds the key \p val and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelHashSet_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        value_type * get_with( Q const& val, Less pred ) const
        {
            return bucket( val ).get_with( val, pred );
        }

        /// Clears the set (non-atomic)
        /**
            The function erases all items from the set.

            The function is not atomic. It cleans up each bucket and then resets the item counter to zero.
            If there are a thread that performs insertion while \p clear is working the result is undefined in general case:
            <tt> empty() </tt> may return \p true but the set may contain item(s).
            Therefore, \p clear may be used only for debugging purposes.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        void clear()
        {
            for ( size_t i = 0; i < bucket_count(); ++i )
                m_Buckets[i].clear();
            m_ItemCounter.reset();
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
            Thus, the correct item counting feature is an important part of Michael's set implementation.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Returns item count in the set
        size_t size() const
        {
            return m_ItemCounter;
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
    };

}} // namespace cds::container

#endif // ifndef __CDS_CONTAINER_MICHAEL_SET_H

//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_MICHAEL_SET_NOGC_H
#define CDSLIB_CONTAINER_MICHAEL_SET_NOGC_H

#include <cds/container/details/michael_set_base.h>
#include <cds/gc/nogc.h>
#include <cds/details/allocator.h>

namespace cds { namespace container {

    /// Michael's hash set (template specialization for gc::nogc)
    /** @ingroup cds_nonintrusive_set
        \anchor cds_nonintrusive_MichaelHashSet_nogc

        This specialization is so-called append-only when no item
        reclamation may be performed. The class does not support deleting of list item.

        See \ref cds_nonintrusive_MichaelHashSet_hp "MichaelHashSet" for description of template parameters.
        The template parameter \p OrderedList should be any \p gc::nogc -derived ordered list, for example,
        \ref cds_nonintrusive_MichaelList_nogc "append-only MichaelList".
    */
    template <
        class OrderedList,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = michael_set::traits
#else
        class Traits
#endif
    >
    class MichaelHashSet< cds::gc::nogc, OrderedList, Traits >
    {
    public:
        typedef cds::gc::nogc gc;        ///< Garbage collector
        typedef OrderedList bucket_type; ///< type of ordered list to be used as a bucket implementation
        typedef Traits      traits;      ///< Set traits

        typedef typename bucket_type::value_type        value_type;     ///< type of value stored in the list
        typedef typename bucket_type::key_comparator    key_comparator; ///< key comparison functor

        /// Hash functor for \ref value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;
        typedef typename traits::item_counter  item_counter; ///< Item counter type

        /// Bucket table allocator
        typedef cds::details::Allocator< bucket_type, typename traits::allocator >  bucket_table_allocator;

        //@cond
        typedef cds::container::michael_set::implementation_tag implementation_tag;
        //@endcond

    protected:
        //@cond
        typedef typename bucket_type::iterator        bucket_iterator;
        typedef typename bucket_type::const_iterator  bucket_const_iterator;
        //@endcond

    protected:
        item_counter    m_ItemCounter;   ///< Item counter
        hash            m_HashFunctor;   ///< Hash functor
        bucket_type *   m_Buckets;       ///< bucket table

    private:
        //@cond
        const size_t    m_nHashBitmask;
        //@endcond

    protected:
        //@cond
        /// Calculates hash value of \p key
        template <typename Q>
        size_t hash_value( const Q& key ) const
        {
            return m_HashFunctor( key ) & m_nHashBitmask;
        }

        /// Returns the bucket (ordered list) for \p key
        template <typename Q>
        bucket_type&    bucket( const Q& key )
        {
            return m_Buckets[ hash_value( key ) ];
        }
        //@endcond

    public:
        /// Forward iterator
        /**
            The forward iterator for Michael's set is based on \p OrderedList forward iterator and has some features:
            - it has no post-increment operator
            - it iterates items in unordered fashion
        */
        typedef michael_set::details::iterator< bucket_type, false >    iterator;

        /// Const forward iterator
        /**
            For iterator's features and requirements see \ref iterator
        */
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
        /// Initialize hash set
        /** @copydetails cds_nonintrusive_MichaelHashSet_hp_ctor
        */
        MichaelHashSet(
            size_t nMaxItemCount,   ///< estimation of max item count in the hash set
            size_t nLoadFactor      ///< load factor: estimation of max number of items in the bucket
        ) : m_nHashBitmask( michael_set::details::init_hash_bitmask( nMaxItemCount, nLoadFactor ))
        {
            // GC and OrderedList::gc must be the same
            static_assert( std::is_same<gc, typename bucket_type::gc>::value, "GC and OrderedList::gc must be the same");

            // atomicity::empty_item_counter is not allowed as a item counter
            static_assert( !std::is_same<item_counter, atomicity::empty_item_counter>::value,
                           "cds::atomicity::empty_item_counter is not allowed as a item counter");

            m_Buckets = bucket_table_allocator().NewArray( bucket_count() );
        }

        /// Clears hash set and destroys it
        ~MichaelHashSet()
        {
            clear();
            bucket_table_allocator().Delete( m_Buckets, bucket_count() );
        }

        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            Return an iterator pointing to inserted item if success, otherwise \ref end()
        */
        template <typename Q>
        iterator insert( const Q& val )
        {
            bucket_type& refBucket = bucket( val );
            bucket_iterator it = refBucket.insert( val );

            if ( it != refBucket.end() ) {
                ++m_ItemCounter;
                return iterator( it, &refBucket, m_Buckets + bucket_count() );
            }

            return end();
        }

        /// Inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Return an iterator pointing to inserted item if success \ref end() otherwise
        */
        template <typename... Args>
        iterator emplace( Args&&... args )
        {
            bucket_type& refBucket = bucket( value_type(std::forward<Args>(args)...));
            bucket_iterator it = refBucket.emplace( std::forward<Args>(args)... );

            if ( it != refBucket.end() ) {
                ++m_ItemCounter;
                return iterator( it, &refBucket, m_Buckets + bucket_count() );
            }

            return end();
        }

        /// Ensures that the item \p val exists in the set
        /**
            The operation inserts new item if the key \p val is not found in the set.
            Otherwise, the function returns an iterator that points to item found.

            Returns <tt> std::pair<iterator, bool> </tt> where \p first is an iterator pointing to
            item found or inserted, \p second is true if new item has been added or \p false if the item
            already is in the set.

            @warning For \ref cds_nonintrusive_MichaelList_nogc "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyList_nogc "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Q>
        std::pair<iterator, bool> ensure( const Q& val )
        {
            bucket_type& refBucket = bucket( val );
            std::pair<bucket_iterator, bool> ret = refBucket.ensure( val );

            if ( ret.first != refBucket.end() ) {
                if ( ret.second )
                    ++m_ItemCounter;
                return std::make_pair( iterator( ret.first, &refBucket, m_Buckets + bucket_count() ), ret.second );
            }

            return std::make_pair( end(), ret.second );
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_MichealSet_nogc_find
            The function searches the item with key equal to \p key
            and returns an iterator pointed to item found if the key is found,
            and \ref end() otherwise
        */
        template <typename Q>
        iterator find( Q const& key )
        {
            bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.find( key );
            if ( it != refBucket.end() )
                return iterator( it, &refBucket, m_Buckets + bucket_count() );

            return end();
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichealSet_nogc_find "find(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        iterator find_with( Q const& key, Less pred )
        {
            bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.find_with( key, pred );
            if ( it != refBucket.end() )
                return iterator( it, &refBucket, m_Buckets + bucket_count() );

            return end();
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
            The emptiness is checked by the item counting: if item count is zero then the set is empty.
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
            Since \p %MichaelHashSet cannot dynamically extend the hash table size,
            the value returned is an constant depending on object initialization parameters;
            see MichaelHashSet::MichaelHashSet for explanation.
        */
        size_t bucket_count() const
        {
            return m_nHashBitmask + 1;
        }
    };

}} // cds::container

#endif // ifndef CDSLIB_CONTAINER_MICHAEL_SET_NOGC_H

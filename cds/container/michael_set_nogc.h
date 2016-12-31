/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#ifndef CDSLIB_CONTAINER_MICHAEL_SET_NOGC_H
#define CDSLIB_CONTAINER_MICHAEL_SET_NOGC_H

#include <cds/container/details/michael_set_base.h>
#include <cds/gc/nogc.h>

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
        typedef cds::gc::nogc gc;         ///< Garbage collector
        typedef OrderedList ordered_list; ///< type of ordered list to be used as a bucket implementation
        typedef Traits      traits;       ///< Set traits

        typedef typename ordered_list::value_type     value_type;     ///< type of value stored in the list
        typedef typename ordered_list::key_comparator key_comparator; ///< key comparison functor
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename ordered_list::stat           stat;           ///< Internal statistics
#endif

        /// Hash functor for \ref value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;
        typedef typename traits::item_counter item_counter; ///< Item counter type
        typedef typename traits::allocator    allocator;    ///< Bucket table allocator

        // GC and OrderedList::gc must be the same
        static_assert(std::is_same<gc, typename ordered_list::gc>::value, "GC and OrderedList::gc must be the same");

        // atomicity::empty_item_counter is not allowed as a item counter
        static_assert(!std::is_same<item_counter, atomicity::empty_item_counter>::value,
            "cds::atomicity::empty_item_counter is not allowed as a item counter");

    protected:
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

        /// Bucket table allocator
        typedef typename allocator::template rebind< internal_bucket_type >::other bucket_table_allocator;

        typedef typename internal_bucket_type::iterator        bucket_iterator;
        typedef typename internal_bucket_type::const_iterator  bucket_const_iterator;
        //@endcond

    public:
        //@cond
        typedef typename bucket_stat::stat stat;
        //@endcond

    protected:
        //@cond
        const size_t    m_nHashBitmask;
        item_counter    m_ItemCounter;      ///< Item counter
        hash            m_HashFunctor;      ///< Hash functor
        internal_bucket_type*   m_Buckets;  ///< bucket table
        stat            m_Stat; ///< Internal statistics
        //@endcond

    public:
    ///@name Forward iterators
    //@{
        /// Forward iterator
        /**
            The forward iterator for Michael's set is based on \p OrderedList forward iterator and has some features:
            - it has no post-increment operator
            - it iterates items in unordered fashion

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
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            Return an iterator pointing to inserted item if success, otherwise \ref end()
        */
        template <typename Q>
        iterator insert( const Q& val )
        {
            internal_bucket_type& refBucket = bucket( val );
            bucket_iterator it = refBucket.insert( val );

            if ( it != refBucket.end()) {
                ++m_ItemCounter;
                return iterator( it, &refBucket, m_Buckets + bucket_count());
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
            typename internal_bucket_type::node_type * pNode = internal_bucket_type::alloc_node( std::forward<Args>( args )... );
            internal_bucket_type& refBucket = bucket( internal_bucket_type::node_to_value( *pNode ));
            bucket_iterator it = refBucket.insert_node( pNode );
            if ( it != refBucket.end()) {
                ++m_ItemCounter;
                return iterator( it, &refBucket, m_Buckets + bucket_count());
            }

            return end();
        }

        /// Updates the element
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val not found in the set, then \p val is inserted iff \p bAllowInsert is \p true.

            Returns <tt> std::pair<iterator, bool> </tt> where \p first is an iterator pointing to
            item found or inserted, or \p end() if \p bAllowInsert is \p false,

            \p second is true if new item has been added or \p false if the item is already in the set.

            @warning For \ref cds_intrusive_MichaelList_hp "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_hp "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Q>
        std::pair<iterator, bool> update( Q const& val, bool bAllowInsert = true )
        {
            internal_bucket_type& refBucket = bucket( val );
            std::pair<bucket_iterator, bool> ret = refBucket.update( val, bAllowInsert );

            if ( ret.first != refBucket.end()) {
                if ( ret.second )
                    ++m_ItemCounter;
                return std::make_pair( iterator( ret.first, &refBucket, m_Buckets + bucket_count()), ret.second );
            }
            return std::make_pair( end(), ret.second );
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<iterator, bool> ensure( Q const& val )
        {
            return update( val, true );
        }
        //@endcond

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns an iterator pointed to item found if the key is found,
            or \ref end() otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        iterator contains( Q const& key )
        {
            internal_bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.contains( key );
            if ( it != refBucket.end())
                return iterator( it, &refBucket, m_Buckets + bucket_count());

            return end();
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("use contains()")
        iterator find( Q const& key )
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
        iterator contains( Q const& key, Less pred )
        {
            internal_bucket_type& refBucket = bucket( key );
            bucket_iterator it = refBucket.contains( key, pred );
            if ( it != refBucket.end())
                return iterator( it, &refBucket, m_Buckets + bucket_count());

            return end();
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("use contains()")
        iterator find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

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

    private:
        //@cond
        template <typename Stat>
        typename std::enable_if< Stat::empty >::type construct_bucket( internal_bucket_type* bucket )
        {
            new (bucket) internal_bucket_type;
        }

        template <typename Stat>
        typename std::enable_if< !Stat::empty >::type construct_bucket( internal_bucket_type* bucket )
        {
            new (bucket) internal_bucket_type( m_Stat );
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

}} // cds::container

#endif // ifndef CDSLIB_CONTAINER_MICHAEL_SET_NOGC_H

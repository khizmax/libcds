// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_SPLIT_LIST_NOGC_H
#define CDSLIB_INTRUSIVE_SPLIT_LIST_NOGC_H

#include <limits>

#include <cds/intrusive/details/split_list_base.h>
#include <cds/gc/nogc.h>
#include <cds/details/type_padding.h>

namespace cds { namespace intrusive {

    /// Split-ordered list (template specialization for gc::nogc)
    /** @ingroup cds_intrusive_map
        \anchor cds_intrusive_SplitListSet_nogc

        This specialization is intended for so-called persistent usage when no item
        reclamation may be performed. The class does not support deleting of list item.

        See \ref cds_intrusive_SplitListSet_hp "SplitListSet" for description of template parameters.
        The template parameter \p OrderedList should be any gc::nogc-derived ordered list, for example,
        \ref cds_intrusive_MichaelList_nogc "persistent MichaelList",
        \ref cds_intrusive_LazyList_nogc "persistent LazyList"
    */
    template <
        class OrderedList,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = split_list::traits
#else
        class Traits
#endif
    >
    class SplitListSet< cds::gc::nogc, OrderedList, Traits >
    {
    public:
        typedef cds::gc::nogc gc;     ///< Garbage collector
        typedef Traits        traits; ///< Traits template parameters

        /// Hash functor for \p value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type   hash;

    protected:
        //@cond
        typedef split_list::details::rebind_list_traits<OrderedList, traits> ordered_list_adapter;
        //@endcond

    public:
#   ifdef CDS_DOXYGEN_INVOKED
        typedef OrderedList ordered_list;   ///< type of ordered list used as base for split-list
#   else
        typedef typename ordered_list_adapter::result ordered_list;
#   endif
        typedef typename ordered_list::value_type     value_type;     ///< type of value stored in the split-list
        typedef typename ordered_list::key_comparator key_comparator; ///< key comparison functor
        typedef typename ordered_list::disposer       disposer;       ///< Node disposer functor

        typedef typename traits::bit_reversal bit_reversal; ///< Bit reversal algorithm, see \p split_list::traits::bit_reversal
        typedef typename traits::item_counter item_counter; ///< Item counter type
        typedef typename traits::back_off     back_off;     ///< back-off strategy
        typedef typename traits::memory_model memory_model; ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::stat         stat;         ///< Internal statistics, see \p spit_list::stat

        // GC and OrderedList::gc must be the same
        static_assert(std::is_same<gc, typename ordered_list::gc>::value, "GC and OrderedList::gc must be the same");

        // atomicity::empty_item_counter is not allowed as a item counter
        static_assert(!std::is_same<item_counter, cds::atomicity::empty_item_counter>::value,
            "cds::atomicity::empty_item_counter is not allowed as a item counter");

    protected:
        //@cond
        typedef typename ordered_list::node_type  list_node_type;  ///< Node type as declared in ordered list
        typedef split_list::node<list_node_type>  node_type;       ///< split-list node type

        /// Split-list node traits
        /**
            This traits is intended for converting between underlying ordered list node type \ref list_node_type
            and split-list node type \ref node_type
        */
        typedef typename ordered_list_adapter::node_traits node_traits;

        /// Bucket table implementation
        typedef typename split_list::details::bucket_table_selector<
            traits::dynamic_bucket_table
            , gc
            , typename ordered_list_adapter::aux_node
            , opt::allocator< typename traits::allocator >
            , opt::memory_model< memory_model >
            , opt::free_list< typename traits::free_list >
        >::type bucket_table;

        typedef typename bucket_table::aux_node_type aux_node_type; ///< dummy node type

        typedef typename ordered_list::iterator list_iterator;
        typedef typename ordered_list::const_iterator list_const_iterator;
        //@endcond

    protected:
        //@cond
        /// Ordered list wrapper to access protected members
        class ordered_list_wrapper: public ordered_list
        {
            typedef ordered_list base_class;
            typedef typename base_class::auxiliary_head bucket_head_type;

        public:
            list_iterator insert_at_( aux_node_type * pHead, value_type& val )
            {
                assert( pHead != nullptr );
                bucket_head_type h(static_cast<list_node_type *>(pHead));
                return base_class::insert_at_( h, val );
            }

            template <typename Func>
            std::pair<list_iterator, bool> update_at_( aux_node_type * pHead, value_type& val, Func func, bool bAllowInsert )
            {
                assert( pHead != nullptr );
                bucket_head_type h(static_cast<list_node_type *>(pHead));
                return base_class::update_at_( h, val, func, bAllowInsert );
            }

            template <typename Q, typename Compare, typename Func>
            bool find_at( aux_node_type * pHead, split_list::details::search_value_type<Q>& val, Compare cmp, Func f )
            {
                assert( pHead != nullptr );
                bucket_head_type h(static_cast<list_node_type *>(pHead));
                return base_class::find_at( h, val, cmp, f );
            }

            template <typename Q, typename Compare>
            list_iterator find_at_( aux_node_type * pHead, split_list::details::search_value_type<Q> const & val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(static_cast<list_node_type *>(pHead));
                return base_class::find_at_( h, val, cmp );
            }

            bool insert_aux_node( aux_node_type * pNode )
            {
                return base_class::insert_aux_node( pNode );
            }
            bool insert_aux_node( aux_node_type * pHead, aux_node_type * pNode )
            {
                bucket_head_type h(static_cast<list_node_type *>(pHead));
                return base_class::insert_aux_node( h, pNode );
            }

            template <typename Predicate>
            void erase_for( Predicate pred )
            {
                return base_class::erase_for( pred );
            }
        };
        //@endcond

    public:
        /// Initialize split-ordered list of default capacity
        /**
            The default capacity is defined in bucket table constructor.
            See split_list::expandable_bucket_table, split_list::static_ducket_table
            which selects by split_list::dynamic_bucket_table option.
        */
        SplitListSet()
            : m_nBucketCountLog2(1)
            , m_nMaxItemCount( max_item_count(2, m_Buckets.load_factor()))
        {
            init();
        }

        /// Initialize split-ordered list
        SplitListSet(
            size_t nItemCount           ///< estimate average of item count
            , size_t nLoadFactor = 1    ///< load factor - average item count per bucket. Small integer up to 10, default is 1.
            )
            : m_Buckets( nItemCount, nLoadFactor )
            , m_nBucketCountLog2(1)
            , m_nMaxItemCount( max_item_count(2, m_Buckets.load_factor()))
        {
            init();
        }

        /// Destroys split-list
        ~SplitListSet()
        {
            m_List.clear();
        }
    public:
        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert_( val ) != end();
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the set, then \p val is inserted
            iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refers to the same thing.

            The functor may change non-key fields of the \p item.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the list.

            @warning For \ref cds_intrusive_MichaelList_hp "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_hp "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            std::pair<iterator, bool> ret = update_( val, func, bAllowInsert );
            return std::make_pair( ret.first != end(), ret.second );
        }
        //@cond
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
            Otherwise, you may use \p contains( Q const&, Less pred ) functions with explicit predicate for key comparing.
        */
        template <typename Q>
        value_type * contains( Q const& key )
        {
            iterator it = find_( key );
            if ( it == end())
                return nullptr;
            return &*it;
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        value_type * find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        value_type * contains( Q const& key, Less pred )
        {
            iterator it = find_with_( key, pred );
            if ( it == end())
                return nullptr;
            return &*it;
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        value_type * find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds the key \p key
        /** \anchor cds_intrusive_SplitListSet_nogc_find_func
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

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return find_( key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return find_( key, key_comparator(), f );
        }
        //@endcond

        /// Finds the key \p key with \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_intrusive_SplitListSet_nogc_find_func "find(Q&, Func)"
            but \p cmp is used for key compare.
            \p Less has the interface like \p std::less.
            \p cmp must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_( key, typename ordered_list_adapter::template make_compare_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_( key, typename ordered_list_adapter::template make_compare_from_less<Less>(), f );
        }
        //@endcond


        /// Clears the set (non-atomic, not thread-safe)
        /**
            The function unlink all items from the set.
            The function is not atomic. It cleans up each bucket and then resets the item counter to zero.
            If there are a thread that performs insertion while \p %clear() is working the result is undefined in general case:
            <tt> empty() </tt> may return \p true but the set may contain item(s).
            Therefore, \p %clear() may be used only for debugging purposes.

            For each item the \p disposer is called after unlinking.
        */
        void clear()
        {
            m_List.erase_for( []( value_type const& val ) -> bool { return !node_traits::to_node_ptr( val )->is_dummy(); } );
            m_ItemCounter.reset();
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
            Thus, the correct item counting feature is an important part of split-list implementation.
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

        /// Returns internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }

        /// Returns internal statistics for \p OrderedList
        typename OrderedList::stat const& list_statistics() const
        {
            return m_List.statistics();
        }

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
            : public split_list::details::iterator_type<node_traits, ordered_list, IsConst>
        {
            typedef split_list::details::iterator_type<node_traits, ordered_list, IsConst> iterator_base_class;
            typedef typename iterator_base_class::list_iterator list_iterator;
        public:
            iterator_type()
                : iterator_base_class()
            {}

            iterator_type( iterator_type const& src )
                : iterator_base_class( src )
            {}

            // This ctor should be protected...
            iterator_type( list_iterator itCur, list_iterator itEnd )
                : iterator_base_class( itCur, itEnd )
            {}
        };
        //@endcond

    public:
    ///@name Forward iterators
    //@{
        /// Forward iterator
        /**
            The forward iterator for a split-list has some features:
            - it has no post-increment operator
            - it depends on iterator of underlying \p OrderedList
        */
        typedef iterator_type<false>    iterator;

        /// Const forward iterator
        /**
            For iterator's features and requirements see \ref iterator
        */
        typedef iterator_type<true>     const_iterator;

        /// Returns a forward iterator addressing the first element in a split-list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( m_List.begin(), m_List.end());
        }

        /// Returns an iterator that addresses the location succeeding the last element in a split-list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.

            The returned value can be used only to control reaching the end of the split-list.
            For empty list \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( m_List.end(), m_List.end());
        }

        /// Returns a forward const iterator addressing the first element in a split-list
        const_iterator begin() const
        {
            return const_iterator( m_List.begin(), m_List.end());
        }

        /// Returns a forward const iterator addressing the first element in a split-list
        const_iterator cbegin() const
        {
            return const_iterator( m_List.cbegin(), m_List.cend());
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a split-list
        const_iterator end() const
        {
            return const_iterator( m_List.end(), m_List.end());
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a split-list
        const_iterator cend() const
        {
            return const_iterator( m_List.cend(), m_List.cend());
        }
    //@}

    protected:
        //@cond
        iterator insert_( value_type& val )
        {
            size_t nHash = hash_value( val );
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            node_traits::to_node_ptr( val )->m_nHash = split_list::regular_hash<bit_reversal>( nHash );

            list_iterator it = m_List.insert_at_( pHead, val );
            if ( it != m_List.end()) {
                inc_item_count();
                m_Stat.onInsertSuccess();
                return iterator( it, m_List.end());
            }
            m_Stat.onInsertFailed();
            return end();
        }

        template <typename Func>
        std::pair<iterator, bool> update_( value_type& val, Func func, bool bAllowInsert )
        {
            size_t nHash = hash_value( val );
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            node_traits::to_node_ptr( val )->m_nHash = split_list::regular_hash<bit_reversal>( nHash );

            std::pair<list_iterator, bool> ret = m_List.update_at_( pHead, val, func, bAllowInsert );
            if ( ret.first != m_List.end()) {
                if ( ret.second ) {
                    inc_item_count();
                    m_Stat.onUpdateNew();
                }
                else
                    m_Stat.onUpdateExist();
                return std::make_pair( iterator(ret.first, m_List.end()), ret.second );
            }
            return std::make_pair( end(), ret.second );
        }

        template <typename Q, typename Less >
        iterator find_with_( Q& val, Less pred )
        {
            CDS_UNUSED( pred );
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            auto it = m_List.find_at_( pHead, sv, typename ordered_list_adapter::template make_compare_from_less<Less>());
            m_Stat.onFind( it != m_List.end());
            return iterator( it, m_List.end());
        }

        template <typename Q>
        iterator find_( Q const& val )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            auto it = m_List.find_at_( pHead, sv, key_comparator());
            m_Stat.onFind( it != m_List.end());
            return iterator( it, m_List.end());
        }

        template <typename Q, typename Compare, typename Func>
        bool find_( Q& val, Compare cmp, Func f )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );
            return m_Stat.onFind( m_List.find_at( pHead, sv, cmp,
                [&f](value_type& item, split_list::details::search_value_type<Q>& v){ f(item, v.val ); }));
        }

        aux_node_type * alloc_aux_node( size_t nHash )
        {
            m_Stat.onHeadNodeAllocated();
            aux_node_type* p = m_Buckets.alloc_aux_node();
            if ( p )
                p->m_nHash = nHash;
            return p;
        }

        void free_aux_node( aux_node_type * p )
        {
            m_Buckets.free_aux_node( p );
            m_Stat.onHeadNodeFreed();
        }

        /// Calculates hash value of \p key
        template <typename Q>
        size_t hash_value( Q const& key ) const
        {
            return m_HashFunctor( key );
        }

        size_t bucket_no( size_t nHash ) const
        {
            return nHash & ((1 << m_nBucketCountLog2.load( memory_model::memory_order_relaxed )) - 1);
        }

        static size_t parent_bucket( size_t nBucket )
        {
            assert( nBucket > 0 );
            return nBucket & ~(1 << bitop::MSBnz( nBucket ));
        }

        aux_node_type * init_bucket( size_t const nBucket )
        {
            assert( nBucket > 0 );
            size_t nParent = parent_bucket( nBucket );

            aux_node_type * pParentBucket = m_Buckets.bucket( nParent );
            if ( pParentBucket == nullptr ) {
                pParentBucket = init_bucket( nParent );
                m_Stat.onRecursiveInitBucket();
            }

            assert( pParentBucket != nullptr );

            // Allocate an aux node for new bucket
            aux_node_type * pBucket = m_Buckets.bucket( nBucket );

            back_off bkoff;
            for ( ;; pBucket = m_Buckets.bucket( nBucket )) {
                if ( pBucket )
                    return pBucket;

                pBucket = alloc_aux_node( split_list::dummy_hash<bit_reversal>( nBucket ));
                if ( pBucket ) {
                    if ( m_List.insert_aux_node( pParentBucket, pBucket )) {
                        m_Buckets.bucket( nBucket, pBucket );
                        m_Stat.onNewBucket();
                        return pBucket;
                    }

                    // Another thread set the bucket. Wait while it done
                    free_aux_node( pBucket );
                    m_Stat.onBucketInitContenton();
                    break;
                }

                // There are no free buckets. It means that the bucket table is full
                // Wait while another thread set the bucket or a free bucket will be available
                m_Stat.onBucketsExhausted();
                bkoff();
            }

            // Another thread set the bucket. Wait while it done
            for ( pBucket = m_Buckets.bucket( nBucket ); pBucket == nullptr; pBucket = m_Buckets.bucket( nBucket )) {
                bkoff();
                m_Stat.onBusyWaitBucketInit();
            }

            return pBucket;
        }

        aux_node_type * get_bucket( size_t nHash )
        {
            size_t nBucket = bucket_no( nHash );

            aux_node_type * pHead = m_Buckets.bucket( nBucket );
            if ( pHead == nullptr )
                pHead = init_bucket( nBucket );

            assert( pHead->is_dummy());

            return pHead;
        }

        void init()
        {
            // Initialize bucket 0
            aux_node_type * pNode = alloc_aux_node( 0 /*split_list::dummy_hash<bit_reversal>(0)*/ );

            // insert_aux_node cannot return false for empty list
            CDS_VERIFY( m_List.insert_aux_node( pNode ));

            m_Buckets.bucket( 0, pNode );
        }

        static size_t max_item_count( size_t nBucketCount, size_t nLoadFactor )
        {
            return nBucketCount * nLoadFactor;
        }

        void inc_item_count()
        {
            size_t nMaxCount = m_nMaxItemCount.load( memory_model::memory_order_relaxed );
            if ( ++m_ItemCounter <= nMaxCount )
                return;

            size_t sz = m_nBucketCountLog2.load( memory_model::memory_order_relaxed );
            const size_t nBucketCount = static_cast<size_t>(1) << sz;
            if ( nBucketCount < m_Buckets.capacity()) {
                // we may grow the bucket table
                const size_t nLoadFactor = m_Buckets.load_factor();
                if ( nMaxCount < max_item_count( nBucketCount, nLoadFactor ))
                    return; // someone already have updated m_nBucketCountLog2, so stop here

                m_nMaxItemCount.compare_exchange_strong( nMaxCount, max_item_count( nBucketCount << 1, nLoadFactor ),
                    memory_model::memory_order_relaxed, atomics::memory_order_relaxed );
                m_nBucketCountLog2.compare_exchange_strong( sz, sz + 1, memory_model::memory_order_relaxed, atomics::memory_order_relaxed );
            }
            else
                m_nMaxItemCount.store( std::numeric_limits<size_t>::max(), memory_model::memory_order_relaxed );
        }
        //@endcond

    protected:
        //@cond
        static unsigned const c_padding = cds::opt::actual_padding< traits::padding >::value;

        typedef typename cds::details::type_padding< bucket_table, c_padding >::type padded_bucket_table;
        padded_bucket_table     m_Buckets;          ///< bucket table

        typedef typename cds::details::type_padding< ordered_list_wrapper, c_padding >::type padded_ordered_list;
        padded_ordered_list     m_List;             ///< Ordered list containing split-list items

        atomics::atomic<size_t> m_nBucketCountLog2; ///< log2( current bucket count )
        atomics::atomic<size_t> m_nMaxItemCount;    ///< number of items container can hold, before we have to resize
        hash                    m_HashFunctor;      ///< Hash functor
        item_counter            m_ItemCounter;      ///< Item counter
        stat                    m_Stat;             ///< Internal statistics
        //@endcond
    };

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_SPLIT_LIST_NOGC_H

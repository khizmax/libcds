// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_SPLIT_LIST_RCU_H
#define CDSLIB_INTRUSIVE_SPLIT_LIST_RCU_H

#include <limits>

#include <cds/intrusive/details/split_list_base.h>
#include <cds/details/binary_functor_wrapper.h>
#include <cds/details/type_padding.h>

namespace cds { namespace intrusive {

    /// Split-ordered list RCU specialization
    /** @ingroup cds_intrusive_map
        \anchor cds_intrusive_SplitListSet_rcu

        Hash table implementation based on split-ordered list algorithm discovered by Ori Shalev and Nir Shavit, see
        - [2003] Ori Shalev, Nir Shavit "Split-Ordered Lists - Lock-free Resizable Hash Tables"
        - [2008] Nir Shavit "The Art of Multiprocessor Programming"

        The split-ordered list is a lock-free implementation of an extensible unbounded hash table. It uses original
        recursive split-ordering algorithm discovered by Ori Shalev and Nir Shavit that allows to split buckets
        without moving an item on resizing, see \ref cds_SplitList_algo_desc "short algo description".

        <b>Implementation</b>

        Template parameters are:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p OrderedList - ordered list implementation used as bucket for hash set, for example, MichaelList, LazyList.
            The intrusive ordered list implementation specifies the type \p T stored in the hash-set,
            the comparing functor for the type \p T and other features specific for the ordered list.
        - \p Traits - set traits, default isd \p split_list::traits.
            Instead of defining \p Traits struct you can use option-based syntax provided by \p split_list::make_traits metafunction.

        @note About required features of hash functor see \ref cds_SplitList_hash_functor "SplitList general description".

        \par How to use
        Before including <tt><cds/intrusive/split_list_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.
        For example, for \ref cds_urcu_general_buffered_gc "general-purpose buffered RCU" and
        MichaelList-based split-list you should include:
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/intrusive/michael_list_rcu.h>
        #include <cds/intrusive/split_list_rcu.h>

        // Declare Michael's list for type Foo and default traits:
        typedef cds::intrusive::MichaelList< cds::urcu::gc< cds::urcu::general_buffered<> >, Foo > rcu_michael_list;

        // Declare split-list based on rcu_michael_list
        typedef cds::intrusive::SplitListSet< cds::urcu::gc< cds::urcu::general_buffered<> >, rcu_michael_list > rcu_split_list;
        \endcode

    */
    template <
        class RCU,
        class OrderedList,
#   ifdef CDS_DOXYGEN_INVOKED
        class Traits = split_list::traits
#   else
        class Traits
#   endif
    >
    class SplitListSet< cds::urcu::gc< RCU >, OrderedList, Traits >
    {
    public:
        typedef cds::urcu::gc< RCU > gc;   ///< RCU garbage collector
        typedef Traits           traits;   ///< Traits template parameters

        /// Hash functor for \ref value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type   hash;

    protected:
        //@cond
        typedef split_list::details::rebind_list_traits<OrderedList, traits> ordered_list_adapter;
        //@endcond

    public:
#   ifdef CDS_DOXYGEN_INVOKED
        typedef OrderedList         ordered_list;   ///< type of ordered list used as base for split-list
#   else
        typedef typename ordered_list_adapter::result    ordered_list;
#   endif
        typedef typename ordered_list::value_type     value_type;     ///< type of value stored in the split-list
        typedef typename ordered_list::key_comparator key_comparator; ///< key compare functor
        typedef typename ordered_list::disposer       disposer;       ///< Node disposer functor
        typedef typename ordered_list::rcu_lock       rcu_lock;       ///< RCU scoped lock
        typedef typename ordered_list::exempt_ptr     exempt_ptr;     ///< pointer to extracted node
        typedef typename ordered_list::raw_ptr        raw_ptr;        ///< pointer to the node for \p get() function
        /// Group of \p extract_xxx functions require external locking if underlying ordered list requires that
        static constexpr const bool c_bExtractLockExternal = ordered_list::c_bExtractLockExternal;

        typedef typename traits::bit_reversal bit_reversal; ///< Bit reversal algorithm, see \p split_list::traits::bit_reversal
        typedef typename traits::item_counter item_counter; ///< Item counter type
        typedef typename traits::back_off     back_off;     ///< back-off strategy for spinning
        typedef typename traits::memory_model memory_model; ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::stat         stat;         ///< Internal statistics

        // GC and OrderedList::gc must be the same
        static_assert( std::is_same<gc, typename ordered_list::gc>::value, "GC and OrderedList::gc must be the same");

        // atomicity::empty_item_counter is not allowed as a item counter
        static_assert( !std::is_same<item_counter, cds::atomicity::empty_item_counter>::value,
                        "cds::atomicity::empty_item_counter is not allowed as a item counter");

    protected:
        //@cond
        typedef typename ordered_list::node_type    list_node_type;  ///< Node type as declared in ordered list
        typedef split_list::node<list_node_type>    node_type;       ///< split-list node type

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

        typedef typename bucket_table::aux_node_type aux_node_type; ///< auxiliary node type

        //@endcond

    protected:
        //@cond
        /// Ordered list wrapper to access protected members of OrderedList
        class ordered_list_wrapper: public ordered_list
        {
            typedef ordered_list base_class;
            typedef typename base_class::auxiliary_head bucket_head_type;

        public:
            bool insert_at( aux_node_type * pHead, value_type& val )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::insert_at( h, val );
            }

            template <typename Func>
            bool insert_at( aux_node_type * pHead, value_type& val, Func f )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::insert_at( h, val, f );
            }

            template <typename Func>
            std::pair<bool, bool> update_at( aux_node_type * pHead, value_type& val, Func func, bool bAllowInsert )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::update_at( h, val, func, bAllowInsert );
            }

            bool unlink_at( aux_node_type * pHead, value_type& val )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::unlink_at( h, val );
            }

            template <typename Q, typename Compare, typename Func>
            bool erase_at( aux_node_type * pHead, split_list::details::search_value_type<Q> const& val, Compare cmp, Func f )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::erase_at( h, val, cmp, f );
            }

            template <typename Q, typename Compare>
            bool erase_at( aux_node_type * pHead, split_list::details::search_value_type<Q> const& val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::erase_at( h, val, cmp );
            }

            template <typename Q, typename Compare>
            value_type * extract_at( aux_node_type * pHead, split_list::details::search_value_type<Q>& val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::extract_at( h, val, cmp );
            }

            template <typename Q, typename Compare, typename Func>
            bool find_at( aux_node_type * pHead, split_list::details::search_value_type<Q>& val, Compare cmp, Func f )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::find_at( h, val, cmp, f );
            }

            template <typename Q, typename Compare>
            bool find_at( aux_node_type * pHead, split_list::details::search_value_type<Q> const & val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::find_at( h, val, cmp );
            }

            template <typename Q, typename Compare>
            raw_ptr get_at( aux_node_type * pHead, split_list::details::search_value_type<Q>& val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::get_at( h, val, cmp );
            }

            bool insert_aux_node( aux_node_type * pNode )
            {
                return base_class::insert_aux_node( pNode );
            }
            bool insert_aux_node( aux_node_type * pHead, aux_node_type * pNode )
            {
                bucket_head_type h(pHead);
                return base_class::insert_aux_node( h, pNode );
            }
        };

        template <typename Less>
        struct less_wrapper: public cds::opt::details::make_comparator_from_less<Less>
        {
            typedef cds::opt::details::make_comparator_from_less<Less> base_wrapper;

            template <typename Q1, typename Q2>
            int operator()( split_list::details::search_value_type<Q1> const& v1, Q2 const& v2 ) const
            {
                return base_wrapper::operator()( v1.val, v2 );
            }

            template <typename Q1, typename Q2>
            int operator()( Q1 const& v1, split_list::details::search_value_type<Q2> const& v2 ) const
            {
                return base_wrapper::operator()( v1, v2.val );
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
            , size_t nLoadFactor = 1    ///< load factor - average item count per bucket. Small integer up to 8, default is 1.
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
            gc::force_dispose();
        }

    public:
        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            The function makes RCU lock internally.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            size_t nHash = hash_value( val );
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            node_traits::to_node_ptr( val )->m_nHash = split_list::regular_hash<bit_reversal>( nHash );

            if ( m_List.insert_at( pHead, val )) {
                inc_item_count();
                m_Stat.onInsertSuccess();
                return true;
            }
            m_Stat.onInsertFailed();
            return false;
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

            The function makes RCU lock internally.

            @warning For \ref cds_intrusive_MichaelList_rcu "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_rcu "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
            */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            size_t nHash = hash_value( val );
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            node_traits::to_node_ptr( val )->m_nHash = split_list::regular_hash<bit_reversal>( nHash );

            if ( m_List.insert_at( pHead, val, f )) {
                inc_item_count();
                m_Stat.onInsertSuccess();
                return true;
            }
            m_Stat.onInsertFailed();
            return false;
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
            refers to the same stuff.

            The functor may change non-key fields of the \p item.

            The function applies RCU lock internally.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the list.

            @warning For \ref cds_intrusive_MichaelList_rcu "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_rcu "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            size_t nHash = hash_value( val );
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            node_traits::to_node_ptr( val )->m_nHash = split_list::regular_hash<bit_reversal>( nHash );

            std::pair<bool, bool> bRet = m_List.update_at( pHead, val, func, bAllowInsert );
            if ( bRet.first && bRet.second ) {
                inc_item_count();
                m_Stat.onUpdateNew();
            }
            else
                m_Stat.onUpdateExist();
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
            The function searches the item \p val in the set and unlinks it from the set
            if it is found and is equal to \p val.

            Difference between \ref erase and \p unlink functions: \p erase finds <i>a key</i>
            and deletes the item found. \p unlink finds an item by key and deletes it
            only if \p val is an item of that set, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            RCU \p synchronize method can be called, therefore, RCU should not be locked.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            size_t nHash = hash_value( val );
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            if ( m_List.unlink_at( pHead, val )) {
                --m_ItemCounter;
                m_Stat.onEraseSuccess();
                return true;
            }
            m_Stat.onEraseFailed();
            return false;
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_SplitListSet_rcu_erase
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns \p true.
            If the item with key equal to \p key is not found the function return \p false.

            Difference between \ref erase and \p unlink functions: \p erase finds <i>a key</i>
            and deletes the item found. \p unlink finds an item by key and deletes it
            only if \p key is an item of that set, i.e. the pointer to item found
            is equal to <tt> &key </tt>.

            RCU \p synchronize method can be called, therefore, RCU should not be locked.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_( key, key_comparator());
        }

        /// Deletes the item from the set using \p pred for searching
        /**
            The function is an analog of \ref cds_intrusive_SplitListSet_rcu_erase "erase(Q const&)"
            but \p cmp is used for key compare.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_( key, typename ordered_list_adapter::template make_compare_from_less<Less>());
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_SplitListSet_rcu_erase_func
            The function searches an item with key equal to \p key in the set,
            call \p f functor with item found, unlinks it from the set, and returns \p true.
            The \ref disposer specified by \p OrderedList class template parameter is called
            by garbage collector \p GC asynchronously.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode

            If the item with key equal to \p key is not found the function return \p false.

            RCU \p synchronize method can be called, therefore, RCU should not be locked.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return erase_( key, key_comparator(), f );
        }

        /// Deletes the item from the set using \p pred for searching
        /**
            The function is an analog of \ref cds_intrusive_SplitListSet_rcu_erase_func "erase(Q const&, Func)"
            but \p cmp is used for key compare.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_( key, typename ordered_list_adapter::template make_compare_from_less<Less>(), f );
        }

        /// Extracts an item from the set
        /** \anchor cds_intrusive_SplitListSet_rcu_extract
            The function searches an item with key equal to \p key in the set,
            unlinks it, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item with the key equal to \p key is not found the function returns an empty \p exempt_ptr.

            Depends on \p bucket_type you should or should not lock RCU before calling of this function:
            - for the set based on \ref cds_intrusive_MichaelList_rcu "MichaelList" RCU should not be locked
            - for the set based on \ref cds_intrusive_LazyList_rcu "LazyList" RCU should be locked
            See ordered list implementation for details.

            \code
            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::intrusive::MichaelList< rcu, Foo > rcu_michael_list;
            typedef cds::intrusive::SplitListSet< rcu, rcu_michael_list, foo_traits > rcu_splitlist_set;

            rcu_splitlist_set theSet;
            // ...

            rcu_splitlist_set::exempt_ptr p;

            // For MichaelList we should not lock RCU

            // Now, you can apply extract function
            // Note that you must not delete the item found inside the RCU lock
            p = theList.extract( 10 );
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
            return exempt_ptr(extract_( key, key_comparator()));
        }

        /// Extracts an item from the set using \p pred for searching
        /**
            The function is an analog of \p extract(Q const&) but \p pred is used for key compare.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            return exempt_ptr( extract_with_( key, pred ));
        }

        /// Finds the key \p key
        /** \anchor cds_intrusive_SplitListSet_rcu_find_func
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor can change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p key argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function applies RCU lock internally.

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
            The function is an analog of \ref cds_intrusive_SplitListSet_rcu_find_func "find(Q&, Func)"
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

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
            Otherwise, you may use \p contains( Q const&, Less pred ) functions with explicit predicate for key comparing.
        */
        template <typename Q>
        bool contains( Q const& key )
        {
            return find_value( key, key_comparator());
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_value( key, typename ordered_list_adapter::template make_compare_from_less<Less>());
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds the key \p key and return the item found
        /** \anchor cds_intrusive_SplitListSet_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::intrusive::SplitListSet< your_template_parameters > set_class;
            set_class theSet;
            // ...
            typename set_class::raw_ptr rp;
            {
                // Lock RCU
                hash_set::rcu_lock lock;

                rp = theSet.get( 5 );
                if ( rp ) {
                    // Deal with rp
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // rp can be retired by disposer at any time after RCU has been unlocked
            }
            \endcode
        */
        template <typename Q>
        raw_ptr get( Q const& key )
        {
            return get_( key, key_comparator());
        }

        /// Finds the key \p key and return the item found
        /**
            The function is an analog of \ref cds_intrusive_SplitListSet_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        raw_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return get_( key, typename ordered_list_adapter::template make_compare_from_less<Less>());
        }


        /// Returns item count in the set
        size_t size() const
        {
            return m_ItemCounter;
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
            Thus, the correct item counting feature is an important part of split-list set implementation.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Clears the set (not atomic)
        void clear()
        {
            iterator it = begin();
            while ( it != end()) {
                iterator i(it);
                ++i;
                unlink( *it );
                it = i;
            }
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
            :public split_list::details::iterator_type<node_traits, ordered_list, IsConst>
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
    ///@name Forward iterators (thread-safe under RCU lock)
    //@{
        /// Forward iterator
        /**
            The forward iterator for a split-list has some features:
            - it has no post-increment operator
            - it depends on iterator of underlying \p OrderedList

            You may safely use iterators in multi-threaded environment only under RCU lock.
            Otherwise, a crash is possible if another thread deletes the element the iterator points to.
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
            return cbegin();
        }
        /// Returns a forward const iterator addressing the first element in a split-list
        const_iterator cbegin() const
        {
            return const_iterator( m_List.cbegin(), m_List.cend());
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a split-list
        const_iterator end() const
        {
            return cend();
        }
        /// Returns an const iterator that addresses the location succeeding the last element in a split-list
        const_iterator cend() const
        {
            return const_iterator( m_List.cend(), m_List.cend());
        }
    //@}

    protected:
        //@cond
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
            return nHash & ( (1 << m_nBucketCountLog2.load(memory_model::memory_order_relaxed)) - 1 );
        }

        static size_t parent_bucket( size_t nBucket )
        {
            assert( nBucket > 0 );
            return nBucket & ~( 1 << bitop::MSBnz( nBucket ));
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
            size_t nMaxCount = m_nMaxItemCount.load(memory_model::memory_order_relaxed);
            if ( ++m_ItemCounter <= nMaxCount )
                return;

            size_t sz = m_nBucketCountLog2.load(memory_model::memory_order_relaxed);
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

        template <typename Q, typename Compare>
        bool find_value( Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            return m_Stat.onFind( m_List.find_at( pHead, sv, cmp ));
        }

        template <typename Q, typename Compare>
        raw_ptr get_( Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            raw_ptr p = m_List.get_at( pHead, sv, cmp );
            m_Stat.onFind( !!p );
            return p;
        }

        template <typename Q, typename Compare>
        value_type * extract_( Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            value_type * pNode = m_List.extract_at( pHead, sv, cmp );
            if ( pNode ) {
                --m_ItemCounter;
                m_Stat.onExtractSuccess();
            }
            else
                m_Stat.onExtractFailed();
            return pNode;
        }

        template <typename Q, typename Less>
        value_type * extract_with_( Q const& val, Less pred )
        {
            CDS_UNUSED( pred );
            return extract_( val, typename ordered_list_adapter::template make_compare_from_less<Less>());
        }

        template <typename Q, typename Compare>
        bool erase_( const Q& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            if ( m_List.erase_at( pHead, sv, cmp )) {
                --m_ItemCounter;
                m_Stat.onEraseSuccess();
                return true;
            }
            m_Stat.onEraseFailed();
            return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_( Q const& val, Compare cmp, Func f )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            if ( m_List.erase_at( pHead, sv, cmp, f )) {
                --m_ItemCounter;
                m_Stat.onEraseSuccess();
                return true;
            }
            m_Stat.onEraseFailed();
            return false;
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
        stat                    m_Stat;             ///< Internal statistics accumulator
        //@endcond
    };

}}  // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_SPLIT_LIST_RCU_H

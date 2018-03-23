// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_SPLIT_LIST_H
#define CDSLIB_INTRUSIVE_SPLIT_LIST_H

#include <limits>
#include <cds/intrusive/details/split_list_base.h>
#include <cds/details/type_padding.h>

namespace cds { namespace intrusive {

    /// Split-ordered list
    /** @ingroup cds_intrusive_map
        \anchor cds_intrusive_SplitListSet_hp

        Hash table implementation based on split-ordered list algorithm discovered by Ori Shalev and Nir Shavit, see
        - [2003] Ori Shalev, Nir Shavit "Split-Ordered Lists - Lock-free Resizable Hash Tables"
        - [2008] Nir Shavit "The Art of Multiprocessor Programming"

        The split-ordered list is a lock-free implementation of an extensible unbounded hash table. It uses original
        recursive split-ordering algorithm discovered by Ori Shalev and Nir Shavit that allows to split buckets
        without item moving on resizing.

        \anchor cds_SplitList_algo_desc
        <b>Short description</b>
        [from [2003] Ori Shalev, Nir Shavit "Split-Ordered Lists - Lock-free Resizable Hash Tables"]

        The algorithm keeps all the items in one lock-free linked list, and gradually assigns the bucket pointers to
        the places in the list where a sublist of 'correct' items can be found. A bucket is initialized upon first
        access by assigning it to a new 'dummy' node (dashed contour) in the list, preceding all items that should be
        in that bucket. A newly created bucket splits an older bucket's chain, reducing the access cost to its items. The
        table uses a modulo 2**i hash (there are known techniques for 'pre-hashing' before a modulo 2**i hash
        to overcome possible binary correlations among values). The table starts at size 2 and repeatedly doubles in size.

        Unlike moving an item, the operation of directing a bucket pointer can be done
        in a single CAS operation, and since items are not moved, they are never 'lost'.
        However, to make this approach work, one must be able to keep the items in the
        list sorted in such a way that any bucket's sublist can be 'split' by directing a new
        bucket pointer within it. This operation must be recursively repeatable, as every
        split bucket may be split again and again as the hash table grows. To achieve this
        goal the authors introduced recursive split-ordering, a new ordering on keys that keeps items
        in a given bucket adjacent in the list throughout the repeated splitting process.

        Magically, yet perhaps not surprisingly, recursive split-ordering is achieved by
        simple binary reversal: reversing the bits of the hash key so that the new key's
        most significant bits (MSB) are those that were originally its least significant.
        The split-order keys of regular nodes are exactly the bit-reverse image of the original
        keys after turning on their MSB. For example, items 9 and 13 are in the <tt>1 mod
        4</tt> bucket, which can be recursively split in two by inserting a new node between
        them.

        To insert (respectively delete or search for) an item in the hash table, hash its
        key to the appropriate bucket using recursive split-ordering, follow the pointer to
        the appropriate location in the sorted items list, and traverse the list until the key's
        proper location in the split-ordering (respectively until the key or a key indicating
        the item is not in the list is found). Because of the combinatorial structure induced
        by the split-ordering, this will require traversal of no more than an expected constant number of items.

        The design is modular: to implement the ordered items list, you can use one of several
        non-blocking list-based set algorithms: MichaelList, LazyList.

        <b>Implementation</b>

        Template parameters are:
        - \p GC - Garbage collector. Note the \p GC must be the same as the \p GC used for \p OrderedList
        - \p OrderedList - ordered list implementation used as a bucket for hash set, for example, \p MichaelList, \p LazyList.
            The intrusive ordered list implementation specifies the type \p T stored in the split-list set, the comparison
            functor for the type \p T and other features specific for the ordered list.
        - \p Traits - split-list traits, default is \p split_list::traits.
            Instead of defining \p Traits struct you can use option-based syntax provided by \p split_list::make_traits metafunction.

        There are several specialization of the split-list class for different \p GC:
        - for \ref cds_urcu_gc "RCU type" include <tt><cds/intrusive/split_list_rcu.h></tt> - see
            \ref cds_intrusive_SplitListSet_rcu "RCU-based split-list"
        - for cds::gc::nogc include <tt><cds/intrusive/split_list_nogc.h></tt> - see
            \ref cds_intrusive_SplitListSet_nogc "persistent SplitListSet".

        \anchor cds_SplitList_hash_functor
        <b>Hash functor</b>

        Some member functions of split-ordered list accept the key parameter of type \p Q which differs from \p value_type.
        It is expected that type \p Q contains full key of \p value_type, and for equal keys of type \p Q and \p value_type
        the hash values of these keys must be equal too.
        The hash functor \p Traits::hash should accept parameters of both type:
        \code
        // Our node type
        struct Foo {
            std::string     key_    ;   // key field
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

        Split-list based on \p IterableList differs from split-list based on \p MichaelList or \p LazyList
        because \p %IterableList stores data "as is" - it cannot use any hook.

        Suppose, your split-list contains values of type \p Foo.
        For \p %MichaelList and \p %LazyList, \p Foo declaration should be based on ordered-list node:
        - \p %MichaelList:
        \code
        struct Foo: public cds::intrusive::split_list::node< cds::intrusive::michael_list::node< cds::gc::HP > >
        {
            // ... field declarations
        };
        \endcode
        - \p %LazyList:
        \code
        struct Foo: public cds::intrusive::split_list::node< cds::intrusive::lazy_list::node< cds::gc::HP > >
        {
            // ... field declarations
        };
        \endcode

        For \p %IterableList, \p Foo should be based on \p void:
        \code
        struct Foo: public cds::intrusive::split_list::node<void>
        {
            // ... field declarations
        };
        \endcode

        Everything else is the same.
        Consider split-list based on \p MichaelList.

        First, you should choose ordered list type to use in your split-list set:
        \code
        // For gc::HP-based MichaelList implementation
        #include <cds/intrusive/michael_list_hp.h>

        // cds::intrusive::SplitListSet declaration
        #include <cds/intrusive/split_list.h>

        // Type of set items
            //  Note you should declare your struct based on cds::intrusive::split_list::node
            //  which is a wrapper for ordered-list node struct.
            //  In our case, the node type for HP-based MichaelList is cds::intrusive::michael_list::node< cds::gc::HP >
        struct Foo: public cds::intrusive::split_list::node< cds::intrusive::michael_list::node< cds::gc::HP > >
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

        // Declare base ordered-list type for split-list
        typedef cds::intrusive::MichaelList< cds::gc::HP, Foo,
            typename cds::intrusive::michael_list::make_traits<
                // hook option
                cds::intrusive::opt::hook< cds::intrusive::michael_list::base_hook< cds::opt::gc< cds::gc::HP > > >
                // item comparator option
                ,cds::opt::compare< FooCmp >
            >::type
        >  Foo_list;
        \endcode

        Second, you should declare split-list set container:
        \code

        // Declare hash functor
        // Note, the hash functor accepts parameter type Foo and std::string
        struct FooHash {
            size_t operator()( const Foo& f ) const
            {
                return cds::opt::v::hash<std::string>()( f.key_ );
            }
            size_t operator()( const std::string& s ) const
            {
                return cds::opt::v::hash<std::string>()( s );
            }
        };

        // Split-list set typedef
        typedef cds::intrusive::SplitListSet<
            cds::gc::HP
            ,Foo_list
            ,typename cds::intrusive::split_list::make_traits<
                cds::opt::hash< FooHash >
            >::type
        > Foo_set;
        \endcode

        Now, you can use \p Foo_set in your application.
        \code
            Foo_set    fooSet;
            Foo * foo = new Foo;
            foo->key_ = "First";

            fooSet.insert( *foo );

            // and so on ...
        \endcode
    */
    template <
        class GC,
        class OrderedList,
#   ifdef CDS_DOXYGEN_INVOKED
        class Traits = split_list::traits
#   else
        class Traits
#   endif
    >
    class SplitListSet
    {
    public:
        typedef GC     gc;     ///< Garbage collector
        typedef Traits traits; ///< Set traits

    protected:
        //@cond
        typedef split_list::details::rebind_list_traits<OrderedList, traits> ordered_list_adapter;
        //@endcond

    public:
#   ifdef CDS_DOXYGEN_INVOKED
        typedef OrderedList         ordered_list;   ///< type of ordered list used as a base for split-list
#   else
        typedef typename ordered_list_adapter::result   ordered_list;
#   endif
        typedef typename ordered_list::value_type       value_type;     ///< type of value stored in the split-list
        typedef typename ordered_list::key_comparator   key_comparator; ///< key comparison functor
        typedef typename ordered_list::disposer         disposer;       ///< Node disposer functor

        /// Hash functor for \p %value_type and all its derivatives you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;

        typedef typename traits::bit_reversal      bit_reversal; ///< Bit reversal algorithm, see \p split_list::traits::bit_reversal
        typedef typename traits::item_counter      item_counter; ///< Item counter type
        typedef typename traits::back_off          back_off;     ///< back-off strategy for spinning
        typedef typename traits::memory_model      memory_model; ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename traits::stat              stat;         ///< Internal statistics, see \p spit_list::stat
        typedef typename ordered_list::guarded_ptr guarded_ptr;  ///< Guarded pointer

        /// Count of hazard pointer required
        static constexpr const size_t c_nHazardPtrCount = ordered_list::c_nHazardPtrCount + 4; // +4 - for iterators

    protected:
        //@cond
        typedef split_list::node<typename ordered_list_adapter::ordered_list_node_type> node_type; ///< split-list node type
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

        typedef typename bucket_table::aux_node_type aux_node_type;   ///< auxiliary node type
        //@endcond

    protected:
        //@cond
        /// Ordered list wrapper to access protected members
        class ordered_list_wrapper: public ordered_list
        {
            typedef ordered_list base_class;
            typedef typename base_class::auxiliary_head bucket_head_type;

        public:
            bool insert_at( aux_node_type* pHead, value_type& val )
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

            template <typename Q>
            typename std::enable_if<
                std::is_same< Q, value_type>::value && is_iterable_list< ordered_list >::value,
                std::pair<bool, bool>
            >::type
            upsert_at( aux_node_type * pHead, Q& val, bool bAllowInsert )
            {
                assert( pHead != nullptr );
                bucket_head_type h( pHead );
                return base_class::upsert_at( h, val, bAllowInsert );
            }

            bool unlink_at( aux_node_type * pHead, value_type& val )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::unlink_at( h, val );
            }

            template <typename Iterator>
            typename std::enable_if<
                std::is_same< Iterator, typename ordered_list::iterator>::value && is_iterable_list< ordered_list >::value,
                bool
            >::type
            erase_at( Iterator iter )
            {
                return base_class::erase_at( iter );
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
            guarded_ptr extract_at( aux_node_type * pHead, split_list::details::search_value_type<Q> const& val, Compare cmp )
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
            bool find_at( aux_node_type * pHead, split_list::details::search_value_type<Q> const& val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::find_at( h, val, cmp );
            }

            template <typename Q, typename Compare>
            typename std::enable_if<
                std::is_same<Q, Q>::value && is_iterable_list< ordered_list >::value,
                typename base_class::iterator
            >::type
            find_iterator_at( aux_node_type * pHead, split_list::details::search_value_type<Q> const& val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h( pHead );
                return base_class::find_iterator_at( h, val, cmp );
            }

            template <typename Q, typename Compare>
            guarded_ptr get_at( aux_node_type * pHead, split_list::details::search_value_type<Q> const& val, Compare cmp )
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

            template <typename Predicate>
            void destroy( Predicate pred )
            {
                base_class::destroy( pred );
            }
        };
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
            : public split_list::details::iterator_type<node_traits, ordered_list, IsConst>
        {
            typedef split_list::details::iterator_type<node_traits, ordered_list, IsConst> iterator_base_class;
            typedef typename iterator_base_class::list_iterator list_iterator;

            friend class SplitListSet;

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
            The forward iterator is based on \p OrderedList forward iterator and has some features:
            - it has no post-increment operator
            - it iterates items in unordered fashion
            - iterator cannot be moved across thread boundary because it may contain GC's guard that is thread-private GC data.

            Iterator thread safety depends on type of \p OrderedList:
            - for \p MichaelList and \p LazyList: iterator guarantees safety even if you delete the item that iterator points to
              because that item is guarded by hazard pointer.
              However, in case of concurrent deleting operations it is no guarantee that you iterate all item in the set.
              Moreover, a crash is possible when you try to iterate the next element that has been deleted by concurrent thread.
              Use this iterator on the concurrent container for debugging purpose only.
            - for \p IterableList: iterator is thread-safe. You may use it freely in concurrent environment.
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

    public:
        /// Initialize split-ordered list of default capacity
        /**
            The default capacity is defined in bucket table constructor.
            See \p split_list::expandable_bucket_table, \p split_list::static_bucket_table
            which selects by \p split_list::dynamic_bucket_table option.
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

        /// Destroys split-list set
        ~SplitListSet()
        {
            // list contains aux node that cannot be retired
            // all aux nodes will be destroyed by bucket table dtor
            m_List.destroy(
                []( node_type * pNode ) -> bool {
                    return !pNode->is_dummy();
                }
            );
            gc::force_dispose();
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
            The user-defined functor is called only if the inserting is success.

            @warning For \ref cds_intrusive_MichaelList_hp "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_hp "LazyList" provides exclusive access to inserted item and does not require any node-level
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

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p val
            already is in the list.
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
            size_t nHash = hash_value( val );
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            node_traits::to_node_ptr( val )->m_nHash = split_list::regular_hash<bit_reversal>( nHash );

            std::pair<bool, bool> bRet = m_List.upsert_at( pHead, val, bAllowInsert );
            if ( bRet.first && bRet.second ) {
                inc_item_count();
                m_Stat.onUpdateNew();
            }
            else
                m_Stat.onUpdateExist();
            return bRet;
        }

        /// Unlinks the item \p val from the set
        /**
            The function searches the item \p val in the set and unlinks it from the set
            if it is found and is equal to \p val.

            Difference between \ref erase and \p unlink functions: \p erase finds <i>a key</i>
            and deletes the item found. \p unlink finds an item by key and deletes it
            only if \p val is an item of that set, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

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
        /** \anchor cds_intrusive_SplitListSet_hp_erase
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns \p true.
            If the item with key equal to \p key is not found the function return \p false.

            Difference between \ref erase and \p unlink functions: \p erase finds <i>a key</i>
            and deletes the item found. \p unlink finds an item by key and deletes it
            only if \p key is an item of that set, i.e. the pointer to item found
            is equal to <tt> &key </tt>.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_( key, key_comparator());
        }

        /// Deletes the item from the set with comparing functor \p pred
        /**

            The function is an analog of \ref cds_intrusive_SplitListSet_hp_erase "erase(Q const&)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool erase_with( const Q& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_( key, typename ordered_list_adapter::template make_compare_from_less<Less>());
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_SplitListSet_hp_erase_func
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

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return erase_( key, key_comparator(), f );
        }

        /// Deletes the item from the set with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_SplitListSet_hp_erase_func "erase(Q const&, Func)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_( key, typename ordered_list_adapter::template make_compare_from_less<Less>(), f );
        }

        /// Deletes the item pointed by iterator \p iter (only for \p IterableList based set)
        /**
            Returns \p true if the operation is successful, \p false otherwise.
            The function can return \p false if the node the iterator points to has already been deleted
            by other thread.

            The function does not invalidate the iterator, it remains valid and can be used for further traversing.

            @note \p %erase_at() is supported only for \p %SplitListSet based on \p IterableList.
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

            if ( m_List.erase_at( iter.underlying_iterator())) {
                --m_ItemCounter;
                m_Stat.onEraseSuccess();
                return true;
            }
            return false;
        }

        /// Extracts the item with specified \p key
        /** \anchor cds_intrusive_SplitListSet_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the set, and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p Q that may be not the same as \p value_type.

            The \p disposer specified in \p OrderedList class' template parameter is called automatically
            by garbage collector \p GC when returned \p guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::SplitListSet< your_template_args > splitlist_set;
            splitlist_set theSet;
            // ...
            {
                splitlist_set::guarded_ptr gp( theSet.extract( 5 ));
                if ( gp) {
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
            return extract_( key );
        }

        /// Extracts the item using compare functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_SplitListSet_hp_extract "extract(Q const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            return extract_with_( key, pred );
        }

        /// Finds the key \p key
        /** \anchor cds_intrusive_SplitListSet_hp_find_func
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

        /// Finds \p key and returns iterator pointed to the item found (only for \p IterableList)
        /**
            If \p key is not found the function returns \p end().

            @note This function is supported only for the set based on \p IterableList
        */
        template <typename Q>
#ifdef CDS_DOXYGEN_INVOKED
        iterator
#else
        typename std::enable_if< std::is_same<Q, Q>::value && is_iterable_list< ordered_list >::value, iterator >::type
#endif
        find( Q& key )
        {
            return find_iterator_( key, key_comparator());
        }
        //@cond
        template <typename Q>
        typename std::enable_if< std::is_same<Q, Q>::value && is_iterable_list< ordered_list >::value, iterator >::type
        find( Q const& key )
        {
            return find_iterator_( key, key_comparator());
        }
        //@endcond


        /// Finds the key \p key with \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_intrusive_SplitListSet_hp_find_func "find(Q&, Func)"
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
            CDS_UNUSED( pred );
            return find_iterator_( key, typename ordered_list_adapter::template make_compare_from_less<Less>());
        }
        //@cond
        template <typename Q, typename Less>
        typename std::enable_if< std::is_same<Q, Q>::value && is_iterable_list< ordered_list >::value, iterator >::type
        find_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_iterator_( key, typename ordered_list_adapter::template make_compare_from_less<Less>());
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
            return find_( key, key_comparator());
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
            CDS_UNUSED( pred );
            return find_( key, typename ordered_list_adapter::template make_compare_from_less<Less>());
        }

        /// Finds the key \p key and return the item found
        /** \anchor cds_intrusive_SplitListSet_hp_get
            The function searches the item with key equal to \p key
            and returns the item found as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            The \p disposer specified in \p OrderedList class' template parameter is called
            by garbage collector \p GC automatically when returned \p guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::SplitListSet< your_template_params >  splitlist_set;
            splitlist_set theSet;
            // ...
            {
                splitlist_set::guarded_ptr gp = theSet.get( 5 );
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
            return get_( key );
        }

        /// Finds the key \p key and return the item found
        /**
            The function is an analog of \ref cds_intrusive_SplitListSet_hp_get "get( Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred )
        {
            return get_with_( key, pred );
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

        /// Clears the set (non-atomic)
        /**
            The function unlink all items from the set.
            The function is not atomic. After call the split-list can be non-empty.

            For each item the \p disposer is called after unlinking.
        */
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
        aux_node_type * alloc_aux_node( size_t nHash )
        {
            m_Stat.onHeadNodeAllocated();
            aux_node_type* p = m_Buckets.alloc_aux_node();
            if ( p ) {
                CDS_TSAN_ANNOTATE_IGNORE_WRITES_BEGIN;
                // p->m_nHash is read-only data member
                p->m_nHash = nHash;
                CDS_TSAN_ANNOTATE_IGNORE_WRITES_END;
#       ifdef CDS_DEBUG
                cds_assert( !p->m_busy.load( atomics::memory_order_acquire ));
                p->m_busy.store( true, atomics::memory_order_release );
#       endif
            }
            return p;
        }

        void free_aux_node( aux_node_type * p )
        {
#       ifdef CDS_DEBUG
            cds_assert( p->m_busy.load( atomics::memory_order_acquire ));
            p->m_busy.store( false, atomics::memory_order_release );
#       endif

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
            // GC and OrderedList::gc must be the same
            static_assert(std::is_same<gc, typename ordered_list::gc>::value, "GC and OrderedList::gc must be the same");

            // atomicity::empty_item_counter is not allowed as a item counter
            static_assert(!std::is_same<item_counter, cds::atomicity::empty_item_counter>::value,
                "cds::atomicity::empty_item_counter is not allowed as a item counter");

            // Initialize bucket 0
            aux_node_type * pNode = alloc_aux_node( 0 /*split_list::dummy_hash<bit_reversal>(0)*/ );
            assert( pNode != nullptr );

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

        template <typename Q, typename Compare, typename Func>
        bool find_( Q& val, Compare cmp, Func f )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            return m_Stat.onFind(
                m_List.find_at( pHead, sv, cmp,
                    [&f]( value_type& item, split_list::details::search_value_type<Q>& v ) { f( item, v.val ); } )
            );
        }

        template <typename Q, typename Compare>
        bool find_( Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            return m_Stat.onFind( m_List.find_at( pHead, sv, cmp ));
        }

        template <typename Q, typename Compare>
        iterator find_iterator_( Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            return iterator( m_List.find_iterator_at( pHead, sv, cmp ), m_List.end());
        }

        template <typename Q, typename Compare>
        guarded_ptr get_( Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            guarded_ptr gp = m_List.get_at( pHead, sv, cmp );
            m_Stat.onFind( !gp.empty());
            return gp;
        }

        template <typename Q>
        guarded_ptr get_( Q const& key )
        {
            return get_( key, key_comparator());
        }

        template <typename Q, typename Less>
        guarded_ptr get_with_( Q const& key, Less )
        {
            return get_( key, typename ordered_list_adapter::template make_compare_from_less<Less>());
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

        template <typename Q, typename Compare>
        bool erase_( Q const& val, Compare cmp )
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

        template <typename Q, typename Compare>
        guarded_ptr extract_( Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const> sv( val, split_list::regular_hash<bit_reversal>( nHash ));
            aux_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            guarded_ptr gp = m_List.extract_at( pHead, sv, cmp );
            if ( gp ) {
                --m_ItemCounter;
                m_Stat.onExtractSuccess();
            }
            else
                m_Stat.onExtractFailed();
            return gp;
        }

        template <typename Q>
        guarded_ptr extract_( Q const& key )
        {
            return extract_( key, key_comparator());
        }

        template <typename Q, typename Less>
        guarded_ptr extract_with_( Q const& key, Less )
        {
            return extract_( key, typename ordered_list_adapter::template make_compare_from_less<Less>());
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

}}  // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_SPLIT_LIST_H

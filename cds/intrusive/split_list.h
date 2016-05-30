/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSLIB_INTRUSIVE_SPLIT_LIST_H
#define CDSLIB_INTRUSIVE_SPLIT_LIST_H

#include <limits>
#include <cds/intrusive/details/split_list_base.h>

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
            The intrusive ordered list implementation specifies the type \p T stored in the hash-set, the reclamation
            schema \p GC used by hash-set, the comparison functor for the type \p T and other features specific for
            the ordered list.
        - \p Traits - split-list traits, default is \p split_list::traits.
            Instead of defining \p Traits struct you may use option-based syntax with \p split_list::make_traits metafunction.

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
        typedef split_list::details::rebind_list_traits<OrderedList, traits> wrapped_ordered_list;
        //@endcond

    public:
#   ifdef CDS_DOXYGEN_INVOKED
        typedef OrderedList         ordered_list;   ///< type of ordered list used as a base for split-list
#   else
        typedef typename wrapped_ordered_list::result   ordered_list;
#   endif
        typedef typename ordered_list::value_type       value_type;     ///< type of value stored in the split-list
        typedef typename ordered_list::key_comparator   key_comparator; ///< key comparison functor
        typedef typename ordered_list::disposer         disposer;       ///< Node disposer functor

        /// Hash functor for \p %value_type and all its derivatives that you use
        typedef typename cds::opt::v::hash_selector< typename traits::hash >::type hash;

        typedef typename traits::item_counter      item_counter; ///< Item counter type
        typedef typename traits::back_off          back_off;     ///< back-off strategy for spinning
        typedef typename traits::memory_model      memory_model; ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::stat              stat;         ///< Internal statistics, see \p spit_list::stat
        typedef typename ordered_list::guarded_ptr guarded_ptr;  ///< Guarded pointer

        /// Count of hazard pointer required
        static CDS_CONSTEXPR const size_t c_nHazardPtrCount = ordered_list::c_nHazardPtrCount + 4; // +4 - for iterators

    protected:
        typedef typename ordered_list::node_type    list_node_type;  ///< Node type as declared in ordered list
        typedef split_list::node<list_node_type>    node_type;       ///< split-list node type
        typedef node_type                           dummy_node_type; ///< dummy node type

        /// Split-list node traits
        /**
            This traits is intended for converting between underlying ordered list node type \p list_node_type
            and split-list node type \p node_type
        */
        typedef split_list::node_traits<typename ordered_list::node_traits>  node_traits;

        //@cond
        /// Bucket table implementation
        typedef typename split_list::details::bucket_table_selector<
            traits::dynamic_bucket_table
            , gc
            , dummy_node_type
            , opt::allocator< typename traits::allocator >
            , opt::memory_model< memory_model >
        >::type bucket_table;
        //@endcond

    protected:
        //@cond
        /// Ordered list wrapper to access protected members
        class ordered_list_wrapper: public ordered_list
        {
            typedef ordered_list base_class;
            typedef typename base_class::auxiliary_head       bucket_head_type;

        public:
            bool insert_at( dummy_node_type * pHead, value_type& val )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::insert_at( h, val );
            }

            template <typename Func>
            bool insert_at( dummy_node_type * pHead, value_type& val, Func f )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::insert_at( h, val, f );
            }

            template <typename Func>
            std::pair<bool, bool> update_at( dummy_node_type * pHead, value_type& val, Func func, bool bAllowInsert )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::update_at( h, val, func, bAllowInsert );
            }

            bool unlink_at( dummy_node_type * pHead, value_type& val )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::unlink_at( h, val );
            }

            template <typename Q, typename Compare, typename Func>
            bool erase_at( dummy_node_type * pHead, split_list::details::search_value_type<Q> const& val, Compare cmp, Func f )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::erase_at( h, val, cmp, f );
            }

            template <typename Q, typename Compare>
            bool erase_at( dummy_node_type * pHead, split_list::details::search_value_type<Q> const& val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::erase_at( h, val, cmp );
            }

            template <typename Q, typename Compare>
            bool extract_at( dummy_node_type * pHead, typename guarded_ptr::native_guard& guard, split_list::details::search_value_type<Q> const& val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::extract_at( h, guard, val, cmp );
            }

            template <typename Q, typename Compare, typename Func>
            bool find_at( dummy_node_type * pHead, split_list::details::search_value_type<Q>& val, Compare cmp, Func f )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::find_at( h, val, cmp, f );
            }

            template <typename Q, typename Compare>
            bool find_at( dummy_node_type * pHead, split_list::details::search_value_type<Q> const& val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::find_at( h, val, cmp );
            }

            template <typename Q, typename Compare>
            bool get_at( dummy_node_type * pHead, typename guarded_ptr::native_guard& guard, split_list::details::search_value_type<Q> const& val, Compare cmp )
            {
                assert( pHead != nullptr );
                bucket_head_type h(pHead);
                return base_class::get_at( h, guard, val, cmp );
            }

            bool insert_aux_node( dummy_node_type * pNode )
            {
                return base_class::insert_aux_node( pNode );
            }
            bool insert_aux_node( dummy_node_type * pHead, dummy_node_type * pNode )
            {
                bucket_head_type h(pHead);
                return base_class::insert_aux_node( h, pNode );
            }
        };
        //@endcond

    protected:
        ordered_list_wrapper    m_List;             ///< Ordered list containing split-list items
        bucket_table            m_Buckets;          ///< bucket table
        atomics::atomic<size_t> m_nBucketCountLog2; ///< log2( current bucket count )
        atomics::atomic<size_t> m_nMaxItemCount;    ///< number of items container can hold, before we have to resize
        item_counter            m_ItemCounter;      ///< Item counter
        hash                    m_HashFunctor;      ///< Hash functor
        stat                    m_Stat;             ///< Internal statistics

    protected:
        //@cond
        typedef cds::details::Allocator< dummy_node_type, typename traits::allocator >   dummy_node_allocator;

        dummy_node_type * alloc_dummy_node( size_t nHash )
        {
            m_Stat.onHeadNodeAllocated();
            return dummy_node_allocator().New( nHash );
        }
        void free_dummy_node( dummy_node_type * p )
        {
            dummy_node_allocator().Delete( p );
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

        dummy_node_type * init_bucket( size_t nBucket )
        {
            assert( nBucket > 0 );
            size_t nParent = parent_bucket( nBucket );

            dummy_node_type * pParentBucket = m_Buckets.bucket( nParent );
            if ( pParentBucket == nullptr ) {
                pParentBucket = init_bucket( nParent );
                m_Stat.onRecursiveInitBucket();
            }

            assert( pParentBucket != nullptr );

            // Allocate a dummy node for new bucket
            {
                dummy_node_type * pBucket = alloc_dummy_node( split_list::dummy_hash( nBucket ));
                if ( m_List.insert_aux_node( pParentBucket, pBucket )) {
                    m_Buckets.bucket( nBucket, pBucket );
                    m_Stat.onNewBucket();
                    return pBucket;
                }
                free_dummy_node( pBucket );
            }

            // Another thread set the bucket. Wait while it done

            // In this point, we must wait while nBucket is empty.
            // The compiler can decide that waiting loop can be "optimized" (stripped)
            // To prevent this situation, we use waiting on volatile bucket_head_ptr pointer.
            m_Stat.onBucketInitContenton();
            back_off bkoff;
            while ( true ) {
                dummy_node_type volatile * p = m_Buckets.bucket( nBucket );
                if ( p != nullptr )
                    return const_cast<dummy_node_type *>( p );
                bkoff();
                m_Stat.onBusyWaitBucketInit();
            }
        }

        dummy_node_type * get_bucket( size_t nHash )
        {
            size_t nBucket = bucket_no( nHash );

            dummy_node_type * pHead = m_Buckets.bucket( nBucket );
            if ( pHead == nullptr )
                pHead = init_bucket( nBucket );

            assert( pHead->is_dummy());

            return pHead;
        }

        void init()
        {
            // GC and OrderedList::gc must be the same
            static_assert( std::is_same<gc, typename ordered_list::gc>::value, "GC and OrderedList::gc must be the same");

            // atomicity::empty_item_counter is not allowed as a item counter
            static_assert( !std::is_same<item_counter, cds::atomicity::empty_item_counter>::value,
                           "cds::atomicity::empty_item_counter is not allowed as a item counter");

            // Initialize bucket 0
            dummy_node_type * pNode = alloc_dummy_node( 0 /*split_list::dummy_hash(0)*/ );

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
            split_list::details::search_value_type<Q>  sv( val, split_list::regular_hash( nHash ));
            dummy_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            return m_Stat.onFind(
                m_List.find_at( pHead, sv, cmp,
                    [&f](value_type& item, split_list::details::search_value_type<Q>& val){ f(item, val.val ); })
            );
        }

        template <typename Q, typename Compare>
        bool find_( Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash( nHash ));
            dummy_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            return m_Stat.onFind( m_List.find_at( pHead, sv, cmp ));
        }

        template <typename Q, typename Compare>
        bool get_( typename guarded_ptr::native_guard& guard, Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash( nHash ));
            dummy_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            return m_Stat.onFind( m_List.get_at( pHead, guard, sv, cmp ));
        }

        template <typename Q>
        bool get_( typename guarded_ptr::native_guard& guard, Q const& key )
        {
            return get_( guard, key, key_comparator());
        }

        template <typename Q, typename Less>
        bool get_with_( typename guarded_ptr::native_guard& guard, Q const& key, Less )
        {
            return get_( guard, key, typename wrapped_ordered_list::template make_compare_from_less<Less>());
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_( Q const& val, Compare cmp, Func f )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash( nHash ));
            dummy_node_type * pHead = get_bucket( nHash );
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
            split_list::details::search_value_type<Q const>  sv( val, split_list::regular_hash( nHash ));
            dummy_node_type * pHead = get_bucket( nHash );
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
        bool extract_( typename guarded_ptr::native_guard& guard, Q const& val, Compare cmp )
        {
            size_t nHash = hash_value( val );
            split_list::details::search_value_type<Q const> sv( val, split_list::regular_hash( nHash ));
            dummy_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            if ( m_List.extract_at( pHead, guard, sv, cmp )) {
                --m_ItemCounter;
                m_Stat.onExtractSuccess();
                return true;
            }
            m_Stat.onExtractFailed();
            return false;
        }

        template <typename Q>
        bool extract_( typename guarded_ptr::native_guard& guard, Q const& key )
        {
            return extract_( guard, key, key_comparator());
        }

        template <typename Q, typename Less>
        bool extract_with_( typename guarded_ptr::native_guard& guard, Q const& key, Less )
        {
            return extract_( guard, key, typename wrapped_ordered_list::template make_compare_from_less<Less>());
        }
        //@endcond

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
            dummy_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            node_traits::to_node_ptr( val )->m_nHash = split_list::regular_hash( nHash );

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
            dummy_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            node_traits::to_node_ptr( val )->m_nHash = split_list::regular_hash( nHash );

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
            refers to the same thing.

            The functor may change non-key fields of the \p item.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successfull,
            \p second is \p true if new item has been added or \p false if the item with \p val
            already is in the list.

            @warning For \ref cds_intrusive_MichaelList_hp "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_hp "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            size_t nHash = hash_value( val );
            dummy_node_type * pHead = get_bucket( nHash );
            assert( pHead != nullptr );

            node_traits::to_node_ptr( val )->m_nHash = split_list::regular_hash( nHash );

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

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            size_t nHash = hash_value( val );
            dummy_node_type * pHead = get_bucket( nHash );
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
            return erase_( key, typename wrapped_ordered_list::template make_compare_from_less<Less>());
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
            return erase_( key, typename wrapped_ordered_list::template make_compare_from_less<Less>(), f );
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
            guarded_ptr gp;
            extract_( gp.guard(), key );
            return gp;
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
            guarded_ptr gp;
            extract_with_( gp.guard(), key, pred );
            return gp;
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
            return find_( key, typename wrapped_ordered_list::template make_compare_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_( key, typename wrapped_ordered_list::template make_compare_from_less<Less>(), f );
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
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_( key, typename wrapped_ordered_list::template make_compare_from_less<Less>());
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
            guarded_ptr gp;
            get_( gp.guard(), key );
            return gp;
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
            guarded_ptr gp;
            get_with_( gp.guard(), key, pred );
            return gp;
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
            The function is not atomic. Therefore, \p clear may be used only for debugging purposes.

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
    ///@name Forward iterators (only for debugging purpose)
    //@{
        /// Forward iterator
        /**
            The forward iterator for a split-list has some features:
            - it has no post-increment operator
            - it depends on iterator of underlying \p OrderedList
            - The iterator cannot be moved across thread boundary since it may contain GC's guard that is thread-private GC data.
            - Iterator ensures thread-safety even if you delete the item that iterator points to. However, in case of concurrent
              deleting operations it is no guarantee that you iterate all item in the set.
              Moreover, a crash is possible when you try to iterate the next element that has been deleted by concurrent thread.

            @warning Use this iterator on the concurrent container for debugging purpose only.
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
    };

}}  // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_SPLIT_LIST_H

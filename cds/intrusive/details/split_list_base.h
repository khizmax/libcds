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

#ifndef CDSLIB_INTRUSIVE_DETAILS_SPLIT_LIST_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_SPLIT_LIST_BASE_H

#include <cds/intrusive/details/base.h>
#include <cds/algo/atomic.h>
#include <cds/details/allocator.h>
#include <cds/algo/int_algo.h>
#include <cds/algo/bitop.h>
#include <cds/opt/hash.h>

namespace cds { namespace intrusive {

    /// Split-ordered list related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace split_list {
        /// Split-ordered list node
        /**
            Template parameter:
            - OrderedListNode - node type for underlying ordered list
        */
        template <typename OrderedListNode>
        struct node: public OrderedListNode
        {
            //@cond
            typedef OrderedListNode base_class;
            //@endcond

            size_t  m_nHash ;   ///< Hash value for node

            /// Default constructor
            node()
                : m_nHash(0)
            {
                assert( is_dummy() );
            }

            /// Initializes dummy node with \p nHash value
            node( size_t nHash )
                : m_nHash( nHash )
            {
                assert( is_dummy() );
            }

            /// Checks if the node is dummy node
            bool is_dummy() const
            {
                return (m_nHash & 1) == 0;
            }
        };

        /// SplitListSet internal statistics. May be used for debugging or profiling
        /**
            Template argument \p Counter defines type of counter.
            Default is \p cds::atomicity::event_counter, that is weak, i.e. it is not guaranteed
            strict event counting.
            You may use stronger type of counter like as \p cds::atomicity::item_counter,
            or even integral type, for example, \p int.
        */
        template <typename Counter = cds::atomicity::event_counter >
        struct stat
        {
            typedef Counter     counter_type;   ///< Counter type

            counter_type    m_nInsertSuccess;        ///< Count of success inserting
            counter_type    m_nInsertFailed;         ///< Count of failed inserting
            counter_type    m_nUpdateNew;            ///< Count of new item created by \p ensure() member function
            counter_type    m_nUpdateExist;          ///< Count of \p ensure() call for existing item
            counter_type    m_nEraseSuccess;         ///< Count of success erasing of items
            counter_type    m_nEraseFailed;          ///< Count of attempts to erase unknown item
            counter_type    m_nExtractSuccess;       ///< Count of success extracting of items
            counter_type    m_nExtractFailed;        ///< Count of attempts to extract unknown item
            counter_type    m_nFindSuccess;          ///< Count of success finding
            counter_type    m_nFindFailed;           ///< Count of failed finding
            counter_type    m_nHeadNodeAllocated;    ///< Count of allocated head node
            counter_type    m_nHeadNodeFreed;        ///< Count of freed head node
            counter_type    m_nBucketCount;          ///< Current bucket count
            counter_type    m_nInitBucketRecursive;  ///< Count of recursive bucket initialization
            counter_type    m_nInitBucketContention; ///< Count of bucket init contention encountered
            counter_type    m_nBusyWaitBucketInit;   ///< Count of busy wait cycle while a bucket is initialized

            //@cond
            void onInsertSuccess()       { ++m_nInsertSuccess; }
            void onInsertFailed()        { ++m_nInsertFailed; }
            void onUpdateNew()           { ++m_nUpdateNew; }
            void onUpdateExist()         { ++m_nUpdateExist; }
            void onEraseSuccess()        { ++m_nEraseSuccess; }
            void onEraseFailed()         { ++m_nEraseFailed; }
            void onExtractSuccess()      { ++m_nExtractSuccess; }
            void onExtractFailed()       { ++m_nExtractFailed; }
            void onFindSuccess()         { ++m_nFindSuccess; }
            void onFindFailed()          { ++m_nFindFailed; }
            bool onFind(bool bSuccess)
            {
                if ( bSuccess )
                    onFindSuccess();
                else
                    onFindFailed();
                return bSuccess;
            }
            void onHeadNodeAllocated()   { ++m_nHeadNodeAllocated; }
            void onHeadNodeFreed()       { ++m_nHeadNodeFreed; }
            void onNewBucket()           { ++m_nBucketCount; }
            void onRecursiveInitBucket() { ++m_nInitBucketRecursive; }
            void onBucketInitContenton() { ++m_nInitBucketContention; }
            void onBusyWaitBucketInit()  { ++m_nBusyWaitBucketInit; }
            //@endcond
        };

        /// Dummy queue statistics - no counting is performed, no overhead. Support interface like \p split_list::stat
        struct empty_stat {
            //@cond
            void onInsertSuccess()       const {}
            void onInsertFailed()        const {}
            void onUpdateNew()           const {}
            void onUpdateExist()         const {}
            void onEraseSuccess()        const {}
            void onEraseFailed()         const {}
            void onExtractSuccess()      const {}
            void onExtractFailed()       const {}
            void onFindSuccess()         const {}
            void onFindFailed()          const {}
            bool onFind( bool bSuccess ) const { return bSuccess; }
            void onHeadNodeAllocated()   const {}
            void onHeadNodeFreed()       const {}
            void onNewBucket()           const {}
            void onRecursiveInitBucket() const {}
            void onBucketInitContenton() const {}
            void onBusyWaitBucketInit()  const {}
            //@endcond
        };

        /// SplitListSet traits
        struct traits
        {
            /// Hash function
            /**
                Hash function converts the key fields of struct \p T stored in the split list
                into hash value of type \p size_t that is an index in hash table.
                By default, \p std::hash is used.
            */
            typedef opt::none       hash;

            /// Item counter
            /**
                The item counting is an important part of \p SplitListSet algorithm:
                the <tt>empty()</tt> member function depends on correct item counting.
                Therefore, \p cds::atomicity::empty_item_counter is not allowed as a type of the option.

                Default is \p cds::atomicity::item_counter.
            */
            typedef cds::atomicity::item_counter item_counter;

            /// Bucket table allocator
            /**
                Allocator for bucket table. Default is \ref CDS_DEFAULT_ALLOCATOR
            */
            typedef CDS_DEFAULT_ALLOCATOR   allocator;

            /// Internal statistics (by default, disabled)
            /**
                Possible statistics types are: \p split_list::stat (enable internal statistics),
                \p split_list::empty_stat (the default, internal statistics disabled),
                user-provided class that supports \p %split_list::stat interface.
            */
            typedef split_list::empty_stat  stat;


            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering memory_model;

            /// What type of bucket table is used
            /**
                \p true - use \p split_list::expandable_bucket_table that can be expanded
                    if the load factor of the set is exhausted.
                \p false - use \p split_list::static_bucket_table that cannot be expanded
                    and is allocated in \p SplitListSet constructor.

                Default is \p true.
            */
            static const bool dynamic_bucket_table = true;

            /// Back-off strategy
            typedef cds::backoff::Default back_off;
        };

        /// [value-option] Split-list dynamic bucket table option
        /**
            The option is used to select bucket table implementation.
            Possible values of \p Value are:
            - \p true - select \p expandable_bucket_table
            - \p false - select \p static_bucket_table
        */
        template <bool Value>
        struct dynamic_bucket_table
        {
            //@cond
            template <typename Base> struct pack: public Base
            {
                enum { dynamic_bucket_table = Value };
            };
            //@endcond
        };

        /// Metafunction converting option list to \p split_list::traits
        /**
            Available \p Options:
            - \p opt::hash - mandatory option, specifies hash functor.
            - \p opt::item_counter - optional, specifies item counting policy. See \p traits::item_counter
                for default type.
            - \p opt::memory_model - C++ memory model for atomic operations.
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p opt::allocator - optional, bucket table allocator. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - \p split_list::dynamic_bucket_table - use dynamic or static bucket table implementation.
                Dynamic bucket table expands its size up to maximum bucket count when necessary
            - \p opt::back_off - back-off strategy used for spinning, defult is \p cds::backoff::Default.
            - \p opt::stat - internal statistics, default is \p split_list::empty_stat (disabled).
                To enable internal statistics use \p split_list::stat.
        */
        template <typename... Options>
        struct make_traits {
            typedef typename cds::opt::make_options< traits, Options...>::type type  ;   ///< Result of metafunction
        };

        /// Static bucket table
        /**
            Non-resizeable bucket table for \p SplitListSet class.
            The capacity of table (max bucket count) is defined in the constructor call.

            Template parameter:
            - \p GC - garbage collector
            - \p Node - node type, must be a type based on \p split_list::node
            - \p Options... - options

            \p Options are:
            - \p opt::allocator - allocator used to allocate bucket table. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::memory_model - memory model used. Possible types are \p opt::v::sequential_consistent, \p opt::v::relaxed_ordering
        */
        template <typename GC, typename Node, typename... Options>
        class static_bucket_table
        {
            //@cond
            struct default_options
            {
                typedef CDS_DEFAULT_ALLOCATOR       allocator;
                typedef opt::v::relaxed_ordering    memory_model;
            };
            typedef typename opt::make_options< default_options, Options... >::type options;
            //@endcond

        public:
            typedef GC      gc;         ///< Garbage collector
            typedef Node    node_type;  ///< Bucket node type
            typedef atomics::atomic<node_type *> table_entry;  ///< Table entry type

            /// Bucket table allocator
            typedef cds::details::Allocator< table_entry, typename options::allocator >  bucket_table_allocator;

            /// Memory model for atomic operations
            typedef typename options::memory_model  memory_model;

        protected:
            const size_t   m_nLoadFactor; ///< load factor (average count of items per bucket)
            const size_t   m_nCapacity;   ///< Bucket table capacity
            table_entry *  m_Table;       ///< Bucket table

        protected:
            //@cond
            void allocate_table()
            {
                m_Table = bucket_table_allocator().NewArray( m_nCapacity, nullptr );
            }

            void destroy_table()
            {
                bucket_table_allocator().Delete( m_Table, m_nCapacity );
            }
            //@endcond

        public:
            /// Constructs bucket table for 512K buckets. Load factor is 1.
            static_bucket_table()
                : m_nLoadFactor(1)
                , m_nCapacity( 512 * 1024 )
            {
                allocate_table();
            }

            /// Creates the table with specified size rounded up to nearest power-of-two
            static_bucket_table(
                size_t nItemCount,        ///< Max expected item count in split-ordered list
                size_t nLoadFactor        ///< Load factor
                )
                : m_nLoadFactor( nLoadFactor > 0 ? nLoadFactor : (size_t) 1 ),
                m_nCapacity( cds::beans::ceil2( nItemCount / m_nLoadFactor ) )
            {
                // m_nCapacity must be power of 2
                assert( cds::beans::is_power2( m_nCapacity ) );
                allocate_table();
            }

            /// Destroys bucket table
            ~static_bucket_table()
            {
                destroy_table();
            }

            /// Returns head node of bucket \p nBucket
            node_type * bucket( size_t nBucket ) const
            {
                assert( nBucket < capacity() );
                return m_Table[ nBucket ].load(memory_model::memory_order_acquire);
            }

            /// Set \p pNode as a head of bucket \p nBucket
            void bucket( size_t nBucket, node_type * pNode )
            {
                assert( nBucket < capacity() );
                assert( bucket( nBucket ) == nullptr );

                m_Table[ nBucket ].store( pNode, memory_model::memory_order_release );
            }

            /// Returns the capacity of the bucket table
            size_t capacity() const
            {
                return m_nCapacity;
            }

            /// Returns the load factor, i.e. average count of items per bucket
            size_t load_factor() const
            {
                return m_nLoadFactor;
            }
        };

        /// Expandable bucket table
        /**
            This bucket table can dynamically grow its capacity when necessary
            up to maximum bucket count.

            Template parameter:
            - \p GC - garbage collector
            - \p Node - node type, must be derived from \p split_list::node
            - \p Options... - options

            \p Options are:
            - \p opt::allocator - allocator used to allocate bucket table. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::memory_model - memory model used. Possible types are \p opt::v::sequential_consistent, \p opt::v::relaxed_ordering
        */
        template <typename GC, typename Node, typename... Options>
        class expandable_bucket_table
        {
            //@cond
            struct default_options
            {
                typedef CDS_DEFAULT_ALLOCATOR       allocator;
                typedef opt::v::relaxed_ordering    memory_model;
            };
            typedef typename opt::make_options< default_options, Options... >::type options;
            //@endcond
        public:
            typedef GC      gc;        ///< Garbage collector
            typedef Node    node_type; ///< Bucket node type
            typedef atomics::atomic<node_type *> table_entry; ///< Table entry type

            /// Memory model for atomic operations
            typedef typename options::memory_model memory_model;

        protected:
            typedef atomics::atomic<table_entry *>   segment_type; ///< Bucket table segment type

        public:
            /// Bucket table allocator
            typedef cds::details::Allocator< segment_type, typename options::allocator >  bucket_table_allocator;

            /// Bucket table segment allocator
            typedef cds::details::Allocator< table_entry, typename options::allocator >  segment_allocator;

        protected:
            /// Bucket table metrics
            struct metrics {
                size_t    nSegmentCount;    ///< max count of segments in bucket table
                size_t    nSegmentSize;     ///< the segment's capacity. The capacity must be power of two.
                size_t    nSegmentSizeLog2; ///< <tt> log2( m_nSegmentSize )</tt>
                size_t    nLoadFactor;      ///< load factor
                size_t    nCapacity;        ///< max capacity of bucket table

                //@cond
                metrics()
                    : nSegmentCount(1024)
                    , nSegmentSize(512)
                    , nSegmentSizeLog2( cds::beans::log2( nSegmentSize ) )
                    , nLoadFactor(1)
                    , nCapacity( nSegmentCount * nSegmentSize )
                {}
                //@endcond
            };
            const metrics   m_metrics; ///< Dynamic bucket table metrics

        protected:
            segment_type * m_Segments; ///< bucket table - array of segments

        protected:
            //@cond
            metrics calc_metrics( size_t nItemCount, size_t nLoadFactor )
            {
                metrics m;

                // Calculate m_nSegmentSize and m_nSegmentCount by nItemCount
                m.nLoadFactor = nLoadFactor > 0 ? nLoadFactor : 1;

                size_t nBucketCount = (size_t)( ((float) nItemCount) / m.nLoadFactor );
                if ( nBucketCount <= 2 ) {
                    m.nSegmentCount = 1;
                    m.nSegmentSize = 2;
                }
                else if ( nBucketCount <= 1024 ) {
                    m.nSegmentCount = 1;
                    m.nSegmentSize = ((size_t) 1) << beans::log2ceil( nBucketCount );
                }
                else {
                    nBucketCount = beans::log2ceil( nBucketCount );
                    m.nSegmentCount =
                        m.nSegmentSize = ((size_t) 1) << ( nBucketCount / 2 );
                    if ( nBucketCount & 1 )
                        m.nSegmentSize *= 2;
                    if ( m.nSegmentCount * m.nSegmentSize * m.nLoadFactor < nItemCount )
                        m.nSegmentSize *= 2;
                }
                m.nCapacity = m.nSegmentCount * m.nSegmentSize;
                m.nSegmentSizeLog2 = cds::beans::log2( m.nSegmentSize );
                assert( m.nSegmentSizeLog2 != 0 )   ;   //
                return m;
            }

            segment_type * allocate_table()
            {
                return bucket_table_allocator().NewArray( m_metrics.nSegmentCount, nullptr );
            }

            void destroy_table( segment_type * pTable )
            {
                bucket_table_allocator().Delete( pTable, m_metrics.nSegmentCount );
            }

            table_entry * allocate_segment()
            {
                return segment_allocator().NewArray( m_metrics.nSegmentSize, nullptr );
            }

            void destroy_segment( table_entry * pSegment )
            {
                segment_allocator().Delete( pSegment, m_metrics.nSegmentSize );
            }

            void init_segments()
            {
                // m_nSegmentSize must be 2**N
                assert( cds::beans::is_power2( m_metrics.nSegmentSize ));
                assert( ( ((size_t) 1) << m_metrics.nSegmentSizeLog2) == m_metrics.nSegmentSize );

                // m_nSegmentCount must be 2**K
                assert( cds::beans::is_power2( m_metrics.nSegmentCount ));

                m_Segments = allocate_table();
            }

            //@endcond

        public:
            /// Constructs bucket table for 512K buckets. Load factor is 1.
            expandable_bucket_table()
                : m_metrics( calc_metrics( 512 * 1024, 1 ))
            {
                init_segments();
            }

            /// Creates the table with specified capacity rounded up to nearest power-of-two
            expandable_bucket_table(
                size_t nItemCount,        ///< Max expected item count in split-ordered list
                size_t nLoadFactor        ///< Load factor
                )
                : m_metrics( calc_metrics( nItemCount, nLoadFactor ))
            {
                init_segments();
            }

            /// Destroys bucket table
            ~expandable_bucket_table()
            {
                segment_type * pSegments = m_Segments;
                for ( size_t i = 0; i < m_metrics.nSegmentCount; ++i ) {
                    table_entry * pEntry = pSegments[i].load(memory_model::memory_order_relaxed);
                    if ( pEntry != nullptr )
                        destroy_segment( pEntry );
                }
                destroy_table( pSegments );
            }

            /// Returns head node of the bucket \p nBucket
            node_type * bucket( size_t nBucket ) const
            {
                size_t nSegment = nBucket >> m_metrics.nSegmentSizeLog2;
                assert( nSegment < m_metrics.nSegmentCount );

                table_entry * pSegment = m_Segments[ nSegment ].load(memory_model::memory_order_acquire);
                if ( pSegment == nullptr )
                    return nullptr;    // uninitialized bucket
                return pSegment[ nBucket & (m_metrics.nSegmentSize - 1) ].load(memory_model::memory_order_acquire);
            }

            /// Set \p pNode as a head of bucket \p nBucket
            void bucket( size_t nBucket, node_type * pNode )
            {
                size_t nSegment = nBucket >> m_metrics.nSegmentSizeLog2;
                assert( nSegment < m_metrics.nSegmentCount );

                segment_type& segment = m_Segments[nSegment];
                if ( segment.load( memory_model::memory_order_relaxed ) == nullptr ) {
                    table_entry * pNewSegment = allocate_segment();
                    table_entry * pNull = nullptr;
                    if ( !segment.compare_exchange_strong( pNull, pNewSegment, memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                        destroy_segment( pNewSegment );
                    }
                }
                segment.load(memory_model::memory_order_acquire)[ nBucket & (m_metrics.nSegmentSize - 1) ].store( pNode, memory_model::memory_order_release );
            }

            /// Returns the capacity of the bucket table
            size_t capacity() const
            {
                return m_metrics.nCapacity;
            }

            /// Returns the load factor, i.e. average count of items per bucket
            size_t load_factor() const
            {
                return m_metrics.nLoadFactor;
            }
        };

        /// Split-list node traits
        /**
            This traits is intended for converting between underlying ordered list node type
            and split-list node type

            Template parameter:
            - \p BaseNodeTraits - node traits of base ordered list type
        */
        template <class BaseNodeTraits>
        struct node_traits: private BaseNodeTraits
        {
            typedef BaseNodeTraits base_class;     ///< Base ordered list node type
            typedef typename base_class::value_type value_type;     ///< Value type
            typedef typename base_class::node_type  base_node_type; ///< Ordered list node type
            typedef node<base_node_type>            node_type;      ///< Spit-list node type

            /// Convert value reference to node pointer
            static node_type * to_node_ptr( value_type& v )
            {
                return static_cast<node_type *>( base_class::to_node_ptr( v ) );
            }

            /// Convert value pointer to node pointer
            static node_type * to_node_ptr( value_type * v )
            {
                return static_cast<node_type *>( base_class::to_node_ptr( v ) );
            }

            /// Convert value reference to node pointer (const version)
            static node_type const * to_node_ptr( value_type const& v )
            {
                return static_cast<node_type const*>( base_class::to_node_ptr( v ) );
            }

            /// Convert value pointer to node pointer (const version)
            static node_type const * to_node_ptr( value_type const * v )
            {
                return static_cast<node_type const *>( base_class::to_node_ptr( v ) );
            }

            /// Convert node refernce to value pointer
            static value_type * to_value_ptr( node_type&  n )
            {
                return base_class::to_value_ptr( static_cast<base_node_type &>( n ) );
            }

            /// Convert node pointer to value pointer
            static value_type * to_value_ptr( node_type *  n )
            {
                return base_class::to_value_ptr( static_cast<base_node_type *>( n ) );
            }

            /// Convert node reference to value pointer (const version)
            static const value_type * to_value_ptr( node_type const & n )
            {
                return base_class::to_value_ptr( static_cast<base_node_type const &>( n ) );
            }

            /// Convert node pointer to value pointer (const version)
            static const value_type * to_value_ptr( node_type const * n )
            {
                return base_class::to_value_ptr( static_cast<base_node_type const *>( n ) );
            }
        };

        //@cond
        namespace details {
            template <bool Value, typename GC, typename Node, typename... Options>
            struct bucket_table_selector;

            template <typename GC, typename Node, typename... Options>
            struct bucket_table_selector< true, GC, Node, Options...>
            {
                typedef expandable_bucket_table<GC, Node, Options...>    type;
            };

            template <typename GC, typename Node, typename... Options>
            struct bucket_table_selector< false, GC, Node, Options...>
            {
                typedef static_bucket_table<GC, Node, Options...>    type;
            };

            template <typename GC, class Alloc >
            struct dummy_node_disposer {
                template <typename Node>
                void operator()( Node * p )
                {
                    typedef cds::details::Allocator< Node, Alloc >  node_deallocator;
                    node_deallocator().Delete( p );
                }
            };

            template <typename Q>
            struct search_value_type
            {
                Q&      val;
                size_t  nHash;

                search_value_type( Q& v, size_t h )
                    : val( v )
                    , nHash( h )
                {}
            };

            template <class OrderedList, class Traits>
            class rebind_list_traits
            {
                typedef OrderedList native_ordered_list;
                typedef Traits      traits;

                typedef typename native_ordered_list::gc                gc;
                typedef typename native_ordered_list::key_comparator    native_key_comparator;
                typedef typename native_ordered_list::node_type         node_type;
                typedef typename native_ordered_list::value_type        value_type;
                typedef typename native_ordered_list::node_traits       node_traits;
                typedef typename native_ordered_list::disposer          native_disposer;

                typedef split_list::node<node_type>                     splitlist_node_type;

                struct key_compare {
                    int operator()( value_type const& v1, value_type const& v2 ) const
                    {
                        splitlist_node_type const * n1 = static_cast<splitlist_node_type const *>( node_traits::to_node_ptr( v1 ));
                        splitlist_node_type const * n2 = static_cast<splitlist_node_type const *>( node_traits::to_node_ptr( v2 ));
                        if ( n1->m_nHash != n2->m_nHash )
                            return n1->m_nHash < n2->m_nHash ? -1 : 1;

                        if ( n1->is_dummy() ) {
                            assert( n2->is_dummy() );
                            return 0;
                        }

                        assert( !n1->is_dummy() && !n2->is_dummy() );

                        return native_key_comparator()( v1, v2 );
                    }

                    template <typename Q>
                    int operator()( value_type const& v, search_value_type<Q> const& q ) const
                    {
                        splitlist_node_type const * n = static_cast<splitlist_node_type const *>( node_traits::to_node_ptr( v ));
                        if ( n->m_nHash != q.nHash )
                            return n->m_nHash < q.nHash ? -1 : 1;

                        assert( !n->is_dummy() );
                        return native_key_comparator()( v, q.val );
                    }

                    template <typename Q>
                    int operator()( search_value_type<Q> const& q, value_type const& v ) const
                    {
                        return -operator()( v, q );
                    }
                };

                struct wrapped_disposer
                {
                    void operator()( value_type * v )
                    {
                        splitlist_node_type * p = static_cast<splitlist_node_type *>( node_traits::to_node_ptr( v ));
                        if ( p->is_dummy() )
                            dummy_node_disposer<gc, typename traits::allocator>()( p );
                        else
                            native_disposer()( v );
                    }
                };

            public:
                template <typename Less>
                struct make_compare_from_less: public cds::opt::details::make_comparator_from_less<Less>
                {
                    typedef cds::opt::details::make_comparator_from_less<Less>  base_class;

                    template <typename Q>
                    int operator()( value_type const& v, search_value_type<Q> const& q ) const
                    {
                        splitlist_node_type const * n = static_cast<splitlist_node_type const *>( node_traits::to_node_ptr( v ));
                        if ( n->m_nHash != q.nHash )
                            return n->m_nHash < q.nHash ? -1 : 1;

                        assert( !n->is_dummy() );
                        return base_class()( v, q.val );
                    }

                    template <typename Q>
                    int operator()( search_value_type<Q> const& q, value_type const& v ) const
                    {
                        splitlist_node_type const * n = static_cast<splitlist_node_type const *>( node_traits::to_node_ptr( v ));
                        if ( n->m_nHash != q.nHash )
                            return q.nHash < n->m_nHash ? -1 : 1;

                        assert( !n->is_dummy() );
                        return base_class()( q.val, v );
                    }

                    template <typename Q1, typename Q2>
                    int operator()( Q1 const& v1, Q2 const& v2 ) const
                    {
                        return base_class()( v1, v2 );
                    }
                };

                typedef typename native_ordered_list::template rebind_traits<
                    opt::compare< key_compare >
                    ,opt::disposer< wrapped_disposer >
                    ,opt::boundary_node_type< splitlist_node_type >
                >::type result;
            };

            template <typename OrderedList, bool IsConst>
            struct select_list_iterator;

            template <typename OrderedList>
            struct select_list_iterator<OrderedList, false>
            {
                typedef typename OrderedList::iterator  type;
            };

            template <typename OrderedList>
            struct select_list_iterator<OrderedList, true>
            {
                typedef typename OrderedList::const_iterator  type;
            };

            template <typename NodeTraits, typename OrderedList, bool IsConst>
            class iterator_type
            {
                typedef OrderedList     ordered_list_type;
                friend class iterator_type <NodeTraits, OrderedList, !IsConst >;

            protected:
                typedef typename select_list_iterator<ordered_list_type, IsConst>::type    list_iterator;
                typedef NodeTraits      node_traits;

            private:
                list_iterator   m_itCur;
                list_iterator   m_itEnd;

            public:
                typedef typename list_iterator::value_ptr   value_ptr;
                typedef typename list_iterator::value_ref   value_ref;

            public:
                iterator_type()
                {}

                iterator_type( iterator_type const& src )
                    : m_itCur( src.m_itCur )
                    , m_itEnd( src.m_itEnd )
                {}

                // This ctor should be protected...
                iterator_type( list_iterator itCur, list_iterator itEnd )
                    : m_itCur( itCur )
                    , m_itEnd( itEnd )
                {
                    // skip dummy nodes
                    while ( m_itCur != m_itEnd && node_traits::to_node_ptr( *m_itCur )->is_dummy() )
                        ++m_itCur;
                }

                value_ptr operator ->() const
                {
                    return m_itCur.operator->();
                }

                value_ref operator *() const
                {
                    return m_itCur.operator*();
                }

                /// Pre-increment
                iterator_type& operator ++()
                {
                    if ( m_itCur != m_itEnd ) {
                        do {
                            ++m_itCur;
                        } while ( m_itCur != m_itEnd && node_traits::to_node_ptr( *m_itCur )->is_dummy() );
                    }
                    return *this;
                }

                iterator_type& operator = (iterator_type const& src)
                {
                    m_itCur = src.m_itCur;
                    m_itEnd = src.m_itEnd;
                    return *this;
                }

                template <bool C>
                bool operator ==(iterator_type<node_traits, ordered_list_type, C> const& i ) const
                {
                    return m_itCur == i.m_itCur;
                }
                template <bool C>
                bool operator !=(iterator_type<node_traits, ordered_list_type, C> const& i ) const
                {
                    return m_itCur != i.m_itCur;
                }
            };
        }   // namespace details
        //@endcond

        //@cond
        // Helper functions

        /// Reverses bit order in \p nHash
        static inline size_t reverse_bits( size_t nHash )
        {
            return bitop::RBO( nHash );
        }

        static inline size_t regular_hash( size_t nHash )
        {
            return reverse_bits( nHash ) | size_t(1);
        }

        static inline size_t dummy_hash( size_t nHash )
        {
            return reverse_bits( nHash ) & ~size_t(1);
        }
        //@endcond

    } // namespace split_list

    //@cond
    // Forward declaration
    template <class GC, class OrderedList, class Traits = split_list::traits>
    class SplitListSet;
    //@endcond

}}  // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_SPLIT_LIST_BASE_H

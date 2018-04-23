// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_SPLIT_LIST_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_SPLIT_LIST_BASE_H

#include <cds/intrusive/details/base.h>
#include <cds/algo/atomic.h>
#include <cds/algo/bit_reversal.h>
#include <cds/details/allocator.h>
#include <cds/algo/int_algo.h>
#include <cds/algo/bitop.h>
#include <cds/opt/hash.h>
#include <cds/intrusive/free_list_selector.h>
#include <cds/details/size_t_cast.h>
#include <memory>

namespace cds { namespace intrusive {

    /// Split-ordered list related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace split_list {
        //@cond
        struct hash_node
        {
            size_t  m_nHash;   ///< Hash value for node

            /// Default constructor
            hash_node()
                : m_nHash( 0 )
            {
                assert( is_dummy());
            }

            /// Initializes dummy node with \p nHash value
            explicit hash_node( size_t nHash )
                : m_nHash( nHash )
            {
                assert( is_dummy());
            }

            /// Checks if the node is dummy node
            bool is_dummy() const
            {
                return (m_nHash & 1) == 0;
            }
        };
        //@endcond

        /// Split-ordered list node
        /**
            Template parameter:
            - \p OrderedListNode - node type for underlying ordered list
        */
        template <typename OrderedListNode>
        struct node: public OrderedListNode, public hash_node
        {
            //@cond
            typedef OrderedListNode base_class;
            //@endcond

            /// Default constructor
            node()
                : hash_node(0)
            {
                assert( is_dummy());
            }

            /// Initializes dummy node with \p nHash value
            explicit node( size_t nHash )
                : hash_node( nHash )
            {
                assert( is_dummy());
            }

            /// Checks if the node is dummy node
            bool is_dummy() const
            {
                return hash_node::is_dummy();
            }
        };

        //@cond
        // for IterableList
        template <>
        struct node<void>: public hash_node
        {
            // Default ctor
            node()
                : hash_node( 0 )
            {
                assert( is_dummy());
            }

            /// Initializes dummy node with \p nHash value
            explicit node( size_t nHash )
                : hash_node( nHash )
            {
                assert( is_dummy());
            }

            /// Checks if the node is dummy node
            bool is_dummy() const
            {
                return hash_node::is_dummy();
            }
        };
        //@endcond

        /// \p SplitListSet internal statistics. May be used for debugging or profiling
        /**
            Template argument \p Counter defines type of counter, default is \p cds::atomicity::event_counter.
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
            counter_type    m_nBucketsExhausted;     ///< Count of failed bucket allocation

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
            void onBucketsExhausted()    { ++m_nBucketsExhausted; }
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
            void onBucketsExhausted()    const {}
            //@endcond
        };

        /// Option to control bit reversal algorithm
        /**
            Bit reversal is a significant part of split-list.
            \p Type can be one of predefined algorithm in \p cds::algo::bit_reversal namespace.
        */
        template <typename Type>
        struct bit_reversal {
            //@cond
            template <typename Base>
            struct pack: public Base
            {
                typedef Type bit_reversal;
            };
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

            /// Bit reversal algorithm
            /**
                Bit reversal is a significant part of split-list.
                There are several predefined algorithm in \p cds::algo::bit_reversal namespace,
                \p cds::algo::bit_reversal::lookup is the best general purpose one.

                There are more efficient bit reversal algoritm for particular processor architecture,
                for example, based on x86 SIMD/AVX instruction set, see <a href="http://stackoverflow.com/questions/746171/best-algorithm-for-bit-reversal-from-msb-lsb-to-lsb-msb-in-c">here</a>
            */
            typedef cds::algo::bit_reversal::lookup bit_reversal;

            /// Item counter
            /**
                The item counting is an important part of \p SplitListSet algorithm:
                the <tt>empty()</tt> member function depends on correct item counting.
                Therefore, \p cds::atomicity::empty_item_counter is not allowed as a type of the option.

                Default is \p cds::atomicity::item_counter; to avoid false sharing you may use \p atomicity::cache_friendly_item_counter
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

            /// Padding; default is cache-line padding
            enum {
                padding = cds::opt::cache_line_padding
            };

            /// Free-list of auxiliary nodes
            /**
                The split-list contains auxiliary nodes marked the start of buckets.
                To increase performance, there is a pool of preallocated aux nodes. The part of the pool is a free-list
                of aux nodes.

                Default is:
                - \p cds::intrusive::FreeList - if architecture and/or compiler does not support double-width CAS primitive
                - \p cds::intrusive::TaggedFreeList - if architecture and/or compiler supports double-width CAS primitive
            */
            typedef FreeListImpl free_list;
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
            - \p split_list::bit_reversal - bit reversal algorithm, see \p traits::bit_reversal for explanation
                default is \p cds::algo::bit_reversal::lookup
            - \p opt::item_counter - optional, specifies item counting policy. See \p traits::item_counter
                for default type.
            - \p opt::memory_model - C++ memory model for atomic operations.
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consistent memory model).
            - \p opt::allocator - optional, bucket table allocator. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - \p split_list::dynamic_bucket_table - use dynamic or static bucket table implementation.
                Dynamic bucket table expands its size up to maximum bucket count when necessary
            - \p opt::back_off - back-off strategy used for spinning, default is \p cds::backoff::Default.
            - \p opt::stat - internal statistics, default is \p split_list::empty_stat (disabled).
                To enable internal statistics use \p split_list::stat.
            - \p opt::padding - a padding to solve false-sharing issues; default is cache-line padding
            - \p opt::free_list - a free-list implementation, see \p traits::free_list
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
            - \p opt::free_list - free-list implementation; default is \p TaggedFreeList if the processor supports double-with CAS
                 otherwise \p FreeList.
        */
        template <typename GC, typename Node, typename... Options>
        class static_bucket_table
        {
            //@cond
            struct default_options
            {
                typedef CDS_DEFAULT_ALLOCATOR       allocator;
                typedef opt::v::relaxed_ordering    memory_model;
                typedef FreeListImpl                free_list;
            };
            typedef typename opt::make_options< default_options, Options... >::type options;
            //@endcond

        public:
            typedef GC       gc;         ///< Garbage collector
            typedef Node     node_type;  ///< Bucket node type
            typedef typename options::allocator allocator; ///< allocator
            typedef typename options::memory_model  memory_model; ///< Memory model for atomic operations
            typedef typename options::free_list free_list; ///< Free-list

            /// Auxiliary node type
            struct aux_node_type: public node_type, public free_list::node
            {
#           ifdef CDS_DEBUG
                atomics::atomic<bool> m_busy;

                aux_node_type()
                {
                    m_busy.store( false, atomics::memory_order_release );
                }
#           endif
            };

            typedef atomics::atomic<aux_node_type *> table_entry;  ///< Table entry type
            typedef cds::details::Allocator< table_entry, allocator > bucket_table_allocator; ///< Bucket table allocator

        protected:
            //@cond
            const size_t   m_nLoadFactor; ///< load factor (average count of items per bucket)
            const size_t   m_nCapacity;   ///< Bucket table capacity
            table_entry *  m_Table;       ///< Bucket table

            typedef typename std::allocator_traits<allocator>::template rebind_alloc< aux_node_type > aux_node_allocator;

            aux_node_type*          m_auxNode;           ///< Array of pre-allocated auxiliary nodes
            atomics::atomic<size_t> m_nAuxNodeAllocated; ///< how many auxiliary node allocated
            free_list               m_freeList;          ///< Free list
            //@endcond

        protected:
            //@cond
            void allocate_table()
            {
                m_Table = bucket_table_allocator().NewArray( m_nCapacity, nullptr );
                m_auxNode = aux_node_allocator().allocate( m_nCapacity );
            }

            void destroy_table()
            {
                m_freeList.clear( []( typename free_list::node* ) {} );
                aux_node_allocator().deallocate( m_auxNode, m_nCapacity );
                bucket_table_allocator().Delete( m_Table, m_nCapacity );
            }
            //@endcond

        public:
            /// Constructs bucket table for 512K buckets. Load factor is 1.
            static_bucket_table()
                : m_nLoadFactor(1)
                , m_nCapacity( 512 * 1024 )
                , m_nAuxNodeAllocated( 0 )
            {
                allocate_table();
            }

            /// Creates the table with specified size rounded up to nearest power-of-two
            static_bucket_table(
                size_t nItemCount,        ///< Max expected item count in split-ordered list
                size_t nLoadFactor        ///< Load factor
                )
                : m_nLoadFactor( nLoadFactor > 0 ? nLoadFactor : (size_t) 1 )
                , m_nCapacity( cds::beans::ceil2( nItemCount / m_nLoadFactor ))
                , m_nAuxNodeAllocated( 0 )
            {
                // m_nCapacity must be power of 2
                assert( cds::beans::is_power2( m_nCapacity ));
                allocate_table();
            }

            /// Destroys bucket table
            ~static_bucket_table()
            {
                destroy_table();
            }

            /// Returns head node of bucket \p nBucket
            aux_node_type * bucket( size_t nBucket ) const
            {
                assert( nBucket < capacity());
                return m_Table[ nBucket ].load(memory_model::memory_order_acquire);
            }

            /// Set \p pNode as a head of bucket \p nBucket
            void bucket( size_t nBucket, aux_node_type * pNode )
            {
                assert( nBucket < capacity());
                assert( bucket( nBucket ) == nullptr );

                m_Table[ nBucket ].store( pNode, memory_model::memory_order_release );
            }

            /// Allocates auxiliary node; can return \p nullptr if the table exhausted
            aux_node_type* alloc_aux_node()
            {
                if ( m_nAuxNodeAllocated.load( memory_model::memory_order_relaxed ) < capacity()) {
                    // alloc next free node from m_auxNode
                    size_t const idx = m_nAuxNodeAllocated.fetch_add( 1, memory_model::memory_order_relaxed );
                    if ( idx < capacity()) {
                        CDS_TSAN_ANNOTATE_NEW_MEMORY( &m_auxNode[idx], sizeof( aux_node_type ));
                        return new( &m_auxNode[idx] ) aux_node_type();
                    }
                }

                // get from free-list
                auto pFree = m_freeList.get();
                if ( pFree )
                    return static_cast<aux_node_type*>( pFree );

                // table exhausted
                return nullptr;
            }

            /// Places node type to free-list
            void free_aux_node( aux_node_type* p )
            {
                m_freeList.put( static_cast<aux_node_type*>( p ));
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
            - \p opt::free_list - free-list implementation; default is \p TaggedFreeList if the processor supports double-with CAS
                otherwise \p FreeList.
            */
        template <typename GC, typename Node, typename... Options>
        class expandable_bucket_table
        {
            //@cond
            struct default_options
            {
                typedef CDS_DEFAULT_ALLOCATOR       allocator;
                typedef opt::v::relaxed_ordering    memory_model;
                typedef FreeListImpl                free_list;
            };
            typedef typename opt::make_options< default_options, Options... >::type options;
            //@endcond

        public:
            typedef GC       gc;                            ///< Garbage collector
            typedef Node     node_type;                     ///< Bucket node type
            typedef typename options::allocator allocator;  ///< allocator

            /// Memory model for atomic operations
            typedef typename options::memory_model memory_model;

            /// Free-list
            typedef typename options::free_list free_list;

            /// Auxiliary node type
            struct aux_node_type: public node_type, public free_list::node
            {
#           ifdef CDS_DEBUG
                atomics::atomic<bool> m_busy;

                aux_node_type()
                {
                    m_busy.store( false, atomics::memory_order_release );
                }
#           endif
            };

        protected:
            //@cond
            typedef atomics::atomic<aux_node_type *> table_entry;    ///< Table entry type
            typedef atomics::atomic<table_entry *>   segment_type;   ///< Bucket table segment type

            struct aux_node_segment {
                atomics::atomic< size_t > aux_node_count; // how many aux nodes allocated from the segment
                aux_node_segment*         next_segment;
                // aux_node_type     nodes[];

                aux_node_segment()
                    : next_segment( nullptr )
                {
                    aux_node_count.store( 0, atomics::memory_order_release );
                }

                aux_node_type* segment()
                {
                    return reinterpret_cast<aux_node_type*>( this + 1 );
                }
            };

            /// Bucket table metrics
            struct metrics {
                size_t    nSegmentCount;    ///< max count of segments in bucket table
                size_t    nSegmentSize;     ///< the segment's capacity. The capacity must be power of two.
                size_t    nSegmentSizeLog2; ///< <tt> log2( m_nSegmentSize )</tt>
                size_t    nLoadFactor;      ///< load factor
                size_t    nCapacity;        ///< max capacity of bucket table

                metrics()
                    : nSegmentCount( 1024 )
                    , nSegmentSize( 512 )
                    , nSegmentSizeLog2( cds::beans::log2( nSegmentSize ))
                    , nLoadFactor( 1 )
                    , nCapacity( nSegmentCount * nSegmentSize )
                {}
            };

            /// Bucket table allocator
            typedef cds::details::Allocator< segment_type, allocator > bucket_table_allocator;

            /// Bucket table segment allocator
            typedef cds::details::Allocator< table_entry, allocator > segment_allocator;

            // Aux node segment allocator
            typedef typename std::allocator_traits< allocator >::template rebind_alloc<char> raw_allocator;

            //@endcond

        public:
            /// Constructs bucket table for 512K buckets. Load factor is 1.
            expandable_bucket_table()
                : m_metrics( calc_metrics( 512 * 1024, 1 ))
            {
                init();
            }

            /// Creates the table with specified capacity rounded up to nearest power-of-two
            expandable_bucket_table(
                size_t nItemCount,        ///< Max expected item count in split-ordered list
                size_t nLoadFactor        ///< Load factor
                )
                : m_metrics( calc_metrics( nItemCount, nLoadFactor ))
            {
                init();
            }

            /// Destroys bucket table
            ~expandable_bucket_table()
            {
                m_freeList.clear( []( typename free_list::node* ) {} );

                for ( auto aux_segment  = m_auxNodeList.load( atomics::memory_order_relaxed ); aux_segment; ) {
                    auto next_segment = aux_segment->next_segment;
                    free_aux_segment( aux_segment );
                    aux_segment = next_segment;
                }

                segment_type * pSegments = m_Segments;
                for ( size_t i = 0; i < m_metrics.nSegmentCount; ++i ) {
                    table_entry* pEntry = pSegments[i].load(memory_model::memory_order_relaxed);
                    if ( pEntry != nullptr )
                        destroy_segment( pEntry );
                }

                destroy_table( pSegments );
            }

            /// Returns head node of the bucket \p nBucket
            aux_node_type * bucket( size_t nBucket ) const
            {
                size_t nSegment = nBucket >> m_metrics.nSegmentSizeLog2;
                assert( nSegment < m_metrics.nSegmentCount );

                table_entry* pSegment = m_Segments[ nSegment ].load(memory_model::memory_order_acquire);
                if ( pSegment == nullptr )
                    return nullptr;    // uninitialized bucket
                return pSegment[ nBucket & (m_metrics.nSegmentSize - 1) ].load(memory_model::memory_order_acquire);
            }

            /// Set \p pNode as a head of bucket \p nBucket
            void bucket( size_t nBucket, aux_node_type * pNode )
            {
                size_t nSegment = nBucket >> m_metrics.nSegmentSizeLog2;
                assert( nSegment < m_metrics.nSegmentCount );

                segment_type& segment = m_Segments[nSegment];
                if ( segment.load( memory_model::memory_order_relaxed ) == nullptr ) {
                    table_entry* pNewSegment = allocate_segment();
                    table_entry * pNull = nullptr;
                    if ( !segment.compare_exchange_strong( pNull, pNewSegment, memory_model::memory_order_release, atomics::memory_order_relaxed ))
                        destroy_segment( pNewSegment );
                }

                assert( segment.load( atomics::memory_order_relaxed )[nBucket & (m_metrics.nSegmentSize - 1)].load( atomics::memory_order_relaxed ) == nullptr );
                segment.load(memory_model::memory_order_acquire)[ nBucket & (m_metrics.nSegmentSize - 1) ].store( pNode, memory_model::memory_order_release );
            }

            /// Allocates auxiliary node; can return \p nullptr if the table exhausted
            aux_node_type* alloc_aux_node()
            {
                aux_node_segment* aux_segment = m_auxNodeList.load( memory_model::memory_order_acquire );

                for ( ;; ) {
                    assert( aux_segment != nullptr );

                    // try to allocate from current aux segment
                    if ( aux_segment->aux_node_count.load( memory_model::memory_order_acquire ) < m_metrics.nSegmentSize ) {
                        size_t idx = aux_segment->aux_node_count.fetch_add( 1, memory_model::memory_order_relaxed );
                        if ( idx < m_metrics.nSegmentSize ) {
                            CDS_TSAN_ANNOTATE_NEW_MEMORY( aux_segment->segment() + idx, sizeof( aux_node_type ));
                            return new( aux_segment->segment() + idx ) aux_node_type();
                        }
                    }

                    // try allocate from free-list
                    auto pFree = m_freeList.get();
                    if ( pFree )
                        return static_cast<aux_node_type*>( pFree );

                    // free-list is empty, current segment is full
                    // try to allocate new aux segment
                    // We can allocate more aux segments than we need but it is not a problem in this context
                    aux_node_segment* new_aux_segment = allocate_aux_segment();
                    new_aux_segment->next_segment = aux_segment;
                    new_aux_segment->aux_node_count.fetch_add( 1, memory_model::memory_order_relaxed );

                    if ( m_auxNodeList.compare_exchange_strong( aux_segment, new_aux_segment, memory_model::memory_order_release, atomics::memory_order_acquire )) {
                        CDS_TSAN_ANNOTATE_NEW_MEMORY( new_aux_segment->segment(), sizeof( aux_node_type ));
                        return new( new_aux_segment->segment()) aux_node_type();
                    }

                    free_aux_segment( new_aux_segment );
                }
            }

            /// Places auxiliary node type to free-list
            void free_aux_node( aux_node_type* p )
            {
                m_freeList.put( static_cast<aux_node_type*>( p ));
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

        protected:
            //@cond
            metrics calc_metrics( size_t nItemCount, size_t nLoadFactor )
            {
                metrics m;

                // Calculate m_nSegmentSize and m_nSegmentCount by nItemCount
                m.nLoadFactor = nLoadFactor > 0 ? nLoadFactor : 1;

                size_t nBucketCount = ( nItemCount + m.nLoadFactor - 1 ) / m.nLoadFactor;
                if ( nBucketCount <= 2 ) {
                    m.nSegmentCount = 1;
                    m.nSegmentSize = 2;
                }
                else if ( nBucketCount <= 1024 ) {
                    m.nSegmentCount = 1;
                    m.nSegmentSize = ((size_t)1) << beans::log2ceil( nBucketCount );
                }
                else {
                    nBucketCount = beans::log2ceil( nBucketCount );
                    m.nSegmentCount =
                        m.nSegmentSize = ((size_t)1) << (nBucketCount / 2);
                    if ( nBucketCount & 1 )
                        m.nSegmentSize *= 2;
                    if ( m.nSegmentCount * m.nSegmentSize * m.nLoadFactor < nItemCount )
                        m.nSegmentSize *= 2;
                }
                m.nCapacity = m.nSegmentCount * m.nSegmentSize;
                m.nSegmentSizeLog2 = cds::beans::log2( m.nSegmentSize );
                assert( m.nSegmentSizeLog2 != 0 );   //
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

            table_entry* allocate_segment()
            {
                return segment_allocator().NewArray( m_metrics.nSegmentSize, nullptr );
            }

            void destroy_segment( table_entry* pSegment )
            {
                segment_allocator().Delete( pSegment, m_metrics.nSegmentSize );
            }

            aux_node_segment* allocate_aux_segment()
            {
                char* p = raw_allocator().allocate( sizeof( aux_node_segment ) + sizeof( aux_node_type ) * m_metrics.nSegmentSize );
                CDS_TSAN_ANNOTATE_NEW_MEMORY( p, sizeof( aux_node_segment ));
                return new(p) aux_node_segment();
            }

            void free_aux_segment( aux_node_segment* p )
            {
                raw_allocator().deallocate( reinterpret_cast<char*>( p ), sizeof( aux_node_segment ) + sizeof( aux_node_type ) * m_metrics.nSegmentSize );
            }

            void init()
            {
                // m_nSegmentSize must be 2**N
                assert( cds::beans::is_power2( m_metrics.nSegmentSize ));
                assert( (((size_t)1) << m_metrics.nSegmentSizeLog2) == m_metrics.nSegmentSize );

                // m_nSegmentCount must be 2**K
                assert( cds::beans::is_power2( m_metrics.nSegmentCount ));

                m_Segments = allocate_table();
                m_auxNodeList = allocate_aux_segment();
            }
            //@endcond

        protected:
            //@cond
            metrics const       m_metrics;  ///< Dynamic bucket table metrics
            segment_type*       m_Segments; ///< bucket table - array of segments
            atomics::atomic<aux_node_segment*> m_auxNodeList;  ///< segment list of aux nodes
            free_list           m_freeList; ///< List of free aux nodes
            //@endcond
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

            template <class OrderedList, class Traits, bool Iterable >
            class ordered_list_adapter;

            template <class OrderedList, class Traits>
            class ordered_list_adapter< OrderedList, Traits, false >
            {
                typedef OrderedList native_ordered_list;
                typedef Traits      traits;

                typedef typename native_ordered_list::gc                gc;
                typedef typename native_ordered_list::key_comparator    native_key_comparator;
                typedef typename native_ordered_list::node_type         node_type;
                typedef typename native_ordered_list::value_type        value_type;
                typedef typename native_ordered_list::node_traits       native_node_traits;
                typedef typename native_ordered_list::disposer          native_disposer;

                typedef split_list::node<node_type>                     splitlist_node_type;

                struct key_compare {
                    int operator()( value_type const& v1, value_type const& v2 ) const
                    {
                        splitlist_node_type const * n1 = static_cast<splitlist_node_type const *>(native_node_traits::to_node_ptr( v1 ));
                        splitlist_node_type const * n2 = static_cast<splitlist_node_type const *>(native_node_traits::to_node_ptr( v2 ));
                        if ( n1->m_nHash != n2->m_nHash )
                            return n1->m_nHash < n2->m_nHash ? -1 : 1;

                        if ( n1->is_dummy()) {
                            assert( n2->is_dummy());
                            return 0;
                        }

                        assert( !n1->is_dummy() && !n2->is_dummy());

                        return native_key_comparator()(v1, v2);
                    }

                    template <typename Q>
                    int operator()( value_type const& v, search_value_type<Q> const& q ) const
                    {
                        splitlist_node_type const * n = static_cast<splitlist_node_type const *>(native_node_traits::to_node_ptr( v ));
                        if ( n->m_nHash != q.nHash )
                            return n->m_nHash < q.nHash ? -1 : 1;

                        assert( !n->is_dummy());
                        return native_key_comparator()(v, q.val);
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
                        splitlist_node_type * p = static_cast<splitlist_node_type *>(native_node_traits::to_node_ptr( v ));
                        if ( !p->is_dummy())
                            native_disposer()(v);
                    }
                };

            public:
                typedef node_type ordered_list_node_type;
                typedef splitlist_node_type aux_node;

                struct node_traits: private native_node_traits
                {
                    typedef native_node_traits              base_class;     ///< Base ordered list node type
                    typedef typename base_class::value_type value_type;     ///< Value type
                    typedef typename base_class::node_type  base_node_type; ///< Ordered list node type
                    typedef node<base_node_type>            node_type;      ///< Split-list node type

                    /// Convert value reference to node pointer
                    static node_type * to_node_ptr( value_type& v )
                    {
                        return static_cast<node_type *>(base_class::to_node_ptr( v ));
                    }

                    /// Convert value pointer to node pointer
                    static node_type * to_node_ptr( value_type * v )
                    {
                        return static_cast<node_type *>(base_class::to_node_ptr( v ));
                    }

                    /// Convert value reference to node pointer (const version)
                    static node_type const * to_node_ptr( value_type const& v )
                    {
                        return static_cast<node_type const*>(base_class::to_node_ptr( v ));
                    }

                    /// Convert value pointer to node pointer (const version)
                    static node_type const * to_node_ptr( value_type const * v )
                    {
                        return static_cast<node_type const *>(base_class::to_node_ptr( v ));
                    }

                    /// Convert node reference to value pointer
                    static value_type * to_value_ptr( node_type&  n )
                    {
                        return base_class::to_value_ptr( static_cast<base_node_type &>(n));
                    }

                    /// Convert node pointer to value pointer
                    static value_type * to_value_ptr( node_type *  n )
                    {
                        return base_class::to_value_ptr( static_cast<base_node_type *>(n));
                    }

                    /// Convert node reference to value pointer (const version)
                    static const value_type * to_value_ptr( node_type const & n )
                    {
                        return base_class::to_value_ptr( static_cast<base_node_type const &>(n));
                    }

                    /// Convert node pointer to value pointer (const version)
                    static const value_type * to_value_ptr( node_type const * n )
                    {
                        return base_class::to_value_ptr( static_cast<base_node_type const *>(n));
                    }
                };

                template <typename Less>
                struct make_compare_from_less: public cds::opt::details::make_comparator_from_less<Less>
                {
                    typedef cds::opt::details::make_comparator_from_less<Less>  base_class;

                    template <typename Q>
                    int operator()( value_type const& v, search_value_type<Q> const& q ) const
                    {
                        splitlist_node_type const * n = static_cast<splitlist_node_type const *>(native_node_traits::to_node_ptr( v ));
                        if ( n->m_nHash != q.nHash )
                            return n->m_nHash < q.nHash ? -1 : 1;

                        assert( !n->is_dummy());
                        return base_class()(v, q.val);
                    }

                    template <typename Q>
                    int operator()( search_value_type<Q> const& q, value_type const& v ) const
                    {
                        splitlist_node_type const * n = static_cast<splitlist_node_type const *>(native_node_traits::to_node_ptr( v ));
                        if ( n->m_nHash != q.nHash )
                            return q.nHash < n->m_nHash ? -1 : 1;

                        assert( !n->is_dummy());
                        return base_class()(q.val, v);
                    }

                    int operator()( value_type const& lhs, value_type const& rhs ) const
                    {
                        splitlist_node_type const * n1 = static_cast<splitlist_node_type const *>(native_node_traits::to_node_ptr( lhs ));
                        splitlist_node_type const * n2 = static_cast<splitlist_node_type const *>(native_node_traits::to_node_ptr( rhs ));
                        if ( n1->m_nHash != n2->m_nHash )
                            return n1->m_nHash < n2->m_nHash ? -1 : 1;

                        if ( n1->is_dummy()) {
                            assert( n2->is_dummy());
                            return 0;
                        }

                        assert( !n1->is_dummy() && !n2->is_dummy());

                        return native_key_comparator()( lhs, rhs );
                    }
                };

                typedef typename native_ordered_list::template rebind_traits<
                    opt::compare< key_compare >
                    , opt::disposer< wrapped_disposer >
                    , opt::boundary_node_type< splitlist_node_type >
                >::type result;
            };

            template <class OrderedList, class Traits>
            class ordered_list_adapter< OrderedList, Traits, true >
            {
                typedef OrderedList native_ordered_list;
                typedef Traits      traits;

                typedef typename native_ordered_list::gc                gc;
                typedef typename native_ordered_list::key_comparator    native_key_comparator;
                typedef typename native_ordered_list::value_type        value_type;
                typedef typename native_ordered_list::disposer          native_disposer;

                struct key_compare {
                    int operator()( value_type const& v1, value_type const& v2 ) const
                    {
                        hash_node const& n1 = static_cast<hash_node const&>( v1 );
                        hash_node const& n2 = static_cast<hash_node const&>( v2 );
                        if ( n1.m_nHash != n2.m_nHash )
                            return n1.m_nHash < n2.m_nHash ? -1 : 1;

                        if ( n1.is_dummy()) {
                            assert( n2.is_dummy());
                            return 0;
                        }

                        assert( !n1.is_dummy() && !n2.is_dummy());

                        return native_key_comparator()(v1, v2);
                    }

                    template <typename Q>
                    int operator()( value_type const& v, search_value_type<Q> const& q ) const
                    {
                        hash_node const& n = static_cast<hash_node const&>( v );
                        if ( n.m_nHash != q.nHash )
                            return n.m_nHash < q.nHash ? -1 : 1;

                        assert( !n.is_dummy());
                        return native_key_comparator()(v, q.val);
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
                        if ( !static_cast<hash_node*>( v )->is_dummy())
                            native_disposer()( v );
                    }
                };

            public:
                typedef void ordered_list_node_type;

                struct aux_node: public native_ordered_list::node_type, public hash_node
                {
                    aux_node()
                    {
                        typedef typename native_ordered_list::node_type list_node_type;

                        list_node_type::data.store( typename list_node_type::marked_data_ptr(
                            static_cast<value_type*>( static_cast<hash_node *>( this ))),
                            atomics::memory_order_release
                        );
                    }
                };

                struct node_traits
                {
                    static hash_node * to_node_ptr( value_type& v )
                    {
                        return static_cast<hash_node *>( &v );
                    }

                    static hash_node * to_node_ptr( value_type * v )
                    {
                        return static_cast<hash_node *>( v );
                    }

                    static hash_node const * to_node_ptr( value_type const& v )
                    {
                        return static_cast<hash_node const*>( &v );
                    }

                    static hash_node const * to_node_ptr( value_type const * v )
                    {
                        return static_cast<hash_node const *>( v );
                    }
                };

                template <typename Less>
                struct make_compare_from_less: public cds::opt::details::make_comparator_from_less<Less>
                {
                    typedef cds::opt::details::make_comparator_from_less<Less>  base_class;

                    template <typename Q>
                    int operator()( value_type const& v, search_value_type<Q> const& q ) const
                    {
                        hash_node const& n = static_cast<hash_node const&>( v );
                        if ( n.m_nHash != q.nHash )
                            return n.m_nHash < q.nHash ? -1 : 1;

                        assert( !n.is_dummy());
                        return base_class()(v, q.val);
                    }

                    template <typename Q>
                    int operator()( search_value_type<Q> const& q, value_type const& v ) const
                    {
                        hash_node const& n = static_cast<hash_node const&>( v );
                        if ( n.m_nHash != q.nHash )
                            return q.nHash < n.m_nHash ? -1 : 1;

                        assert( !n.is_dummy());
                        return base_class()(q.val, v);
                    }

                    int operator()( value_type const& lhs, value_type const& rhs ) const
                    {
                        hash_node const& n1 = static_cast<hash_node const&>( lhs );
                        hash_node const& n2 = static_cast<hash_node const&>( rhs );
                        if ( n1.m_nHash != n2.m_nHash )
                            return n1.m_nHash < n2.m_nHash ? -1 : 1;

                        if ( n1.is_dummy()) {
                            assert( n2.is_dummy());
                            return 0;
                        }

                        assert( !n1.is_dummy() && !n2.is_dummy());

                        return base_class()( lhs, rhs );
                    }
                };

                typedef typename native_ordered_list::template rebind_traits<
                    opt::compare< key_compare >
                    , opt::disposer< wrapped_disposer >
                    , opt::boundary_node_type< aux_node >
                >::type result;
            };

            template <class OrderedList, class Traits>
            using rebind_list_traits = ordered_list_adapter< OrderedList, Traits, is_iterable_list<OrderedList>::value >;

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
                    while ( m_itCur != m_itEnd && node_traits::to_node_ptr( *m_itCur )->is_dummy())
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
                        } while ( m_itCur != m_itEnd && node_traits::to_node_ptr( *m_itCur )->is_dummy());
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

            protected:
                list_iterator const& underlying_iterator() const
                {
                    return m_itCur;
                }
            };
        }   // namespace details
        //@endcond

        //@cond
        // Helper functions
        template <typename BitReversalAlgo>
        static inline size_t regular_hash( size_t nHash )
        {
            return static_cast<size_t>( BitReversalAlgo()( cds::details::size_t_cast( nHash ))) | size_t(1);
        }

        template <typename BitReversalAlgo>
        static inline size_t dummy_hash( size_t nHash )
        {
            return static_cast<size_t>( BitReversalAlgo()( cds::details::size_t_cast( nHash ))) & ~size_t(1);
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

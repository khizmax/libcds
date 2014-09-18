//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_SPLIT_LIST_BASE_H
#define __CDS_INTRUSIVE_SPLIT_LIST_BASE_H

#include <cds/intrusive/base.h>
#include <cds/cxx11_atomic.h>
#include <cds/details/allocator.h>
#include <cds/int_algo.h>
#include <cds/bitop.h>
#include <cds/details/functor_wrapper.h>

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


        /// Type traits for SplitListSet class
        struct type_traits {
            /// Hash function
            /**
                Hash function converts the key fields of struct \p T stored in the split list
                into value of type \p size_t called hash value that is an index of hash table.

                This is mandatory type and has no predefined one.
            */
            typedef opt::none       hash;

            /// Item counter
            /**
                The item counting is an important part of SplitListSet algorithm:
                the <tt>empty()</tt> member function depends on correct item counting.
                Therefore, atomicity::empty_item_counter is not allowed as a type of the option.

                Default is atomicity::item_counter.
            */
            typedef atomicity::item_counter item_counter;

            /// Bucket table allocator
            /**
                Allocator for bucket table. Default is \ref CDS_DEFAULT_ALLOCATOR
            */
            typedef CDS_DEFAULT_ALLOCATOR   allocator;

            /// C++ memory model for atomic operations
            /**
                Can be opt::v::relaxed_ordering (relaxed memory model, the default) or opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering    memory_model;

            /// What type of bucket table is used
            /**
                \p true - use split_list::expandable_bucket_table that can be expanded
                    if the load factor of the set is exhausted
                \p false - use split_list::static_bucket_table that cannot be expanded

                Default is \p true.
            */
            static const bool dynamic_bucket_table = true;

            /// back-off strategy used
            /**
                If the option is not specified, the cds::backoff::Default is used.
            */
            typedef cds::backoff::Default             back_off;
        };

        /// [value-option] Split-list dynamic bucket table option
        /**
            The option is used to select bucket table implementation.
            Possible values of \p Value are:
            - \p true - select \ref expandable_bucket_table implementation
            - \p false - select \ref static_bucket_table implementation
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

        /// Metafunction converting option list to traits struct
        /**
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>

            Available \p Options:
            - opt::hash - mandatory option, specifies hash functor.
            - opt::item_counter - optional, specifies item counting policy. See type_traits::item_counter
                for default type.
            - opt::memory_model - C++ memory model for atomic operations.
                Can be opt::v::relaxed_ordering (relaxed memory model, the default) or opt::v::sequential_consistent (sequentially consisnent memory model).
            - opt::allocator - optional, bucket table allocator. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - split_list::dynamic_bucket_table - use dynamic or static bucket table implementation.
                Dynamic bucket table expands its size up to maximum bucket count when necessary
            - opt::back_off - back-off strategy used for spinning. If the option is not specified, the cds::backoff::Default is used.

            See \ref MichaelHashSet, \ref type_traits.
        */
        template <CDS_DECL_OPTIONS6>
        struct make_traits {
            typedef typename cds::opt::make_options< type_traits, CDS_OPTIONS6>::type type  ;   ///< Result of metafunction
        };


        /// Static bucket table
        /**
            Non-resizeable bucket table for SplitListSet class.
            The capacity of table (max bucket count) is defined in the constructor call.

            Template parameter:
            - \p GC - garbage collector used
            - \p Node - node type, must be a type based on\ref node template
            - \p Options... - options

            \p Options are:
            - \p opt::allocator - allocator used to allocate bucket table. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::memory_model - memory model used. Possible types are opt::v::sequential_consistent, opt::v::relaxed_ordering
        */
        template <typename GC, typename Node, CDS_DECL_OPTIONS2>
        class static_bucket_table
        {
            //@cond
            struct default_options
            {
                typedef CDS_DEFAULT_ALLOCATOR       allocator;
                typedef opt::v::relaxed_ordering    memory_model;
            };
            typedef typename opt::make_options< default_options, CDS_OPTIONS2 >::type   options;
            //@endcond

        public:
            typedef GC      gc          ;   ///< Garbage collector
            typedef Node    node_type   ;   ///< Bucket node type
            typedef CDS_ATOMIC::atomic<node_type *> table_entry ;   ///< Table entry type

            /// Bucket table allocator
            typedef cds::details::Allocator< table_entry, typename options::allocator >  bucket_table_allocator;

            /// Memory model for atomic operations
            typedef typename options::memory_model     memory_model;

        protected:
            const size_t   m_nLoadFactor    ;    ///< load factor (average count of items per bucket)
            const size_t   m_nCapacity      ;    ///< Bucket table capacity
            table_entry *  m_Table          ;    ///< Bucket table

        protected:
            //@cond
            void allocate_table()
            {
                m_Table = bucket_table_allocator().NewArray( m_nCapacity, null_ptr<node_type *>() );
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

            /// Constructs
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

            /// Destroy bucket table
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

            /// Set head node \p pNode of bucket \p nBucket
            void bucket( size_t nBucket, node_type * pNode )
            {
                assert( nBucket < capacity() );
                assert( bucket(nBucket) == null_ptr<node_type *>() );

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
            - \p GC - garbage collector used
            - \p Node - node type, must be an instantiation of \ref node template
            - \p Options... - options

            \p Options are:
            - \p opt::allocator - allocator used to allocate bucket table. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::memory_model - memory model used. Possible types are opt::v::sequential_consistent, opt::v::relaxed_ordering
        */
        template <typename GC, typename Node, CDS_DECL_OPTIONS2>
        class expandable_bucket_table
        {
            //@cond
            struct default_options
            {
                typedef CDS_DEFAULT_ALLOCATOR       allocator;
                typedef opt::v::relaxed_ordering    memory_model;
            };
            typedef typename opt::make_options< default_options, CDS_OPTIONS2 >::type   options;
            //@endcond
        public:
            typedef GC      gc          ;   ///< Garbage collector
            typedef Node    node_type   ;   ///< Bucket node type
            typedef CDS_ATOMIC::atomic<node_type *> table_entry ;   ///< Table entry type

            /// Memory model for atomic operations
            typedef typename options::memory_model     memory_model;

        protected:
            typedef CDS_ATOMIC::atomic<table_entry *>   segment_type    ;   ///< Bucket table segment type

        public:
            /// Bucket table allocator
            typedef cds::details::Allocator< segment_type, typename options::allocator >  bucket_table_allocator;

            /// Bucket table segment allocator
            typedef cds::details::Allocator< table_entry, typename options::allocator >  segment_allocator;

        protected:
            /// Bucket table metrics
            struct metrics {
                size_t    nSegmentCount     ;    ///< max count of segments in bucket table
                size_t    nSegmentSize      ;    ///< the segment's capacity. The capacity must be power of two.
                size_t    nSegmentSizeLog2  ;    ///< log2( m_nSegmentSize )
                size_t    nLoadFactor       ;    ///< load factor
                size_t    nCapacity         ;    ///< max capacity of bucket table

                metrics()
                    : nSegmentCount(1024)
                    , nSegmentSize(512)
                    , nSegmentSizeLog2( cds::beans::log2( nSegmentSize ) )
                    , nLoadFactor(1)
                    , nCapacity( nSegmentCount * nSegmentSize )
                {}
            };

            const metrics   m_metrics   ;    ///< Dynamic bucket table metrics

        protected:
            //const size_t   m_nLoadFactor;    ///< load factor (average count of items per bucket)
            //const size_t   m_nCapacity  ;    ///< Bucket table capacity
            segment_type * m_Segments   ;    ///< bucket table - array of segments

        protected:
            //@cond
            metrics calc_metrics( size_t nItemCount, size_t nLoadFactor )
            {
                metrics m;

                // Calculate m_nSegmentSize and m_nSegmentCount  by nItemCount
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
                return bucket_table_allocator().NewArray( m_metrics.nSegmentCount, null_ptr<table_entry *>() );
            }

            void destroy_table( segment_type * pTable )
            {
                bucket_table_allocator().Delete( pTable, m_metrics.nSegmentCount );
            }

            table_entry * allocate_segment()
            {
                return segment_allocator().NewArray( m_metrics.nSegmentSize, null_ptr<node_type *>() );
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

            /// Constructs
            expandable_bucket_table(
                size_t nItemCount,        ///< Max expected item count in split-ordered list
                size_t nLoadFactor        ///< Load factor
                )
                : m_metrics( calc_metrics( nItemCount, nLoadFactor ))
            {
                init_segments();
            }

            /// Destroy bucket table
            ~expandable_bucket_table()
            {
                segment_type * pSegments = m_Segments;
                for ( size_t i = 0; i < m_metrics.nSegmentCount; ++i ) {
                    table_entry * pEntry = pSegments[i].load(memory_model::memory_order_relaxed);
                    if ( pEntry != null_ptr<table_entry *>() )
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
                if ( pSegment == null_ptr<table_entry *>() )
                    return null_ptr<node_type *>() ;    // uninitialized bucket
                return pSegment[ nBucket & (m_metrics.nSegmentSize - 1) ].load(memory_model::memory_order_acquire);
            }

            /// Set head node \p pNode of bucket \p nBucket
            void bucket( size_t nBucket, node_type * pNode )
            {
                size_t nSegment = nBucket >> m_metrics.nSegmentSizeLog2;
                assert( nSegment < m_metrics.nSegmentCount );

                segment_type& segment = m_Segments[nSegment];
                if ( segment.load(memory_model::memory_order_relaxed) == null_ptr<table_entry *>() ) {
                    table_entry * pNewSegment = allocate_segment();
                    table_entry * pNull = null_ptr<table_entry *>();
                    if ( !segment.compare_exchange_strong( pNull, pNewSegment, memory_model::memory_order_release, CDS_ATOMIC::memory_order_relaxed )) {
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
            typedef BaseNodeTraits base_class ;     ///< Base ordered list type
            typedef typename base_class::value_type value_type      ;   ///< Value type
            typedef typename base_class::node_type  base_node_type  ;   ///< Ordered list node type
            typedef node<base_node_type>            node_type       ;   ///< Spit-list node type

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
            template <bool Value, typename GC, typename Node, CDS_DECL_OPTIONS2>
            struct bucket_table_selector;

            template <typename GC, typename Node, CDS_SPEC_OPTIONS2>
            struct bucket_table_selector< true, GC, Node, CDS_OPTIONS2>
            {
                typedef expandable_bucket_table<GC, Node, CDS_OPTIONS2>    type;
            };

            template <typename GC, typename Node, CDS_SPEC_OPTIONS2>
            struct bucket_table_selector< false, GC, Node, CDS_OPTIONS2>
            {
                typedef static_bucket_table<GC, Node, CDS_OPTIONS2>    type;
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
                /*
                operator Q&() const
                {
                    return val;
                }
                */
            };

#       ifndef CDS_CXX11_LAMBDA_SUPPORT
            template <typename Func>
            class find_functor_wrapper: protected cds::details::functor_wrapper<Func>
            {
                typedef cds::details::functor_wrapper<Func> base_class;
            public:
                find_functor_wrapper( Func f )
                    : base_class( f )
                {}

                template <typename ValueType, typename Q>
                void operator()( ValueType& item, split_list::details::search_value_type<Q>& val )
                {
                    base_class::get()( item, val.val );
                }
            };
#       endif

            template <class OrderedList, class Options>
            class rebind_list_options
            {
                typedef OrderedList native_ordered_list;
                typedef Options     options;

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
                            dummy_node_disposer<gc, typename options::allocator>()( p );
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

                typedef typename native_ordered_list::template rebind_options<
                    opt::compare< key_compare >
                    ,opt::disposer< wrapped_disposer >
                    ,opt::boundary_node_type< splitlist_node_type >
                >::type    result;
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

    }   // namespace split_list

    //@cond
    // Forward declaration
    template <class GC, class OrderedList, class Traits = split_list::type_traits>
    class SplitListSet;
    //@endcond

}}  // namespace cds::intrusive

#endif // #ifndef __CDS_INTRUSIVE_SPLIT_LIST_BASE_H

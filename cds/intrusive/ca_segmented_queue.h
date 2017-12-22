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

#ifndef CDSLIB_INTRUSIVE_CA_SEGMENTED_QUEUE_H
#define CDSLIB_INTRUSIVE_CA_SEGMENTED_QUEUE_H

#include <mutex>
#include <cds/intrusive/details/base.h>
#include <cds/details/marked_ptr.h>
#include <cds/algo/int_algo.h>
#include <cds/sync/spinlock.h>
#include <cds/opt/permutation.h>

#include <boost/intrusive/slist.hpp>

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning( push )
#   pragma warning( disable: 4355 ) // warning C4355: 'this' : used in base member initializer list
#endif

namespace cds { namespace intrusive {

    /// CASegmentedQueue -related declarations
    namespace ca_segmented_queue {

        /// CASegmentedQueue internal statistics. May be used for debugging or profiling
        template <typename Counter = cds::atomicity::event_counter >
        struct stat
        {
            typedef Counter  counter_type;  ///< Counter type

            counter_type    m_nPush;            ///< Push count
            counter_type    m_nPushPopulated;   ///< Number of attempts to push to populated (non-empty) cell
            counter_type    m_nPushContended;   ///< Number of failed CAS when pushing
            counter_type    m_nPop;             ///< Pop count
            counter_type    m_nPopEmpty;        ///< Number of dequeuing from empty queue
            counter_type    m_nPopContended;    ///< Number of failed CAS when popping

            counter_type    m_nCreateSegmentReq;    ///< Number of request to create new segment
            counter_type    m_nDeleteSegmentReq;    ///< Number to request to delete segment
            counter_type    m_nSegmentCreated;  ///< Number of created segments
            counter_type    m_nSegmentDeleted;  ///< Number of deleted segments

            //@cond
            void onPush()               { ++m_nPush; }
            void onPushPopulated()      { ++m_nPushPopulated; }
            void onPushContended()      { ++m_nPushContended; }
            void onPop()                { ++m_nPop;  }
            void onPopEmpty()           { ++m_nPopEmpty; }
            void onPopContended()       { ++m_nPopContended; }
            void onCreateSegmentReq()   { ++m_nCreateSegmentReq; }
            void onDeleteSegmentReq()   { ++m_nDeleteSegmentReq; }
            void onSegmentCreated()     { ++m_nSegmentCreated; }
            void onSegmentDeleted()     { ++m_nSegmentDeleted; }
            //@endcond
        };

        /// Dummy CASegmentedQueue statistics, no overhead
        struct empty_stat {
            //@cond
            void onPush() const             {}
            void onPushPopulated() const    {}
            void onPushContended() const    {}
            void onPop() const              {}
            void onPopEmpty() const         {}
            void onPopContended() const     {}
            void onCreateSegmentReq() const {}
            void onDeleteSegmentReq() const {}
            void onSegmentCreated() const   {}
            void onSegmentDeleted() const   {}
            //@endcond
        };

        class SkewGenerator {
            /// Generator Size
            size_t size;

            /// Current element index
            size_t current;

            /// Initializes the generator of length nLength
            generator( size_t nLength )
            {
              size = nLength;
              current = 0;
            }

            /// Returns current value
            operator int()
            {
              return current;
            }

            /// Goes to next value. Returns false if the permutation is exchausted
            bool next()
            {
              if(size - 1 <= current )
                  return false;

              ++current;
              return true;
            }

            /// Resets the generator to produce the new sequence
            void reset()
            {
              current = 0;
            }
        };

        /// CASegmentedQueue default traits
        struct traits {
            /// Element disposer that is called when the item to be dequeued. Default is opt::v::empty_disposer (no disposer)
            typedef opt::v::empty_disposer disposer;

            /// Item counter, default is atomicity::item_counter
            /**
                The item counting is an essential part of segmented queue algorithm.
                The \p empty() member function is based on checking <tt>size() == 0</tt>.
                Therefore, dummy item counter like atomicity::empty_item_counter is not the proper counter.
            */
            typedef atomicity::item_counter item_counter;

            /// Internal statistics, possible predefined types are \ref stat, \ref empty_stat (the default)
            typedef ca_segmented_queue::empty_stat        stat;

            /// Memory model, default is opt::v::relaxed_ordering. See cds::opt::memory_model for the full list of possible types
            typedef opt::v::relaxed_ordering  memory_model;

            /// Alignment of critical data, default is cache line alignment. See cds::opt::alignment option specification
            enum { alignment = opt::cache_line_alignment };

            /// Padding of segment data, default is no special padding
            /**
                The segment is just an array of atomic data pointers,
                so, the high load leads to false sharing and performance degradation.
                A padding of segment data can eliminate false sharing issue.
                On the other hand, the padding leads to increase segment size.
            */
            enum { padding = opt::no_special_padding };

            /// Segment allocator. Default is \ref CDS_DEFAULT_ALLOCATOR
            typedef CDS_DEFAULT_ALLOCATOR allocator;

            /// Lock type used to maintain an internal list of allocated segments
            typedef cds::sync::spin lock_type;

            /// Random \ref cds::opt::permutation_generator "permutation generator" for sequence [0, quasi_factor)
            typedef cds::opt::v::skew_permutation<SkewGenerator>    permutation_generator;
        };

        /// Metafunction converting option list to traits for CASegmentedQueue
        /**
            The metafunction can be useful if a few fields in \p ca_segmented_queue::traits should be changed.
            For example:
            \code
            typedef cds::intrusive::ca_segmented_queue::make_traits<
                cds::opt::item_counter< cds::atomicity::item_counter >
            >::type my_ca_segmented_queue_traits;
            \endcode
            This code creates \p %CASegmentedQueue type traits with item counting feature,
            all other \p %ca_segmented_queue::traits members left unchanged.

            \p Options are:
            - \p opt::disposer - the functor used to dispose removed items.
            - \p opt::stat - internal statistics, possible type: \p ca_segmented_queue::stat, \p ca_segmented_queue::empty_stat (the default)
            - \p opt::item_counter - item counting feature. Note that \p atomicity::empty_item_counetr is not suitable
                for segmented queue.
            - \p opt::memory_model - memory model, default is \p opt::v::relaxed_ordering.
                See option description for the full list of possible models
            - \p opt::alignment - the alignment for critical data, see option description for explanation
            - \p opt::padding - the padding of segment data, default no special padding.
                See \p traits::padding for explanation.
            - \p opt::allocator - the allocator to be used for maintaining segments.
            - \p opt::lock_type - a mutual exclusion lock type used to maintain internal list of allocated
                segments. Default is \p cds::opt::Spin, \p std::mutex is also suitable.
            - \p opt::permutation_generator - a random permutation generator for sequence [0, quasi_factor),
                default is \p cds::opt::v::random2_permutation<int>
        */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#   endif
        };
    } // namespace ca_segmented_queue

    /// Cach-Aware Segmented queue
    /** @ingroup cds_intrusive_queue

        The queue is based on work
        - [2010] Gidenstam,Sundell,Tsigas Cache-Aware Lock-free Queues for Multiple Producers-Consumers and Weak Memory Consistenc

        A lock-free FIFO queue data structure is presented in this paper. The
        algorithm supports multiple producers and multiple consumers and weak mem-
        ory models. It has been designed to be cache-aware and work directly on weak
        memory models. It utilizes the cache behavior in concert with lazy updates of
        shared data, and a dynamic lock-free memory management scheme to decrease
        unnecessary synchronization and increase performance. Experiments on an 8-
        way multi-core platform show significantly better performance for the new algo-
        rithm compared to previous fast lock-free algorithms.

        Template parameters:
        - \p GC - a garbage collector, possible types are cds::gc::HP, cds::gc::DHP
        - \p T - the type of values stored in the queue
        - \p Traits - queue type traits, default is \p ca_segmented_queue::traits.
            \p ca_ca_segmented_queue::make_traits metafunction can be used to construct the
            type traits.

        The queue stores the pointers to enqueued items so no special node hooks are needed.
    */
    template <class GC, typename T, typename Traits = ca_segmented_queue::traits >
    class CASegmentedQueue
    {
    public:
        typedef GC  gc;         ///< Garbage collector
        typedef T   value_type; ///< type of the value stored in the queue
        typedef Traits traits;  ///< Queue traits

        typedef typename traits::disposer      disposer    ;   ///< value disposer, called only in \p clear() when the element to be dequeued
        typedef typename traits::allocator     allocator;   ///< Allocator maintaining the segments
        typedef typename traits::memory_model  memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::item_counter  item_counter;   ///< Item counting policy, see cds::opt::item_counter option setter
        typedef typename traits::stat          stat;   ///< Internal statistics policy
        typedef typename traits::lock_type     lock_type;   ///< Type of mutex for maintaining an internal list of allocated segments.
        typedef typename traits::permutation_generator permutation_generator; ///< Random permutation generator for sequence [0, quasi-factor)

        static const size_t c_nHazardPtrCount = 2 ; ///< Count of hazard pointer required for the algorithm

    protected:
        //@cond
        // Segment cell. LSB is used as deleted mark
        typedef cds::details::marked_ptr< value_type, 1 > regular_cell;
        typedef atomics::atomic< regular_cell > atomic_cell;
        typedef typename cds::opt::details::apply_padding< atomic_cell, traits::padding >::type cell;

        // Segment
        struct segment: public boost::intrusive::slist_base_hook<>
        {
            cell * cells;    // Cell array of size \ref m_nQuasiFactor
            size_t version;  // version tag (ABA prevention tag)
            // cell array is placed here in one continuous memory block

            // Initializes the segment
            explicit segment( size_t nCellCount )
                // MSVC warning C4355: 'this': used in base member initializer list
                : cells( reinterpret_cast< cell *>( this + 1 ))
                , version( 0 )
            {
                init( nCellCount );
            }

            segment() = delete;

            void init( size_t nCellCount )
            {
                cell * pLastCell = cells + nCellCount;
                for ( cell* pCell = cells; pCell < pLastCell; ++pCell )
                    pCell->data.store( regular_cell(), atomics::memory_order_relaxed );
                atomics::atomic_thread_fence( memory_model::memory_order_release );
            }
        };

        typedef typename opt::details::alignment_setter< atomics::atomic<segment *>, traits::alignment >::type aligned_segment_ptr;
        //@endcond

    protected:
        //@cond
        class segment_list
        {
            typedef boost::intrusive::slist< segment, boost::intrusive::cache_last< true > > list_impl;
            typedef std::unique_lock< lock_type > scoped_lock;

            aligned_segment_ptr m_pHead;
            aligned_segment_ptr m_pTail;

            list_impl           m_List;
            mutable lock_type   m_Lock;
            size_t const        m_nQuasiFactor;
            stat&               m_Stat;

        private:
            struct segment_disposer
            {
                void operator()( segment * pSegment )
                {
                    assert( pSegment != nullptr );
                    free_segment( pSegment );
                }
            };

            struct gc_segment_disposer
            {
                void operator()( segment * pSegment )
                {
                    assert( pSegment != nullptr );
                    retire_segment( pSegment );
                }
            };

        public:
            segment_list( size_t nQuasiFactor, stat& st )
                : m_pHead( nullptr )
                , m_pTail( nullptr )
                , m_nQuasiFactor( nQuasiFactor )
                , m_Stat( st )
            {
                assert( cds::beans::is_power2( nQuasiFactor ));
            }

            ~segment_list()
            {
                m_List.clear_and_dispose( gc_segment_disposer());
            }

            segment * head( typename gc::Guard& guard )
            {
                return guard.protect( m_pHead );
            }

            segment * tail( typename gc::Guard& guard )
            {
                return guard.protect( m_pTail );
            }

#       ifdef _DEBUG
            bool populated( segment const& s ) const
            {
                // The lock should be held
                cell const * pLastCell = s.cells + quasi_factor();
                for ( cell const * pCell = s.cells; pCell < pLastCell; ++pCell ) {
                    if ( !pCell->data.load( memory_model::memory_order_relaxed ).all())
                        return false;
                }
                return true;
            }
            bool exhausted( segment const& s ) const
            {
                // The lock should be held
                cell const * pLastCell = s.cells + quasi_factor();
                for ( cell const * pCell = s.cells; pCell < pLastCell; ++pCell ) {
                    if ( !pCell->data.load( memory_model::memory_order_relaxed ).bits())
                        return false;
                }
                return true;
            }
#       endif

            segment * create_tail( segment * pTail, typename gc::Guard& guard )
            {
                // pTail is guarded by GC

                m_Stat.onCreateSegmentReq();

                scoped_lock l( m_Lock );

                if ( !m_List.empty() && ( pTail != &m_List.back() || get_version(pTail) != m_List.back().version )) {
                    m_pTail.store( &m_List.back(), memory_model::memory_order_relaxed );

                    return guard.assign( &m_List.back());
                }

#           ifdef _DEBUG
                assert( m_List.empty() || populated( m_List.back()));
#           endif

                segment * pNew = allocate_segment();
                m_Stat.onSegmentCreated();

                if ( m_List.empty())
                    m_pHead.store( pNew, memory_model::memory_order_release );
                m_List.push_back( *pNew );
                m_pTail.store( pNew, memory_model::memory_order_release );
                return guard.assign( pNew );
            }

            segment * remove_head( segment * pHead, typename gc::Guard& guard )
            {
                // pHead is guarded by GC
                m_Stat.onDeleteSegmentReq();

                segment * pRet;
                {
                    scoped_lock l( m_Lock );

                    if ( m_List.empty()) {
                        m_pTail.store( nullptr, memory_model::memory_order_relaxed );
                        m_pHead.store( nullptr, memory_model::memory_order_relaxed );
                        return guard.assign( nullptr );
                    }

                    if ( pHead != &m_List.front() || get_version(pHead) != m_List.front().version ) {
                        m_pHead.store( &m_List.front(), memory_model::memory_order_relaxed );
                        return guard.assign( &m_List.front());
                    }

#           ifdef _DEBUG
                    assert( exhausted( m_List.front()));
#           endif

                    m_List.pop_front();
                    if ( m_List.empty()) {
                        pRet = guard.assign( nullptr );
                        m_pTail.store( nullptr, memory_model::memory_order_relaxed );
                    }
                    else
                        pRet = guard.assign( &m_List.front());
                    m_pHead.store( pRet, memory_model::memory_order_release );
                }

                retire_segment( pHead );
                m_Stat.onSegmentDeleted();

                return pRet;
            }

            size_t quasi_factor() const
            {
                return m_nQuasiFactor;
            }

        private:
            typedef cds::details::Allocator< segment, allocator >   segment_allocator;

            static size_t get_version( segment * pSegment )
            {
                return pSegment ? pSegment->version : 0;
            }

            segment * allocate_segment()
            {
                return segment_allocator().NewBlock( sizeof(segment) + sizeof(cell) * m_nQuasiFactor, quasi_factor());
            }

            static void free_segment( segment * pSegment )
            {
                segment_allocator().Delete( pSegment );
            }

            static void retire_segment( segment * pSegment )
            {
                gc::template retire<segment_disposer>( pSegment );
            }
        };
        //@endcond

    protected:
        segment_list              m_SegmentList;  ///< List of segments

        item_counter              m_ItemCounter;  ///< Item counter
        stat                      m_Stat;         ///< Internal statistics

    public:
        /// Initializes the empty queue
        CASegmentedQueue(
            size_t nQuasiFactor     ///< Quasi factor. If it is not a power of 2 it is rounded up to nearest power of 2. Minimum is 2.
            )
            : m_SegmentList( cds::beans::ceil2(nQuasiFactor), m_Stat )
        {
            static_assert( (!std::is_same< item_counter, cds::atomicity::empty_item_counter >::value),
                "cds::atomicity::empty_item_counter is not supported for CASegmentedQueue"
                );
            assert( m_SegmentList.quasi_factor() > 1 );
        }

        /// Clears the queue and deletes all internal data
        ~CASegmentedQueue()
        {
            clear();
        }

        /// Inserts a new element at last segment of the queue
        bool enqueue( value_type& val )
        {
            // LSB is used as a flag in marked pointer
            assert( (reinterpret_cast<uintptr_t>( &val ) & 1) == 0 );

            typename gc::Guard segmentGuard;
            segment * pTailSegment = m_SegmentList.tail( segmentGuard );
            if ( !pTailSegment ) {
                // no segments, create the new one
                pTailSegment = m_SegmentList.create_tail( pTailSegment, segmentGuard );
                assert( pTailSegment );
            }

            permutation_generator gen(0, quasi_factor());

            // First, increment item counter.
            // We sure that the item will be enqueued
            // but if we increment the counter after inserting we can get a negative counter value
            // if dequeuing occurs before incrementing (enqueue/dequeue race)
            ++m_ItemCounter;

            while ( true ) {
                CDS_DEBUG_ONLY( size_t nLoopCount = 0);
                do {
                    typename permutation_generator::integer_type i = gen;
                    CDS_DEBUG_ONLY( ++nLoopCount );
                    if ( pTailSegment->cells[i].data.load(memory_model::memory_order_relaxed).all()) {
                        // Cell is not empty, go next
                        m_Stat.onPushPopulated();
                    }
                    else {
                        // Empty cell found, try to enqueue here
                        regular_cell nullCell;
                        if ( pTailSegment->cells[i].data.compare_exchange_strong( nullCell, regular_cell( &val ),
                            memory_model::memory_order_release, atomics::memory_order_relaxed ))
                        {
                            // Ok to push item
                            m_Stat.onPush();
                            return true;
                        }
                        assert( nullCell.ptr());
                        m_Stat.onPushContended();
                    }
                } while ( gen.next());

                assert( nLoopCount == quasi_factor());

                // No available position, create a new segment
                pTailSegment = m_SegmentList.create_tail( pTailSegment, segmentGuard );

                // Get new permutation
                gen.reset();
            }
        }

        /// Removes an element from first segment of the queue and returns it
        /**
            If the queue is empty the function returns \p nullptr.

            The disposer specified in \p Traits template argument is <b>not</b> called for returned item.
            You should manually dispose the item:
            \code
            struct my_disposer {
                void operator()( foo * p )
                {
                    delete p;
                }
            };
            cds::intrusive::CASegmentedQueue< cds::gc::HP, foo > theQueue;
            // ...

            // Dequeue an item
            foo * pItem = theQueue.dequeue();
            // deal with pItem
            //...

            // pItem is not longer needed and can be deleted
            // Do it via gc::HP::retire
            cds::gc::HP::template retire< my_disposer >( pItem );
            \endcode
        */
        value_type * dequeue()
        {
            typename gc::Guard itemGuard;
            if ( do_dequeue( itemGuard )) {
                value_type * pVal = itemGuard.template get<value_type>();
                assert( pVal );
                return pVal;
            }
            return nullptr;

        }

        /// Synonym for \p enqueue(value_type&) member function
        bool push( value_type& val )
        {
            return enqueue( val );
        }

        /// Synonym for \p dequeue() member function
        value_type * pop()
        {
            return dequeue();
        }

        /// Checks if the queue is empty
        /**
            The original segmented queue algorithm does not allow to check emptiness accurately
            because \p empty() is unlinearizable.
            This function tests queue's emptiness checking <tt>size() == 0</tt>,
            so, the item counting feature is an essential part of queue's algorithm.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Clear the queue
        /**
            The function repeatedly calls \p dequeue() until it returns \p nullptr.
            The disposer specified in \p Traits template argument is called for each removed item.
        */
        void clear()
        {
            clear_with( disposer());
        }

        /// Clear the queue
        /**
            The function repeatedly calls \p dequeue() until it returns \p nullptr.
            \p Disposer is called for each removed item.
        */
        template <class Disposer>
        void clear_with( Disposer )
        {
            typename gc::Guard itemGuard;
            while ( do_dequeue( itemGuard )) {
                assert( itemGuard.template get<value_type>());
                gc::template retire<Disposer>( itemGuard.template get<value_type>());
                itemGuard.clear();
            }
        }

        /// Returns queue's item count
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns reference to internal statistics
        /**
            The type of internal statistics is specified by \p Traits template argument.
        */
        const stat& statistics() const
        {
            return m_Stat;
        }

        /// Returns quasi factor, a power-of-two number
        size_t quasi_factor() const
        {
            return m_SegmentList.quasi_factor();
        }

    protected:
        //@cond
        bool do_dequeue( typename gc::Guard& itemGuard )
        {
            typename gc::Guard segmentGuard;
            segment * pHeadSegment = m_SegmentList.head( segmentGuard );

            permutation_generator gen(0, quasi_factor());
            while ( true ) {
                if ( !pHeadSegment ) {
                    // Queue is empty
                    m_Stat.onPopEmpty();
                    return false;
                }

                bool bHadNullValue = false;
                regular_cell item;
                CDS_DEBUG_ONLY( size_t nLoopCount = 0 );
                do {
                    typename permutation_generator::integer_type i = gen;
                    CDS_DEBUG_ONLY( ++nLoopCount );

                    // Guard the item
                    // In segmented queue the cell cannot be reused
                    // So no loop is needed here to protect the cell
                    item = pHeadSegment->cells[i].data.load( memory_model::memory_order_relaxed );
                    itemGuard.assign( item.ptr());

                    // Check if this cell is empty, which means an element
                    // can be enqueued to this cell in the future
                    if ( !item.ptr())
                        bHadNullValue = true;
                    else {
                        // If the item is not deleted yet
                        if ( !item.bits()) {
                            // Try to mark the cell as deleted
                            if ( pHeadSegment->cells[i].data.compare_exchange_strong( item, item | 1,
                                memory_model::memory_order_acquire, atomics::memory_order_relaxed ))
                            {
                                --m_ItemCounter;
                                m_Stat.onPop();

                                return true;
                            }
                            assert( item.bits());
                            m_Stat.onPopContended();
                        }
                    }
                } while ( gen.next());

                assert( nLoopCount == quasi_factor());

                // scanning the entire segment without finding a candidate to dequeue
                // If there was an empty cell, the queue is considered empty
                if ( bHadNullValue ) {
                    m_Stat.onPopEmpty();
                    return false;
                }

                // All nodes have been dequeued, we can safely remove the first segment
                pHeadSegment = m_SegmentList.remove_head( pHeadSegment, segmentGuard );

                // Get new permutation
                gen.reset();
            }
        }
        //@endcond
    };
}} // namespace cds::intrusive

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning( pop )
#endif

#endif // #ifndef CDSLIB_INTRUSIVE_ca_segmented_queue_H

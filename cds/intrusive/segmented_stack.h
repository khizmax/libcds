// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_SEGMENTED_STACK_H
#define CDSLIB_INTRUSIVE_SEGMENTED_STACK_H

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

    /// SegmentedStack -related declarations
    namespace segmented_stack {

        /// SegmentedStack internal statistics. May be used for debugging or profiling
        template <typename Counter = cds::atomicity::event_counter >
        struct stat
        {
            typedef Counter  counter_type;  ///< Counter type

            counter_type    m_nPush;            ///< Push count
            counter_type    m_nPushPopulated;   ///< Number of attempts to push to populated (non-empty) cell
            counter_type    m_nPushContended;   ///< Number of failed CAS when pushing
            counter_type    m_nPop;             ///< Pop count
            counter_type    m_nPopEmpty;        ///< Number of poping from empty stack
            counter_type    m_nPopContended;    ///< Number of failed CAS when popping

            counter_type    m_nCreateSegmentReq;    ///< Number of request to create new segment
            counter_type    m_nDeleteSegmentReq;    ///< Number to request to delete segment
            counter_type    m_nSegmentCreated;        ///< Number of created segments
            counter_type    m_nSegmentDeleted;        ///< Number of deleted segments
            counter_type    m_nSucceededCommits;    ///< Number of succeeded commits
            counter_type    m_nFailedCommits;        ///< Number of failed commits

            //@cond
            void onPush()                 { ++m_nPush; }
            void onPushPopulated()        { ++m_nPushPopulated; }
            void onPushContended()        { ++m_nPushContended; }
            void onPop()                  { ++m_nPop;  }
            void onPopEmpty()             { ++m_nPopEmpty; }
            void onPopContended()         { ++m_nPopContended; }
            void onCreateSegmentReq()     { ++m_nCreateSegmentReq; }
            void onDeleteSegmentReq()     { ++m_nDeleteSegmentReq; }
            void onSegmentCreated()       { ++m_nSegmentCreated; }
            void onSegmentDeleted()       { ++m_nSegmentDeleted; }
            void onSucceededCommit()      { ++m_nSucceededCommits; }
            void onFailedCommit()         { ++m_nFailedCommits; }
            //@endcond
        };

        /// Dummy SegmentedStack statistics, no overhead
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
            void onSucceededCommit() const  {}
            void onFailedCommit() const     {}
            //@endcond
        };

        /// SegmentedStack default traits
        struct traits {
            /// Element disposer that is called when the item to be pop. Default is opt::v::empty_disposer (no disposer)
            typedef opt::v::empty_disposer disposer;

            /// Item counter, default is atomicity::item_counter
            /**
                The item counting is an essential part of segmented stack algorithm.
                The \p empty() member function is based on checking <tt>size() == 0</tt>.
                Therefore, dummy item counter like atomicity::empty_item_counter is not the proper counter.
            */
            typedef atomicity::item_counter item_counter;

            /// Internal statistics, possible predefined types are \ref stat, \ref empty_stat (the default)
            typedef segmented_stack::empty_stat        stat;

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
        };

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
    } // namespace segmented_stack

    /// Segmented stack
    /** @ingroup cds_intrusive_stack

        The stack is based on work
        - [2014] Henzinger, Kirsch, Payer, Sezgin, Sokolova Quantitative Relaxation of Concurrent Data Structures

        Template parameters:
        - \p GC - a garbage collector, possible types are cds::gc::HP, cds::gc::DHP
        - \p T - the type of values stored in the stack
        - \p Traits - stack type traits, default is \p segmented_stack::traits.
            \p segmented_stack::make_traits metafunction can be used to construct the
            type traits.

    */
    template <class GC, typename T, typename Traits = segmented_stack::traits >
    class SegmentedStack
    {
    public:
        typedef GC  gc;         ///< Garbage collector
        typedef T   value_type; ///< type of the value stored in the stack
        typedef Traits traits;  ///< Stack traits

        typedef typename traits::disposer      disposer    ;   ///< value disposer, called only in \p clear() when the element to be pop
        typedef typename traits::allocator     allocator;   ///< Allocator maintaining the segments
        typedef typename traits::memory_model  memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::item_counter  item_counter;   ///< Item counting policy, see cds::opt::item_counter option setter
        typedef typename traits::stat          stat;   ///< Internal statistics policy
        typedef typename traits::lock_type     lock_type;   ///< Type of mutex for maintaining an internal list of allocated segments.

        static const size_t c_nHazardPtrCount = 3 ; ///< Count of hazard pointer required for the algorithm

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
            bool retired;     // Mark that indicates is the segment removed
            size_t version;  // version tag (ABA prevention tag)
            // cell array is placed here in one continuous memory block

            // Initializes the segment
            explicit segment( size_t nCellCount )
                // MSVC warning C4355: 'this': used in base member initializer list
                : cells( reinterpret_cast< cell *>( this + 1 ))
                , retired( false )
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
                    auto item = pCell->data.load(memory_model::memory_order_relaxed);
                    if (item.ptr() && !item.bits())
                    {
                        return false;
                    }
                }
                return true;
            }
#       endif

            segment * create_head( segment * pHead, typename gc::Guard& guard )
            {
                // pHead is guarded by GC

                m_Stat.onCreateSegmentReq();

                scoped_lock l( m_Lock );

                if ( !m_List.empty() && (pHead != &m_List.front() || get_version(pHead) != m_List.front().version )) {
                    m_pHead.store( &m_List.front(), memory_model::memory_order_relaxed );

                    return guard.assign( &m_List.front());
                }

#           ifdef _DEBUG
                assert( m_List.empty() || populated( m_List.front()));
#           endif

                segment * pNew = allocate_segment();
                m_Stat.onSegmentCreated();

                m_List.push_front( *pNew );
                m_pHead.store(pNew, memory_model::memory_order_release);

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
                        m_pHead.store( nullptr, memory_model::memory_order_relaxed );
                        return guard.assign( nullptr );
                    }

                    if ( pHead != &m_List.front() || get_version(pHead) != m_List.front().version ) {
                        m_pHead.store( &m_List.front(), memory_model::memory_order_relaxed );
                        return guard.assign( &m_List.front());
                    }

                    if ( empty(pHead) ) {  
                        // pHead is empty
                        pHead->retired = true;
                        m_List.pop_front();
                        if ( m_List.empty()) {
                            pRet = guard.assign( nullptr );
                        }
                        else
                            pRet = guard.assign( &m_List.front());
                        m_pHead.store( pRet, memory_model::memory_order_release );
                    }
                    else {   
                        // pHead is not empty
                        // since the lock is held, no one changed pHead
                        return pHead;
                    }
                }

                retire_segment( pHead );
                m_Stat.onSegmentDeleted();
            
                return pRet;
            }


            size_t quasi_factor() const
            {
                return m_nQuasiFactor;
            }

            bool retired(segment *pSegment)
            {
                scoped_lock lock(m_Lock);
                return (pSegment->retired);
            }

        private:
            typedef cds::details::Allocator< segment, allocator >   segment_allocator;

            bool empty(segment const * s)
            {
                cell const * pLastCell = s->cells + quasi_factor();
                for (cell const * pCell = s->cells; pCell < pLastCell; ++pCell) {
                    auto item = pCell->data.load(memory_model::memory_order_relaxed);
                    if (item.ptr() && !item.bits()) { 
                        // segments contains non deleted items
                        return false;
                    }
                }
                // check to the segment slots not changed
                for (cell const * pCell = s->cells; pCell < pLastCell; ++pCell) {
                    auto item = pCell->data.load(memory_model::memory_order_relaxed);
                    if (item.ptr() && !item.bits()) { 
                        // segments contains non deleted items
                        return false;
                    }
                }
                return true;
            }

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
        /// Initializes the empty stack
        SegmentedStack(
            size_t nQuasiFactor     ///< Quasi factor. If it is not a power of 2 it is rounded up to nearest power of 2. Minimum is 2.
            )
            : m_SegmentList( cds::beans::ceil2(nQuasiFactor), m_Stat )
        {
            static_assert( (!std::is_same< item_counter, cds::atomicity::empty_item_counter >::value),
                "cds::atomicity::empty_item_counter is not supported for SegmentedStack"
                );
            assert( m_SegmentList.quasi_factor() > 1 );
        }

        /// Clears the stack and deletes all internal data
        ~SegmentedStack()
        {
            clear();
        }

        /// Inserts a new element at last segment of the stack
        bool push( value_type& val )
        {
            // LSB is used as a flag in marked pointer
            assert( (reinterpret_cast<uintptr_t>( &val ) & 1) == 0 );

            ++m_ItemCounter;

            typename gc::Guard segmentGuard;
            while ( true ) {
                segment * pHeadSegment = m_SegmentList.head(segmentGuard);
                if ( !pHeadSegment) {
                    // no segments, create the new one
                    pHeadSegment = m_SegmentList.create_head(pHeadSegment, segmentGuard );
                    assert(pHeadSegment);
                }

                typename gc::Guard segGuard;
                size_t i = 0;
                size_t qf = quasi_factor();
                do
                {
                    if ( pHeadSegment->cells[i].data.load(memory_model::memory_order_relaxed).all() ) {
                        // Cell is not empty, go next
                        m_Stat.onPushPopulated();
                    }
                    else {
                        typename gc::Guard itemGuard;
                        regular_cell nullCell;
                        regular_cell newCell(&val);
                        itemGuard.assign(newCell.ptr());
                        if ( pHeadSegment->cells[i].data.compare_exchange_strong(nullCell, newCell, 
                            memory_model::memory_order_acquire, atomics::memory_order_relaxed) )
                        {
                            if (committed(pHeadSegment, newCell, i)) {
                                m_Stat.onPush();
                                return true;
                            }
                        }

                        assert(nullCell.ptr());
                        m_Stat.onPushContended();
                    }
                    i++;
                } while (i < qf);

                // No available position, create a new segment
                pHeadSegment = m_SegmentList.create_head(pHeadSegment, segmentGuard);
            }
        }

        value_type * pop()
        {
            typename gc::Guard itemGuard;
            if ( do_pop( itemGuard )) {
                value_type * pVal = itemGuard.template get<value_type>();
                assert( pVal );
                return pVal;
            }
            return nullptr;
        }

        /// Checks if the stack is empty
        bool empty() const
        {
            return size() == 0;
        }

        /// Clear the stack
        void clear()
        {
            clear_with( disposer());
        }

        template <class Disposer>
        void clear_with( Disposer )
        {
            typename gc::Guard itemGuard;
            while ( do_pop( itemGuard )) {
                assert( itemGuard.template get<value_type>());
                gc::template retire<Disposer>( itemGuard.template get<value_type>());
                itemGuard.clear();
            }
        }

        /// Returns stack's item count
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns reference to internal statistics
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
        bool committed(segment *pHeadSegment, regular_cell &new_item, size_t index)
        {
            if (new_item.bits()) {   
                // item already poped
                m_Stat.onSucceededCommit();
                return true;
            }
            else if (!m_SegmentList.retired(pHeadSegment)) {
                m_Stat.onSucceededCommit();
                return true;
            }
            else {
                // segment is marked as removed,
                // so try to mark item as removed
                // if not succeeded, item already poped
                if (!pHeadSegment->cells[index].data.compare_exchange_strong(new_item, new_item | 1, 
                    memory_model::memory_order_acquire, atomics::memory_order_relaxed))
                {
                    m_Stat.onSucceededCommit();
                    return true;
                }
            }

            m_Stat.onFailedCommit();
            return false;
        }
        //@endcond

        //@cond
        bool do_pop( typename gc::Guard& itemGuard )
        {
            typename gc::Guard segmentGuard;
            while (true) {
                segment * pHeadSegment = m_SegmentList.head(segmentGuard);
                if (!pHeadSegment) {
                    // Stack is empty
                    m_Stat.onPopEmpty();
                    return false;
                }
                size_t i = 1;
                size_t qf = quasi_factor();
                regular_cell item;
                do
                {
                    CDS_DEBUG_ONLY(++nLoopCount);
                    // Guard the item
                    // In segmented stack the cell cannot be reused
                    // So no loop is needed here to protect the cell
                    item = pHeadSegment->cells[qf - i].data.load(memory_model::memory_order_relaxed);
                    itemGuard.assign(item.ptr());

                    // Check if this cell is not empty and not marked deleted
                    if (item.ptr() && !item.bits()) {
                        // Try to mark the cell as deleted
                        if (pHeadSegment->cells[qf - i].data.compare_exchange_strong(item, item | 1,
                            memory_model::memory_order_acquire, atomics::memory_order_relaxed))
                        {
                            --m_ItemCounter;
                            m_Stat.onPop();

                            return true;
                        }
                        assert(item.bits());
                        m_Stat.onPopContended();
                    }
                    i++;
                } while (i <= qf);
                // No nodes to pop, try remove the first segment
                pHeadSegment = m_SegmentList.remove_head(pHeadSegment, segmentGuard);
            }
        }
        //@endcond
    };
}} // namespace cds::intrusive

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning( pop )
#endif

#endif // #ifndef CDSLIB_INTRUSIVE_SEGMENTED_STACK_H

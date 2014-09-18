//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_DEQUE_STAT_H
#define __CDS_INTRUSIVE_DEQUE_STAT_H

#include <cds/cxx11_atomic.h>

namespace cds { namespace intrusive {

    /// Deque internal statistics. May be used for debugging or profiling
    /** @ingroup cds_intrusive_helper
        Template argument \p Counter defines type of counter.
        Default is cds::atomics::event_counter.
        You may use other counter type like as cds::atomics::item_counter,
        or even integral type, for example, \p int.
    */
    template <typename Counter = cds::atomicity::event_counter >
    struct deque_stat
    {
        typedef Counter     counter_type    ;   ///< Counter type

        counter_type m_PushFrontCount       ;   ///< push front event count
        counter_type m_PushBackCount        ;   ///< push back event count
        counter_type m_PopFrontCount        ;   ///< pop front event count
        counter_type m_PopBackCount         ;   ///< pop back event count
        counter_type m_PopEmptyCount        ;   ///< pop from empty deque event count
        counter_type m_PushFrontContentionCount ;   ///< \p push_front contention count
        counter_type m_PushBackContentionCount ;    ///< \p push_back contention count
        counter_type m_PopFrontContentionCount ;   ///< \p pop_front contention count
        counter_type m_PopBackContentionCount ;    ///< \p pop_back contention count

        /// Register \p push_front call
        void onPushFront()               { ++m_PushFrontCount; }

        /// Register \p push_back call
        void onPushBack()               { ++m_PushBackCount; }

        /// Register \p pop_front call
        void onPopFront()                { ++m_PopFrontCount; }

        /// Register \p pop_back call
        void onPopBack()                { ++m_PopBackCount; }

        /// Register popping from empty deque
        void onPopEmpty()               { ++m_PopEmptyCount; }

        /// Register "\p push_front contention" event
        void onPushFrontContention()    { ++m_PushFrontContentionCount; }

        /// Register "\p push_back contention" event
        void onPushBackContention()     { ++m_PushBackContentionCount; }

        /// Register "\p pop_front contention" event
        void onPopFrontContention()     { ++m_PopFrontContentionCount; }

        /// Register "\p pop_back contention" event
        void onPopBackContention()      { ++m_PopBackContentionCount; }
    };


    /// Dummy deque statistics - no counting is performed. Support interface like \ref deque_stat
    /** @ingroup cds_intrusive_helper
    */
    struct deque_dummy_stat
    {
        //@cond
        void onPushFront()              {}
        void onPushBack()               {}
        void onPopFront()               {}
        void onPopBack()                {}
        void onPopEmpty()               {}
        void onPushFrontContention()    {}
        void onPushBackContention()     {}
        void onPopFrontContention()     {}
        void onPopBackContention()      {}
        //@endcond
    };

}}  // namespace cds::intrusive


#endif  // #ifndef __CDS_INTRUSIVE_DEQUE_STAT_H

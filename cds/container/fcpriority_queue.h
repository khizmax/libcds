// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_FCPRIORITY_QUEUE_H
#define CDSLIB_CONTAINER_FCPRIORITY_QUEUE_H

#include <cds/algo/flat_combining.h>
#include <cds/algo/elimination_opt.h>
#include <queue>

namespace cds { namespace container {

    /// FCPriorityQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace fcpqueue {

        /// FCPriorityQueue internal statistics
        template <typename Counter = cds::atomicity::event_counter >
        struct stat: public cds::algo::flat_combining::stat<Counter>
        {
            typedef cds::algo::flat_combining::stat<Counter>    flat_combining_stat; ///< Flat-combining statistics
            typedef typename flat_combining_stat::counter_type  counter_type;        ///< Counter type

            counter_type    m_nPush     ;  ///< Count of push operations
            counter_type    m_nPushMove ;  ///< Count of push operations with move semantics
            counter_type    m_nPop      ;  ///< Count of success pop operations
            counter_type    m_nFailedPop;  ///< Count of failed pop operations (pop from empty queue)

            //@cond
            void    onPush()             { ++m_nPush; }
            void    onPushMove()         { ++m_nPushMove; }
            void    onPop( bool bFailed ) { if ( bFailed ) ++m_nFailedPop; else ++m_nPop;  }
            //@endcond
        };

        /// FCPriorityQueue dummy statistics, no overhead
        struct empty_stat: public cds::algo::flat_combining::empty_stat
        {
            //@cond
            void    onPush()       {}
            void    onPushMove()   {}
            void    onPop(bool)    {}
            //@endcond
        };

        /// FCPriorityQueue traits
        struct traits: public cds::algo::flat_combining::traits
        {
            typedef empty_stat      stat;   ///< Internal statistics
        };

        /// Metafunction converting option list to traits
        /**
            \p Options are:
            - any \p cds::algo::flat_combining::make_traits options
            - \p opt::stat - internal statistics, possible type: \p fcpqueue::stat, \p fcpqueue::empty_stat (the default)
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

    } // namespace fcpqueue

    /// Flat-combining priority queue
    /**
        @ingroup cds_nonintrusive_priority_queue
        @ingroup cds_flat_combining_container

        \ref cds_flat_combining_description "Flat combining" sequential priority queue.
        The class can be considered as a concurrent FC-based wrapper for \p std::priority_queue.

        Template parameters:
        - \p T - a value type stored in the queue
        - \p PriorityQueue - sequential priority queue implementation, default is \p std::priority_queue<T>
        - \p Traits - type traits of flat combining, default is \p fcpqueue::traits.
            \p fcpqueue::make_traits metafunction can be used to construct specialized \p %fcpqueue::traits
    */
    template <typename T,
        class PriorityQueue = std::priority_queue<T>,
        typename Traits = fcpqueue::traits
    >
    class FCPriorityQueue
#ifndef CDS_DOXYGEN_INVOKED
        : public cds::algo::flat_combining::container
#endif
    {
    public:
        typedef T               value_type;          ///< Value type
        typedef PriorityQueue   priority_queue_type; ///< Sequential priority queue class
        typedef Traits          traits;              ///< Priority queue type traits

        typedef typename traits::stat  stat;    ///< Internal statistics type

    protected:
        //@cond
        // Priority queue operation IDs
        enum fc_operation {
            op_push = cds::algo::flat_combining::req_Operation,
            op_push_move,
            op_pop,
            op_clear
        };

        // Flat combining publication list record
        struct fc_record: public cds::algo::flat_combining::publication_record
        {
            union {
                value_type const *  pValPush; // Value to push
                value_type *        pValPop;  // Pop destination
            };
            bool            bEmpty; // true if the queue is empty
        };
        //@endcond

        /// Flat combining kernel
        typedef cds::algo::flat_combining::kernel< fc_record, traits > fc_kernel;

    protected:
        //@cond
        mutable fc_kernel   m_FlatCombining;
        priority_queue_type m_PQueue;
        //@endcond

    public:
        /// Initializes empty priority queue object
        FCPriorityQueue()
        {}

        /// Initializes empty priority queue object and gives flat combining parameters
        FCPriorityQueue(
            unsigned int nCompactFactor     ///< Flat combining: publication list compacting factor
            ,unsigned int nCombinePassCount ///< Flat combining: number of combining passes for combiner thread
            )
            : m_FlatCombining( nCompactFactor, nCombinePassCount )
        {}

        /// Inserts a new element in the priority queue
        /**
            The function always returns \p true
        */
        bool push(
            value_type const& val ///< Value to be copied to inserted element
        )
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pValPush = &val;

            m_FlatCombining.combine( op_push, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPush();
            return true;
        }

        /// Inserts a new element in the priority queue (move semantics)
        /**
            The function always returns \p true
        */
        bool push(
            value_type&& val ///< Value to be moved to inserted element
        )
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pValPush = &val;

            m_FlatCombining.combine( op_push_move, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPushMove();
            return true;
        }

        /// Removes the top element from priority queue
        /**
            The function returns \p false if the queue is empty, \p true otherwise.
            If the queue is empty \p val is not changed.
        */
        bool pop(
            value_type& val ///< Target to be received the copy of top element
        )
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pValPop = &val;

            m_FlatCombining.combine( op_pop, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPop( pRec->bEmpty );
            return !pRec->bEmpty;
        }

        /// Exclusive access to underlying priority queue object
        /**
            The functor \p f can do any operation with underlying \p priority_queue_type in exclusive mode.
            For example, you can iterate over the queue.
            \p Func signature is:
            \code
                void f( priority_queue_type& deque );
            \endcode
        */
        template <typename Func>
        void apply( Func f )
        {
            auto& pqueue = m_PQueue;
            m_FlatCombining.invoke_exclusive( [&pqueue, &f]() { f( pqueue ); } );
        }

        /// Exclusive access to underlying priority queue object
        /**
            The functor \p f can do any operation with underlying \p proiprity_queue_type in exclusive mode.
            For example, you can iterate over the queue.
            \p Func signature is:
            \code
                void f( priority_queue_type const& queue );
            \endcode
        */
        template <typename Func>
        void apply( Func f ) const
        {
            auto const& pqueue = m_PQueue;
            m_FlatCombining.invoke_exclusive( [&pqueue, &f]() { f( pqueue ); } );
        }

        /// Clears the priority queue
        void clear()
        {
            auto pRec = m_FlatCombining.acquire_record();

           m_FlatCombining.combine( op_clear, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );
        }

        /// Returns the number of elements in the priority queue.
        /**
            Note that <tt>size() == 0</tt> does not mean that the queue is empty because
            combining record can be in process.
            To check emptiness use \ref empty function.
        */
        size_t size() const
        {
            return m_PQueue.size();
        }

        /// Checks if the priority queue is empty
        /**
            If the combining is in process the function waits while combining done.
        */
        bool empty()
        {
            bool bRet = false;
            auto const& pq = m_PQueue;
            m_FlatCombining.invoke_exclusive( [&pq, &bRet]() { bRet = pq.empty(); } );
            return bRet;
        }

        /// Internal statistics
        stat const& statistics() const
        {
            return m_FlatCombining.statistics();
        }

    public: // flat combining cooperation, not for direct use!
        //@cond
        /*
            The function is called by \ref cds::algo::flat_combining::kernel "flat combining kernel"
            object if the current thread becomes a combiner. Invocation of the function means that
            the priority queue should perform an action recorded in \p pRec.
        */
        void fc_apply( fc_record * pRec )
        {
            assert( pRec );

            // this function is called under FC mutex, so switch TSan off
            //CDS_TSAN_ANNOTATE_IGNORE_RW_BEGIN;

            switch ( pRec->op()) {
            case op_push:
                assert( pRec->pValPush );
                m_PQueue.push( *(pRec->pValPush));
                break;
            case op_push_move:
                assert( pRec->pValPush );
                m_PQueue.push( std::move( *(pRec->pValPush )));
                break;
            case op_pop:
                assert( pRec->pValPop );
                pRec->bEmpty = m_PQueue.empty();
                if ( !pRec->bEmpty ) {
                    *(pRec->pValPop) = std::move( m_PQueue.top());
                    m_PQueue.pop();
                }
                break;
            case op_clear:
                while ( !m_PQueue.empty())
                    m_PQueue.pop();
                break;
            default:
                assert(false);
                break;
            }

            //CDS_TSAN_ANNOTATE_IGNORE_RW_END;
        }
        //@endcond
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_FCPRIORITY_QUEUE_H

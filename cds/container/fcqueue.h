// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_FCQUEUE_H
#define CDSLIB_CONTAINER_FCQUEUE_H

#include <cds/algo/flat_combining.h>
#include <cds/algo/elimination_opt.h>
#include <queue>

namespace cds { namespace container {

    /// FCQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace fcqueue {

        /// FCQueue internal statistics
        template <typename Counter = cds::atomicity::event_counter >
        struct stat: public cds::algo::flat_combining::stat<Counter>
        {
            typedef cds::algo::flat_combining::stat<Counter>    flat_combining_stat; ///< Flat-combining statistics
            typedef typename flat_combining_stat::counter_type  counter_type;        ///< Counter type

            counter_type    m_nEnqueue     ;   ///< Count of enqueue operations
            counter_type    m_nEnqMove     ;   ///< Count of enqueue operations with move semantics
            counter_type    m_nDequeue     ;   ///< Count of success dequeue operations
            counter_type    m_nFailedDeq   ;   ///< Count of failed dequeue operations (pop from empty queue)
            counter_type    m_nCollided    ;   ///< How many pairs of enqueue/dequeue were collided, if elimination is enabled

            //@cond
            void    onEnqueue()               { ++m_nEnqueue; }
            void    onEnqMove()               { ++m_nEnqMove; }
            void    onDequeue( bool bFailed ) { if ( bFailed ) ++m_nFailedDeq; else ++m_nDequeue;  }
            void    onCollide()               { ++m_nCollided; }
            //@endcond
        };

        /// FCQueue dummy statistics, no overhead
        struct empty_stat: public cds::algo::flat_combining::empty_stat
        {
            //@cond
            void    onEnqueue()     {}
            void    onEnqMove()     {}
            void    onDequeue(bool) {}
            void    onCollide()     {}
            //@endcond
        };

        /// FCQueue type traits
        struct traits: public cds::algo::flat_combining::traits
        {
            typedef empty_stat      stat;   ///< Internal statistics
            static constexpr const bool enable_elimination = false; ///< Enable \ref cds_elimination_description "elimination"
        };

        /// Metafunction converting option list to traits
        /**
            \p Options are:
            - any \p cds::algo::flat_combining::make_traits options
            - \p opt::stat - internal statistics, possible type: \p fcqueue::stat, \p fcqueue::empty_stat (the default)
            - \p opt::enable_elimination - enable/disable operation \ref cds_elimination_description "elimination"
                By default, the elimination is disabled. For queue, the elimination is possible if the queue
                is empty.
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

    } // namespace fcqueue

    /// Flat-combining queue
    /**
        @ingroup cds_nonintrusive_queue
        @ingroup cds_flat_combining_container

        \ref cds_flat_combining_description "Flat combining" sequential queue.
        The class can be considered as a concurrent FC-based wrapper for \p std::queue.

        Template parameters:
        - \p T - a value type stored in the queue
        - \p Queue - sequential queue implementation, default is \p std::queue<T>
        - \p Trats - type traits of flat combining, default is \p fcqueue::traits.
            \p fcqueue::make_traits metafunction can be used to construct \p %fcqueue::traits specialization.
    */
    template <typename T,
        class Queue = std::queue<T>,
        typename Traits = fcqueue::traits
    >
    class FCQueue
#ifndef CDS_DOXYGEN_INVOKED
        : public cds::algo::flat_combining::container
#endif
    {
    public:
        typedef T           value_type;     ///< Value type
        typedef Queue       queue_type;     ///< Sequential queue class
        typedef Traits      traits;         ///< Queue type traits

        typedef typename traits::stat  stat;   ///< Internal statistics type
        static constexpr const bool c_bEliminationEnabled = traits::enable_elimination; ///< \p true if elimination is enabled

    protected:
        //@cond
        /// Queue operation IDs
        enum fc_operation {
            op_enq = cds::algo::flat_combining::req_Operation, ///< Enqueue
            op_enq_move,    ///< Enqueue (move semantics)
            op_deq,         ///< Dequeue
            op_clear        ///< Clear
        };

        /// Flat combining publication list record
        struct fc_record: public cds::algo::flat_combining::publication_record
        {
            union {
                value_type const *  pValEnq;  ///< Value to enqueue
                value_type *        pValDeq;  ///< Dequeue destination
            };
            bool            bEmpty; ///< \p true if the queue is empty
        };
        //@endcond

        /// Flat combining kernel
        typedef cds::algo::flat_combining::kernel< fc_record, traits > fc_kernel;

    protected:
        //@cond
        mutable fc_kernel m_FlatCombining;
        queue_type        m_Queue;
        //@endcond

    public:
        /// Initializes empty queue object
        FCQueue()
        {}

        /// Initializes empty queue object and gives flat combining parameters
        FCQueue(
            unsigned int nCompactFactor     ///< Flat combining: publication list compacting factor
            ,unsigned int nCombinePassCount ///< Flat combining: number of combining passes for combiner thread
            )
            : m_FlatCombining( nCompactFactor, nCombinePassCount )
        {}

        /// Inserts a new element at the end of the queue
        /**
            The content of the new element initialized to a copy of \p val.

            The function always returns \p true
        */
        bool enqueue( value_type const& val )
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pValEnq = &val;

            constexpr_if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_enq, pRec, *this );
            else
                m_FlatCombining.combine( op_enq, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onEnqueue();
            return true;
        }

        /// Inserts a new element at the end of the queue (a synonym for \ref enqueue)
        bool push( value_type const& val )
        {
            return enqueue( val );
        }

        /// Inserts a new element at the end of the queue (move semantics)
        /**
            \p val is moved to inserted element
        */
        bool enqueue( value_type&& val )
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pValEnq = &val;

            constexpr_if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_enq_move, pRec, *this );
            else
                m_FlatCombining.combine( op_enq_move, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );

            m_FlatCombining.internal_statistics().onEnqMove();
            return true;
        }

        /// Inserts a new element at the end of the queue (move semantics, synonym for \p enqueue)
        bool push( value_type&& val )
        {
            return enqueue( val );
        }

        /// Removes the next element from the queue
        /**
            \p val takes a copy of the element
        */
        bool dequeue( value_type& val )
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pValDeq = &val;

            constexpr_if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_deq, pRec, *this );
            else
                m_FlatCombining.combine( op_deq, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );

            m_FlatCombining.internal_statistics().onDequeue( pRec->bEmpty );
            return !pRec->bEmpty;
        }

        /// Removes the next element from the queue (a synonym for \ref dequeue)
        bool pop( value_type& val )
        {
            return dequeue( val );
        }

        /// Clears the queue
        void clear()
        {
            auto pRec = m_FlatCombining.acquire_record();

            constexpr_if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_clear, pRec, *this );
            else
                m_FlatCombining.combine( op_clear, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );
        }

        /// Exclusive access to underlying queue object
        /**
            The functor \p f can do any operation with underlying \p queue_type in exclusive mode.
            For example, you can iterate over the queue.
            \p Func signature is:
            \code
                void f( queue_type& queue );
            \endcode
        */
        template <typename Func>
        void apply( Func f )
        {
            auto& queue = m_Queue;
            m_FlatCombining.invoke_exclusive( [&queue, &f]() { f( queue ); } );
        }

        /// Exclusive access to underlying queue object
        /**
            The functor \p f can do any operation with underlying \p queue_type in exclusive mode.
            For example, you can iterate over the queue.
            \p Func signature is:
            \code
                void f( queue_type const& queue );
            \endcode
        */
        template <typename Func>
        void apply( Func f ) const
        {
            auto const& queue = m_Queue;
            m_FlatCombining.invoke_exclusive( [&queue, &f]() { f( queue ); } );
        }

        /// Returns the number of elements in the queue.
        /**
            Note that <tt>size() == 0</tt> is not mean that the queue is empty because
            combining record can be in process.
            To check emptiness use \ref empty function.
        */
        size_t size() const
        {
            return m_Queue.size();
        }

        /// Checks if the queue is empty
        /**
            If the combining is in process the function waits while combining done.
        */
        bool empty() const
        {
            bool bRet = false;
            auto const& queue = m_Queue;
            m_FlatCombining.invoke_exclusive( [&queue, &bRet]() { bRet = queue.empty(); } );
            return bRet;
        }

        /// Internal statistics
        stat const& statistics() const
        {
            return m_FlatCombining.statistics();
        }

    public: // flat combining cooperation, not for direct use!
        //@cond
        /// Flat combining supporting function. Do not call it directly!
        /**
            The function is called by \ref cds::algo::flat_combining::kernel "flat combining kernel"
            object if the current thread becomes a combiner. Invocation of the function means that
            the queue should perform an action recorded in \p pRec.
        */
        void fc_apply( fc_record * pRec )
        {
            assert( pRec );

            switch ( pRec->op()) {
            case op_enq:
                assert( pRec->pValEnq );
                m_Queue.push( *(pRec->pValEnq ));
                break;
            case op_enq_move:
                assert( pRec->pValEnq );
                m_Queue.push( std::move( *(pRec->pValEnq )));
                break;
            case op_deq:
                assert( pRec->pValDeq );
                pRec->bEmpty = m_Queue.empty();
                if ( !pRec->bEmpty ) {
                    *(pRec->pValDeq) = std::move( m_Queue.front());
                    m_Queue.pop();
                }
                break;
            case op_clear:
                while ( !m_Queue.empty())
                    m_Queue.pop();
                break;
            default:
                assert(false);
                break;
            }
        }

        /// Batch-processing flat combining
        void fc_process( typename fc_kernel::iterator itBegin, typename fc_kernel::iterator itEnd )
        {
            typedef typename fc_kernel::iterator fc_iterator;

            for ( fc_iterator it = itBegin, itPrev = itEnd; it != itEnd; ++it ) {
                switch ( it->op( atomics::memory_order_acquire )) {
                case op_enq:
                case op_enq_move:
                case op_deq:
                    if ( m_Queue.empty()) {
                        if ( itPrev != itEnd && collide( *itPrev, *it ))
                            itPrev = itEnd;
                        else
                            itPrev = it;
                    }
                    break;
                }
            }
        }
        //@endcond

    private:
        //@cond
        bool collide( fc_record& rec1, fc_record& rec2 )
        {
            switch ( rec1.op()) {
                case op_enq:
                    if ( rec2.op() == op_deq ) {
                        assert(rec1.pValEnq);
                        assert(rec2.pValDeq);
                        *rec2.pValDeq = *rec1.pValEnq;
                        rec2.bEmpty = false;
                        goto collided;
                    }
                    break;
                case op_enq_move:
                    if ( rec2.op() == op_deq ) {
                        assert(rec1.pValEnq);
                        assert(rec2.pValDeq);
                        *rec2.pValDeq = std::move( *rec1.pValEnq );
                        rec2.bEmpty = false;
                        goto collided;
                    }
                    break;
                case op_deq:
                    switch ( rec2.op()) {
                    case op_enq:
                    case op_enq_move:
                        return collide( rec2, rec1 );
                    }
            }
            return false;

        collided:
            m_FlatCombining.operation_done( rec1 );
            m_FlatCombining.operation_done( rec2 );
            m_FlatCombining.internal_statistics().onCollide();
            return true;
        }
        //@endcond

    };
}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_FCQUEUE_H

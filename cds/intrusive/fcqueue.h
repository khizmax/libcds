// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_FCQUEUE_H
#define CDSLIB_INTRUSIVE_FCQUEUE_H

#include <cds/algo/flat_combining.h>
#include <cds/algo/elimination_opt.h>
#include <cds/intrusive/options.h>
#include <boost/intrusive/list.hpp>

namespace cds { namespace intrusive {

    /// \p FCQueue related definitions
    namespace fcqueue {

        /// \p FCQueue internal statistics
        template <typename Counter = cds::atomicity::event_counter >
        struct stat: public cds::algo::flat_combining::stat<Counter>
        {
            typedef cds::algo::flat_combining::stat<Counter>    flat_combining_stat; ///< Flat-combining statistics
            typedef typename flat_combining_stat::counter_type  counter_type;        ///< Counter type

            counter_type    m_nEnqueue     ;   ///< Count of push operations
            counter_type    m_nDequeue     ;   ///< Count of success pop operations
            counter_type    m_nFailedDeq   ;   ///< Count of failed pop operations (pop from empty queue)
            counter_type    m_nCollided    ;   ///< How many pairs of push/pop were collided, if elimination is enabled

            //@cond
            void    onEnqueue()                 { ++m_nEnqueue; }
            void    onDequeue( bool bFailed )   { if ( bFailed ) ++m_nFailedDeq; else ++m_nDequeue;  }
            void    onCollide()                 { ++m_nCollided; }
            //@endcond
        };

        /// FCQueue dummy statistics, no overhead
        struct empty_stat: public cds::algo::flat_combining::empty_stat
        {
            //@cond
            void    onEnqueue()     {}
            void    onDequeue(bool) {}
            void    onCollide()     {}
            //@endcond
        };

        /// \p FCQueue type traits
        struct traits: public cds::algo::flat_combining::traits
        {
            typedef cds::intrusive::opt::v::empty_disposer  disposer ; ///< Disposer to erase removed elements. Used only in \p FCQueue::clear() function
            typedef empty_stat      stat;   ///< Internal statistics
            static constexpr const bool enable_elimination = false; ///< Enable \ref cds_elimination_description "elimination"
        };

        /// Metafunction converting option list to traits
        /**
            \p Options are:
            - any \p cds::algo::flat_combining::make_traits options
            - \p opt::disposer - the functor used to dispose removed items. Default is \p opt::intrusive::v::empty_disposer.
                This option is used only in \p FCQueue::clear() function.
            - \p opt::stat - internal statistics, possible type: \p fcqueue::stat, \p fcqueue::empty_stat (the default)
            - \p opt::enable_elimination - enable/disable operation \ref cds_elimination_description "elimination"
                By default, the elimination is disabled (\p false)
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

    /// Flat-combining intrusive queue
    /**
        @ingroup cds_intrusive_queue
        @ingroup cds_flat_combining_intrusive

        \ref cds_flat_combining_description "Flat combining" sequential intrusive queue.

        Template parameters:
        - \p T - a value type stored in the queue
        - \p Container - sequential intrusive container with \p push_back and \p pop_front functions.
            Default is \p boost::intrusive::list
        - \p Traits - type traits of flat combining, default is \p fcqueue::traits.
            \p fcqueue::make_traits metafunction can be used to construct \p %fcqueue::traits specialization
    */
    template <typename T
        ,class Container = boost::intrusive::list<T>
        ,typename Traits = fcqueue::traits
    >
    class FCQueue
#ifndef CDS_DOXYGEN_INVOKED
        : public cds::algo::flat_combining::container
#endif
    {
    public:
        typedef T           value_type;     ///< Value type
        typedef Container   container_type; ///< Sequential container type
        typedef Traits      traits;         ///< Queue traits

        typedef typename traits::disposer   disposer;   ///< The disposer functor. The disposer is used only in \ref clear() function
        typedef typename traits::stat       stat;   ///< Internal statistics type
        static constexpr const bool c_bEliminationEnabled = traits::enable_elimination; ///< \p true if elimination is enabled

    protected:
        //@cond
        /// Queue operation IDs
        enum fc_operation {
            op_enq = cds::algo::flat_combining::req_Operation, ///< Enqueue
            op_deq,                 ///< Dequeue
            op_clear,               ///< Clear
            op_clear_and_dispose    ///< Clear and dispose
        };

        /// Flat combining publication list record
        struct fc_record: public cds::algo::flat_combining::publication_record
        {
            value_type * pVal;  ///< Value to enqueue or dequeue
            bool         bEmpty; ///< \p true if the queue is empty
        };
        //@endcond

        /// Flat combining kernel
        typedef cds::algo::flat_combining::kernel< fc_record, traits > fc_kernel;

    protected:
        //@cond
        mutable fc_kernel m_FlatCombining;
        container_type    m_Queue;
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
            The function always returns \p true.
        */
        bool enqueue( value_type& val )
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pVal = &val;

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
        bool push( value_type& val )
        {
            return enqueue( val );
        }

        /// Removes the next element from the queue
        /**
            If the queue is empty the function returns \p nullptr
        */
        value_type * dequeue()
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pVal = nullptr;

            constexpr_if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_deq, pRec, *this );
            else
                m_FlatCombining.combine( op_deq, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );

            m_FlatCombining.internal_statistics().onDequeue( pRec->bEmpty );
            return pRec->pVal;
        }

        /// Removes the next element from the queue (a synonym for \ref dequeue)
        value_type * pop()
        {
            return dequeue();
        }

        /// Clears the queue
        /**
            If \p bDispose is \p true, the disposer provided in \p Traits class' template parameter
            will be called for each removed element.
        */
        void clear( bool bDispose = false )
        {
            auto pRec = m_FlatCombining.acquire_record();

            constexpr_if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( bDispose ? op_clear_and_dispose : op_clear, pRec, *this );
            else
                m_FlatCombining.combine( bDispose ? op_clear_and_dispose : op_clear, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );
        }

        /// Exclusive access to underlying queue object
        /**
            The functor \p f can do any operation with underlying \p container_type in exclusive mode.
            For example, you can iterate over the queue.
            \p Func signature is:
            \code
                void f( container_type& queue );
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
            The functor \p f can do any operation with underlying \p container_type in exclusive mode.
            For example, you can iterate over the queue.
            \p Func signature is:
            \code
                void f( container_type const& queue );
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
            If the combining is in process the function waits while it is done.
        */
        bool empty() const
        {
            bool bRet = false;
            auto const& queue = m_Queue;
            m_FlatCombining.invoke_exclusive([&queue, &bRet]() { bRet = queue.empty(); });
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

            // this function is called under FC mutex, so switch TSan off
            // All TSan warnings are false positive
            //CDS_TSAN_ANNOTATE_IGNORE_RW_BEGIN;

            switch ( pRec->op()) {
            case op_enq:
                assert( pRec->pVal );
                m_Queue.push_back( *(pRec->pVal ));
                break;
            case op_deq:
                pRec->bEmpty = m_Queue.empty();
                if ( !pRec->bEmpty ) {
                    pRec->pVal = &m_Queue.front();
                    m_Queue.pop_front();
                }
                break;
            case op_clear:
                m_Queue.clear();
                break;
            case op_clear_and_dispose:
                m_Queue.clear_and_dispose( disposer());
                break;
            default:
                assert(false);
                break;
            }
            //CDS_TSAN_ANNOTATE_IGNORE_RW_END;
        }

        /// Batch-processing flat combining
        void fc_process( typename fc_kernel::iterator itBegin, typename fc_kernel::iterator itEnd )
        {
            // this function is called under FC mutex, so switch TSan off
            // All TSan warnings are false positive
            //CDS_TSAN_ANNOTATE_IGNORE_RW_BEGIN;

            typedef typename fc_kernel::iterator fc_iterator;
            for ( fc_iterator it = itBegin, itPrev = itEnd; it != itEnd; ++it ) {
                switch ( it->op( atomics::memory_order_acquire )) {
                case op_enq:
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
            //CDS_TSAN_ANNOTATE_IGNORE_RW_END;
        }
        //@endcond

    private:
        //@cond
        bool collide( fc_record& rec1, fc_record& rec2 )
        {
            assert( m_Queue.empty());

            switch ( rec1.op()) {
                case op_enq:
                    if ( rec2.op() == op_deq ) {
                        assert(rec1.pVal);
                        rec2.pVal = rec1.pVal;
                        rec2.bEmpty = false;
                        m_FlatCombining.operation_done( rec1 );
                        m_FlatCombining.operation_done( rec2 );
                        m_FlatCombining.internal_statistics().onCollide();
                        return true;
                    }
                    break;
                case op_deq:
                    if ( rec2.op() == op_enq ) {
                        assert(rec2.pVal);
                        rec1.pVal = rec2.pVal;
                        rec1.bEmpty = false;
                        m_FlatCombining.operation_done( rec1 );
                        m_FlatCombining.operation_done( rec2 );
                        m_FlatCombining.internal_statistics().onCollide();
                        return true;
                    }
                    break;
            }
            return false;
        }
        //@endcond
    };

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_FCQUEUE_H

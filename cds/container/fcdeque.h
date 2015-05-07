//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_FCDEQUE_H
#define CDSLIB_CONTAINER_FCDEQUE_H

#include <cds/algo/flat_combining.h>
#include <cds/algo/elimination_opt.h>
#include <deque>

namespace cds { namespace container {

    /// FCDeque related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace fcdeque {

        /// FCDeque internal statistics
        template <typename Counter = cds::atomicity::event_counter >
        struct stat: public cds::algo::flat_combining::stat<Counter>
        {
            typedef cds::algo::flat_combining::stat<Counter>    flat_combining_stat; ///< Flat-combining statistics
            typedef typename flat_combining_stat::counter_type  counter_type;        ///< Counter type

            counter_type    m_nPushFront     ;  ///< Count of push_front operations
            counter_type    m_nPushFrontMove ;  ///< Count of push_front operations with move semantics
            counter_type    m_nPushBack      ;  ///< Count of push_back operations
            counter_type    m_nPushBackMove  ;  ///< Count of push_back operations with move semantics
            counter_type    m_nPopFront      ;  ///< Count of success pop_front operations
            counter_type    m_nFailedPopFront;  ///< Count of failed pop_front operations (pop from empty deque)
            counter_type    m_nPopBack       ;  ///< Count of success pop_back operations
            counter_type    m_nFailedPopBack ;  ///< Count of failed pop_back operations (pop from empty deque)
            counter_type    m_nCollided      ;  ///< How many pairs of push/pop were collided, if elimination is enabled

            //@cond
            void    onPushFront()             { ++m_nPushFront; }
            void    onPushFrontMove()         { ++m_nPushFrontMove; }
            void    onPushBack()              { ++m_nPushBack; }
            void    onPushBackMove()          { ++m_nPushBackMove; }
            void    onPopFront( bool bFailed ) { if ( bFailed ) ++m_nFailedPopFront; else ++m_nPopFront;  }
            void    onPopBack( bool bFailed ) { if ( bFailed ) ++m_nFailedPopBack; else ++m_nPopBack;  }
            void    onCollide()               { ++m_nCollided; }
            //@endcond
        };

        /// FCDeque dummy statistics, no overhead
        struct empty_stat: public cds::algo::flat_combining::empty_stat
        {
            //@cond
            void    onPushFront()       {}
            void    onPushFrontMove()   {}
            void    onPushBack()        {}
            void    onPushBackMove()    {}
            void    onPopFront(bool)    {}
            void    onPopBack(bool)     {}
            void    onCollide()         {}
            //@endcond
        };

        /// FCDeque type traits
        struct traits: public cds::algo::flat_combining::traits
        {
            typedef empty_stat      stat;   ///< Internal statistics
            static CDS_CONSTEXPR const bool enable_elimination = false; ///< Enable \ref cds_elimination_description "elimination"
        };

        /// Metafunction converting option list to traits
        /**
            \p Options are:
            - \p opt::lock_type - mutex type, default is \p cds::sync::spin
            - \p opt::back_off - back-off strategy, defalt is \p cds::backoff::delay_of<2>
            - \p opt::allocator - allocator type, default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::stat - internal statistics, possible type: \ref stat, \ref empty_stat (the default)
            - \p opt::memory_model - C++ memory ordering model.
                List of all available memory ordering see opt::memory_model.
                Default if cds::opt::v:relaxed_ordering
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

    /// Flat-combining deque
    /**
        @ingroup cds_nonintrusive_deque
        @ingroup cds_flat_combining_container

        \ref cds_flat_combining_description "Flat combining" sequential deque.
        The class can be considered as a concurrent FC-based wrapper for \p std::deque.

        Template parameters:
        - \p T - a value type stored in the deque
        - \p Deque - sequential deque implementation, for example, \p std::deque<T> (the default)
            or \p boost::container::deque
        - \p Trats - type traits of flat combining, default is \p fcdeque::traits.
            \p fcdeque::make_traits metafunction can be used to construct specialized \p %fcdeque::traits
    */
    template <typename T,
        class Deque = std::deque<T>,
        typename Traits = fcdeque::traits
    >
    class FCDeque
#ifndef CDS_DOXYGEN_INVOKED
        : public cds::algo::flat_combining::container
#endif
    {
    public:
        typedef T           value_type;     ///< Value type
        typedef Deque       deque_type;     ///< Sequential deque class
        typedef Traits      traits;         ///< Deque type traits

        typedef typename traits::stat  stat;   ///< Internal statistics type
        static CDS_CONSTEXPR const bool c_bEliminationEnabled = traits::enable_elimination; ///< \p true if elimination is enabled

    protected:
        //@cond
        /// Deque operation IDs
        enum fc_operation {
            op_push_front = cds::algo::flat_combining::req_Operation, ///< Push front
            op_push_front_move,     ///< Push front (move semantics)
            op_push_back,           ///< Push back
            op_push_back_move,      ///< Push back (move semantics)
            op_pop_front,           ///< Pop front
            op_pop_back,            ///< Pop back
            op_clear,               ///< Clear
            op_empty                ///< Empty
        };

        /// Flat combining publication list record
        struct fc_record: public cds::algo::flat_combining::publication_record
        {
            union {
                value_type const *  pValPush; ///< Value to push
                value_type *        pValPop;  ///< Pop destination
            };
            bool            bEmpty; ///< \p true if the deque is empty
        };
        //@endcond

        /// Flat combining kernel
        typedef cds::algo::flat_combining::kernel< fc_record, traits > fc_kernel;

    protected:
        //@cond
        fc_kernel   m_FlatCombining;
        deque_type  m_Deque;
        //@endcond

    public:
        /// Initializes empty deque object
        FCDeque()
        {}

        /// Initializes empty deque object and gives flat combining parameters
        FCDeque(
            unsigned int nCompactFactor     ///< Flat combining: publication list compacting factor
            ,unsigned int nCombinePassCount ///< Flat combining: number of combining passes for combiner thread
            )
            : m_FlatCombining( nCompactFactor, nCombinePassCount )
        {}

        /// Inserts a new element at the beginning of the deque container
        /**
            The function always returns \p true
        */
        bool push_front(
            value_type const& val ///< Value to be copied to inserted element
        )
        {
            fc_record * pRec = m_FlatCombining.acquire_record();
            pRec->pValPush = &val;

            if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_push_front, pRec, *this );
            else
                m_FlatCombining.combine( op_push_front, pRec, *this );

            assert( pRec->is_done() );
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPushFront();
            return true;
        }

        /// Inserts a new element at the beginning of the deque container (move semantics)
        /**
            The function always returns \p true
        */
        bool push_front(
            value_type&& val ///< Value to be moved to inserted element
        )
        {
            fc_record * pRec = m_FlatCombining.acquire_record();
            pRec->pValPush = &val;

            if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_push_front_move, pRec, *this );
            else
                m_FlatCombining.combine( op_push_front_move, pRec, *this );

            assert( pRec->is_done() );
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPushFrontMove();
            return true;
        }

        /// Inserts a new element at the end of the deque container
        /**
            The function always returns \p true
        */
        bool push_back(
            value_type const& val ///< Value to be copied to inserted element
        )
        {
            fc_record * pRec = m_FlatCombining.acquire_record();
            pRec->pValPush = &val;

            if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_push_back, pRec, *this );
            else
                m_FlatCombining.combine( op_push_back, pRec, *this );

            assert( pRec->is_done() );
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPushBack();
            return true;
        }

        /// Inserts a new element at the end of the deque container (move semantics)
        /**
            The function always returns \p true
        */
        bool push_back(
            value_type&& val ///< Value to be moved to inserted element
        )
        {
            fc_record * pRec = m_FlatCombining.acquire_record();
            pRec->pValPush = &val;

            if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_push_back_move, pRec, *this );
            else
                m_FlatCombining.combine( op_push_back_move, pRec, *this );

            assert( pRec->is_done() );
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPushBackMove();
            return true;
        }

        /// Removes the first element in the deque container
        /**
            The function returns \p false if the deque is empty, \p true otherwise.
            If the deque is empty \p val is not changed.
        */
        bool pop_front(
            value_type& val ///< Target to be received the copy of removed element
        )
        {
            fc_record * pRec = m_FlatCombining.acquire_record();
            pRec->pValPop = &val;

            if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_pop_front, pRec, *this );
            else
                m_FlatCombining.combine( op_pop_front, pRec, *this );

            assert( pRec->is_done() );
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPopFront( pRec->bEmpty );
            return !pRec->bEmpty;
        }

        /// Removes the last element in the deque container
        /**
            The function returns \p false if the deque is empty, \p true otherwise.
            If the deque is empty \p val is not changed.
        */
        bool pop_back(
            value_type& val ///< Target to be received the copy of removed element
        )
        {
            fc_record * pRec = m_FlatCombining.acquire_record();
            pRec->pValPop = &val;

            if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_pop_back, pRec, *this );
            else
                m_FlatCombining.combine( op_pop_back, pRec, *this );

            assert( pRec->is_done() );
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPopBack( pRec->bEmpty );
            return !pRec->bEmpty;
        }

        /// Clears the deque
        void clear()
        {
            fc_record * pRec = m_FlatCombining.acquire_record();

            if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_clear, pRec, *this );
            else
                m_FlatCombining.combine( op_clear, pRec, *this );

            assert( pRec->is_done() );
            m_FlatCombining.release_record( pRec );
        }

        /// Returns the number of elements in the deque.
        /**
            Note that <tt>size() == 0</tt> is not mean that the deque is empty because
            combining record can be in process.
            To check emptiness use \ref empty function.
        */
        size_t size() const
        {
            return m_Deque.size();
        }

        /// Checks if the deque is empty
        /**
            If the combining is in process the function waits while combining done.
        */
        bool empty()
        {
            fc_record * pRec = m_FlatCombining.acquire_record();

            if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_empty, pRec, *this );
            else
                m_FlatCombining.combine( op_empty, pRec, *this );

            assert( pRec->is_done() );
            m_FlatCombining.release_record( pRec );
            return pRec->bEmpty;
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
            the deque should perform an action recorded in \p pRec.
        */
        void fc_apply( fc_record * pRec )
        {
            assert( pRec );

            switch ( pRec->op() ) {
            case op_push_front:
                assert( pRec->pValPush );
                m_Deque.push_front( *(pRec->pValPush) );
                break;
            case op_push_front_move:
                assert( pRec->pValPush );
                m_Deque.push_front( std::move( *(pRec->pValPush )) );
                break;
            case op_push_back:
                assert( pRec->pValPush );
                m_Deque.push_back( *(pRec->pValPush) );
                break;
            case op_push_back_move:
                assert( pRec->pValPush );
                m_Deque.push_back( std::move( *(pRec->pValPush )) );
                break;
            case op_pop_front:
                assert( pRec->pValPop );
                pRec->bEmpty = m_Deque.empty();
                if ( !pRec->bEmpty ) {
                    *(pRec->pValPop) = m_Deque.front();
                    m_Deque.pop_front();
                }
                break;
            case op_pop_back:
                assert( pRec->pValPop );
                pRec->bEmpty = m_Deque.empty();
                if ( !pRec->bEmpty ) {
                    *(pRec->pValPop) = m_Deque.back();
                    m_Deque.pop_back();
                }
                break;
            case op_clear:
                while ( !m_Deque.empty() )
                    m_Deque.pop_front();
                break;
            case op_empty:
                pRec->bEmpty = m_Deque.empty();
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
                switch ( it->op() ) {
                case op_push_front:
                case op_push_front_move:
                    if ( itPrev != itEnd
                      && (itPrev->op() == op_pop_front || ( m_Deque.empty() && itPrev->op() == op_pop_back )))
                    {
                        collide( *it, *itPrev );
                        itPrev = itEnd;
                    }
                    else
                        itPrev = it;
                    break;
                case op_push_back:
                case op_push_back_move:
                    if ( itPrev != itEnd
                        && (itPrev->op() == op_pop_back || ( m_Deque.empty() && itPrev->op() == op_pop_front )))
                    {
                        collide( *it, *itPrev );
                        itPrev = itEnd;
                    }
                    else
                        itPrev = it;
                    break;
                case op_pop_front:
                    if ( itPrev != itEnd
                        && ( itPrev->op() == op_push_front || itPrev->op() == op_push_front_move
                          || ( m_Deque.empty() && ( itPrev->op() == op_push_back || itPrev->op() == op_push_back_move ))))
                    {
                        collide( *itPrev, *it );
                        itPrev = itEnd;
                    }
                    else
                        itPrev = it;
                    break;
                case op_pop_back:
                    if ( itPrev != itEnd
                        && ( itPrev->op() == op_push_back || itPrev->op() == op_push_back_move
                        || ( m_Deque.empty() && ( itPrev->op() == op_push_front || itPrev->op() == op_push_front_move ))))
                    {
                        collide( *itPrev, *it );
                        itPrev = itEnd;
                    }
                    else
                        itPrev = it;
                    break;
                }
            }
        }
        //@endcond

    private:
        //@cond
        void collide( fc_record& recPush, fc_record& recPop )
        {
            *(recPop.pValPop) = *(recPush.pValPush);
            recPop.bEmpty = false;
            m_FlatCombining.operation_done( recPush );
            m_FlatCombining.operation_done( recPop );
            m_FlatCombining.internal_statistics().onCollide();
        }
        //@endcond
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_FCDEQUE_H

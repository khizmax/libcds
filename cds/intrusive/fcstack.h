// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_FCSTACK_H
#define CDSLIB_INTRUSIVE_FCSTACK_H

#include <cds/algo/flat_combining.h>
#include <cds/algo/elimination_opt.h>
#include <cds/intrusive/options.h>
#include <boost/intrusive/slist.hpp>

namespace cds { namespace intrusive {

    /// FCStack related definitions
    namespace fcstack {

        /// FCStack internal statistics
        template <typename Counter = cds::atomicity::event_counter >
        struct stat: public cds::algo::flat_combining::stat<Counter>
        {
            typedef cds::algo::flat_combining::stat<Counter>    flat_combining_stat; ///< Flat-combining statistics
            typedef typename flat_combining_stat::counter_type  counter_type;        ///< Counter type

            counter_type    m_nPush     ;   ///< Count of push operations
            counter_type    m_nPop      ;   ///< Count of success pop operations
            counter_type    m_nFailedPop;   ///< Count of failed pop operations (pop from empty stack)
            counter_type    m_nCollided ;   ///< How many pairs of push/pop were collided, if elimination is enabled

            //@cond
            void    onPush()               { ++m_nPush; }
            void    onPop( bool bFailed )  { if ( bFailed ) ++m_nFailedPop; else ++m_nPop;  }
            void    onCollide()            { ++m_nCollided; }
            //@endcond
        };

        /// FCStack dummy statistics, no overhead
        struct empty_stat: public cds::algo::flat_combining::empty_stat
        {
            //@cond
            void    onPush()        {}
            void    onPop(bool)     {}
            void    onCollide()     {}
            //@endcond
        };

        /// FCStack type traits
        struct traits: public cds::algo::flat_combining::traits
        {
            typedef cds::intrusive::opt::v::empty_disposer  disposer ; ///< Disposer to erase removed elements. Used only in \p FCStack::clear() function
            typedef empty_stat      stat;   ///< Internal statistics
            static constexpr const bool enable_elimination = false; ///< Enable \ref cds_elimination_description "elimination"
        };

        /// Metafunction converting option list to traits
        /**
            \p Options are:
            - any \p cds::algo::flat_combining::make_traits options
            - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::intrusive::v::empty_disposer.
                This option is used only in \p FCStack::clear() function.
            - \p opt::stat - internal statistics, possible type: \p fcstack::stat, \p fcstack::empty_stat (the default)
            - \p opt::enable_elimination - enable/disable operation \ref cds_elimination_description "elimination"
                By default, the elimination is disabled.
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

    } // namespace fcstack

    /// Flat-combining intrusive stack
    /**
        @ingroup cds_intrusive_stack
        @ingroup cds_flat_combining_intrusive

        \ref cds_flat_combining_description "Flat combining" sequential intrusive stack.

        Template parameters:
        - \p T - a value type stored in the stack
        - \p Container - sequential intrusive container with \p push_front and \p pop_front functions.
            Possible containers are \p boost::intrusive::slist (the default), \p boost::inrtrusive::list
        - \p Traits - type traits of flat combining, default is \p fcstack::traits.
            \p fcstack::make_traits metafunction can be used to construct specialized \p %traits
    */
    template <typename T
        ,class Container = boost::intrusive::slist<T>
        ,typename Traits = fcstack::traits
    >
    class FCStack
#ifndef CDS_DOXYGEN_INVOKED
        : public cds::algo::flat_combining::container
#endif
    {
    public:
        typedef T           value_type;     ///< Value type
        typedef Container   container_type; ///< Sequential container type
        typedef Traits      traits;         ///< Stack traits

        typedef typename traits::disposer  disposer;   ///< The disposer functor. The disposer is used only in \ref clear() function
        typedef typename traits::stat  stat;   ///< Internal statistics type
        static constexpr const bool c_bEliminationEnabled = traits::enable_elimination; ///< \p true if elimination is enabled

    protected:
        //@cond
        /// Stack operation IDs
        enum fc_operation {
            op_push = cds::algo::flat_combining::req_Operation, ///< Push
            op_pop,                 ///< Pop
            op_clear,               ///< Clear
            op_clear_and_dispose    ///< Clear and dispose
        };

        /// Flat combining publication list record
        struct fc_record: public cds::algo::flat_combining::publication_record
        {
            value_type * pVal;  ///< Value to push or pop
            bool         bEmpty; ///< \p true if the stack is empty
        };
        //@endcond

        /// Flat combining kernel
        typedef cds::algo::flat_combining::kernel< fc_record, traits > fc_kernel;

    protected:
        //@cond
        mutable fc_kernel m_FlatCombining;
        container_type    m_Stack;
        //@endcond

    public:
        /// Initializes empty stack object
        FCStack()
        {}

        /// Initializes empty stack object and gives flat combining parameters
        FCStack(
            unsigned int nCompactFactor     ///< Flat combining: publication list compacting factor
            ,unsigned int nCombinePassCount ///< Flat combining: number of combining passes for combiner thread
            )
            : m_FlatCombining( nCompactFactor, nCombinePassCount )
        {}

        /// Inserts a new element at the top of stack
        /**
            The content of the new element initialized to a copy of \p val.
        */
        bool push( value_type& val )
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pVal = &val;

            constexpr_if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_push, pRec, *this );
            else
                m_FlatCombining.combine( op_push, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );
            m_FlatCombining.internal_statistics().onPush();
            return true;
        }

        /// Removes the element on top of the stack
        value_type * pop()
        {
            auto pRec = m_FlatCombining.acquire_record();
            pRec->pVal = nullptr;

            constexpr_if ( c_bEliminationEnabled )
                m_FlatCombining.batch_combine( op_pop, pRec, *this );
            else
                m_FlatCombining.combine( op_pop, pRec, *this );

            assert( pRec->is_done());
            m_FlatCombining.release_record( pRec );

            m_FlatCombining.internal_statistics().onPop( pRec->bEmpty );
            return pRec->pVal;
        }

        /// Clears the stack
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

        /// Exclusive access to underlying stack object
        /**
            The functor \p f can do any operation with underlying \p container_type in exclusive mode.
            For example, you can iterate over the stack.
            \p Func signature is:
            \code
                void f( container_type& stack );
            \endcode
        */
        template <typename Func>
        void apply( Func f )
        {
            auto& stack = m_Stack;
            m_FlatCombining.invoke_exclusive( [&stack, &f]() { f( stack ); } );
        }

        /// Exclusive access to underlying stack object
        /**
            The functor \p f can do any operation with underlying \p container_type in exclusive mode.
            For example, you can iterate over the stack.
            \p Func signature is:
            \code
                void f( container_type const& stack );
            \endcode
        */
        template <typename Func>
        void apply( Func f ) const
        {
            auto const& stack = m_Stack;
            m_FlatCombining.invoke_exclusive( [&stack, &f]() { f( stack ); } );
        }

        /// Returns the number of elements in the stack.
        /**
            Note that <tt>size() == 0</tt> is not mean that the stack is empty because
            combining record can be in process.
            To check emptiness use \ref empty function.
        */
        size_t size() const
        {
            return m_Stack.size();
        }

        /// Checks if the stack is empty
        /**
            If the combining is in process the function waits while it is done.
        */
        bool empty() const
        {
            bool bRet = false;
            auto const& stack = m_Stack;
            m_FlatCombining.invoke_exclusive( [&stack, &bRet]() { bRet = stack.empty(); } );
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
            the stack should perform an action recorded in \p pRec.
        */
        void fc_apply( fc_record* pRec )
        {
            assert( pRec );

            switch ( pRec->op()) {
            case op_push:
                assert( pRec->pVal );
                m_Stack.push_front( *(pRec->pVal ));
                break;
            case op_pop:
                pRec->bEmpty = m_Stack.empty();
                if ( !pRec->bEmpty ) {
                    pRec->pVal = &m_Stack.front();
                    m_Stack.pop_front();
                }
                break;
            case op_clear:
                m_Stack.clear();
                break;
            case op_clear_and_dispose:
                m_Stack.clear_and_dispose( disposer());
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
                case op_push:
                case op_pop:
                    if ( itPrev != itEnd && collide( *itPrev, *it ))
                        itPrev = itEnd;
                    else
                        itPrev = it;
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
                case op_push:
                    if ( rec2.op() == op_pop ) {
                        assert(rec1.pVal);
                        rec2.pVal = rec1.pVal;
                        rec2.bEmpty = false;
                        m_FlatCombining.operation_done( rec1 );
                        m_FlatCombining.operation_done( rec2 );
                        m_FlatCombining.internal_statistics().onCollide();
                        return true;
                    }
                    break;
                case op_pop:
                    if ( rec2.op() == op_push ) {
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

#endif // #ifndef CDSLIB_INTRUSIVE_FCSTACK_H

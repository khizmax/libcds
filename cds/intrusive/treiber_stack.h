//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_TREIBER_STACK_H
#define __CDS_INTRUSIVE_TREIBER_STACK_H

#include <type_traits>
#include <cds/intrusive/single_link_struct.h>
#include <cds/ref.h>
#include <cds/algo/elimination.h>
#include <cds/opt/buffer.h>
#include <cds/lock/spinlock.h>
#include <cds/lock/scoped_lock.h>
#include <cds/details/type_padding.h>

namespace cds { namespace intrusive {

    /// TreiberStack related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace treiber_stack {

        //@cond
        /// Operation id for the \ref cds_elimination_description "elimination back-off"
        enum operation_id {
            op_push,    ///< push op id
            op_pop      ///< pop op id
        };

        /// Operation descriptor for the \ref cds_elimination_description "elimination back-off"
        template <typename T>
        struct operation: public cds::algo::elimination::operation_desc
        {
            operation_id    idOp;   ///< Op id
            T *             pVal;   ///< for push: pointer to argument; for pop: accepts a return value
            atomics::atomic<unsigned int> nStatus; ///< Internal elimination status

            operation()
                : pVal( nullptr )
                , nStatus(0)
            {}
        };
        //@endcond

        /// Stack internal statistics. May be useful for debugging or profiling
        /**
            Template argument \p Counter defines type of counter.
            Default is cds::atomicity::event_counter.
            You may use stronger type of counter like as cds::atomicity::item_counter,
            or even an integral type, for example, \p int
        */
        template <typename Counter = cds::atomicity::event_counter >
        struct stat
        {
            typedef Counter     counter_type    ;   ///< Counter type

            counter_type m_PushCount        ;  ///< Push call count
            counter_type m_PopCount         ;  ///< Pop call count
            counter_type m_PushRace         ;  ///< Count of push race conditions encountered
            counter_type m_PopRace          ;  ///< Count of pop race conditions encountered
            counter_type m_ActivePushCollision  ; ///< Count of active push collision for elimination back-off
            counter_type m_ActivePopCollision   ; ///< Count of active pop collision for elimination back-off
            counter_type m_PassivePushCollision ; ///< Count of passive push collision for elimination back-off
            counter_type m_PassivePopCollision  ; ///< Count of passive pop collision for elimination back-off
            counter_type m_EliminationFailed    ; ///< Count of unsuccessful elimination back-off

            //@cond
            void onPush()               { ++m_PushCount; }
            void onPop()                { ++m_PopCount; }
            void onPushRace()           { ++m_PushRace; }
            void onPopRace()            { ++m_PopRace; }
            void onActiveCollision( operation_id opId )
            {
                if ( opId == treiber_stack::op_push )
                    ++m_ActivePushCollision;
                else
                    ++m_ActivePopCollision;
            }
            void onPassiveCollision( operation_id opId )
            {
                if ( opId == treiber_stack::op_push )
                    ++m_PassivePushCollision;
                else
                    ++m_PassivePopCollision;
            }
            void onEliminationFailed()          { ++m_EliminationFailed;}
            //@endcond
        };

        /// Empty (no overhead) stack statistics. Support interface like treiber_stack::stat
        struct empty_stat
        {
            //@cond
            void onPush()       {}
            void onPop()        {}
            void onPushRace()   {}
            void onPopRace()    {}
            void onActiveCollision( operation_id )  {}
            void onPassiveCollision( operation_id ) {}
            void onEliminationFailed() {}
            //@endcond
        };

        //@cond
        namespace details {

            template <bool EnableElimination, typename T, typename Traits>
            class elimination_backoff;

            template <typename T, typename Traits>
            class elimination_backoff<false, T, Traits>
            {
                typedef typename Traits::back_off   back_off;

                back_off    m_bkoff;
            public:
                elimination_backoff()
                {}

                elimination_backoff( size_t )
                {}

                void reset()
                {
                    m_bkoff.reset();
                }

                template <typename Stat>
                bool backoff(treiber_stack::operation< T >&, Stat& )
                {
                    m_bkoff();
                    return false;
                }
            };

            template <typename T, typename Traits>
            class elimination_backoff<true, T, Traits>
            {
                typedef typename Traits::back_off   back_off;

                /// Back-off for elimination (usually delay)
                typedef typename Traits::elimination_backoff elimination_backoff_type;
                /// Lock type used in elimination back-off
                typedef typename Traits::lock_type elimination_lock_type;
                /// Random engine used in elimination back-off
                typedef typename Traits::random_engine elimination_random_engine;

                /// Per-thread elimination record
                typedef cds::algo::elimination::record  elimination_rec;

                /// Collision array record
                struct collision_array_record {
                    elimination_rec *     pRec;
                    elimination_lock_type lock;
                };

                /// Collision array used in elimination-backoff; each item is optimized for cache-line size
                typedef typename Traits::buffer::template rebind<
                    typename cds::details::type_padding<collision_array_record, cds::c_nCacheLineSize >::type
                >::other collision_array;

                /// Operation descriptor used in elimination back-off
                typedef treiber_stack::operation< T >  operation_desc;

#           if !(defined(CDS_CXX11_LAMBDA_SUPPORT) && !(CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == CDS_COMPILER_MSVC10))
                struct bkoff_predicate {
                    operation_desc * pOp;
                    bkoff_predicate( operation_desc * p ): pOp(p) {}
                    bool operator()() { return pOp->nStatus.load( atomics::memory_order_acquire ) != op_busy; }
                };
#           endif

                /// Elimination back-off data
                struct elimination_data {
                    elimination_random_engine   randEngine; ///< random engine
                    collision_array             collisions; ///< collision array

                    elimination_data()
                    {
                        //TODO: check Traits::buffer must be static!
                    }
                    elimination_data( size_t nCollisionCapacity )
                        : collisions( nCollisionCapacity )
                    {}
                };

                elimination_data m_Elimination;

                enum operation_status {
                    op_free = 0,
                    op_busy = 1,
                    op_collided = 2
                };

                typedef cds::lock::scoped_lock< elimination_lock_type > slot_scoped_lock;

            public:
                elimination_backoff()
                {
                    m_Elimination.collisions.zeroize();
                }

                elimination_backoff( size_t nCollisionCapacity )
                    : m_Elimination( nCollisionCapacity )
                {
                    m_Elimination.collisions.zeroize();
                }

                void reset()
                {}

                template <typename Stat>
                bool backoff( operation_desc& op, Stat& stat )
                {
                    elimination_backoff_type bkoff;
                    op.nStatus.store( op_busy, atomics::memory_order_relaxed );

                    elimination_rec * myRec = cds::algo::elimination::init_record( op );

                    collision_array_record& slot = m_Elimination.collisions[m_Elimination.randEngine() % m_Elimination.collisions.capacity()];
                    {
                        slot.lock.lock();
                        elimination_rec * himRec = slot.pRec;
                        if ( himRec ) {
                            operation_desc * himOp = static_cast<operation_desc *>( himRec->pOp );
                            assert( himOp );
                            if ( himOp->idOp != op.idOp ) {
                                if ( op.idOp == treiber_stack::op_push )
                                    himOp->pVal = op.pVal;
                                else
                                    op.pVal = himOp->pVal;
                                slot.pRec = nullptr;
                                slot.lock.unlock();

                                himOp->nStatus.store( op_collided, atomics::memory_order_release );
                                cds::algo::elimination::clear_record();
                                stat.onActiveCollision( op.idOp );
                                return true;
                            }
                            himOp->nStatus.store( op_free, atomics::memory_order_release );
                        }
                        slot.pRec = myRec;
                        slot.lock.unlock();
                    }

                    // Wait for colliding operation
#               if defined(CDS_CXX11_LAMBDA_SUPPORT) && !(CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == CDS_COMPILER_MSVC10)
                    // MSVC++ 2010 compiler error C2065: 'op_busy' : undeclared identifier
                    bkoff( [&op]() -> bool { return op.nStatus.load( atomics::memory_order_acquire ) != op_busy; } );
#               else
                    // Local structs is not supported by old compilers (for example, GCC 4.3)
                    //struct bkoff_predicate {
                    //    operation_desc * pOp;
                    //    bkoff_predicate( operation_desc * p ): pOp(p) {}
                    //    bool operator()() { return pOp->nStatus.load( atomics::memory_order_acquire ) != op_busy; }
                    //};
                    bkoff( bkoff_predicate(&op) );
#               endif

                    {
                        slot_scoped_lock l( slot.lock );
                        if ( slot.pRec == myRec )
                            slot.pRec = nullptr;
                    }

                    bool bCollided = op.nStatus.load( atomics::memory_order_acquire ) == op_collided;

                    if ( !bCollided )
                        stat.onEliminationFailed();
                    else
                        stat.onPassiveCollision( op.idOp );

                    cds::algo::elimination::clear_record();
                    return bCollided;
                }
            };

        } // namespace details
        //@endcond
    } // namespace treiber_stack

    /// Treiber stack
    /** @ingroup cds_intrusive_stack
        Intrusive implementation of well-known Treiber's stack algorithm:
        - R. K. Treiber. Systems programming: Coping with parallelism. Technical Report RJ 5118, IBM Almaden Research Center, April 1986.

        \ref cds_elimination_description "Elimination back-off technique" can be used optionally.
        The idea of elimination algorithm is taken from:
        - [2004] Danny Hendler, Nir Shavit, Lena Yerushalmi "A Scalable Lock-free Stack Algorithm"

        The elimination algorithm uses a single elimination array as a back-off schema
        on a shared lock-free stack. If the threads fail on the stack, they attempt to eliminate
        on the array, and if they fail in eliminating, they attempt to access the stack again and so on.

        @note Hendler's et al paper describes a lock-free implementation of elimination back-off which is quite complex.
        The main difficulty is the managing life-time of elimination record.
        Our implementation uses simplified lock-based (spin-based) approach which allows
        the elimination record allocation on thread's stack.
        This approach demonstrates sufficient performance under high load.

        Template arguments:
        - \p GC - garbage collector type: gc::HP, gc::HRC, gc::PTB
        - \p T - type to be inserted into the stack
        - \p Options - options

        \p Options are:
        - opt::hook - hook used. Possible values are: single_link::base_hook, single_link::member_hook, single_link::traits_hook.
            If the option is not specified, <tt>single_link::base_hook<></tt> is used.
            For Gidenstam's gc::HRC, only single_link::base_hook is supported.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::Default is used.
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer. This option is used only
            in \ref clear function.
        - opt::link_checker - the type of node's link fields checking. Default is \ref opt::debug_check_link.
            Note: for gc::HRC garbage collector, link checking policy is always selected as \ref opt::always_check_link.
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter
        - opt::stat - the type to gather internal statistics.
            Possible option value are: \ref treiber_stack::stat, \ref treiber_stack::empty_stat (the default),
            user-provided class that supports treiber_stack::stat interface.
        - opt::enable_elimination - enable elimination back-off for the stack. Default value is \p valse.

        If elimination back-off is enabled (\p %cds::opt::enable_elimination< true >) additional options can be specified:
        - opt::buffer - a buffer type for elimination array, see \p opt::v::static_buffer, \p opt::v::dynamic_buffer.
            The buffer can be any size: \p Exp2 template parameter of those classes can be \p false.
            The size should be selected empirically for your application and hardware, there are no common rules for that.
            Default is <tt> %opt::v::static_buffer< any_type, 4 > </tt>.
        - opt::random_engine - a random engine to generate a random position in elimination array.
            Default is opt::v::c_rand.
        - opt::elimination_backoff - back-off strategy to wait for elimination, default is cds::backoff::delay<>
        - opt::lock_type - a lock type used in elimination back-off, default is cds::lock::Spin.

        Garbage collecting schema \p GC must be consistent with the single_link::node GC.

        Be careful when you want destroy an item popped, see \ref cds_intrusive_item_destroying "Destroying items of intrusive containers".

        @anchor cds_intrusive_TreiberStack_examples
        \par Examples

        Example of how to use \p single_link::base_hook.
        Your class that objects will be pushed on \p %TreiberStack should be based on \p single_link::node class
        \code
        #include <cds/intrusive/stack/treiber_stack.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::intrusive;
        typedef cds::gc::HP gc;

        struct myData: public ci::single_link::node< gc >
        {
            // ...
        };

        // Stack type
        typedef ci::TreiberStack< gc,
            myData,
            ci::opt::hook< ci::single_link::base_hook< gc > >
        > stack_t;

        // Stack with elimination back-off enabled
        typedef ci::TreiberStack< gc,
            myData,
            ci::opt::hook< ci::single_link::base_hook< gc > >,
            cds::opt::enable_elimination<true>
        > elimination_stack_t;
        \endcode

        Example of how to use \p base_hook with different tags.
        \code
        #include <cds/intrusive/stack/treiber_stack.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::intrusive;
        typedef cds::gc::HP gc;

        // It is not necessary to declare complete type for tags
        struct tag1;
        struct tag2;

        struct myData
            : public ci::single_link::node< gc, tag1 >
            , public ci::single_link::node< gc, tag2 >
        {
            // ...
        };

        typedef ci::TreiberStack< gc, myData, ci::opt::hook< ci::single_link::base_hook< gc, tag1 > > stack1_t;
        typedef ci::TreiberStack< gc, myData, ci::opt::hook< ci::single_link::base_hook< gc, tag2 > > stack2_t;

        // You may add myData objects in the objects of type stack1_t and stack2_t independently
        void foo() {
            stack1_t    s1;
            stack2_t    s2;

            myData i1, i2;
            s1.push( i1 );
            s2.push( i2 );
            s2.push( i1 )   ;   // i1 is now contained in s1 and s2.

            myData * p;

            p = s1.pop()    ;   // pop i1 from s1
            p = s1.pop()    ;   // p == nullptr, s1 is empty
            p = s2.pop()    ;   // pop i1 from s2
            p = s2.pop()    ;   // pop i2 from s2
            p = s2.pop()    ;   // p == nullptr, s2 is empty
        }
        \endcode

        Example of how to use \p member_hook.
        Your class that will be pushed on \p %TreiberStack should have a member of type \p single_link::node
        \code
        #include <cds/intrusive/stack/treiber_stack.h>
        #include <cds/gc/hp.h>
        #include <stddef.h>     // offsetof macro

        namespace ci = cds::intrusive;
        typedef cds::gc::HP gc;

        struct myData
        {
            // ...
            ci::single_link::node< gc >      member_hook_;
            // ...
        };

        typedef ci::TreiberStack< gc, myData,
            ci::opt::hook<
                ci::single_link::member_hook< offsetof(myData, member_hook_),
                gc
            >
        > stack_t;
        \endcode
    */
    template <typename GC, typename T, CDS_DECL_OPTIONS13>
    class TreiberStack
    {
        //@cond
        struct default_options
        {
            typedef cds::backoff::Default           back_off;
            typedef single_link::base_hook<>        hook;
            typedef opt::v::empty_disposer          disposer;
            typedef atomicity::empty_item_counter   item_counter;
            typedef opt::v::relaxed_ordering        memory_model;
            typedef treiber_stack::empty_stat       stat;
            static CDS_CONSTEXPR_CONST opt::link_check_type link_checker = opt::debug_check_link;

            // Elimination back-off options
            static CDS_CONSTEXPR_CONST bool enable_elimination = false;
            typedef cds::backoff::delay<>          elimination_backoff;
            typedef opt::v::static_buffer< int, 4 > buffer;
            typedef opt::v::c_rand                  random_engine;
            typedef cds::lock::Spin                 lock_type;
        };
        //@endcond

    public:
        //@cond
        typedef typename opt::make_options<
            typename cds::opt::find_type_traits< default_options, CDS_OPTIONS13 >::type
            ,CDS_OPTIONS13
        >::type   options;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename GC2, typename T2, CDS_DECL_OTHER_OPTIONS13>
        struct rebind {
            typedef TreiberStack< GC2, T2, CDS_OTHER_OPTIONS13> other   ;   ///< Rebinding result
        };

    public:
        typedef T  value_type   ;   ///< type of value stored in the stack
        typedef typename options::hook      hook        ;   ///< hook type
        typedef typename hook::node_type    node_type   ;   ///< node type
        typedef typename options::disposer  disposer    ;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef typename single_link::get_link_checker< node_type, options::link_checker >::type link_checker   ;   ///< link checker
        typedef typename options::memory_model  memory_model      ;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename options::item_counter item_counter ;   ///< Item counting policy used
        typedef typename options::stat      stat        ;   ///< Internal statistics policy used

        typedef GC  gc          ;   ///< Garbage collector
        typedef typename options::back_off  back_off    ;   ///< back-off strategy

    public: // related to elimination back-off

        /// Elimination back-off is enabled or not
        static CDS_CONSTEXPR_CONST bool enable_elimination = options::enable_elimination;
        /// back-off strategy used to wait for elimination
        typedef typename options::elimination_backoff elimination_backoff_type;
        /// Lock type used in elimination back-off
        typedef typename options::lock_type elimination_lock_type;
        /// Random engine used in elimination back-off
        typedef typename options::random_engine elimination_random_engine;


    protected:
        typename node_type::atomic_node_ptr m_Top       ;   ///< Top of the stack
        item_counter        m_ItemCounter   ;   ///< Item counter
        stat                m_stat          ;   ///< Internal statistics

        //@cond
        treiber_stack::details::elimination_backoff<enable_elimination, value_type, options> m_Backoff;

        typedef intrusive::node_to_value<TreiberStack>  node_to_value;
        typedef treiber_stack::operation< value_type >  operation_desc;
        //@endcond

    protected:
        //@cond
        void clear_links( node_type * pNode ) CDS_NOEXCEPT
        {
            pNode->m_pNext.store( nullptr, memory_model::memory_order_relaxed );
        }

        template <bool EnableElimination>
        struct elimination_backoff_impl;

        void init()
        {
            // GC and node_type::gc must be the same
            static_assert(( std::is_same<gc, typename node_type::gc>::value ), "GC and node_type::gc must be the same");

            // For cds::gc::HRC, only base_hook is allowed
            static_assert((
                std::conditional<
                std::is_same<gc, cds::gc::HRC>::value,
                std::is_same< typename hook::hook_type, opt::base_hook_tag >,
                boost::true_type
                >::type::value
                ), "For cds::gc::HRC, only base_hook is allowed");

            static_assert( (!enable_elimination || std::is_same<typename elimination_random_engine::result_type, unsigned int>::value),
                "Random engine result type must be unsigned int" );
        }

        //@endcond

    public:
        /// Constructs empty stack
        TreiberStack()
            : m_Top( nullptr )
        {
            init();
        }

        /// Constructs empty stack and initializes elimination back-off data
        /**
            This form should be used if you use elimination back-off with dynamically allocated collision array, i.e
            \p Options... contains cds::opt::buffer< cds::opt::v::dynamic_buffer >.
            \p nCollisionCapacity parameter specifies the capacity of collision array.
        */
        TreiberStack( size_t nCollisionCapacity )
            : m_Top( nullptr )
            , m_Backoff( nCollisionCapacity )
        {
            init();
        }

        /// Destructor calls \ref cds_intrusive_TreiberStack_clear "clear" member function
        ~TreiberStack()
        {
            clear();
        }

        /// Push the item \p val on the stack
        /**
            No copying is made since it is intrusive stack.
        */
        bool push( value_type& val )
        {
            node_type * pNew = node_traits::to_node_ptr( val );
            link_checker::is_empty( pNew );

            m_Backoff.reset();

            operation_desc op;
            if ( enable_elimination ) {
                op.idOp = treiber_stack::op_push;
                op.pVal = &val;
            }

            node_type * t = m_Top.load(memory_model::memory_order_relaxed);
            while ( true ) {
                pNew->m_pNext.store( t, memory_model::memory_order_relaxed );
                if ( m_Top.compare_exchange_weak( t, pNew, memory_model::memory_order_release, atomics::memory_order_relaxed )) {     // #1 sync-with #2
                    ++m_ItemCounter;
                    m_stat.onPush();
                    return true;
                }
                m_stat.onPushRace();

                if ( m_Backoff.backoff( op, m_stat ))
                    return true;
            }
        }

        /// Pop an item from the stack
        /**
            If stack is empty, returns \p nullptr.
            The disposer is <b>not</b> called for popped item.
            See \ref cds_intrusive_item_destroying "Destroying items of intrusive containers".
        */
        value_type * pop()
        {
            m_Backoff.reset();
            typename gc::Guard  guard;

            operation_desc op;
            if ( enable_elimination ) {
                op.idOp = treiber_stack::op_pop;
            }

            while ( true ) {
                node_type * t = guard.protect( m_Top, node_to_value() );
                if ( t == nullptr )
                    return nullptr;    // stack is empty

                node_type * pNext = t->m_pNext.load(memory_model::memory_order_relaxed);
                if ( m_Top.compare_exchange_weak( t, pNext, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {              // #2
                    clear_links( t );
                    --m_ItemCounter;
                    m_stat.onPop();
                    return node_traits::to_value_ptr( *t );
                }

                m_stat.onPopRace();
                if ( m_Backoff.backoff( op, m_stat )) {
                    // may return nullptr if stack is empty
                    return op.pVal;
                }
            }
        }

        /// Check if stack is empty
        bool empty() const
        {
            // http://www.manning-sandbox.com/thread.jspa?threadID=46245&tstart=0
            return m_Top.load( memory_model::memory_order_relaxed ) == nullptr;
        }

        /// Clear the stack
        /** @anchor cds_intrusive_TreiberStack_clear
            For each removed item the disposer is called.

            <b>Caution</b>
            It is possible that after <tt>clear()</tt> the <tt>empty()</tt> returns \p false
            if some other thread pushes an item into the stack during \p clear works
        */
        void clear()
        {
            back_off bkoff;
            node_type * pTop;
            while ( true ) {
                pTop = m_Top.load( memory_model::memory_order_relaxed );
                if ( pTop == nullptr )
                    return;
                if ( m_Top.compare_exchange_weak( pTop, nullptr, memory_model::memory_order_acq_rel, atomics::memory_order_relaxed ) ) {    // sync-with #1 and #2
                    m_ItemCounter.reset();
                    break;
                }
                bkoff();
            }

            while( pTop ) {
                node_type * p = pTop;
                pTop = p->m_pNext.load(memory_model::memory_order_relaxed);
                clear_links( p );
                gc::template retire<disposer>( node_traits::to_value_ptr( *p ) );
            }
        }

        /// Returns stack's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact is not mean that the stack
            is empty. To check emptyness use \ref empty() method.
        */
        size_t    size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns reference to internal statistics
        stat const& statistics() const
        {
            return m_stat;
        }
    };

}} // namespace cds::intrusive

#endif  // #ifndef __CDS_INTRUSIVE_TREIBER_STACK_H

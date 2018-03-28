// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_TREIBER_STACK_H
#define CDSLIB_INTRUSIVE_TREIBER_STACK_H

#include <type_traits>
#include <mutex>        // unique_lock
#include <cds/intrusive/details/single_link_struct.h>
#include <cds/algo/elimination.h>
#include <cds/opt/buffer.h>
#include <cds/sync/spinlock.h>
#include <cds/details/type_padding.h>

namespace cds { namespace intrusive {

    /// TreiberStack related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace treiber_stack {

        /// Stack node
        /**
            Template parameters:
            - GC - garbage collector used
            - Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <class GC, typename Tag = opt::none >
        using node = cds::intrusive::single_link::node< GC, Tag >;

        /// Base hook
        /**
            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < typename... Options >
        using base_hook = cds::intrusive::single_link::base_hook< Options...>;

        /// Member hook
        /**
            \p MemberOffset specifies offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < size_t MemberOffset, typename... Options >
        using member_hook = cds::intrusive::single_link::member_hook< MemberOffset, Options... >;

        /// Traits hook
        /**
            \p NodeTraits defines type traits for node.
            See \ref node_traits for \p NodeTraits interface description

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename NodeTraits, typename... Options >
        using traits_hook = cds::intrusive::single_link::traits_hook< NodeTraits, Options... >;

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
                , nStatus( 0 /*op_free*/ )
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
            void onEliminationFailed()
            {
                ++m_EliminationFailed;
            }
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

        /// TreiberStack default type traits
        struct traits
        {
            /// Back-off strategy
            typedef cds::backoff::Default           back_off;

            /// Hook, possible types are \p treiber_stack::base_hook, \p treiber_stack::member_hook, \p treiber_stack::traits_hook
            typedef treiber_stack::base_hook<>      hook;

            /// The functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used only in \p TreiberStack::clear() function
            typedef opt::v::empty_disposer          disposer;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef cds::atomicity::empty_item_counter   item_counter;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// Internal statistics (by default, disabled)
            /**
                Possible option value are: \p treiber_stack::stat, \p treiber_stack::empty_stat (the default),
                user-provided class that supports \p %treiber_stack::stat interface.
            */
            typedef treiber_stack::empty_stat       stat;

            /// Link checking, see \p cds::opt::link_checker
            static constexpr const opt::link_check_type link_checker = opt::debug_check_link;

            /** @name Elimination back-off traits
                The following traits is used only if elimination enabled
            */
            ///@{

            /// Enable elimination back-off; by default, it is disabled
            static constexpr const bool enable_elimination = false;

            /// Back-off strategy to wait for elimination, default is \p cds::backoff::delay<>
            typedef cds::backoff::delay<>          elimination_backoff;

            /// Buffer type for elimination array
            /**
                Possible types are \p opt::v::initialized_static_buffer, \p opt::v::initialized_dynamic_buffer.
                The buffer can be any size: \p Exp2 template parameter of those classes can be \p false.
                The size should be selected empirically for your application and hardware, there are no common rules for that.
                Default is <tt> %opt::v::initialized_static_buffer< any_type, 4 > </tt>.
            */
            typedef opt::v::initialized_static_buffer< int, 4 > buffer;

            /// Random engine to generate a random position in elimination array
            typedef opt::v::c_rand  random_engine;

            /// Lock type used in elimination, default is cds::sync::spin
            typedef cds::sync::spin lock_type;

            ///@}
        };

        /// Metafunction converting option list to \p treiber_stack::traits
        /**
            Supported \p Options are:
            - \p opt::hook - hook used. Possible hooks are: \p treiber_stack::base_hook, \p treiber_stack::member_hook, \p treiber_stack::traits_hook.
                If the option is not specified, \p %treiber_stack::base_hook<> is used.
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used only
                in \p TreiberStack::clear function.
            - \p opt::link_checker - the type of node's link fields checking. Default is \ref opt::debug_check_link.
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter, i.e.
                no item counting. Use \p cds::atomicity::item_counter to enable item counting.
            - \p opt::stat - the type to gather internal statistics.
                Possible option value are: \p treiber_stack::stat, \p treiber_stack::empty_stat (the default),
                user-provided class that supports \p treiber_stack::stat interface.
            - \p opt::enable_elimination - enable elimination back-off for the stack. Default value is \p false.

            If elimination back-off is enabled, additional options can be specified:
            - \p opt::buffer - a buffer type for elimination array, see \p opt::v::initialized_static_buffer, \p opt::v::initialized_dynamic_buffer.
                The buffer can be any size: \p Exp2 template parameter of those classes can be \p false.
                The size should be selected empirically for your application and hardware, there are no common rules for that.
                Default is <tt> %opt::v::initialized_static_buffer< any_type, 4 > </tt>.
            - \p opt::random_engine - a random engine to generate a random position in elimination array.
                Default is \p opt::v::c_rand.
            - \p opt::elimination_backoff - back-off strategy to wait for elimination, default is \p cds::backoff::delay<>
            - \p opt::lock_type - a lock type used in elimination back-off, default is \p cds::sync::spin

            Example: declare \p %TreiberStack with elimination enabled and internal statistics
            \code
            typedef cds::intrusive::TreiberStack< cds::gc::HP, Foo,
                typename cds::intrusive::treiber_stack::make_traits<
                    cds::opt::enable_elimination< true >,
                    cds::opt::stat< cds::intrusive::treiber_stack::stat<> >
                >::type
            > myStack;
            \endcode
        */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                , Options...
            >::type   type;
#   endif
        };


        //@cond
        namespace details {

            template <bool EnableElimination, typename T, typename Traits>
            class elimination_backoff;

            template <typename T, typename Traits>
            class elimination_backoff<false, T, Traits>
            {
                typedef typename Traits::back_off   back_off;

                struct wrapper
                {
                    back_off m_bkoff;

                    void reset()
                    {
                        m_bkoff.reset();
                    }

                    template <typename Stat>
                    bool backoff( treiber_stack::operation< T >&, Stat& )
                    {
                        m_bkoff();
                        return false;
                    }
                };

            public:
                elimination_backoff()
                {}

                elimination_backoff( size_t )
                {}

                typedef wrapper type;
                type init()
                {
                    return wrapper();
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

                /// Elimination back-off data
                struct elimination_data {
                    mutable elimination_random_engine randEngine; ///< random engine
                    collision_array                   collisions; ///< collision array

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
                    op_waiting = 1,
                    op_collided = 2
                };

                typedef std::unique_lock< elimination_lock_type > slot_scoped_lock;

                template <bool Exp2 = collision_array::c_bExp2>
                typename std::enable_if< Exp2, size_t >::type slot_index() const
                {
                    return m_Elimination.randEngine() & (m_Elimination.collisions.capacity() - 1);
                }

                template <bool Exp2 = collision_array::c_bExp2>
                typename std::enable_if< !Exp2, size_t >::type slot_index() const
                {
                    return m_Elimination.randEngine() % m_Elimination.collisions.capacity();
                }

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

                typedef elimination_backoff& type;

                type init()
                {
                    return *this;
                }

                void reset()
                {}

                template <typename Stat>
                bool backoff( operation_desc& op, Stat& stat )
                {
                    elimination_backoff_type bkoff;
                    op.nStatus.store( op_waiting, atomics::memory_order_relaxed );

                    elimination_rec * myRec = cds::algo::elimination::init_record( op );

                    collision_array_record& slot = m_Elimination.collisions[ slot_index() ];
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
                                himOp->nStatus.store( op_collided, atomics::memory_order_release );
                                slot.lock.unlock();

                                cds::algo::elimination::clear_record();
                                stat.onActiveCollision( op.idOp );
                                return true;
                            }
                            //himOp->nStatus.store( op_free, atomics::memory_order_release );
                        }
                        slot.pRec = myRec;
                        slot.lock.unlock();
                    }

                    // Wait for colliding operation
                    bkoff( [&op]() noexcept -> bool { return op.nStatus.load( atomics::memory_order_acquire ) != op_waiting; } );

                    {
                        slot_scoped_lock l( slot.lock );
                        if ( slot.pRec == myRec )
                            slot.pRec = nullptr;
                    }

                    bool bCollided = op.nStatus.load( atomics::memory_order_relaxed ) == op_collided;

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

    /// Treiber intrusive stack
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
        This implementation uses simplified lock-based (spin-based) approach which allows
        the elimination record allocation on thread's stack.
        This approach demonstrates sufficient performance under high load.

        Template arguments:
        - \p GC - garbage collector type: \p gc::HP, \p gc::DHP.
            Garbage collecting schema must be the same as \p treiber_stack::node GC.
        - \p T - a type the stack contains. A value of type \p T must be derived
            from \p treiber_stack::node for \p treiber_stack::base_hook,
            or it should have a member of type \p %treiber_stack::node for \p treiber_stack::member_hook,
            or it should be convertible to \p %treiber_stack::node for \p treiber_stack::traits_hook.
        - \p Traits - stack traits, default is \p treiber_stack::traits. You can use \p treiber_stack::make_traits
            metafunction to make your traits or just derive your traits from \p %treiber_stack::traits:
            \code
            struct myTraits: public cds::intrusive::treiber_stack::traits {
                typedef cds::intrusive::treiber_stack::stat<> stat;
            };
            typedef cds::intrusive::TreiberStack< cds::gc::HP, Foo, myTraits > myStack;

            // Equivalent make_traits example:
            typedef cds::intrusive::TreiberStack< cds::gc::HP, Foo,
                typename cds::intrusive::treiber_stack::make_traits<
                    cds::opt::stat< cds::intrusive::treiber_stack::stat<> >
                >::type
            > myStack;
            \endcode

        @note Be careful when you want destroy an item popped, see \ref cds_intrusive_item_destroying "Destroying items of intrusive containers".

        @anchor cds_intrusive_TreiberStack_examples
        \par Examples

        Example of how to use \p treiber_stack::base_hook.
        Your class that objects will be pushed on \p %TreiberStack should be based on \p treiber_stack::node class
        \code
        #include <cds/intrusive/treiber_stack.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::intrusive;
        typedef cds::gc::HP gc;

        struct myData: public ci::treiber_stack::node< gc >
        {
            // ...
        };

        // Stack type
        typedef ci::TreiberStack< gc,
            myData,
            typename cds::intrusive::treiber_stack::make_traits<
                ci::opt::hook< ci::treiber_stack::base_hook< gc > >
            >::type
        > stack_t;

        // Stack with elimination back-off enabled
        typedef ci::TreiberStack< gc,
            myData,
            typename ci::treiber_stack::make_traits<
                ci::opt::hook< ci::treiber_stack::base_hook< gc > >,
                cds::opt::enable_elimination< true >
            >::type
        > elimination_stack_t;
        \endcode

        Example of how to use \p treiber_stack::base_hook with different tags.
        \code
        #include <cds/intrusive/treiber_stack.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::intrusive;
        typedef cds::gc::HP gc;

        // It is not necessary to declare complete type for tags
        struct tag1;
        struct tag2;

        struct myData
            : public ci::treiber_stack::node< gc, tag1 >
            , public ci::treiber_stack::node< gc, tag2 >
        {
            // ...
        };

        typedef ci::TreiberStack< gc,
            myData,
            typename ci::treiber_stack::make_traits<
                ci::opt::hook< ci::treiber_stack::base_hook< gc, tag1 > >
            >::type
        > stack1_t;

        typedef ci::TreiberStack< gc,
            myData,
            typename ci::treiber_stack::make_traits<
                ci::opt::hook< ci::treiber_stack::base_hook< gc, tag2 > >
            >::type
        > stack2_t;

        // You may add myData objects into stack1_t and stack2_t independently
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

        Example of how to use \p treiber_stack::member_hook.
        Your class should have a member of type \p treiber_stack::node
        \code
        #include <stddef.h>     // offsetof macro
        #include <cds/intrusive/treiber_stack.h>
        #include <cds/gc/hp.h>

        namespace ci = cds::intrusive;
        typedef cds::gc::HP gc;

        struct myData
        {
            // ...
            ci::treiber_stack::node< gc >      member_hook_;
            // ...
        };

        typedef ci::TreiberStack< gc,
            myData,
            typename ci::treiber_stack::make_traits<
                ci::opt::hook< ci::treiber_stack::member_hook< offsetof(myData, member_hook_), gc >>
            >::type
        > stack_t;
        \endcode
    */
    template <
        typename GC,
        typename T,
        typename Traits = treiber_stack::traits
    >
    class TreiberStack
    {
    public:
        /// Rebind template arguments
        template <typename GC2, typename T2, typename Traits2>
        struct rebind {
            typedef TreiberStack< GC2, T2, Traits2 > other   ;   ///< Rebinding result
        };

    public:
        typedef GC      gc;             ///< Garbage collector
        typedef T       value_type;     ///< type of value stored in the stack
        typedef Traits  traits;         ///< Stack traits

        typedef typename traits::hook      hook;        ///< hook type
        typedef typename hook::node_type   node_type;   ///< node type
        typedef typename traits::disposer  disposer;    ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef typename single_link::get_link_checker< node_type, traits::link_checker >::type link_checker   ;   ///< link checker
        typedef typename traits::memory_model   memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename traits::item_counter   item_counter;   ///< Item counter class
        typedef typename traits::stat           stat;           ///< Internal statistics
        typedef typename traits::back_off       back_off;       ///< back-off strategy

        /// How many Hazard pointers is required for Treiber's stack implementation
        static constexpr size_t const c_nHazardPtrCount = 1;

    public: // related to elimination back-off

        /// Elimination back-off is enabled or not
        static constexpr const bool enable_elimination = traits::enable_elimination;
        /// back-off strategy used to wait for elimination
        typedef typename traits::elimination_backoff elimination_backoff_type;
        /// Lock type used in elimination back-off
        typedef typename traits::lock_type elimination_lock_type;
        /// Random engine used in elimination back-off
        typedef typename traits::random_engine elimination_random_engine;

    protected:
        typename node_type::atomic_node_ptr m_Top;  ///< Top of the stack
        item_counter        m_ItemCounter;          ///< Item counter
        stat                m_stat;                 ///< Internal statistics

        //@cond
        typedef treiber_stack::details::elimination_backoff<enable_elimination, value_type, traits> elimination_backoff;
        elimination_backoff m_Backoff;

        typedef treiber_stack::operation< value_type >  operation_desc;

        // GC and node_type::gc must be the same
        static_assert( std::is_same<gc, typename node_type::gc>::value, "GC and node_type::gc must be the same");

        static_assert( !enable_elimination || std::is_same<typename elimination_random_engine::result_type, unsigned int>::value,
                       "Random engine result type must be unsigned int");
        //@endcond

    protected:
        //@cond
        void clear_links( node_type * pNode ) noexcept
        {
            pNode->m_pNext.store( nullptr, memory_model::memory_order_relaxed );
        }

        template <bool EnableElimination>
        struct elimination_backoff_impl;
        //@endcond

    public:
        /// Constructs empty stack
        TreiberStack()
            : m_Top( nullptr )
        {}

        /// Constructs empty stack and initializes elimination back-off data
        /**
            This form should be used if you use elimination back-off with dynamically allocated collision array, i.e
            \p Traits contains <tt>typedef cds::opt::v::initialized_dynamic_buffer buffer</tt>.
            \p nCollisionCapacity parameter specifies the capacity of collision array.
        */
        TreiberStack( size_t nCollisionCapacity )
            : m_Top( nullptr )
            , m_Backoff( nCollisionCapacity )
        {}

        /// \p %TreiberStack is not copy-constructible
        TreiberStack( TreiberStack const& ) = delete;

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

            typename elimination_backoff::type bkoff = m_Backoff.init();

            operation_desc op;
            constexpr_if( enable_elimination ) {
                op.idOp = treiber_stack::op_push;
                op.pVal = &val;
            }

            node_type * t = m_Top.load( memory_model::memory_order_relaxed );
            while ( true ) {
                pNew->m_pNext.store( t, memory_model::memory_order_relaxed );
                if ( m_Top.compare_exchange_weak( t, pNew, memory_model::memory_order_release, atomics::memory_order_acquire )) {
                    ++m_ItemCounter;
                    m_stat.onPush();
                    return true;
                }
                m_stat.onPushRace();

                if ( bkoff.backoff( op, m_stat ))
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
            typename elimination_backoff::type bkoff = m_Backoff.init();
            typename gc::Guard  guard;

            operation_desc op;
            constexpr_if( enable_elimination ) {
                op.idOp = treiber_stack::op_pop;
            }

            while ( true ) {
                node_type * t = guard.protect( m_Top,
                    []( node_type * p ) -> value_type * {
                        return node_traits::to_value_ptr( p );
                    });
                if ( t == nullptr )
                    return nullptr;    // stack is empty

                node_type * pNext = t->m_pNext.load(memory_model::memory_order_relaxed);
                if ( m_Top.compare_exchange_weak( t, pNext, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                    clear_links( t );
                    --m_ItemCounter;
                    m_stat.onPop();
                    return node_traits::to_value_ptr( *t );
                }

                m_stat.onPopRace();
                if ( bkoff.backoff( op, m_stat )) {
                    // may return nullptr if stack is empty
                    return op.pVal;
                }
            }
        }

        /// Check if stack is empty
        bool empty() const
        {
            return m_Top.load( memory_model::memory_order_relaxed ) == nullptr;
        }

        /// Clear the stack
        /** @anchor cds_intrusive_TreiberStack_clear
            For each removed item the disposer is called.

            @note It is possible that after <tt>clear()</tt> the <tt>empty()</tt> returns \p false
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
                if ( m_Top.compare_exchange_weak( pTop, nullptr, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                    m_ItemCounter.reset();
                    break;
                }
                bkoff();
            }

            while( pTop ) {
                node_type * p = pTop;
                pTop = p->m_pNext.load(memory_model::memory_order_relaxed);
                clear_links( p );
                gc::template retire<disposer>( node_traits::to_value_ptr( *p ));
            }
        }

        /// Returns stack's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            @warning Even if you use real item counter and it returns 0, this fact is not mean that the stack
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

#endif  // #ifndef CDSLIB_INTRUSIVE_TREIBER_STACK_H

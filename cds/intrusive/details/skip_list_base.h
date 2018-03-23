// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_SKIP_LIST_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_SKIP_LIST_BASE_H

#include <cds/intrusive/details/base.h>
#include <cds/details/marked_ptr.h>
#include <cds/algo/bitop.h>
#include <cds/os/timer.h>
#include <cds/urcu/options.h>

namespace cds { namespace intrusive {
    /// SkipListSet related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace skip_list {
        /// The maximum possible height of any skip-list
        static unsigned int const c_nHeightLimit = 32;

        /// Skip list node
        /**
            Template parameters:
            - \p GC - garbage collector
            - \p Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <class GC, typename Tag = opt::none>
        class node
        {
        public:
            typedef GC      gc;  ///< Garbage collector
            typedef Tag     tag; ///< tag

            typedef cds::details::marked_ptr<node, 1>                     marked_ptr;        ///< marked pointer
            typedef typename gc::template atomic_marked_ptr< marked_ptr>  atomic_marked_ptr; ///< atomic marked pointer specific for GC
            //@cond
            typedef atomic_marked_ptr tower_item_type;

            //@endcond

        protected:
            //@cond
            atomic_marked_ptr           m_pNext;     ///< Next item in bottom-list (list at level 0)
            unsigned int                m_nHeight;   ///< Node height (size of \p m_arrNext array). For node at level 0 the height is 1.
            atomic_marked_ptr *         m_arrNext;   ///< Array of next items for levels 1 .. m_nHeight - 1. For node at level 0 \p m_arrNext is \p nullptr
            atomics::atomic<unsigned int> m_nUnlink; ///< Unlink helper
            //@endcond

        public:
            node()
                : m_pNext( nullptr )
                , m_nHeight( 1 )
                , m_arrNext( nullptr )
            {
                m_nUnlink.store( 1, atomics::memory_order_release );
            }


            /// Constructs a node's tower of height \p nHeight
            void make_tower( unsigned int nHeight, atomic_marked_ptr * nextTower )
            {
                assert( nHeight > 0 );
                assert( (nHeight == 1 && nextTower == nullptr)      // bottom-list node
                        || (nHeight > 1 && nextTower != nullptr)    // node at level of more than 0
                );

                m_arrNext = nextTower;
                m_nHeight = nHeight;
                m_nUnlink.store( nHeight, atomics::memory_order_release );
            }

            //@cond
            atomic_marked_ptr * release_tower()
            {
                atomic_marked_ptr * pTower = m_arrNext;
                m_arrNext = nullptr;
                m_nHeight = 1;
                return pTower;
            }

            atomic_marked_ptr * get_tower() const
            {
                return m_arrNext;
            }

            bool has_tower() const
            {
                return m_nHeight > 1;
            }
            //@endcond

            /// Access to element of next pointer array
            atomic_marked_ptr& next( unsigned int nLevel )
            {
                assert( nLevel < height());
                assert( nLevel == 0 || (nLevel > 0 && m_arrNext != nullptr));

                if ( nLevel ) {
                    // TSan: data race between m_arrNext[ nLevel - 1 ] and make_tower()
                    // In fact, m_arrNext is a const array that is never changed
                    CDS_TSAN_ANNOTATE_HAPPENS_BEFORE( &m_arrNext[ nLevel - 1 ] );
                    return m_arrNext[nLevel - 1];
                }
                return m_pNext;
            }

            /// Access to element of next pointer array (const version)
            atomic_marked_ptr const& next( unsigned int nLevel ) const
            {
                assert( nLevel < height());
                assert( nLevel == 0 || nLevel > 0 && m_arrNext != nullptr );

                if ( nLevel ) {
                    CDS_TSAN_ANNOTATE_HAPPENS_BEFORE( &m_arrNext[nLevel - 1] );
                    return m_arrNext[nLevel - 1];
                }
                return m_pNext;
            }

            /// Access to element of next pointer array (synonym for \p next() function)
            atomic_marked_ptr& operator[]( unsigned int nLevel )
            {
                return next( nLevel );
            }

            /// Access to element of next pointer array (synonym for \p next() function)
            atomic_marked_ptr const& operator[]( unsigned int nLevel ) const
            {
                return next( nLevel );
            }

            /// Height of the node
            unsigned int height() const
            {
                return m_nHeight;
            }

            /// Clears internal links
            void clear()
            {
                assert( m_arrNext == nullptr );
                m_pNext.store( marked_ptr(), atomics::memory_order_release );
            }

            //@cond
            bool is_cleared() const
            {
                return m_pNext == atomic_marked_ptr()
                    && m_arrNext == nullptr
                    && m_nHeight <= 1;
            }

            bool level_unlinked( unsigned nCount = 1 )
            {
                return m_nUnlink.fetch_sub( nCount, atomics::memory_order_relaxed ) == 1;
            }

            bool is_upper_level( unsigned nLevel ) const
            {
                return m_nUnlink.load( atomics::memory_order_relaxed ) == nLevel + 1;
            }
            //@endcond
        };

        //@cond
        struct undefined_gc;
        struct default_hook {
            typedef undefined_gc    gc;
            typedef opt::none       tag;
        };
        //@endcond

        //@cond
        template < typename HookType, typename... Options>
        struct hook
        {
            typedef typename opt::make_options< default_hook, Options...>::type  options;
            typedef typename options::gc    gc;
            typedef typename options::tag   tag;
            typedef node<gc, tag>           node_type;
            typedef HookType                hook_type;
        };
        //@endcond

        /// Base hook
        /**
            \p Options are:
            - \p opt::gc - garbage collector
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < typename... Options >
        struct base_hook: public hook< opt::base_hook_tag, Options... >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - \p opt::gc - garbage collector
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < size_t MemberOffset, typename... Options >
        struct member_hook: public hook< opt::member_hook_tag, Options... >
        {
            //@cond
            static const size_t c_nMemberOffset = MemberOffset;
            //@endcond
        };

        /// Traits hook
        /**
            \p NodeTraits defines type traits for node.
            See \ref node_traits for \p NodeTraits interface description

            \p Options are:
            - \p opt::gc - garbage collector
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename NodeTraits, typename... Options >
        struct traits_hook: public hook< opt::traits_hook_tag, Options... >
        {
            //@cond
            typedef NodeTraits node_traits;
            //@endcond
        };

        /// Option specifying random level generator
        /**
            The random level generator is an important part of skip-list algorithm.
            The node height in the skip-list have a probabilistic distribution
            where half of the nodes that have level \p i pointers also have level <tt>i+1</tt> pointers
            (i = 0..30).
            The random level generator should provide such distribution.

            The \p Type functor interface is:
            \code
            struct random_generator {
                static unsigned int const c_nUpperBound = 32;
                random_generator();
                unsigned int operator()();
            };
            \endcode

            where
            - \p c_nUpperBound - constant that specifies the upper bound of random number generated.
                The generator produces a number from range <tt>[0 .. c_nUpperBound)</tt> (upper bound excluded).
                \p c_nUpperBound must be no more than 32.
            - <tt>random_generator()</tt> - the constructor of generator object initialises the generator instance (its internal state).
            - <tt>unsigned int operator()()</tt> - the main generating function. Returns random level from range <tt>[0 .. c_nUpperBound - 1]</tt>


            Stateful generators are supported.

            Available \p Type implementations:
            - \p skip_list::xor_shift
            - \p skip_list::turbo
        */
        template <typename Type>
        struct random_level_generator {
            //@cond
            template <typename Base>
            struct pack: public Base
            {
                typedef Type random_level_generator;
            };
            //@endcond
        };

        /// Xor-shift random level generator
        /**
            The simplest of the generators described in George Marsaglia's "Xorshift RNGs" paper.
            This is not a high-quality generator but is acceptable for skip-list.

            The random generator should return numbers from range [0 .. MaxHeight - 1].

            From Doug Lea's ConcurrentSkipListMap.java.
        */
        template <unsigned MaxHeight>
        class xor_shift {
            //@cond
            atomics::atomic<unsigned int>    m_nSeed;

            static_assert( MaxHeight > 1, "MaxHeight" );
            static_assert( MaxHeight <= c_nHeightLimit, "MaxHeight is too large" );
            static unsigned int const c_nBitMask = (1u << ( MaxHeight - 1 )) - 1;
            //@endcond

        public:
            /// The upper bound of generator's return value. The generator produces random number in range <tt>[0..c_nUpperBound)</tt>
            static unsigned int const c_nUpperBound = MaxHeight;

            /// Initializes the generator instance
            xor_shift()
            {
                m_nSeed.store( (unsigned int) cds::OS::Timer::random_seed(), atomics::memory_order_relaxed );
            }

            /// Main generator function
            unsigned int operator()()
            {
                /* ConcurrentSkipListMap.java
                private int randomLevel() {
                    int x = randomSeed;
                    x ^= x << 13;
                    x ^= x >>> 17;
                    randomSeed = x ^= x << 5;
                    if ((x & 0x80000001) != 0) // test highest and lowest bits
                        return 0;
                    int level = 1;
                    while (((x >>>= 1) & 1) != 0) ++level;
                    return level;
                }
                */
                unsigned int x = m_nSeed.load( atomics::memory_order_relaxed );
                x ^= x << 13;
                x ^= x >> 17;
                x ^= x << 5;
                m_nSeed.store( x, atomics::memory_order_relaxed );
                unsigned int nLevel = ((x & 0x00000001) != 0) ? 0 : cds::bitop::LSB( (~(x >> 1)) & c_nBitMask );

                assert( nLevel < c_nUpperBound );
                return nLevel;
            }
        };

        /// Xor-shift random level generator, max height 32
        typedef xor_shift<c_nHeightLimit> xorshift32;

        //@cond
        // For backward compatibility
        typedef xorshift32 xorshift;
        //@endcond

        /// \ref xor_shift generator, max height 24
        typedef xor_shift< 24 > xorshift24;

        /// \ref xor_shift generator, max height = 16
        typedef xor_shift< 16 > xorshift16;

        /// Turbo-pascal random level generator
        /**
            This uses a cheap pseudo-random function that was used in Turbo Pascal.

            The random generator should return numbers from range [0..31].

            From Doug Lea's ConcurrentSkipListMap.java.
        */
        template <unsigned MaxHeight>
        class turbo
        {
            //@cond
            atomics::atomic<unsigned int>    m_nSeed;

            static_assert( MaxHeight > 1, "MaxHeight" );
            static_assert( MaxHeight <= c_nHeightLimit, "MaxHeight is too large" );
            static unsigned int const c_nBitMask = (1u << ( MaxHeight - 1 )) - 1;
            //@endcond
        public:
            /// The upper bound of generator's return value. The generator produces random number in range <tt>[0..c_nUpperBound)</tt>
            static unsigned int const c_nUpperBound = MaxHeight;

            /// Initializes the generator instance
            turbo()
            {
                m_nSeed.store( (unsigned int) cds::OS::Timer::random_seed(), atomics::memory_order_relaxed );
            }

            /// Main generator function
            unsigned int operator()()
            {
                /*
                private int randomLevel() {
                    int level = 0;
                    int r = randomSeed;
                    randomSeed = r * 134775813 + 1;
                    if (r < 0) {
                        while ((r <<= 1) > 0)
                            ++level;
                    }
                return level;
                }
                */
                /*
                    The low bits are apparently not very random (the original used only
                    upper 16 bits) so we traverse from highest bit down (i.e., test
                    sign), thus hardly ever use lower bits.
                */
                unsigned int x = m_nSeed.load( atomics::memory_order_relaxed ) * 134775813 + 1;
                m_nSeed.store( x, atomics::memory_order_relaxed );
                unsigned int nLevel = ( x & 0x80000000 ) ? ( c_nUpperBound - 1 - cds::bitop::MSBnz( (x & c_nBitMask ) | 1 )) : 0;

                assert( nLevel < c_nUpperBound );
                return nLevel;
            }
        };

        /// Turbo-Pascal random level generator, max height 32
        typedef turbo<c_nHeightLimit> turbo32;

        //@cond
        // For backward compatibility
        typedef turbo32 turbo_pascal;
        //@endcond

        /// Turbo-Pascal generator, max height 24
        typedef turbo< 24 > turbo24;

        /// Turbo-Pascal generator, max height 16
        typedef turbo< 16 > turbo16;

        /// \p SkipListSet internal statistics
        template <typename EventCounter = cds::atomicity::event_counter>
        struct stat {
            typedef EventCounter event_counter ; ///< Event counter type

            event_counter   m_nNodeHeightAdd[c_nHeightLimit] ; ///< Count of added node of each height
            event_counter   m_nNodeHeightDel[c_nHeightLimit] ; ///< Count of deleted node of each height
            event_counter   m_nInsertSuccess        ; ///< Count of success insertion
            event_counter   m_nInsertFailed         ; ///< Count of failed insertion
            event_counter   m_nInsertRetries        ; ///< Count of unsuccessful retries of insertion
            event_counter   m_nUpdateExist          ; ///< Count of \p update() call for existed node
            event_counter   m_nUpdateNew            ; ///< Count of \p update() call for new node
            event_counter   m_nUnlinkSuccess        ; ///< Count of successful call of \p unlink
            event_counter   m_nUnlinkFailed         ; ///< Count of failed call of \p unlink
            event_counter   m_nEraseSuccess         ; ///< Count of successful call of \p erase
            event_counter   m_nEraseFailed          ; ///< Count of failed call of \p erase
            event_counter   m_nEraseRetry           ; ///< Count of retries while erasing node
            event_counter   m_nFindFastSuccess      ; ///< Count of successful call of \p find and all derivatives (via fast-path)
            event_counter   m_nFindFastFailed       ; ///< Count of failed call of \p find and all derivatives (via fast-path)
            event_counter   m_nFindSlowSuccess      ; ///< Count of successful call of \p find and all derivatives (via slow-path)
            event_counter   m_nFindSlowFailed       ; ///< Count of failed call of \p find and all derivatives (via slow-path)
            event_counter   m_nRenewInsertPosition  ; ///< Count of renewing position events while inserting
            event_counter   m_nLogicDeleteWhileInsert; ///< Count of events "The node has been logically deleted while inserting"
            event_counter   m_nRemoveWhileInsert    ; ///< Count of evnts "The node is removing while inserting"
            event_counter   m_nFastErase            ; ///< Fast erase event counter
            event_counter   m_nFastExtract          ; ///< Fast extract event counter
            event_counter   m_nSlowErase            ; ///< Slow erase event counter
            event_counter   m_nSlowExtract          ; ///< Slow extract event counter
            event_counter   m_nExtractSuccess       ; ///< Count of successful call of \p extract
            event_counter   m_nExtractFailed        ; ///< Count of failed call of \p extract
            event_counter   m_nExtractRetries       ; ///< Count of retries of \p extract call
            event_counter   m_nExtractMinSuccess    ; ///< Count of successful call of \p extract_min
            event_counter   m_nExtractMinFailed     ; ///< Count of failed call of \p extract_min
            event_counter   m_nExtractMinRetries    ; ///< Count of retries of \p extract_min call
            event_counter   m_nExtractMaxSuccess    ; ///< Count of successful call of \p extract_max
            event_counter   m_nExtractMaxFailed     ; ///< Count of failed call of \p extract_max
            event_counter   m_nExtractMaxRetries    ; ///< Count of retries of \p extract_max call
            event_counter   m_nEraseWhileFind       ; ///< Count of erased item while searching
            event_counter   m_nExtractWhileFind     ; ///< Count of extracted item while searching (RCU only)
            event_counter   m_nMarkFailed           ; ///< Count of failed node marking (logical deletion mark)
            event_counter   m_nEraseContention      ; ///< Count of key erasing contention encountered

            //@cond
            void onAddNode( unsigned int nHeight )
            {
                assert( nHeight > 0 && nHeight <= sizeof(m_nNodeHeightAdd) / sizeof(m_nNodeHeightAdd[0]));
                ++m_nNodeHeightAdd[nHeight - 1];
            }
            void onRemoveNode( unsigned int nHeight )
            {
                assert( nHeight > 0 && nHeight <= sizeof(m_nNodeHeightDel) / sizeof(m_nNodeHeightDel[0]));
                ++m_nNodeHeightDel[nHeight - 1];
            }

            void onInsertSuccess()          { ++m_nInsertSuccess    ; }
            void onInsertFailed()           { ++m_nInsertFailed     ; }
            void onInsertRetry()            { ++m_nInsertRetries    ; }
            void onUpdateExist()            { ++m_nUpdateExist      ; }
            void onUpdateNew()              { ++m_nUpdateNew        ; }
            void onUnlinkSuccess()          { ++m_nUnlinkSuccess    ; }
            void onUnlinkFailed()           { ++m_nUnlinkFailed     ; }
            void onEraseSuccess()           { ++m_nEraseSuccess     ; }
            void onEraseFailed()            { ++m_nEraseFailed      ; }
            void onEraseRetry()             { ++m_nEraseRetry; }
            void onFindFastSuccess()        { ++m_nFindFastSuccess  ; }
            void onFindFastFailed()         { ++m_nFindFastFailed   ; }
            void onFindSlowSuccess()        { ++m_nFindSlowSuccess  ; }
            void onFindSlowFailed()         { ++m_nFindSlowFailed   ; }
            void onEraseWhileFind()         { ++m_nEraseWhileFind   ; }
            void onExtractWhileFind()       { ++m_nExtractWhileFind ; }
            void onRenewInsertPosition()    { ++m_nRenewInsertPosition; }
            void onLogicDeleteWhileInsert() { ++m_nLogicDeleteWhileInsert; }
            void onRemoveWhileInsert()      { ++m_nRemoveWhileInsert; }
            void onFastErase()              { ++m_nFastErase;         }
            void onFastExtract()            { ++m_nFastExtract;       }
            void onSlowErase()              { ++m_nSlowErase;         }
            void onSlowExtract()            { ++m_nSlowExtract;       }
            void onExtractSuccess()         { ++m_nExtractSuccess;    }
            void onExtractFailed()          { ++m_nExtractFailed;     }
            void onExtractRetry()           { ++m_nExtractRetries;    }
            void onExtractMinSuccess()      { ++m_nExtractMinSuccess; }
            void onExtractMinFailed()       { ++m_nExtractMinFailed;  }
            void onExtractMinRetry()        { ++m_nExtractMinRetries; }
            void onExtractMaxSuccess()      { ++m_nExtractMaxSuccess; }
            void onExtractMaxFailed()       { ++m_nExtractMaxFailed;  }
            void onExtractMaxRetry()        { ++m_nExtractMaxRetries; }
            void onMarkFailed()             { ++m_nMarkFailed;        }
            void onEraseContention()        { ++m_nEraseContention;   }
            //@endcond
        };

        /// \p SkipListSet empty internal statistics
        struct empty_stat {
            //@cond
            void onAddNode( unsigned int /*nHeight*/ ) const {}
            void onRemoveNode( unsigned int /*nHeight*/ ) const {}
            void onInsertSuccess()          const {}
            void onInsertFailed()           const {}
            void onInsertRetry()            const {}
            void onUpdateExist()            const {}
            void onUpdateNew()              const {}
            void onUnlinkSuccess()          const {}
            void onUnlinkFailed()           const {}
            void onEraseSuccess()           const {}
            void onEraseFailed()            const {}
            void onEraseRetry()             const {}
            void onFindFastSuccess()        const {}
            void onFindFastFailed()         const {}
            void onFindSlowSuccess()        const {}
            void onFindSlowFailed()         const {}
            void onEraseWhileFind()         const {}
            void onExtractWhileFind()       const {}
            void onRenewInsertPosition()    const {}
            void onLogicDeleteWhileInsert() const {}
            void onRemoveWhileInsert()      const {}
            void onFastErase()              const {}
            void onFastExtract()            const {}
            void onSlowErase()              const {}
            void onSlowExtract()            const {}
            void onExtractSuccess()         const {}
            void onExtractFailed()          const {}
            void onExtractRetry()           const {}
            void onExtractMinSuccess()      const {}
            void onExtractMinFailed()       const {}
            void onExtractMinRetry()        const {}
            void onExtractMaxSuccess()      const {}
            void onExtractMaxFailed()       const {}
            void onExtractMaxRetry()        const {}
            void onMarkFailed()             const {}
            void onEraseContention()        const {}
            //@endcond
        };

        //@cond
        // For internal use only!!!
        template <typename Type>
        struct internal_node_builder {
            template <typename Base>
            struct pack: public Base
            {
                typedef Type internal_node_builder;
            };
        };
        //@endcond

        /// \p SkipListSet traits
        struct traits
        {
            /// Hook used
            /**
                Possible values are: \p skip_list::base_hook, \p skip_list::member_hook, \p skip_list::traits_hook.
            */
            typedef base_hook<>       hook;

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// specifies binary predicate used for key compare.
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less;

            /// Disposer
            /**
                The functor used for dispose removed items. Default is \p opt::v::empty_disposer.
            */
            typedef opt::v::empty_disposer          disposer;

            /// Item counter
            /**
                The type for item counting feature.
                By default, item counting is disabled (\p atomicity::empty_item_counter),
                \p atomicity::item_counter or \p atomicity::cache_friendly_item_counter enables it.
            */
            typedef atomicity::empty_item_counter     item_counter;

            /// C++ memory ordering model
            /**
                List of available memory ordering see \p opt::memory_model
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// Random level generator
            /**
                The random level generator is an important part of skip-list algorithm.
                The node height in the skip-list have a probabilistic distribution
                where half of the nodes that have level \p i pointers also have level <tt>i+1</tt> pointers
                (i = 0..30). So, the height of a node is in range [0..31].

                See \p skip_list::random_level_generator option setter.
            */
            typedef turbo32 random_level_generator;

            /// Allocator
            /**
                Although the skip-list is an intrusive container,
                an allocator should be provided to maintain variable randomly-calculated height of the node
                since the node can contain up to 32 next pointers.
                The allocator specified is used to allocate an array of next pointers
                for nodes which height is more than 1.
            */
            typedef CDS_DEFAULT_ALLOCATOR           allocator;

            /// back-off strategy
            /**
                If the option is not specified, the \p cds::backoff::Default is used.
            */
            typedef cds::backoff::Default           back_off;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p skip_list::empty_stat).
                Use \p skip_list::stat to enable it.
            */
            typedef empty_stat                      stat;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_SkipListSet_rcu "RCU-based SkipListSet")
            /**
                List of available options see \p opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;

            //@cond
            // For internal use only!!!
            typedef opt::none                       internal_node_builder;
            //@endcond
        };

        /// Metafunction converting option list to \p SkipListSet traits
        /**
            \p Options are:
            - \p opt::hook - hook used. Possible values are: \p skip_list::base_hook, \p skip_list::member_hook, \p skip_list::traits_hook.
                If the option is not specified, <tt>skip_list::base_hook<></tt> and \p gc::HP is used.
            - \p opt::compare - key comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
            - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::v::empty_disposer. Due the nature
                of GC schema the disposer may be called asynchronously.
            - \p opt::item_counter - the type of item counting feature. Default is disabled, i.e. \p atomicity::empty_item_counter.
                To enable it use \p atomicity::item_counter or \p atomicity::cache_friendly_item_counter
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p skip_list::random_level_generator - random level generator. Can be \p skip_list::xor_shift,
                \p skip_list::turbo32 (the default) or user-provided one.
                See \p skip_list::random_level_generator option description for explanation.
            - \p opt::allocator - although the skip-list is an intrusive container,
                an allocator should be provided to maintain variable randomly-calculated height of the node
                since the node can contain up to 32 next pointers. The allocator option is used to allocate an array of next pointers
                for nodes which height is more than 1. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - \p opt::back_off - back-off strategy, default is \p cds::backoff::Default.
            - \p opt::stat - internal statistics. By default, it is disabled (\p skip_list::empty_stat).
                To enable it use \p skip_list::stat
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

        //@cond
        namespace details {
            template <typename Node>
            class head_node: public Node
            {
                typedef Node node_type;
                typename node_type::atomic_marked_ptr   m_Tower[skip_list::c_nHeightLimit];

            public:
                head_node( unsigned int nHeight )
                {
                    for ( size_t i = 0; i < sizeof(m_Tower) / sizeof(m_Tower[0]); ++i )
                        m_Tower[i].store( typename node_type::marked_ptr(), atomics::memory_order_relaxed );

                    node_type::make_tower( nHeight, m_Tower );
                }

                node_type * head() const
                {
                    return const_cast<node_type *>( static_cast<node_type const *>(this));
                }
            };

            template <typename NodeType, typename AtomicNodePtr, typename Alloc>
            struct intrusive_node_builder
            {
                typedef NodeType        node_type;
                typedef AtomicNodePtr   atomic_node_ptr;
                typedef Alloc           allocator_type;

                typedef cds::details::Allocator< atomic_node_ptr, allocator_type >  tower_allocator;

                template <typename RandomGen>
                static node_type * make_tower( node_type * pNode, RandomGen& gen )
                {
                    return make_tower( pNode, gen() + 1 );
                }

                static node_type * make_tower( node_type * pNode, unsigned int nHeight )
                {
                    if ( nHeight > 1 )
                        pNode->make_tower( nHeight, tower_allocator().NewArray( nHeight - 1, nullptr ));
                    return pNode;
                }

                static void dispose_tower( node_type * pNode )
                {
                    unsigned int nHeight = pNode->height();
                    if ( nHeight > 1 )
                        tower_allocator().Delete( pNode->release_tower(), nHeight );
                }

                struct node_disposer {
                    void operator()( node_type * pNode )
                    {
                        dispose_tower( pNode );
                    }
                };
            };

            // Forward declaration
            template <class GC, typename NodeTraits, typename BackOff, bool IsConst>
            class iterator;

        } // namespace details
        //@endcond

    }   // namespace skip_list

    // Forward declaration
    template <class GC, typename T, typename Traits = skip_list::traits >
    class SkipListSet;

}}   // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_SKIP_LIST_BASE_H

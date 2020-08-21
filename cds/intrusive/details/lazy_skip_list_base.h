#ifndef CDSLIB_INTRUSIVE_DETAILS_LAZY_SKIP_LIST_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_LAZY_SKIP_LIST_BASE_H

#include <cds/intrusive/details/base.h>
#include <cds/details/marked_ptr.h>
#include <cds/algo/bitop.h>
#include <cds/os/timer.h>
#include <cds/urcu/options.h>
#include <mutex>

namespace cds { namespace intrusive {
    namespace lazy_skip_list {
        static unsigned int const c_nHeightLimit = 32;

        template <typename GC, typename Lock = std::mutex, typename Tag = opt::none>
        class node
        {
        public:
            typedef GC      gc;
            typedef Lock    lock_type;
            typedef Tag     tag;

            typedef cds::details::marked_ptr<node, 1>                     marked_ptr;
            typedef typename gc::template atomic_marked_ptr< marked_ptr>  atomic_marked_ptr;
            typedef atomic_marked_ptr tower_item_type;


        protected:
            atomic_marked_ptr           m_pNext;
            unsigned int                m_nHeight;  
            atomic_marked_ptr*          m_arrNext;
            atomics::atomic<unsigned int> m_nUnlink;
            mutable lock_type m_lock;

        public:
            node()
                : m_pNext( nullptr )
                , m_nHeight( 1 )
                , m_arrNext( nullptr )
            {
                m_nUnlink.store( 1, atomics::memory_order_release );
            }


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

            atomic_marked_ptr& operator[]( unsigned int nLevel )
            {
                return next( nLevel );
            }

            atomic_marked_ptr const& operator[]( unsigned int nLevel ) const
            {
                return next( nLevel );
            }

            unsigned int height() const
            {
                return m_nHeight;
            }

            void clear()
            {
                assert( m_arrNext == nullptr );
                m_pNext.store( marked_ptr(), atomics::memory_order_release );
            }

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

            void lock() const
            {
                m_lock.lock();
            }

            void unlock() const
            {
                m_lock.unlock();
            }
        };

        struct undefined_gc;
        struct default_hook {
            typedef undefined_gc    gc;
            typedef opt::none       tag;
            typedef std::mutex      lock_type;
        };

        template < typename HookType, typename... Options>
        struct hook
        {
            typedef typename opt::make_options< default_hook, Options...>::type  options;
            typedef typename options::gc            gc;
            typedef typename options::lock_type     lock_type;
            typedef typename options::tag           tag;
            typedef node<gc, lock_type, tag>        node_type;
            typedef HookType                        hook_type;
        };

        template < typename... Options >
        struct base_hook: public hook< opt::base_hook_tag, Options... >
        {};

        template < size_t MemberOffset, typename... Options >
        struct member_hook: public hook< opt::member_hook_tag, Options... >
        {
            static const size_t c_nMemberOffset = MemberOffset;
        };

        template <typename NodeTraits, typename... Options >
        struct traits_hook: public hook< opt::traits_hook_tag, Options... >
        {
            typedef NodeTraits node_traits;
        };

        template <typename Type>
        struct random_level_generator {
            template <typename Base>
            struct pack: public Base
            {
                typedef Type random_level_generator;
            };
        };

        template <unsigned MaxHeight>
        class xor_shift {
            atomics::atomic<unsigned int>    m_nSeed;

            static_assert( MaxHeight > 1, "MaxHeight" );
            static_assert( MaxHeight <= c_nHeightLimit, "MaxHeight is too large" );
            static unsigned int const c_nBitMask = (1u << ( MaxHeight - 1 )) - 1;

        public:
            static unsigned int const c_nUpperBound = MaxHeight;

            xor_shift()
            {
                m_nSeed.store( (unsigned int) cds::OS::Timer::random_seed(), atomics::memory_order_relaxed );
            }

            unsigned int operator()()
            {
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

        typedef xor_shift<c_nHeightLimit> xorshift32;

        // For backward compatibility
        typedef xorshift32 xorshift;

        typedef xor_shift< 24 > xorshift24;

        typedef xor_shift< 16 > xorshift16;

        template <unsigned MaxHeight>
        class turbo
        {
            atomics::atomic<unsigned int>    m_nSeed;

            static_assert( MaxHeight > 1, "MaxHeight" );
            static_assert( MaxHeight <= c_nHeightLimit, "MaxHeight is too large" );
            static unsigned int const c_nBitMask = (1u << ( MaxHeight - 1 )) - 1;
        public:
            static unsigned int const c_nUpperBound = MaxHeight;

            turbo()
            {
                m_nSeed.store( (unsigned int) cds::OS::Timer::random_seed(), atomics::memory_order_relaxed );
            }

            unsigned int operator()()
            {
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

        typedef turbo<c_nHeightLimit> turbo32;

        // For backward compatibility
        typedef turbo32 turbo_pascal;

        typedef turbo< 24 > turbo24;

        typedef turbo< 16 > turbo16;

        template <typename EventCounter = cds::atomicity::event_counter>
        struct stat {
            typedef EventCounter event_counter ; 

            event_counter   m_nNodeHeightAdd[c_nHeightLimit] ;
            event_counter   m_nNodeHeightDel[c_nHeightLimit] ;
            event_counter   m_nInsertSuccess        ;
            event_counter   m_nInsertFailed         ;
            event_counter   m_nInsertRetries        ;
            event_counter   m_nUpdateExist          ;
            event_counter   m_nUpdateNew            ;
            event_counter   m_nUnlinkSuccess        ;
            event_counter   m_nUnlinkFailed         ;
            event_counter   m_nEraseSuccess         ;
            event_counter   m_nEraseFailed          ;
            event_counter   m_nEraseRetry           ;
            event_counter   m_nFindFastSuccess      ;
            event_counter   m_nFindFastFailed       ;
            event_counter   m_nFindSlowSuccess      ;
            event_counter   m_nFindSlowFailed       ;
            event_counter   m_nRenewInsertPosition  ;
            event_counter   m_nLogicDeleteWhileInsert;
            event_counter   m_nRemoveWhileInsert    ;
            event_counter   m_nFastErase            ;
            event_counter   m_nFastExtract          ;
            event_counter   m_nSlowErase            ;
            event_counter   m_nSlowExtract          ;
            event_counter   m_nExtractSuccess       ;
            event_counter   m_nExtractFailed        ;
            event_counter   m_nExtractRetries       ;
            event_counter   m_nExtractMinSuccess    ;
            event_counter   m_nExtractMinFailed     ;
            event_counter   m_nExtractMinRetries    ;
            event_counter   m_nExtractMaxSuccess    ;
            event_counter   m_nExtractMaxFailed     ;
            event_counter   m_nExtractMaxRetries    ;
            event_counter   m_nEraseWhileFind       ;
            event_counter   m_nExtractWhileFind     ;
            event_counter   m_nMarkFailed           ;
            event_counter   m_nEraseContention      ;

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
        };

        struct empty_stat {
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
        };

        // For internal use only!!!
        template <typename Type>
        struct internal_node_builder {
            template <typename Base>
            struct pack: public Base
            {
                typedef Type internal_node_builder;
            };
        };

        struct traits
        {
            typedef base_hook<>       hook;

            typedef opt::none                       compare;

            typedef opt::none                       less;

            typedef opt::v::empty_disposer          disposer;

            typedef atomicity::empty_item_counter     item_counter;

            typedef opt::v::relaxed_ordering        memory_model;

            typedef turbo32 random_level_generator;

            typedef CDS_DEFAULT_ALLOCATOR           allocator;

            typedef cds::backoff::Default           back_off;

            typedef empty_stat                      stat;

            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;

            // For internal use only!!!
            typedef opt::none                       internal_node_builder;
        };

        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#   endif
        };

        namespace details {
            template <typename Node>
            class head_node: public Node
            {
                typedef Node node_type;
                typename node_type::atomic_marked_ptr   m_Tower[lazy_skip_list::c_nHeightLimit];

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

    }   // namespace lazy_skip_list

    // Forward declaration
    template <typename GC, typename T, typename Traits = lazy_skip_list::traits >
    class LazySkipListSet;

}}   // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_LAZY_SKIP_LIST_BASE_H

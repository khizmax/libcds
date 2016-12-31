/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDSLIB_GC_DETAILS_DHP_H
#define CDSLIB_GC_DETAILS_DHP_H

#include <mutex>        // unique_lock
#include <cds/algo/atomic.h>
#include <cds/algo/int_algo.h>
#include <cds/gc/details/retired_ptr.h>
#include <cds/details/aligned_allocator.h>
#include <cds/details/allocator.h>
#include <cds/sync/spinlock.h>

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning(push)
#   pragma warning(disable:4251)    // C4251: 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif

//@cond
namespace cds { namespace gc {

    /// Dynamic Hazard Pointer reclamation schema
    /**
        The cds::gc::dhp namespace and its members are internal representation of the GC and should not be used directly.
        Use cds::gc::DHP class in your code.

        Dynamic Hazard Pointer (DHP) garbage collector is a singleton. The main user-level part of DHP schema is
        GC class and its nested classes. Before use any DHP-related class you must initialize DHP garbage collector
        by contructing cds::gc::DHP object in beginning of your main().
        See cds::gc::DHP class for explanation.

        \par Implementation issues
            The global list of free guards (\p cds::gc::dhp::details::guard_allocator) is protected by a spin-lock (i.e. serialized).
            It seems that this solution should not introduce significant performance bottleneck, because each thread has its own set
            of guards allocated from the global list of free guards and the access to the global list is occurred only when
            all thread's guard is busy. In this case the thread allocates a next block of guards from the global list.
            Guards allocated for the thread is push back to the global list only when the thread terminates.
    */
    namespace dhp {

        // Forward declarations
        class Guard;
        template <size_t Count> class GuardArray;
        class ThreadGC;
        class GarbageCollector;

        /// Retired pointer type
        typedef cds::gc::details::retired_ptr retired_ptr;

        using cds::gc::details::free_retired_ptr_func;

        /// Details of Dynamic Hazard Pointer algorithm
        namespace details {

            // Forward declaration
            class liberate_set;

            /// Retired pointer buffer node
            struct retired_ptr_node {
                retired_ptr         m_ptr   ;   ///< retired pointer
                atomics::atomic<retired_ptr_node *>  m_pNext     ;   ///< next retired pointer in buffer
                atomics::atomic<retired_ptr_node *>  m_pNextFree ;   ///< next item in free list of \p retired_ptr_node
            };

            /// Internal guard representation
            struct guard_data {
                typedef void * guarded_ptr;  ///< type of value guarded

                atomics::atomic<guarded_ptr>  pPost;       ///< pointer guarded
                atomics::atomic<guard_data *> pGlobalNext; ///< next item of global list of allocated guards
                atomics::atomic<guard_data *> pNextFree;   ///< pointer to the next item in global or thread-local free-list

                guard_data * pThreadNext; ///< next item of thread's local list of guards

                guard_data() CDS_NOEXCEPT
                    : pPost( nullptr )
                    , pGlobalNext( nullptr )
                    , pNextFree( nullptr )
                    , pThreadNext( nullptr )
                {}

                void init() CDS_NOEXCEPT
                {
                    pPost.store( nullptr, atomics::memory_order_relaxed );
                }

                /// Checks if the guard is free, that is, it does not contain any pointer guarded
                bool isFree() const CDS_NOEXCEPT
                {
                    return pPost.load( atomics::memory_order_acquire ) == nullptr;
                }

                guarded_ptr get( atomics::memory_order order = atomics::memory_order_acquire )
                {
                    return pPost.load( order );
                }

                void set( guarded_ptr p, atomics::memory_order order = atomics::memory_order_release )
                {
                    pPost.store( p, order );
                }
            };

            /// Guard allocator
            template <class Alloc = CDS_DEFAULT_ALLOCATOR>
            class guard_allocator
            {
                cds::details::Allocator<details::guard_data>  m_GuardAllocator;   ///< guard allocator

                atomics::atomic<guard_data *>  m_GuardList;     ///< Head of allocated guard list (linked by guard_data::pGlobalNext field)
                atomics::atomic<guard_data *>  m_FreeGuardList; ///< Head of free guard list (linked by guard_data::pNextFree field)
                cds::sync::spin                m_freeListLock;  ///< Access to m_FreeGuardList

                /*
                    Unfortunately, access to the list of free guard is lock-based.
                    Lock-free manipulations with guard free-list are ABA-prone.
                    TODO: working with m_FreeGuardList in lock-free manner.
                */

            private:
                /// Allocates new guard from the heap. The function uses aligned allocator
                guard_data * allocNew()
                {
                    //TODO: the allocator should make block allocation

                    details::guard_data * pGuard = m_GuardAllocator.New();

                    // Link guard to the list
                    // m_GuardList is an accumulating list and it cannot support concurrent deletion,
                    // so, ABA problem is impossible for it
                    details::guard_data * pHead = m_GuardList.load( atomics::memory_order_acquire );
                    do {
                        pGuard->pGlobalNext.store( pHead, atomics::memory_order_relaxed );
                        // pHead is changed by compare_exchange_weak
                    } while ( !m_GuardList.compare_exchange_weak( pHead, pGuard, atomics::memory_order_acq_rel, atomics::memory_order_acquire ));

                    pGuard->init();
                    return pGuard;
                }

            public:
                // Default ctor
                guard_allocator() CDS_NOEXCEPT
                    : m_GuardList( nullptr )
                    , m_FreeGuardList( nullptr )
                {}

                // Destructor
                ~guard_allocator()
                {
                    guard_data * pNext;
                    for ( guard_data * pData = m_GuardList.load( atomics::memory_order_relaxed ); pData != nullptr; pData = pNext ) {
                        pNext = pData->pGlobalNext.load( atomics::memory_order_relaxed );
                        m_GuardAllocator.Delete( pData );
                    }
                }

                /// Allocates a guard from free list or from heap if free list is empty
                guard_data* alloc()
                {
                    // Try to pop a guard from free-list
                    details::guard_data * pGuard;

                    {
                        std::unique_lock<cds::sync::spin> al( m_freeListLock );
                        pGuard = m_FreeGuardList.load(atomics::memory_order_relaxed);
                        if ( pGuard )
                            m_FreeGuardList.store( pGuard->pNextFree.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed );
                    }
                    if ( !pGuard )
                        return allocNew();

                    pGuard->init();
                    return pGuard;
                }

                /// Frees guard \p pGuard
                /**
                    The function places the guard \p pGuard into free-list
                */
                void free( guard_data* pGuard ) CDS_NOEXCEPT
                {
                    pGuard->pPost.store( nullptr, atomics::memory_order_relaxed );

                    std::unique_lock<cds::sync::spin> al( m_freeListLock );
                    pGuard->pNextFree.store( m_FreeGuardList.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed );
                    m_FreeGuardList.store( pGuard, atomics::memory_order_relaxed );
                }

                /// Allocates list of guard
                /**
                    The list returned is linked by guard's \p pThreadNext and \p pNextFree fields.

                    cds::gc::dhp::ThreadGC supporting method
                */
                guard_data * allocList( size_t nCount )
                {
                    assert( nCount != 0 );

                    guard_data * pHead;
                    guard_data * pLast;

                    pHead =
                        pLast = alloc();

                    // The guard list allocated is private for the thread,
                    // so, we can use relaxed memory order
                    while ( --nCount ) {
                        guard_data * p = alloc();
                        pLast->pNextFree.store( pLast->pThreadNext = p, atomics::memory_order_relaxed );
                        pLast = p;
                    }

                    pLast->pNextFree.store( pLast->pThreadNext = nullptr, atomics::memory_order_relaxed );

                    return pHead;
                }

                /// Frees list of guards
                /**
                    The list \p pList is linked by guard's \p pThreadNext field.

                    cds::gc::dhp::ThreadGC supporting method
                */
                void freeList( guard_data * pList ) CDS_NOEXCEPT
                {
                    assert( pList != nullptr );

                    guard_data * pLast = pList;
                    while ( pLast->pThreadNext ) {
                        pLast->pPost.store( nullptr, atomics::memory_order_relaxed );
                        guard_data * p;
                        pLast->pNextFree.store( p = pLast->pThreadNext, atomics::memory_order_relaxed );
                        pLast = p;
                    }

                    std::unique_lock<cds::sync::spin> al( m_freeListLock );
                    pLast->pNextFree.store( m_FreeGuardList.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed );
                    m_FreeGuardList.store( pList, atomics::memory_order_relaxed );
                }

                /// Returns the list's head of guards allocated
                guard_data * begin() CDS_NOEXCEPT
                {
                    return m_GuardList.load(atomics::memory_order_acquire);
                }
            };

            /// Retired pointer buffer
            /**
                The buffer of retired nodes ready for liberating.
                When size of buffer exceeds a threshold the GC calls \p scan() procedure to free
                retired nodes.
            */
            class retired_ptr_buffer
            {
                atomics::atomic<retired_ptr_node *>  m_pHead     ;   ///< head of buffer
                atomics::atomic<size_t>              m_nItemCount;   ///< buffer's item count

            public:
                retired_ptr_buffer() CDS_NOEXCEPT
                    : m_pHead( nullptr )
                    , m_nItemCount(0)
                {}

                ~retired_ptr_buffer() CDS_NOEXCEPT
                {
                    assert( m_pHead.load( atomics::memory_order_relaxed ) == nullptr );
                }

                /// Pushes new node into the buffer. Returns current buffer size
                size_t push( retired_ptr_node& node ) CDS_NOEXCEPT
                {
                    retired_ptr_node * pHead = m_pHead.load(atomics::memory_order_acquire);
                    do {
                        node.m_pNext.store( pHead, atomics::memory_order_relaxed );
                        // pHead is changed by compare_exchange_weak
                    } while ( !m_pHead.compare_exchange_weak( pHead, &node, atomics::memory_order_release, atomics::memory_order_acquire ));

                    return m_nItemCount.fetch_add( 1, atomics::memory_order_relaxed ) + 1;
                }

                /// Pushes [pFirst, pLast] list linked by pNext field.
                size_t push_list( retired_ptr_node* pFirst, retired_ptr_node* pLast, size_t nSize )
                {
                    assert( pFirst );
                    assert( pLast );

                    retired_ptr_node * pHead = m_pHead.load( atomics::memory_order_acquire );
                    do {
                        pLast->m_pNext.store( pHead, atomics::memory_order_relaxed );
                        // pHead is changed by compare_exchange_weak
                    } while ( !m_pHead.compare_exchange_weak( pHead, pFirst, atomics::memory_order_release, atomics::memory_order_acquire ));

                    return m_nItemCount.fetch_add( nSize, atomics::memory_order_relaxed ) + 1;
                }

                /// Result of \ref dhp_gc_privatize "privatize" function.
                /**
                    The \p privatize function returns retired node list as \p first and the size of that list as \p second.
                */
                typedef std::pair<retired_ptr_node *, size_t> privatize_result;

                /// Gets current list of retired pointer and clears the list
                /**@anchor dhp_gc_privatize
                */
                privatize_result privatize() CDS_NOEXCEPT
                {
                    privatize_result res;

                    // Item counter is needed only as a threshold for \p scan() function
                    // So, we may clear the item counter without synchronization with m_pHead
                    res.second = m_nItemCount.exchange( 0, atomics::memory_order_relaxed );
                    res.first = m_pHead.exchange( nullptr, atomics::memory_order_acq_rel );
                    return res;
                }

                /// Returns current size of buffer (approximate)
                size_t size() const CDS_NOEXCEPT
                {
                    return m_nItemCount.load(atomics::memory_order_relaxed);
                }
            };

            /// Pool of retired pointers
            /**
                The class acts as an allocator of retired node.
                Retired pointers are linked in the lock-free list.
            */
            template <class Alloc = CDS_DEFAULT_ALLOCATOR>
            class retired_ptr_pool {
                /// Pool item
                typedef retired_ptr_node    item;

                /// Count of items in block
                static const size_t m_nItemPerBlock = 1024 / sizeof(item) - 1;

                /// Pool block
                struct block {
                    atomics::atomic<block *> pNext;     ///< next block
                    item        items[m_nItemPerBlock]; ///< item array
                };

                atomics::atomic<block *> m_pBlockListHead;   ///< head of of allocated block list

                // To solve ABA problem we use epoch-based approach
                unsigned int const m_nEpochBitmask;             ///< Epoch bitmask (log2( m_nEpochCount))
                atomics::atomic<unsigned int> m_nCurEpoch;      ///< Current epoch
                atomics::atomic<item *>* m_pEpochFree;          ///< List of free item per epoch
                atomics::atomic<item *>  m_pGlobalFreeHead;     ///< Head of unallocated item list

                typedef cds::details::Allocator< block, Alloc > block_allocator;
                typedef cds::details::Allocator< atomics::atomic<item *>, Alloc > epoch_array_alloc;

            private:
                void allocNewBlock()
                {
                    // allocate new block
                    block * pNew = block_allocator().New();

                    // link items within the block
                    item * pLastItem = pNew->items + m_nItemPerBlock - 1;
                    for ( item * pItem = pNew->items; pItem != pLastItem; ++pItem ) {
                        pItem->m_pNextFree.store( pItem + 1, atomics::memory_order_release );
                        CDS_STRICT_DO( pItem->m_pNext.store( nullptr, atomics::memory_order_relaxed ));
                    }

                    // links new block to the block list
                    {
                        block * pHead = m_pBlockListHead.load(atomics::memory_order_relaxed);
                        do {
                            pNew->pNext.store( pHead, atomics::memory_order_relaxed );
                            // pHead is changed by compare_exchange_weak
                        } while ( !m_pBlockListHead.compare_exchange_weak( pHead, pNew, atomics::memory_order_release, atomics::memory_order_acquire ));
                    }

                    // links block's items to the free list
                    {
                        item * pHead = m_pGlobalFreeHead.load(atomics::memory_order_relaxed);
                        do {
                            pLastItem->m_pNextFree.store( pHead, atomics::memory_order_release );
                            // pHead is changed by compare_exchange_weak
                        } while ( !m_pGlobalFreeHead.compare_exchange_weak( pHead, pNew->items, atomics::memory_order_release, atomics::memory_order_acquire ));
                    }
                }

                unsigned int current_epoch() const CDS_NOEXCEPT
                {
                    return m_nCurEpoch.load(atomics::memory_order_acquire) & m_nEpochBitmask;
                }

                unsigned int next_epoch() const CDS_NOEXCEPT
                {
                    return (m_nCurEpoch.load(atomics::memory_order_acquire) - 1) & m_nEpochBitmask;
                }

            public:
                retired_ptr_pool( unsigned int nEpochCount = 8 )
                    : m_pBlockListHead( nullptr )
                    , m_nEpochBitmask( static_cast<unsigned int>(beans::ceil2(nEpochCount)) - 1 )
                    , m_nCurEpoch(0)
                    , m_pEpochFree( epoch_array_alloc().NewArray( m_nEpochBitmask + 1))
                    , m_pGlobalFreeHead( nullptr )
                {


                    for (unsigned int i = 0; i <= m_nEpochBitmask; ++i )
                        m_pEpochFree[i].store( nullptr, atomics::memory_order_relaxed );

                    allocNewBlock();
                }

                ~retired_ptr_pool()
                {
                    block_allocator a;
                    block * p;
                    for ( block * pBlock = m_pBlockListHead.load(atomics::memory_order_relaxed); pBlock; pBlock = p ) {
                        p = pBlock->pNext.load( atomics::memory_order_relaxed );
                        a.Delete( pBlock );
                    }

                    epoch_array_alloc().Delete( m_pEpochFree, m_nEpochBitmask + 1 );
                }

                /// Increments current epoch
                void inc_epoch() CDS_NOEXCEPT
                {
                    m_nCurEpoch.fetch_add( 1, atomics::memory_order_acq_rel );
                }

                /// Allocates the new retired pointer
                retired_ptr_node&  alloc()
                {
                    unsigned int nEpoch;
                    item * pItem;
                    for (;;) {
                        pItem = m_pEpochFree[ nEpoch = current_epoch() ].load(atomics::memory_order_acquire);
                        if ( !pItem )
                            goto retry;
                        if ( m_pEpochFree[nEpoch].compare_exchange_weak( pItem,
                                                                         pItem->m_pNextFree.load(atomics::memory_order_acquire),
                                                                         atomics::memory_order_acquire, atomics::memory_order_relaxed ))
                        {
                            goto success;
                        }
                    }

                    // Epoch free list is empty
                    // Alloc from global free list
                retry:
                    pItem = m_pGlobalFreeHead.load( atomics::memory_order_relaxed );
                    do {
                        if ( !pItem ) {
                            allocNewBlock();
                            goto retry;
                        }
                        // pItem is changed by compare_exchange_weak
                    } while ( !m_pGlobalFreeHead.compare_exchange_weak( pItem,
                                                                        pItem->m_pNextFree.load(atomics::memory_order_acquire),
                                                                        atomics::memory_order_acquire, atomics::memory_order_acquire ));

                success:
                    CDS_STRICT_DO( pItem->m_pNextFree.store( nullptr, atomics::memory_order_relaxed ));
                    return *pItem;
                }

                /// Allocates and initializes new retired pointer
                retired_ptr_node& alloc( const retired_ptr& p )
                {
                    retired_ptr_node& node = alloc();
                    node.m_ptr = p;
                    return node;
                }

                /// Places the list [pHead, pTail] of retired pointers to pool (frees retired pointers)
                /**
                    The list is linked on the m_pNextFree field
                */
                void free_range( retired_ptr_node * pHead, retired_ptr_node * pTail ) CDS_NOEXCEPT
                {
                    assert( pHead != nullptr );
                    assert( pTail != nullptr );

                    unsigned int nEpoch;
                    item * pCurHead;
                    do {
                        pCurHead = m_pEpochFree[nEpoch = next_epoch()].load(atomics::memory_order_acquire);
                        pTail->m_pNextFree.store( pCurHead, atomics::memory_order_release );
                    } while ( !m_pEpochFree[nEpoch].compare_exchange_weak( pCurHead, pHead, atomics::memory_order_release, atomics::memory_order_relaxed ));
                }
            };
        } // namespace details

        /// Memory manager (Garbage collector)
        class CDS_EXPORT_API GarbageCollector
        {
        private:
            friend class ThreadGC;

            /// Internal GC statistics
            struct internal_stat
            {
                atomics::atomic<size_t>  m_nGuardCount       ;   ///< Total guard count
                atomics::atomic<size_t>  m_nFreeGuardCount   ;   ///< Count of free guard

                internal_stat()
                    : m_nGuardCount(0)
                    , m_nFreeGuardCount(0)
                {}
            };

        public:
            /// Exception "No GarbageCollector object is created"
            class not_initialized : public std::runtime_error
            {
            public:
                //@cond
                not_initialized()
                    : std::runtime_error( "Global DHP GarbageCollector is not initialized" )
                {}
                //@endcond
            };

            /// Internal GC statistics
            struct InternalState
            {
                size_t m_nGuardCount       ;   ///< Total guard count
                size_t m_nFreeGuardCount   ;   ///< Count of free guard

                //@cond
                InternalState()
                    : m_nGuardCount(0)
                    , m_nFreeGuardCount(0)
                {}

                InternalState& operator =( internal_stat const& s )
                {
                    m_nGuardCount = s.m_nGuardCount.load(atomics::memory_order_relaxed);
                    m_nFreeGuardCount = s.m_nFreeGuardCount.load(atomics::memory_order_relaxed);

                    return *this;
                }
                //@endcond
            };

        private:
            static GarbageCollector * m_pManager    ;   ///< GC global instance

            atomics::atomic<size_t>  m_nLiberateThreshold;   ///< Max size of retired pointer buffer to call \p scan()
            const size_t             m_nInitialThreadGuardCount; ///< Initial count of guards allocated for ThreadGC

            details::guard_allocator<>      m_GuardPool         ;   ///< Guard pool
            details::retired_ptr_pool<>     m_RetiredAllocator  ;   ///< Pool of free retired pointers
            details::retired_ptr_buffer     m_RetiredBuffer     ;   ///< Retired pointer buffer for liberating

            internal_stat   m_stat  ;   ///< Internal statistics
            bool            m_bStatEnabled  ;   ///< Internal Statistics enabled

        public:
            /// Initializes DHP memory manager singleton
            /**
                This member function creates and initializes DHP global object.
                The function should be called before using CDS data structure based on cds::gc::DHP GC. Usually,
                this member function is called in the \p main() function. See cds::gc::dhp for example.
                After calling of this function you may use CDS data structures based on cds::gc::DHP.

                \par Parameters
                - \p nLiberateThreshold - \p scan() threshold. When count of retired pointers reaches this value,
                    the \ref dhp_gc_liberate "scan()" member function would be called for freeing retired pointers.
                    If \p nLiberateThreshold <= 1, \p scan() would called after each \ref dhp_gc_retirePtr "retirePtr" call.
                - \p nInitialThreadGuardCount - initial count of guard allocated for ThreadGC. When a thread
                    is initialized the GC allocates local guard pool for the thread from common guard pool.
                    By perforce the local thread's guard pool is grown automatically from common pool.
                    When the thread terminated its guard pool is backed to common GC's pool.
                - \p nEpochCount: internally, DHP memory manager uses epoch-based schema to solve
                    ABA problem for internal data. \p nEpochCount specifies the epoch count,
                    i.e. the count of simultaneously working threads that remove the elements
                    of DHP-based concurrent data structure. Default value is 16.
            */
            static void CDS_STDCALL Construct(
                size_t nLiberateThreshold = 1024
                , size_t nInitialThreadGuardCount = 8
                , size_t nEpochCount = 16
            );

            /// Destroys DHP memory manager
            /**
                The member function destroys DHP global object. After calling of this function you may \b NOT
                use CDS data structures based on cds::gc::DHP. Usually, the \p Destruct function is called
                at the end of your \p main(). See cds::gc::dhp for example.
            */
            static void CDS_STDCALL Destruct();

            /// Returns pointer to GarbageCollector instance
            /**
                If DHP GC is not initialized, \p not_initialized exception is thrown
            */
            static GarbageCollector&   instance()
            {
                if ( m_pManager == nullptr )
                    throw not_initialized();
                return *m_pManager;
            }

            /// Checks if global GC object is constructed and may be used
            static bool isUsed() CDS_NOEXCEPT
            {
                return m_pManager != nullptr;
            }

        public:
            //@{
            /// Internal interface

            /// Allocates a guard
            details::guard_data * allocGuard()
            {
                return m_GuardPool.alloc();
            }

            /// Frees guard \p g for reusing in future
            void freeGuard(details::guard_data * pGuard )
            {
                m_GuardPool.free( pGuard );
            }

            /// Allocates guard list for a thread.
            details::guard_data* allocGuardList( size_t nCount )
            {
                return m_GuardPool.allocList( nCount );
            }

            /// Frees thread's guard list pointed by \p pList
            void freeGuardList( details::guard_data * pList )
            {
                m_GuardPool.freeList( pList );
            }

            /// Places retired pointer \p and its deleter \p pFunc into thread's array of retired pointer for deferred reclamation
            /**@anchor dhp_gc_retirePtr
            */
            template <typename T>
            void retirePtr( T * p, void (* pFunc)(T *))
            {
                retirePtr( retired_ptr( reinterpret_cast<void *>( p ), reinterpret_cast<free_retired_ptr_func>( pFunc )));
            }

            /// Places retired pointer \p into thread's array of retired pointer for deferred reclamation
            void retirePtr( retired_ptr const& p )
            {
                if ( m_RetiredBuffer.push( m_RetiredAllocator.alloc(p)) >= m_nLiberateThreshold.load(atomics::memory_order_relaxed))
                    scan();
            }

        protected:
            /// Liberate function
            /** @anchor dhp_gc_liberate
                The main function of Dynamic Hazard Pointer algorithm. It tries to free retired pointers if they are not
                trapped by any guard.
            */
            void scan();
            //@}

        public:
            /// Get internal statistics
            InternalState& getInternalState(InternalState& stat) const
            {
                return stat = m_stat;
            }

            /// Checks if internal statistics enabled
            bool              isStatisticsEnabled() const
            {
                return m_bStatEnabled;
            }

            /// Enables/disables internal statistics
            bool  enableStatistics( bool bEnable )
            {
                bool bEnabled = m_bStatEnabled;
                m_bStatEnabled = bEnable;
                return bEnabled;
            }

        private:
            GarbageCollector( size_t nLiberateThreshold, size_t nInitialThreadGuardCount, size_t nEpochCount );
            ~GarbageCollector();
        };

        /// Thread GC
        /**
            To use Dynamic Hazard Pointer reclamation schema each thread object must be linked with the object of ThreadGC class
            that interacts with GarbageCollector global object. The linkage is performed by calling \ref cds_threading "cds::threading::Manager::attachThread()"
            on the start of each thread that uses DHP GC. Before terminating the thread linked to DHP GC it is necessary to call
            \ref cds_threading "cds::threading::Manager::detachThread()".

            The ThreadGC object maintains two list:
            \li Thread guard list: the list of thread-local guards (linked by \p pThreadNext field)
            \li Free guard list: the list of thread-local free guards (linked by \p pNextFree field)
            Free guard list is a subset of thread guard list.
        */
        class ThreadGC
        {
            GarbageCollector&        m_gc;      ///< reference to GC singleton
            details::guard_data *    m_pList;   ///< Local list of guards owned by the thread
            details::guard_data *    m_pFree;   ///< The list of free guard from m_pList

        public:
            /// Default constructor
            ThreadGC()
                : m_gc( GarbageCollector::instance())
                , m_pList( nullptr )
                , m_pFree( nullptr )
            {}

            /// The object is not copy-constructible
            ThreadGC( ThreadGC const& ) = delete;

            /// Dtor calls fini()
            ~ThreadGC()
            {
                fini();
            }

            /// Initialization. Repeat call is available
            void init()
            {
                if ( !m_pList ) {
                    m_pList =
                        m_pFree = m_gc.allocGuardList( m_gc.m_nInitialThreadGuardCount );
                }
            }

            /// Finalization. Repeat call is available
            void fini()
            {
                if ( m_pList ) {
                    m_gc.freeGuardList( m_pList );
                    m_pList =
                        m_pFree = nullptr;
                }
            }

        public:
            /// Allocates new guard
            dhp::details::guard_data* allocGuard()
            {
                assert( m_pList != nullptr );

                dhp::details::guard_data* ret;
                if ( cds_likely( m_pFree )) {
                    ret = m_pFree;
                    m_pFree = m_pFree->pNextFree.load( atomics::memory_order_relaxed );
                }
                else {
                    ret = m_gc.allocGuard();
                    ret->pThreadNext = m_pList;
                    m_pList = ret;
                }
                return ret;
            }

            /// Frees guard \p g
            void freeGuard( dhp::details::guard_data* g )
            {
                assert( m_pList != nullptr );
                if ( cds_likely( g )) {
                    g->pPost.store( nullptr, atomics::memory_order_relaxed );
                    g->pNextFree.store( m_pFree, atomics::memory_order_relaxed );
                    m_pFree = g;
                }
            }

            /// Guard array
            template <size_t Count>
            using guard_array = dhp::details::guard_data* [Count];

            /// Initializes guard array \p arr
            template <size_t Count>
            void allocGuard( guard_array<Count>& arr )
            {
                assert( m_pList != nullptr );
                size_t nCount = 0;

                while ( m_pFree && nCount < Count ) {
                    arr[nCount] = m_pFree;
                    m_pFree = m_pFree->pNextFree.load(atomics::memory_order_relaxed);
                    ++nCount;
                }

                while ( nCount < Count ) {
                    dhp::details::guard_data*& g = arr[nCount];
                    g = m_gc.allocGuard();
                    g->pThreadNext = m_pList;
                    m_pList = g;
                    ++nCount;
                }
            }

            /// Frees guard array \p arr
            template <size_t Count>
            void freeGuard( guard_array<Count>& arr )
            {
                assert( m_pList != nullptr );

                details::guard_data* first = nullptr;
                details::guard_data* last;
                for ( size_t i = 0; i < Count; ++i ) {
                    details::guard_data* guard = arr[i];
                    if ( cds_likely( guard )) {
                        guard->pPost.store( nullptr, atomics::memory_order_relaxed );
                        if ( first )
                            last->pNextFree.store( guard, atomics::memory_order_relaxed );
                        else
                            first = guard;
                        last = guard;
                    }
                }
                if ( first ) {
                    last->pNextFree.store( m_pFree, atomics::memory_order_relaxed );
                    m_pFree = first;
                }
            }

            /// Places retired pointer \p and its deleter \p pFunc into list of retired pointer for deferred reclamation
            template <typename T>
            void retirePtr( T * p, void (* pFunc)(T *))
            {
                m_gc.retirePtr( p, pFunc );
            }

            /// Run retiring cycle
            void scan()
            {
                m_gc.scan();
            }
        };
    }   // namespace dhp
}}  // namespace cds::gc
//@endcond

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning(pop)
#endif

#endif // #ifndef CDSLIB_GC_DETAILS_DHP_H

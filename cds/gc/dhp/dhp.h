//$$CDS-header$$

#ifndef __CDS_GC_DHP_DHP_H
#define __CDS_GC_DHP_DHP_H

#include <mutex>        // unique_lock
#include <cds/cxx11_atomic.h>
#include <cds/gc/details/retired_ptr.h>
#include <cds/details/aligned_allocator.h>
#include <cds/details/allocator.h>
#include <cds/lock/spinlock.h>

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning(push)
#   pragma warning(disable:4251)    // C4251: 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif

namespace cds { namespace gc {

    /// Pass The Buck reclamation schema
    /**
        \par Sources:
        - [2002] M. Herlihy, V. Luchangco, and M. Moir. The repeat offender problem: A mechanism for supporting
            dynamic-sized lockfree data structures. Technical Report TR-2002-112, Sun Microsystems Laboratories, 2002
        - [2002] M. Herlihy, V. Luchangco, P. Martin, and M. Moir. Dynamic-sized Lockfree Data Structures.
            Technical Report TR-2002-110, Sun Microsystems Laboratories, 2002
        - [2005] M. Herlihy, V. Luchangco, P. Martin, and M. Moir. Nonblocking Memory Management Support
            for Dynamic-Sized Data Structures. ACM Transactions on Computer Systems, Vol.23, No.2, May 2005


        The cds::gc::dhp namespace and its members are internal representation of the Pass-the-Buck GC and should not be used directly.
        Use cds::gc::PTB class in your code.

        Pass-the-Buck (PTB) garbage collector is a singleton. The main user-level part of PTB schema is
        GC class and its nested classes. Before use any PTB-related class you must initialize PTB garbage collector
        by contructing cds::gc::PTB object in beginning of your main().
        See cds::gc::PTB class for explanation.

        \par Implementation issues
            The global list of free guards (cds::gc::dhp::details::guard_allocator) is protected by spin-lock (i.e. serialized).
            It seems that solution should not introduce significant performance bottleneck, because each thread has own set
            of guards allocated from global list of free guards and access to global list is occurred only when
            all thread's guard is busy. In this case the thread allocates next block of guards from global list.
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

        /// Details of Pass the Buck algorithm
        namespace details {

            // Forward declaration
            class liberate_set;

            /// Retired pointer buffer node
            struct retired_ptr_node {
                retired_ptr         m_ptr   ;   ///< retired pointer
                retired_ptr_node *  m_pNext     ;   ///< next retired pointer in buffer
                retired_ptr_node *  m_pNextFree ;   ///< next item in free list of retired_ptr_node
            };

            /// Internal guard representation
            struct guard_data {
                typedef retired_ptr_node *      handoff_ptr ;   ///< trapped value type
                typedef void *  guarded_ptr  ;   ///< type of value guarded

                atomics::atomic<guarded_ptr>         pPost   ;   ///< pointer guarded

#if 0
                typedef cds::SpinLock   handoff_spin ;  ///< type of spin-lock for accessing to \p pHandOff field
                handoff_spin    spinHandOff     ;   ///< access to \p pHandOff field
                handoff_ptr     pHandOff        ;   ///< trapped pointer
#endif

                atomics::atomic<guard_data *>     pGlobalNext ;   ///< next item of global list of allocated guards
                atomics::atomic<guard_data *>     pNextFree   ;   ///< pointer to the next item in global or thread-local free-list

                guard_data *             pThreadNext ;   ///< next item of thread's local list of guards

                //@cond
                guard_data()
                    : pPost( nullptr )
#if 0
                    , pHandOff( nullptr )
#endif
                    , pGlobalNext( nullptr )
                    , pNextFree( nullptr )
                    , pThreadNext( nullptr )
                {}

                void init()
                {
                    pPost.store( nullptr, atomics::memory_order_relaxed );
                }
                //@endcond

                /// Checks if the guard is free, that is, it does not contain any pointer guarded
                bool isFree() const
                {
                    return pPost.load( atomics::memory_order_acquire ) == nullptr;
                }
            };

            /// Guard allocator
            template <class Alloc = CDS_DEFAULT_ALLOCATOR>
            class guard_allocator
            {
                cds::details::Allocator<details::guard_data>  m_GuardAllocator    ;   ///< guard allocator

                atomics::atomic<guard_data *>    m_GuardList ;       ///< Head of allocated guard list (linked by guard_data::pGlobalNext field)
                atomics::atomic<guard_data *>    m_FreeGuardList ;   ///< Head of free guard list (linked by guard_data::pNextFree field)
                SpinLock                m_freeListLock  ;   ///< Access to m_FreeGuardList

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
                    // m_GuardList is accumulated list and it cannot support concurrent deletion,
                    // so, ABA problem is impossible for it
                    details::guard_data * pHead = m_GuardList.load( atomics::memory_order_acquire );
                    do {
                        pGuard->pGlobalNext.store( pHead, atomics::memory_order_relaxed );
                        // pHead is changed by compare_exchange_weak
                    } while ( !m_GuardList.compare_exchange_weak( pHead, pGuard, atomics::memory_order_release, atomics::memory_order_relaxed ));

                    pGuard->init();
                    return pGuard;
                }

            public:
                // Default ctor
                guard_allocator()
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
                guard_data * alloc()
                {
                    // Try to pop a guard from free-list
                    details::guard_data * pGuard;

                    {
                        std::unique_lock<SpinLock> al( m_freeListLock );
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
                void free( guard_data * pGuard )
                {
                    pGuard->pPost.store( nullptr, atomics::memory_order_relaxed );

                    std::unique_lock<SpinLock> al( m_freeListLock );
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
                void freeList( guard_data * pList )
                {
                    assert( pList != nullptr );

                    guard_data * pLast = pList;
                    while ( pLast->pThreadNext ) {
                        pLast->pPost.store( nullptr, atomics::memory_order_relaxed );
                        guard_data * p;
                        pLast->pNextFree.store( p = pLast->pThreadNext, atomics::memory_order_relaxed );
                        pLast = p;
                    }

                    std::unique_lock<SpinLock> al( m_freeListLock );
                    pLast->pNextFree.store( m_FreeGuardList.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed );
                    m_FreeGuardList.store( pList, atomics::memory_order_relaxed );
                }

                /// Returns the list's head of guards allocated
                guard_data * begin()
                {
                    return m_GuardList.load(atomics::memory_order_acquire);
                }
            };

            /// Retired pointer buffer
            /**
                The buffer of retired nodes ready for liberating.
                When size of buffer exceeds a threshold the GC calls \p liberate procedure to free
                retired nodes.
            */
            class retired_ptr_buffer
            {
                atomics::atomic<retired_ptr_node *>  m_pHead     ;   ///< head of buffer
                atomics::atomic<size_t>              m_nItemCount;   ///< buffer's item count

            public:
                //@cond
                retired_ptr_buffer()
                    : m_pHead( nullptr )
                    , m_nItemCount(0)
                {}

                ~retired_ptr_buffer()
                {
                    assert( m_pHead.load( atomics::memory_order_relaxed ) == nullptr );
                }
                //@endcond

                /// Pushes new node into the buffer. Returns current buffer size
                size_t push( retired_ptr_node& node )
                {
                    retired_ptr_node * pHead = m_pHead.load(atomics::memory_order_acquire);
                    do {
                        node.m_pNext = pHead;
                        // pHead is changed by compare_exchange_weak
                    } while ( !m_pHead.compare_exchange_weak( pHead, &node, atomics::memory_order_release, atomics::memory_order_relaxed ));

                    return m_nItemCount.fetch_add( 1, atomics::memory_order_relaxed ) + 1;
                }

                /// Result of \ref ptb_gc_privatve "privatize" function.
                /**
                    The \p privatize function returns retired node list as \p first and the size of that list as \p second.
                */
                typedef std::pair<retired_ptr_node *, size_t> privatize_result;

                /// Gets current list of retired pointer and clears the list
                /**@anchor ptb_gc_privatve
                */
                privatize_result privatize()
                {
                    privatize_result res;
                    res.first = m_pHead.exchange( nullptr, atomics::memory_order_acq_rel );

                    // Item counter is needed only as a threshold for liberate function
                    // So, we may clear the item counter without synchronization with m_pHead
                    res.second = m_nItemCount.exchange( 0, atomics::memory_order_relaxed );
                    return res;
                }

                /// Returns current size of buffer (approximate)
                size_t size() const
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
                    block *     pNext   ;   ///< next block
                    item        items[m_nItemPerBlock]  ;   ///< item array
                };

                atomics::atomic<block *> m_pBlockListHead    ;   ///< head of of allocated block list

                // To solve ABA problem we use epoch-based approach
                static const unsigned int c_nEpochCount = 4     ;   ///< Max epoch count
                atomics::atomic<unsigned int>    m_nCurEpoch ;   ///< Current epoch
                atomics::atomic<item *>  m_pEpochFree[c_nEpochCount]  ;   ///< List of free item per epoch
                atomics::atomic<item *>  m_pGlobalFreeHead   ;   ///< Head of unallocated item list

                cds::details::Allocator< block, Alloc > m_BlockAllocator    ;   ///< block allocator

            private:
                //@cond
                void allocNewBlock()
                {
                    // allocate new block
                    block * pNew = m_BlockAllocator.New();

                    // link items within the block
                    item * pLastItem = pNew->items + m_nItemPerBlock - 1;
                    for ( item * pItem = pNew->items; pItem != pLastItem; ++pItem ) {
                        pItem->m_pNextFree = pItem + 1;
                        CDS_STRICT_DO( pItem->m_pNext = nullptr );
                    }

                    // link new block to block list
                    {
                        block * pHead = m_pBlockListHead.load(atomics::memory_order_acquire);
                        do {
                            pNew->pNext = pHead;
                            // pHead is changed by compare_exchange_weak
                        } while ( !m_pBlockListHead.compare_exchange_weak( pHead, pNew, atomics::memory_order_release, atomics::memory_order_relaxed ));
                    }

                    // link block's items to free list
                    {
                        item * pHead = m_pGlobalFreeHead.load(atomics::memory_order_acquire);
                        do {
                            pLastItem->m_pNextFree = pHead;
                            // pHead is changed by compare_exchange_weak
                        } while ( !m_pGlobalFreeHead.compare_exchange_weak( pHead, pNew->items, atomics::memory_order_release, atomics::memory_order_relaxed ));
                    }
                }

                unsigned int current_epoch() const
                {
                    return m_nCurEpoch.load(atomics::memory_order_acquire) & (c_nEpochCount - 1);
                }
                unsigned int next_epoch() const
                {
                    return (m_nCurEpoch.load(atomics::memory_order_acquire) - 1) & (c_nEpochCount - 1);
                }
                //@endcond

            public:
                //@cond
                retired_ptr_pool()
                    : m_pBlockListHead( nullptr )
                    , m_nCurEpoch(0)
                    , m_pGlobalFreeHead( nullptr )
                {
                    for (unsigned int i = 0; i < sizeof(m_pEpochFree)/sizeof(m_pEpochFree[0]); ++i )
                        m_pEpochFree[i].store( nullptr, atomics::memory_order_relaxed );

                    allocNewBlock();
                }

                ~retired_ptr_pool()
                {
                    block * p;
                    for ( block * pBlock = m_pBlockListHead.load(atomics::memory_order_relaxed); pBlock; pBlock = p ) {
                        p = pBlock->pNext;
                        m_BlockAllocator.Delete( pBlock );
                    }
                }

                /// Increments current epoch
                void inc_epoch()
                {
                    m_nCurEpoch.fetch_add( 1, atomics::memory_order_acq_rel );
                }

                //@endcond

                /// Allocates new retired pointer
                retired_ptr_node&  alloc()
                {
                    unsigned int nEpoch;
                    item * pItem;
                    for (;;) {
                        pItem = m_pEpochFree[ nEpoch = current_epoch() ].load(atomics::memory_order_acquire);
                        if ( !pItem )
                            goto retry;
                        if ( m_pEpochFree[nEpoch].compare_exchange_weak( pItem, pItem->m_pNextFree, atomics::memory_order_release, atomics::memory_order_relaxed ))
                            goto success;
                    }

                    /*
                    item * pItem = m_pEpochFree[ nEpoch = current_epoch() ].load(atomics::memory_order_acquire);
                    while ( pItem ) {
                        if ( m_pEpochFree[nEpoch].compare_exchange_weak( pItem, pItem->m_pNextFree, atomics::memory_order_release, atomics::memory_order_relaxed ))
                            goto success;
                    }
                    */

                    // Epoch free list is empty
                    // Alloc from global free list
                retry:
                    pItem = m_pGlobalFreeHead.load( atomics::memory_order_acquire );
                    do {
                        if ( !pItem ) {
                            allocNewBlock();
                            goto retry;
                        }
                        // pItem is changed by compare_exchange_weak
                    } while ( !m_pGlobalFreeHead.compare_exchange_weak( pItem, pItem->m_pNextFree, atomics::memory_order_release, atomics::memory_order_relaxed ));

                success:
                    CDS_STRICT_DO( pItem->m_pNextFree = nullptr );
                    return *pItem;
                }

                /// Allocates and initializes new retired pointer
                retired_ptr_node& alloc( const retired_ptr& p )
                {
                    retired_ptr_node& node = alloc();
                    node.m_ptr = p;
                    return node;
                }

                /// Places the list (pHead, pTail) of retired pointers to pool (frees retired pointers)
                /**
                    The list is linked on the m_pNextFree field
                */
                void free_range( retired_ptr_node * pHead, retired_ptr_node * pTail )
                {
                    assert( pHead != nullptr );
                    assert( pTail != nullptr );

                    unsigned int nEpoch;
                    item * pCurHead;
                    do {
                        pCurHead = m_pEpochFree[nEpoch = next_epoch()].load(atomics::memory_order_acquire);
                        pTail->m_pNextFree = pCurHead;
                    } while ( !m_pEpochFree[nEpoch].compare_exchange_weak( pCurHead, pHead, atomics::memory_order_release, atomics::memory_order_relaxed ));
                }
            };

            /// Uninitialized guard
            class guard
            {
                friend class ThreadGC;
            protected:
                details::guard_data * m_pGuard ;    ///< Pointer to guard data
            public:
                /// Initialize empty guard.
                guard()
                    : m_pGuard( nullptr )
                {}

                /// The object is not copy-constructible
                guard( guard const& ) = delete;

                /// Object destructor, does nothing
                ~guard()
                {}

                /// Guards pointer \p p
                void set( void * p )
                {
                    assert( m_pGuard != nullptr );
                    m_pGuard->pPost.store( p, atomics::memory_order_release );
                    //CDS_COMPILER_RW_BARRIER;
                }

                /// Clears the guard
                void clear()
                {
                    assert( m_pGuard != nullptr );
                    m_pGuard->pPost.store( nullptr, atomics::memory_order_relaxed );
                    CDS_STRICT_DO( CDS_COMPILER_RW_BARRIER );
                }

                /// Guards pointer \p p
                template <typename T>
                T * operator =( T * p )
                {
                    set( reinterpret_cast<void *>( const_cast<T *>(p) ));
                    return p;
                }

                //@cond
                std::nullptr_t operator=(std::nullptr_t)
                {
                    clear();
                    return nullptr;
                }
                //@endcond

            public: // for ThreadGC.
                /*
                    GCC cannot compile code for template versions of ThreasGC::allocGuard/freeGuard,
                    the compiler produces error: ‘cds::gc::dhp::details::guard_data* cds::gc::dhp::details::guard::m_pGuard’ is protected
                    despite the fact that ThreadGC is declared as friend for guard class.
                    We should not like to declare m_pGuard member as public one.
                    Therefore, we have to add set_guard/get_guard public functions
                */
                /// Set guard data
                void set_guard( details::guard_data * pGuard )
                {
                    assert( m_pGuard == nullptr );
                    m_pGuard = pGuard;
                }

                /// Get current guard data
                details::guard_data * get_guard()
                {
                    return m_pGuard;
                }
                /// Get current guard data
                details::guard_data * get_guard() const
                {
                    return m_pGuard;
                }
            };

        } // namespace details

        /// Guard
        /**
            This class represents auto guard: ctor allocates a guard from guard pool,
            dtor returns the guard back to the pool of free guard.
        */
        class Guard: public details::guard
        {
            //@cond
            typedef details::guard    base_class;
            friend class ThreadGC;
            //@endcond

            ThreadGC&    m_gc    ;    ///< ThreadGC object of current thread
        public:
            /// Allocates a guard from \p gc GC. \p gc must be ThreadGC object of current thread
            Guard(ThreadGC& gc);

            /// Returns guard allocated back to pool of free guards
            ~Guard();    // inline after GarbageCollector

            /// Returns PTB GC object
            ThreadGC& getGC()
            {
                return m_gc;
            }

            /// Guards pointer \p p
            template <typename T>
            T * operator =( T * p )
            {
                return base_class::operator =<T>( p );
            }

            //@cond
            std::nullptr_t operator=(std::nullptr_t)
            {
                return base_class::operator =(nullptr);
            }
            //@endcond
        };

        /// Array of guards
        /**
            This class represents array of auto guards: ctor allocates \p Count guards from guard pool,
            dtor returns the guards allocated back to the pool.
        */
        template <size_t Count>
        class GuardArray
        {
            details::guard      m_arr[Count]    ;    ///< array of guard
            ThreadGC&           m_gc    ;            ///< ThreadGC object of current thread
            const static size_t c_nCapacity = Count ;   ///< Array capacity (equal to \p Count template parameter)

        public:
            /// Rebind array for other size \p OtherCount
            template <size_t OtherCount>
            struct rebind {
                typedef GuardArray<OtherCount>  other   ;   ///< rebinding result
            };

        public:
            /// Allocates array of guards from \p gc which must be the ThreadGC object of current thread
            GuardArray( ThreadGC& gc )    ;    // inline below

            /// The object is not default-constructible
            GuardArray() = delete;

            /// The object is not copy-constructible
            GuardArray( GuardArray const& ) = delete;

            /// Returns guards allocated back to pool
            ~GuardArray()    ;    // inline below

            /// Returns the capacity of array
            CDS_CONSTEXPR size_t capacity() const CDS_NOEXCEPT
            {
                return c_nCapacity;
            }

            /// Returns PTB ThreadGC object
            ThreadGC& getGC() CDS_NOEXCEPT
            {
                return m_gc;
            }

            /// Returns reference to the guard of index \p nIndex (0 <= \p nIndex < \p Count)
            details::guard& operator []( size_t nIndex )
            {
                assert( nIndex < capacity() );
                return m_arr[nIndex];
            }

            /// Returns reference to the guard of index \p nIndex (0 <= \p nIndex < \p Count) [const version]
            const details::guard& operator []( size_t nIndex ) const
            {
                assert( nIndex < capacity() );
                return m_arr[nIndex];
            }

            /// Set the guard \p nIndex. 0 <= \p nIndex < \p Count
            template <typename T>
            void set( size_t nIndex, T * p )
            {
                assert( nIndex < capacity() );
                m_arr[nIndex].set( p );
            }

            /// Clears (sets to \p nullptr) the guard \p nIndex
            void clear( size_t nIndex )
            {
                assert( nIndex < capacity() );
                m_arr[nIndex].clear();
            }

            /// Clears all guards in the array
            void clearAll()
            {
                for ( size_t i = 0; i < capacity(); ++i )
                    clear(i);
            }
        };

        /// Memory manager (Garbage collector)
        class CDS_EXPORT_API GarbageCollector
        {
        private:
            //@cond
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
            //@endcond

        public:
            /// Exception "No GarbageCollector object is created"
            CDS_DECLARE_EXCEPTION( PTBManagerEmpty, "Global PTB GarbageCollector is NULL" );

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

            details::guard_allocator<>      m_GuardPool         ;   ///< Guard pool
            details::retired_ptr_pool<>     m_RetiredAllocator  ;   ///< Pool of free retired pointers
            details::retired_ptr_buffer     m_RetiredBuffer     ;   ///< Retired pointer buffer for liberating
            //atomics::atomic<size_t>      m_nInLiberate       ;   ///< number of parallel \p liberate fnction call

            atomics::atomic<size_t>      m_nLiberateThreshold;   ///< Max size of retired pointer buffer to call liberate
            const size_t    m_nInitialThreadGuardCount; ///< Initial count of guards allocated for ThreadGC

            internal_stat   m_stat  ;   ///< Internal statistics
            bool            m_bStatEnabled  ;   ///< Internal Statistics enabled

        public:
            /// Initializes PTB memory manager singleton
            /**
                This member function creates and initializes PTB global object.
                The function should be called before using CDS data structure based on cds::gc::PTB GC. Usually,
                this member function is called in the \p main() function. See cds::gc::dhp for example.
                After calling of this function you may use CDS data structures based on cds::gc::PTB.

                \par Parameters
                \li \p nLiberateThreshold - the liberate threshold. When count of retired pointers reaches this value,
                    the \ref ptb_gc_liberate "liberate" member function would be called for freeing retired pointers.
                    If \p nLiberateThreshold <= 1, \p liberate would called after each \ref ptb_gc_retirePtr "retirePtr" call.
                \li \p nInitialThreadGuardCount - initial count of guard allocated for ThreadGC. When a thread
                    is initialized the GC allocates local guard pool for the thread from common guard pool.
                    By perforce the local thread's guard pool is grown automatically from common pool.
                    When the thread terminated its guard pool is backed to common GC's pool.

            */
            static void CDS_STDCALL Construct(
                size_t nLiberateThreshold = 1024
                , size_t nInitialThreadGuardCount = 8
            );

            /// Destroys PTB memory manager
            /**
                The member function destroys PTB global object. After calling of this function you may \b NOT
                use CDS data structures based on cds::gc::PTB. Usually, the \p Destruct function is called
                at the end of your \p main(). See cds::gc::dhp for example.
            */
            static void CDS_STDCALL Destruct();

            /// Returns pointer to GarbageCollector instance
            /**
                If PTB GC is not initialized, \p PTBManagerEmpty exception is thrown
            */
            static GarbageCollector&   instance()
            {
                if ( m_pManager == nullptr )
                    throw PTBManagerEmpty();
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
            details::guard_data * allocGuardList( size_t nCount )
            {
                return m_GuardPool.allocList( nCount );
            }

            /// Frees thread's guard list pointed by \p pList
            void freeGuardList( details::guard_data * pList )
            {
                m_GuardPool.freeList( pList );
            }

            /// Places retired pointer \p and its deleter \p pFunc into thread's array of retired pointer for deferred reclamation
            /**@anchor ptb_gc_retirePtr
            */
            template <typename T>
            void retirePtr( T * p, void (* pFunc)(T *) )
            {
                retirePtr( retired_ptr( reinterpret_cast<void *>( p ), reinterpret_cast<free_retired_ptr_func>( pFunc ) ) );
            }

            /// Places retired pointer \p into thread's array of retired pointer for deferred reclamation
            void retirePtr( retired_ptr const& p )
            {
                if ( m_RetiredBuffer.push( m_RetiredAllocator.alloc(p)) >= m_nLiberateThreshold.load(atomics::memory_order_relaxed) )
                    liberate();
            }

        protected:
            /// Liberate function
            /** @anchor ptb_gc_liberate
                The main function of Pass The Buck algorithm. It tries to free retired pointers if they are not
                trapped by any guard.
            */
            void liberate();

            //@}

        private:
            //@cond
#if 0
            void liberate( details::liberate_set& set );
#endif
            //@endcond

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
            //@cond none
            GarbageCollector( size_t nLiberateThreshold, size_t nInitialThreadGuardCount );
            ~GarbageCollector();
            //@endcond
        };

        /// Thread GC
        /**
            To use Pass The Buck reclamation schema each thread object must be linked with the object of ThreadGC class
            that interacts with GarbageCollector global object. The linkage is performed by calling \ref cds_threading "cds::threading::Manager::attachThread()"
            on the start of each thread that uses PTB GC. Before terminating the thread linked to PTB GC it is necessary to call
            \ref cds_threading "cds::threading::Manager::detachThread()".

            The ThreadGC object maintains two list:
            \li Thread guard list: the list of thread-local guards (linked by \p pThreadNext field)
            \li Free guard list: the list of thread-local free guards (linked by \p pNextFree field)
            Free guard list is a subset of thread guard list.
        */
        class ThreadGC
        {
            GarbageCollector&   m_gc    ;   ///< reference to GC singleton
            details::guard_data *    m_pList ;   ///< Local list of guards owned by the thread
            details::guard_data *    m_pFree ;   ///< The list of free guard from m_pList

        public:
            /// Default constructor
            ThreadGC()
                : m_gc( GarbageCollector::instance() )
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
            /// Initializes guard \p g
            void allocGuard( Guard& g )
            {
                assert( m_pList != nullptr );
                if ( m_pFree ) {
                    g.m_pGuard = m_pFree;
                    m_pFree = m_pFree->pNextFree.load(atomics::memory_order_relaxed);
                }
                else {
                    g.m_pGuard = m_gc.allocGuard();
                    g.m_pGuard->pThreadNext = m_pList;
                    m_pList = g.m_pGuard;
                }
            }

            /// Frees guard \p g
            void freeGuard( Guard& g )
            {
                assert( m_pList != nullptr );
                g.m_pGuard->pPost.store( nullptr, atomics::memory_order_relaxed );
                g.m_pGuard->pNextFree.store( m_pFree, atomics::memory_order_relaxed );
                m_pFree = g.m_pGuard;
            }

            /// Initializes guard array \p arr
            template <size_t Count>
            void allocGuard( GuardArray<Count>& arr )
            {
                assert( m_pList != nullptr );
                size_t nCount = 0;

                while ( m_pFree && nCount < Count ) {
                    arr[nCount].set_guard( m_pFree );
                    m_pFree = m_pFree->pNextFree.load(atomics::memory_order_relaxed);
                    ++nCount;
                }

                while ( nCount < Count ) {
                    details::guard& g = arr[nCount++];
                    g.set_guard( m_gc.allocGuard() );
                    g.get_guard()->pThreadNext = m_pList;
                    m_pList = g.get_guard();
                }
            }

            /// Frees guard array \p arr
            template <size_t Count>
            void freeGuard( GuardArray<Count>& arr )
            {
                assert( m_pList != nullptr );

                details::guard_data * pGuard;
                for ( size_t i = 0; i < Count - 1; ++i ) {
                    pGuard = arr[i].get_guard();
                    pGuard->pPost.store( nullptr, atomics::memory_order_relaxed );
                    pGuard->pNextFree.store( arr[i+1].get_guard(), atomics::memory_order_relaxed );
                }
                pGuard = arr[Count-1].get_guard();
                pGuard->pPost.store( nullptr, atomics::memory_order_relaxed );
                pGuard->pNextFree.store( m_pFree, atomics::memory_order_relaxed );
                m_pFree = arr[0].get_guard();
            }

            /// Places retired pointer \p and its deleter \p pFunc into list of retired pointer for deferred reclamation
            template <typename T>
            void retirePtr( T * p, void (* pFunc)(T *) )
            {
                m_gc.retirePtr( p, pFunc );
            }

            //@cond
            void scan()
            {
                m_gc.liberate();
            }
            //@endcond

        };

        //////////////////////////////////////////////////////////
        // Inlines

        inline Guard::Guard(ThreadGC& gc)
            : m_gc( gc )
        {
            getGC().allocGuard( *this );
        }
        inline Guard::~Guard()
        {
            getGC().freeGuard( *this );
        }

        template <size_t Count>
        inline GuardArray<Count>::GuardArray( ThreadGC& gc )
            : m_gc( gc )
        {
            getGC().allocGuard( *this );
        }
        template <size_t Count>
        inline GuardArray<Count>::~GuardArray()
        {
            getGC().freeGuard( *this );
        }

    }   // namespace dhp
}}  // namespace cds::gc

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning(pop)
#endif


#endif // #ifndef __CDS_GC_DHP_DHP_H

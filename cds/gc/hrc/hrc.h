//$$CDS-header$$

#ifndef __CDS_GC_HRC_HRC_H
#define __CDS_GC_HRC_HRC_H

/*
    Editions:
        2008.03.08    Maxim.Khiszinsky    Created
*/

#include <cds/refcounter.h>
#include <cds/lock/spinlock.h>
#include <cds/gc/exception.h>

#include <cds/gc/hrc/details/hrc_fwd.h>
#include <cds/gc/hrc/details/hrc_retired.h>

#include <cds/gc/hzp/details/hp_alloc.h>

#include <cds/details/noncopyable.h>

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning(push)
// warning C4251: 'cds::gc::hzp::GarbageCollector::m_pListHead' : class 'cds::cxx11_atomic::atomic<T>'
// needs to have dll-interface to be used by clients of class 'cds::gc::hzp::GarbageCollector'
#   pragma warning(disable: 4251)
#endif


namespace cds { namespace gc {

    // forwards
    class HRC;

    /// Gidenstam's memory reclamation schema (HRC)
    /**

    \par Sources:
        - [2006] A.Gidenstam "Algorithms for synchronization and consistency
                in concurrent system services", Chapter 5 "Lock-Free Memory Reclamation"
                Thesis for the degree of Doctor    of Philosophy
        - [2005] Anders Gidenstam, Marina Papatriantafilou and Philippas Tsigas "Allocating
                memory in a lock-free manner", Proceedings of the 13th Annual European
                Symposium on Algorithms (ESA 2005), Lecture Notes in Computer
                Science Vol. 3669, pages 229 – 242, Springer-Verlag, 2005


        The \p %cds::gc::hrc namespace and its members are internal representation of the GC and should not be used directly.
        Use \p cds::gc::HRC class in your code.

        This reclamation schema combines Michael's Hazard Pointer schema (see \p cds::gc::hzp)
        for deferred safe reclamation of unused objects and the reference counting
        for controlling lifetime of the objects.

        HRC garbage collector is a singleton. The main user-level part of HRC schema is
        GC class and its nested classes. Before use any HRC-related class you must initialize HRC garbage collector
        by contructing \p %cds::gc::HRC object in beginning of your <tt>main()</tt>.
        See \p cds::gc::HRC class for explanation.
    */
    namespace hrc {

        /// Base class for all HRC-based container's node
        /**
            This interface is placed to the separate class since in the presence of a garbage collector
            the lifetime of the node is greater than lifetime of its container.
            Reclaimed node may be physically deleted later than its container.
            So, the ContainerNode must be smarter than the usual.
        */
        class ContainerNode
        {
        protected:

            friend class GarbageCollector;
            friend class ThreadGC;
            friend class gc::HRC;

            unsigned_ref_counter        m_RC        ;    ///< reference counter
            atomics::atomic<bool>    m_bTrace    ;    ///< \p true - node is tracing by Scan
            atomics::atomic<bool>    m_bDeleted  ;    ///< \p true - node is deleted

        protected:
            //@cond
            ContainerNode() ;               // inline, see hrc_inline.h
            virtual ~ContainerNode()    ;   // inline, see hrc_inline.h
            //@endcond

        public:
            /// Returns count of reference for the node
            unsigned_ref_counter::ref_counter_type  getRefCount() const CDS_NOEXCEPT
            {
                return m_RC.value();
            }

            /// Increments the reference counter of the node
            void            incRefCount() CDS_NOEXCEPT
            {
                m_RC.inc();
            }

            /// Decrements the reference counter of the node. Returns \p true if ref counter is 0.
            bool            decRefCount() CDS_NOEXCEPT
            {
                return m_RC.dec();
            }

            /// Returns the mark whether the node is deleted
            bool            isDeleted() const CDS_NOEXCEPT
            {
                return m_bDeleted.load( atomics::memory_order_acquire );
            }

        protected:
            //@cond
            void clean( atomics::memory_order order ) CDS_NOEXCEPT
            {
                m_bDeleted.store( false, order );
                m_bTrace.store( false, order );
            }
            //@endcond

        protected:    // GC interface
            /**
                [Gidenstam 2006]: "The procedure \p CleanUpNode will make sure that all claimed references from
                the links of the given node will only point to active nodes, thus removing redundant
                passages through an arbitrary number of deleted nodes"

                The pseudocode of this method must be like following:
                \code
                void cleanUp( ThreadGC * pGC )
                    for all x where link[x] of node is reference-counted do
                retry:
                        node1 := link[x];
                        if node1 != nullptr and node1.m_bDeleted then
                            node2 := node1->link[x];
                            pGC->CASRef( this->link[x], node1, node2 );
                            pGC->releaseRef( node2 );
                            pGC->releaseRef( node1 );
                            goto retry;
                        pGC->releaseRef(node1);
                \endcode

                Be aware to use hazard pointers inside implementation of this method. cleanUp is called from
                the container's method when deleting the nodes. However, some hazard pointers may be occupied
                by container's method. You should allocate new hazard pointers inside \p cleanUp method, for example:
                \code
                    gc::hrc::AutoHPArray<2> hpArr( *pGC );
                \endcode
            */
            virtual void    cleanUp( ThreadGC * pGC ) = 0;

            /**
                [Gidenstam 2006]: "The procedure \p TerminateNode will make sure that none of the links in the
                given node will have any claim on any other node. TerminateNode is called on
                a deleted node when there are no claims from any other node or thread to the
                node"

                The pseudocode of this method must be like following:
                \code
                void terminate( ThreadGC * pGC, bool bConcurrent)
                    if !bConcurrent
                        for all this->link where link is reference-counted do
                            link := nullptr;
                    else
                        for all this->link where link is reference-counted do
                            repeat node1 := link;
                            until pGC->CASRef(link,node1,nullptr);
                \endcode
            */
            virtual void    terminate( ThreadGC * pGC, bool bConcurrent ) = 0;

        public:
            /// Method to destroy (deallocate) node. Depends on node's allocator
            //virtual void    destroy() = 0;
        };

        //@cond
        /// HRC GC implementation details
        namespace details {

            /// Hazard pointer guard
            typedef gc::hzp::details::HPGuardT<ContainerNode *>    HPGuard;

            /// Array of hazard pointers.
            /**
                This is wrapper for cds::gc::hzp::details::HPArray class
            */
#ifdef CDS_CXX11_TEMPLATE_ALIAS_SUPPORT
            template <size_t Count> using HPArray = gc::hzp::details::HPArrayT<ContainerNode *, Count >;
#else
            template <size_t Count>
            class HPArray: public gc::hzp::details::HPArrayT<ContainerNode *, Count>
            {};
#endif

            /// HP record of the thread
            /**
                This structure is single writer - multiple reader type. The writer is the thread owned the record
            */
            struct thread_descriptor {
                typedef ContainerNode * hazard_ptr       ; ///< base type of hazard pointer

                hzp::details::HPAllocator<hazard_ptr>   m_hzp           ;   ///< array of hazard pointers. Implicit \ref CDS_DEFAULT_ALLOCATOR dependence
                details::retired_vector                 m_arrRetired    ;   ///< array of retired pointers

                //@cond
                thread_descriptor( const GarbageCollector& HzpMgr ) ;    // inline
                ~thread_descriptor()
                {}
                //@endcond

                /// clear all hazard pointers
                void clear()
                {
                    m_hzp.clear();
                }
            };
        }    // namespace details
        //@endcond

        /// Gidenstam's Garbage Collector
        /**
            This GC combines Hazard Pointers (HP) reclamation method by Michael's and the well-known reference counting
            reclamation schema. The HP method is light-weight algorithm guarding local references only. Reference counting
            schema is harder than HP with relation to the performance but can guard global references too.
            Using Gidenstam's GC it can be possible to safely introduce to the lock-free data structures
            very useful concepts like iterators.

            GarbageCollector is the singleton.
        */
        class CDS_EXPORT_API GarbageCollector
        {
        public:
            typedef cds::atomicity::event_counter  event_counter   ;   ///< event counter type

            /// GC internal statistics
            struct internal_state {
                size_t              nHPCount                ;   ///< HP count per thread (const)
                size_t              nMaxThreadCount         ;   ///< Max thread count (const)
                size_t              nMaxRetiredPtrCount     ;   ///< Max retired pointer count per thread (const)
                size_t              nHRCRecSize             ;   ///< Size of HRC record, bytes (const)

                size_t              nHRCRecAllocated        ;   ///< Count of HRC record allocations
                size_t              nHRCRecUsed             ;   ///< Count of HRC record used
                size_t              nTotalRetiredPtrCount   ;   ///< Current total count of retired pointers
                size_t              nRetiredPtrInFreeHRCRecs;   ///< Count of retired pointer in free (unused) HP records


                event_counter::value_type   evcAllocHRCRec        ; ///< Event count of thread descriptor allocation
                event_counter::value_type   evcRetireHRCRec        ; ///< Event count of thread descriptor reclamation
                event_counter::value_type   evcAllocNewHRCRec    ; ///< Event count of new thread descriptor allocation
                event_counter::value_type   evcDeleteHRCRec        ; ///< Event count of thread descriptor deletion
                event_counter::value_type   evcScanCall            ; ///< Number of calls Scan
                event_counter::value_type   evcHelpScanCalls    ; ///< Number of calls HelpScan
                event_counter::value_type   evcCleanUpAllCalls  ; ///< Number of calls CleanUpAll
                event_counter::value_type   evcDeletedNode        ; ///< Node deletion event counter
                event_counter::value_type   evcScanGuarded      ; ///< Count of retired nodes that could not be deleted on Scan phase
                event_counter::value_type   evcScanClaimGuarded ; ///< Count of retired node that could not be deleted on Scan phase because of m_nClaim != 0

#ifdef CDS_DEBUG
                event_counter::value_type   evcNodeConstruct    ; ///< Count of constructed ContainerNode
                event_counter::value_type   evcNodeDestruct     ; ///< Count of destructed ContainerNode
#endif
            };

            /// "Global GC object is nullptr" exception
            CDS_DECLARE_EXCEPTION( HRCGarbageCollectorEmpty, "Global cds::gc::hrc::GarbageCollector is NULL" );

            /// Not enough required Hazard Pointer count
            CDS_DECLARE_EXCEPTION( HRCTooMany, "Not enough required Hazard Pointer count" );

        private:
            /// Internal statistics by events
            struct statistics {
                event_counter  m_AllocHRCThreadDesc        ; ///< Event count of thread descriptor allocation
                event_counter  m_RetireHRCThreadDesc        ; ///< Event count of thread descriptor reclamation
                event_counter  m_AllocNewHRCThreadDesc        ; ///< Event count of new thread descriptor allocation
                event_counter  m_DeleteHRCThreadDesc        ; ///< Event count of deletion of thread descriptor
                event_counter  m_ScanCalls                    ; ///< Number of calls Scan
                event_counter  m_HelpScanCalls             ; ///< Number of calls HelpScan
                event_counter  m_CleanUpAllCalls           ; ///< Number of calls CleanUpAll

                event_counter  m_DeletedNode                ; ///< Node deletion event counter
                event_counter  m_ScanGuarded               ; ///< Count of retired nodes that could not be deleted on Scan phase
                event_counter  m_ScanClaimGuarded          ; ///< Count of retired node that could not be deleted on Scan phase because of m_nClaim != 0

#           ifdef CDS_DEBUG
                event_counter  m_NodeConstructed           ; ///< Count of ContainerNode constructed
                event_counter  m_NodeDestructed            ; ///< Count of ContainerNode destructed
#           endif
            };

            /// HRC control structure of global thread list
            struct thread_list_node: public details::thread_descriptor
            {
                thread_list_node *  m_pNext     ; ///< next list record
                ThreadGC *          m_pOwner    ; ///< Owner of record
                atomics::atomic<cds::OS::ThreadId>   m_idOwner   ; ///< Id of thread owned; 0 - record is free
                bool                m_bFree        ; ///< Node is help-scanned

                //@cond
                thread_list_node( const GarbageCollector& HzpMgr )
                    : thread_descriptor( HzpMgr ),
                    m_pNext( nullptr ),
                    m_pOwner( nullptr ),
                    m_idOwner(cds::OS::c_NullThreadId),
                    m_bFree( false )
                {}

                ~thread_list_node()
                {
                    assert( m_pOwner == nullptr );
                    assert( m_idOwner.load( atomics::memory_order_relaxed ) == cds::OS::c_NullThreadId );
                }
                //@endcond
            };

        private:
            atomics::atomic<thread_list_node *> m_pListHead  ;  ///< Head of thread list

            static GarbageCollector *    m_pGC    ;    ///< HRC garbage collector instance

            statistics              m_Stat                  ;    ///< Internal statistics
            bool                    m_bStatEnabled          ;    ///< @a true - accumulate internal statistics

            const size_t            m_nHazardPointerCount   ;    ///< max count of thread's hazard pointer
            const size_t            m_nMaxThreadCount       ;    ///< max count of thread
            const size_t            m_nMaxRetiredPtrCount   ;    ///< max count of retired ptr per thread

        private:
            //@cond
            GarbageCollector(
                size_t nHazardPtrCount,            ///< number of hazard pointers
                size_t nMaxThreadCount,            ///< max number of threads
                size_t nRetiredNodeArraySize    ///< size of array of retired node
            );
            ~GarbageCollector();
            //@endcond

            /// Allocates new HRC control structure from the heap (using operator new)
            thread_list_node *    newHRCThreadDesc();

            /// Deletes \p pNode control structure
            void                deleteHRCThreadDesc( thread_list_node * pNode );

            /// Clears retired nodes of \p pNode control structure
            void                clearHRCThreadDesc( thread_list_node * pNode );

            /// Finds HRC control structure for current thread
            thread_list_node *    getHRCThreadDescForCurrentThread() const;

        public:
            /// Create global instance of GarbageCollector
            static void    CDS_STDCALL    Construct(
                size_t nHazardPtrCount = 0,        ///< number of hazard pointers
                size_t nMaxThreadCount = 0,        ///< max threads count
                size_t nMaxNodeLinkCount = 0,    ///< max number of links a @ref ContainerNode can contain
                size_t nMaxTransientLinks = 0    ///< max number of links in live nodes that may transiently point to a deleted node
                );

            /// Destroy global instance of GarbageCollector
            static void    CDS_STDCALL        Destruct();

            /// Get global instance of GarbageCollector
            static GarbageCollector&   instance()
            {
                if ( !m_pGC )
                    throw HRCGarbageCollectorEmpty();
                return *m_pGC;
            }

            /// Checks if global GC object is constructed and may be used
            static bool isUsed()
            {
                return m_pGC != nullptr;
            }

            /// Get max count of hazard pointers as defined in @ref Construct call
            size_t            getHazardPointerCount() const
            {
                return m_nHazardPointerCount;
            }

            /// Get max thread count as defined in @ref Construct call
            size_t            getMaxThreadCount() const
            {
                return m_nMaxThreadCount;
            }

            /// Get max retired pointers count. It is calculated by the parameters of @ref Construct call
            size_t            getMaxRetiredPtrCount() const
            {
                return m_nMaxRetiredPtrCount;
            }

            /// Get internal statistics
            internal_state& getInternalState( internal_state& stat) const;

            /// Check if statistics enabled
            bool              isStatisticsEnabled() const
            {
                return m_bStatEnabled;
            }

            /// Enable internal statistics
            bool              enableStatistics( bool bEnable )
            {
                bool bCurEnabled = m_bStatEnabled;
                m_bStatEnabled = bEnable;
                return bCurEnabled;
            }

            /// Checks that required hazard pointer count \p nRequiredCount is less or equal then max hazard pointer count
            /**
                If \p nRequiredCount > getHazardPointerCount() then the exception HZPTooMany is thrown
            */
            static void checkHPCount( unsigned int nRequiredCount )
            {
                if ( instance().getHazardPointerCount() < nRequiredCount )
                    throw HRCTooMany();
            }

        public:    // Internals for threads

            /// Allocates HRC thread descriptor (thread interface)
            details::thread_descriptor * allocateHRCThreadDesc( ThreadGC * pThreadGC );

            /// Retires HRC thread descriptor (thread interface)
            void retireHRCThreadDesc( details::thread_descriptor * pRec );

            /// The main method of GC
            /**
                The procedure searches through all not yet reclaimed nodes deleted by this thread
                and reclaim only those that does not have any matching hazard pointers and do not have any
                counted references from any links inside of nodes.
                @a Scan is called in context of thread owned \p pRec.
            */
            void Scan( ThreadGC * pThreadGC );

            /// Manage free thread_descriptor records and move all retired pointers to \p pThreadGC
            void HelpScan( ThreadGC * pThreadGC );

            /// Global clean up
            /**
                The procedure try to remove redundant claimed references from links in deleted nodes
                that has been deleted by any thread. \p pThreadGC - ThreadGC of calling thread
            */
            void CleanUpAll( ThreadGC * pThreadGC );

            //@cond
            void try_retire( ThreadGC * pThreadGC ) ;   // inline in hrc_inline.h
            //@endcond

#   ifdef CDS_DEBUG
        public:
            //@cond
            void dbgNodeConstructed() { ++m_Stat.m_NodeConstructed; }
            void dbgNodeDestructed()  { ++m_Stat.m_NodeDestructed;  }
            //@endcond
#   endif

        };

        class AutoHPGuard;

        /// Thread's Garbage collector
        /**
            To use HRC reclamation schema each thread object must be linked with the object of ThreadGC class
            that interacts with GarbageCollector global object. The linkage is performed by calling cds::threading \p Manager::attachThread()
            on the start of each thread that uses HRC GC. Before terminating the thread linked to HRC GC it is necessary to call
            cds::threading \p Manager::detachThread().
        */
        class ThreadGC: cds::details::noncopyable
        {
            GarbageCollector&               m_gc    ; ///< master garbage collector
            details::thread_descriptor *    m_pDesc ; ///< descriptor of GC data for the thread

            friend class GarbageCollector;

        public:
            //@cond
            ThreadGC()
                : m_gc( GarbageCollector::instance() )
                , m_pDesc( nullptr )
            {}
            ~ThreadGC()
            {
                fini();
            }
            //@endcond

            /// Checks if thread GC is initialized
            bool    isInitialized() const { return m_pDesc != nullptr; }

            /// Initialization. Multiple calls is allowed
            void init()
            {
                if ( !m_pDesc )
                    m_pDesc = m_gc.allocateHRCThreadDesc( this );
            }

            /// Finalization. Multiple calls is allowed
            void fini()
            {
                if ( m_pDesc ) {
                    cleanUpLocal();
                    m_gc.Scan( this );
                    details::thread_descriptor * pRec = m_pDesc;
                    m_pDesc = nullptr;
                    if  ( pRec )
                        m_gc.retireHRCThreadDesc( pRec );
                }
            }
        public:    // HRC garbage collector methods

            /// Initializes HP guard \p guard
            details::HPGuard& allocGuard()
            {
                assert( m_pDesc != nullptr );
                return m_pDesc->m_hzp.alloc();
            }

            /// Frees HP guard \p guard
            void freeGuard( details::HPGuard& guard )
            {
                assert( m_pDesc != nullptr );
                m_pDesc->m_hzp.free( guard );
            }

            /// Initializes HP guard array \p arr
            template <size_t Count>
            void allocGuard( details::HPArray<Count>& arr )
            {
                assert( m_pDesc != nullptr );
                m_pDesc->m_hzp.alloc( arr );
            }

            /// Frees HP guard array \p arr
            template <size_t Count>
            void freeGuard( details::HPArray<Count>& arr )
            {
                assert( m_pDesc != nullptr );
                m_pDesc->m_hzp.free( arr );
            }

            /// Retire (deferred delete) node \p pNode guarded by \p hp hazard pointer
            void retireNode( ContainerNode * pNode, details::HPGuard& hp, details::free_retired_ptr_func pFunc )
            {
                assert( !pNode->m_bDeleted.load( atomics::memory_order_relaxed ) );
                assert( pNode == hp );

                retireNode( pNode, pFunc );
                hp.clear();
            }

            /// Retire (deferred delete) node \p pNode. Do not use this function directly!
            void retireNode( ContainerNode * pNode, details::free_retired_ptr_func pFunc )
            {
                assert( !pNode->m_bDeleted.load( atomics::memory_order_relaxed ) );

                pNode->m_bDeleted.store( true, atomics::memory_order_release );
                pNode->m_bTrace.store( false, atomics::memory_order_release );

                m_pDesc->m_arrRetired.push( pNode, pFunc );

                if ( m_pDesc->m_arrRetired.isFull() )
                    m_gc.try_retire( this );
            }

            //@cond
            void scan()
            {
                m_gc.try_retire( this );
            }
            //@endcond

        protected:
            /// The procedure will try to remove redundant claimed references from link in deleted nodes that has been deleted by this thread
            void cleanUpLocal()
            {
                details::retired_vector::iterator itEnd = m_pDesc->m_arrRetired.end();
                for ( details::retired_vector::iterator it = m_pDesc->m_arrRetired.begin(); it != itEnd; ++it ) {
                    details::retired_node& node = *it;
                    ContainerNode * pNode = node.m_pNode.load(atomics::memory_order_acquire);
                    if ( pNode && !node.m_bDone.load(atomics::memory_order_acquire) )
                        pNode->cleanUp( this );
                }
            }
        };

        /// Auto HPGuard.
        class AutoHPGuard
        {
            //@cond
            details::HPGuard&   m_hp  ; ///< hazard pointer
            ThreadGC&           m_mgr ; ///< Thread GC.
            //@endcond
        public:
            typedef details::HPGuard::hazard_ptr hazard_ptr ;  ///< Hazard pointer type

        public:
            /// Allocates HP guard from \p mgr
            AutoHPGuard( ThreadGC& mgr )
                : m_hp( mgr.allocGuard() )
                , m_mgr( mgr )
            {}

            /// Allocates HP guard from \p mgr and protects the pointer \p p of type \p T
            template <typename T>
            AutoHPGuard( ThreadGC& mgr, T * p  )
                : m_hp( mgr.allocGuard() )
                , m_mgr( mgr )
            {
                m_hp = p;
            }

            /// Frees HP guard
            ~AutoHPGuard()
            {
                m_mgr.freeGuard( m_hp );
            }

            /// Returns thread GC
            ThreadGC&    getGC() const CDS_NOEXCEPT
            {
                return m_mgr;
            }

            //@cond
            template <typename T>
            T * operator =( T * p ) CDS_NOEXCEPT
            {
                return m_hp = p;
            }
            //@endcond

            //@cond
            hazard_ptr get() const CDS_NOEXCEPT
            {
                return m_hp;
            }
            //@endcond

            /// Clears the hazard pointer
            void clear() CDS_NOEXCEPT
            {
                m_hp.clear();
            }
        };

        /// Auto-managed array of hazard pointers
        /**
            This class is wrapper around gc::hzp::details::HPArray class.
        */
        template <size_t Count>
        class AutoHPArray: public details::HPArray<Count>
        {
            ThreadGC&    m_mgr    ;    ///< Thread GC

        public:
            /// Allocates array of HP guard from \p mgr
            AutoHPArray( ThreadGC& mgr )
                : m_mgr( mgr )
            {
                mgr.allocGuard( *this );
            }

            /// Frees array of HP guard
            ~AutoHPArray()
            {
                m_mgr.freeGuard( *this );
            }

            /// Returns thread GC
            ThreadGC&    getGC() const
            {
                return m_mgr;
            }
        };


    }    // namespace hrc
}} // namespace cds::gc

#include <cds/gc/hrc/details/hrc_inline.h>

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning(pop)
#endif

#endif // #ifndef __CDS_GC_HRC_HRC_H

/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSLIB_GC_DETAILS_HP_H
#define CDSLIB_GC_DETAILS_HP_H

#include <cds/algo/atomic.h>
#include <cds/os/thread.h>
#include <cds/details/bounded_array.h>
#include <cds/user_setup/cache_line.h>

#include <cds/gc/details/hp_type.h>
#include <cds/gc/details/hp_alloc.h>

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning(push)
    // warning C4251: 'cds::gc::hp::GarbageCollector::m_pListHead' : class 'cds::cxx11_atomic::atomic<T>'
    // needs to have dll-interface to be used by clients of class 'cds::gc::hp::GarbageCollector'
#   pragma warning(disable: 4251)
#endif

/*
    Editions:
        2007.12.24  khizmax Add statistics and CDS_GATHER_HAZARDPTR_STAT macro
        2008.03.06  khizmax Refactoring: implementation of HazardPtrMgr is moved to hazardptr.cpp
        2008.03.08  khizmax Remove HazardPtrMgr singleton. Now you must initialize/destroy HazardPtrMgr calling
                            HazardPtrMgr::Construct / HazardPtrMgr::Destruct before use (usually in main() function).
        2008.12.06  khizmax Refactoring. Changes class name, namespace hierarchy, all helper defs have been moved to details namespace
        2010.01.27  khizmax Introducing memory order constraint
*/

//@cond
namespace cds {
    /// Different safe memory reclamation schemas (garbage collectors)
    /** @ingroup cds_garbage_collector

        This namespace specifies different safe memory reclamation (SMR) algorithms.
        See \ref cds_garbage_collector "Garbage collectors"
    */
    namespace gc {

    /// Michael's Hazard Pointers reclamation schema
    /**
    \par Sources:
        - [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-freeobjects using atomic reads and writes"
        - [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
        - [2004] Andrei Alexandrescy, Maged Michael "Lock-free Data Structures with Hazard Pointers"

        The \p cds::gc::hp namespace and its members are internal representation of Hazard Pointer GC and should not be used directly.
        Use \p cds::gc::HP class in your code.

        Hazard Pointer garbage collector is a singleton. The main user-level part of Hazard Pointer schema is
        GC class and its nested classes. Before use any HP-related class you must initialize HP garbage collector
        by contructing \p cds::gc::HP object in beginning of your \p main().
        See \p cds::gc::HP class for explanation.
    */
    namespace hp {

        // forwards
        class GarbageCollector;
        class ThreadGC;

        namespace details {

            /// Retired pointer
            typedef cds::gc::details::retired_ptr   retired_ptr;

            /// Array of retired pointers
            /**
                The vector of retired pointer ready to delete.

                The Hazard Pointer schema is build on thread-static arrays. For each HP-enabled thread the HP manager allocates
                array of retired pointers. The array belongs to the thread: owner thread writes to the array, other threads
                just read it.
            */
            class retired_vector {
                /// Underlying vector implementation
                typedef cds::details::bounded_array<retired_ptr>    retired_vector_impl;

                retired_vector_impl m_arr   ;   ///< the array of retired pointers
                size_t              m_nSize ;   ///< Current size of \p m_arr

            public:
                /// Iterator
                typedef retired_vector_impl::iterator  iterator;

                /// Constructor
                retired_vector( const cds::gc::hp::GarbageCollector& HzpMgr ); // inline
                ~retired_vector()
                {}

                /// Vector capacity.
                /**
                    The capacity is constant for any thread. It is defined by cds::gc::hp::GarbageCollector.
                */
                size_t capacity() const CDS_NOEXCEPT
                {
                    return m_arr.capacity();
                }

                /// Current vector size (count of retired pointers in the vector)
                size_t size() const CDS_NOEXCEPT
                {
                    return m_nSize;
                }

                /// Set vector size. Uses internally
                void size( size_t nSize )
                {
                    assert( nSize <= capacity() );
                    m_nSize = nSize;
                }

                /// Pushes retired pointer to the vector
                void push( retired_ptr const& p )
                {
                    assert( m_nSize < capacity() );
                    m_arr[ m_nSize ] = p;
                    ++m_nSize;
                }

                /// Checks if the vector is full (size() == capacity() )
                bool isFull() const CDS_NOEXCEPT
                {
                    return m_nSize >= capacity();
                }

                /// Begin iterator
                iterator    begin() CDS_NOEXCEPT
                {
                    return m_arr.begin();
                }

                /// End iterator
                iterator    end() CDS_NOEXCEPT
                {
                    return m_arr.begin() +  m_nSize;
                }

                /// Clears the vector. After clearing, size() == 0
                void clear() CDS_NOEXCEPT
                {
                    m_nSize = 0;
                }
            };

            /// Hazard pointer record of the thread
            /**
                The structure of type "single writer - multiple reader": only the owner thread may write to this structure
                other threads have read-only access.
            */
            struct hp_record {
                hp_allocator<>    m_hzp;         ///< array of hazard pointers. Implicit \ref CDS_DEFAULT_ALLOCATOR dependency
                retired_vector    m_arrRetired ; ///< Retired pointer array

                char padding[cds::c_nCacheLineSize];
                atomics::atomic<unsigned int> m_nSync; ///< dummy var to introduce synchronizes-with relationship between threads

                /// Ctor
                hp_record( const cds::gc::hp::GarbageCollector& HzpMgr );    // inline
                ~hp_record()
                {}

                /// Clears all hazard pointers
                void clear()
                {
                    m_hzp.clear();
                }

                void sync()
                {
                    m_nSync.fetch_add( 1, atomics::memory_order_acq_rel );
                }
            };
        }    // namespace details

        /// GarbageCollector::Scan phase strategy
        /**
            See GarbageCollector::Scan for explanation
        */
        enum scan_type {
            classic,    ///< classic scan as described in Michael's works (see GarbageCollector::classic_scan)
            inplace     ///< inplace scan without allocation (see GarbageCollector::inplace_scan)
        };

        /// Hazard Pointer singleton
        /**
            Safe memory reclamation schema by Michael "Hazard Pointers"

        \par Sources:
            \li [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-freeobjects using atomic reads and writes"
            \li [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
            \li [2004] Andrei Alexandrescy, Maged Michael "Lock-free Data Structures with Hazard Pointers"

        */
        class CDS_EXPORT_API GarbageCollector
        {
        public:
            typedef cds::atomicity::event_counter  event_counter   ;   ///< event counter type

            /// Internal GC statistics
            struct InternalState {
                size_t              nHPCount                ;   ///< HP count per thread (const)
                size_t              nMaxThreadCount         ;   ///< Max thread count (const)
                size_t              nMaxRetiredPtrCount     ;   ///< Max retired pointer count per thread (const)
                size_t              nHPRecSize              ;   ///< Size of HP record, bytes (const)

                size_t              nHPRecAllocated         ;   ///< Count of HP record allocations
                size_t              nHPRecUsed              ;   ///< Count of HP record used
                size_t              nTotalRetiredPtrCount   ;   ///< Current total count of retired pointers
                size_t              nRetiredPtrInFreeHPRecs ;   ///< Count of retired pointer in free (unused) HP records

                event_counter::value_type   evcAllocHPRec   ;   ///< Count of \p hp_record allocations
                event_counter::value_type   evcRetireHPRec  ;   ///< Count of \p hp_record retire events
                event_counter::value_type   evcAllocNewHPRec;   ///< Count of new \p hp_record allocations from heap
                event_counter::value_type   evcDeleteHPRec  ;   ///< Count of \p hp_record deletions

                event_counter::value_type   evcScanCall     ;   ///< Count of Scan calling
                event_counter::value_type   evcHelpScanCall ;   ///< Count of HelpScan calling
                event_counter::value_type   evcScanFromHelpScan;///< Count of Scan calls from HelpScan

                event_counter::value_type   evcDeletedNode  ;   ///< Count of deleting of retired objects
                event_counter::value_type   evcDeferredNode ;   ///< Count of objects that cannot be deleted in Scan phase because of a hazard_pointer guards it
            };

            /// No GarbageCollector object is created
            class not_initialized : public std::runtime_error
            {
            public:
                //@cond
                not_initialized()
                    : std::runtime_error( "Global Hazard Pointer GarbageCollector is not initialized" )
                {}
                //@endcond
            };

            /// Not enough required Hazard Pointer count
            class too_many_hazard_ptr : public std::length_error
            {
            public:
                //@cond
                too_many_hazard_ptr()
                    : std::length_error( "Not enough required Hazard Pointer count" )
                {}
                //@endcond
            };

        private:
            /// Internal GC statistics
            struct Statistics {
                event_counter  m_AllocHPRec            ;    ///< Count of \p hp_record allocations
                event_counter  m_RetireHPRec            ;    ///< Count of \p hp_record retire events
                event_counter  m_AllocNewHPRec            ;    ///< Count of new \p hp_record allocations from heap
                event_counter  m_DeleteHPRec            ;    ///< Count of \p hp_record deletions

                event_counter  m_ScanCallCount            ;    ///< Count of Scan calling
                event_counter  m_HelpScanCallCount        ;    ///< Count of HelpScan calling
                event_counter  m_CallScanFromHelpScan    ;    ///< Count of Scan calls from HelpScan

                event_counter  m_DeletedNode            ;    ///< Count of retired objects deleting
                event_counter  m_DeferredNode            ;    ///< Count of objects that cannot be deleted in Scan phase because of a hazard_pointer guards it
            };

            /// Internal list of cds::gc::hp::details::hp_record
            struct hplist_node : public details::hp_record
            {
                hplist_node *                    m_pNextNode; ///< next hazard ptr record in list
                atomics::atomic<OS::ThreadId>    m_idOwner;   ///< Owner thread id; 0 - the record is free (not owned)
                atomics::atomic<bool>            m_bFree;     ///< true if record if free (not owned)

                //@cond
                hplist_node( const GarbageCollector& HzpMgr )
                    : hp_record( HzpMgr ),
                    m_pNextNode( nullptr ),
                    m_idOwner( OS::c_NullThreadId ),
                    m_bFree( true )
                {}

                ~hplist_node()
                {
                    assert( m_idOwner.load( atomics::memory_order_relaxed ) == OS::c_NullThreadId );
                    assert( m_bFree.load(atomics::memory_order_relaxed) );
                }
                //@endcond
            };

            atomics::atomic<hplist_node *>   m_pListHead  ;  ///< Head of GC list

            static GarbageCollector *    m_pHZPManager  ;   ///< GC instance pointer

            Statistics              m_Stat              ;   ///< Internal statistics
            bool                    m_bStatEnabled      ;   ///< true - statistics enabled

            const size_t            m_nHazardPointerCount   ;   ///< max count of thread's hazard pointer
            const size_t            m_nMaxThreadCount       ;   ///< max count of thread
            const size_t            m_nMaxRetiredPtrCount   ;   ///< max count of retired ptr per thread
            scan_type               m_nScanType             ;   ///< scan type (see \ref scan_type enum)


        private:
            /// Ctor
            GarbageCollector(
                size_t nHazardPtrCount = 0,         ///< Hazard pointer count per thread
                size_t nMaxThreadCount = 0,         ///< Max count of thread
                size_t nMaxRetiredPtrCount = 0,     ///< Capacity of the array of retired objects
                scan_type nScanType = inplace       ///< Scan type (see \ref scan_type enum)
            );

            /// Dtor
            ~GarbageCollector();

            /// Allocate new HP record
            hplist_node * NewHPRec();

            /// Permanently deletes HPrecord \p pNode
            /**
                Caveat: for performance reason this function is defined as inline and cannot be called directly
            */
            void                DeleteHPRec( hplist_node * pNode );

            void detachAllThread();

        public:
            /// Creates GarbageCollector singleton
            /**
                GC is the singleton. If GC instance is not exist then the function creates the instance.
                Otherwise it does nothing.

                The Michael's HP reclamation schema depends of three parameters:

                \p nHazardPtrCount - HP pointer count per thread. Usually it is small number (2-4) depending from
                                     the data structure algorithms. By default, if \p nHazardPtrCount = 0,
                                     the function uses maximum of HP count for CDS library.

                \p nMaxThreadCount - max count of thread with using HP GC in your application. Default is 100.

                \p nMaxRetiredPtrCount - capacity of array of retired pointers for each thread. Must be greater than
                                    \p nHazardPtrCount * \p nMaxThreadCount.
                                    Default is 2 * \p nHazardPtrCount * \p nMaxThreadCount.
            */
            static void    CDS_STDCALL Construct(
                size_t nHazardPtrCount = 0,     ///< Hazard pointer count per thread
                size_t nMaxThreadCount = 0,     ///< Max count of simultaneous working thread in your application
                size_t nMaxRetiredPtrCount = 0, ///< Capacity of the array of retired objects for the thread
                scan_type nScanType = inplace   ///< Scan type (see \ref scan_type enum)
            );

            /// Destroys global instance of GarbageCollector
            /**
                The parameter \p bDetachAll should be used carefully: if its value is \p true,
                then the destroying GC automatically detaches all attached threads. This feature
                can be useful when you have no control over the thread termination, for example,
                when \p libcds is injected into existing external thread.
            */
            static void CDS_STDCALL Destruct(
                bool bDetachAll = false     ///< Detach all threads
            );

            /// Returns pointer to GarbageCollector instance
            static GarbageCollector&   instance()
            {
                if ( !m_pHZPManager )
                    throw not_initialized();
                return *m_pHZPManager;
            }

            /// Checks if global GC object is constructed and may be used
            static bool isUsed() CDS_NOEXCEPT
            {
                return m_pHZPManager != nullptr;
            }

            /// Returns max Hazard Pointer count defined in construction time
            size_t            getHazardPointerCount() const CDS_NOEXCEPT
            {
                return m_nHazardPointerCount;
            }

            /// Returns max thread count defined in construction time
            size_t            getMaxThreadCount() const CDS_NOEXCEPT
            {
                return m_nMaxThreadCount;
            }

            /// Returns max size of retired objects array. It is defined in construction time
            size_t            getMaxRetiredPtrCount() const CDS_NOEXCEPT
            {
                return m_nMaxRetiredPtrCount;
            }

            // Internal statistics

            /// Get internal statistics
            InternalState& getInternalState(InternalState& stat) const;

            /// Checks if internal statistics enabled
            bool              isStatisticsEnabled() const { return m_bStatEnabled; }

            /// Enables/disables internal statistics
            bool              enableStatistics( bool bEnable )
            {
                bool bEnabled = m_bStatEnabled;
                m_bStatEnabled = bEnable;
                return bEnabled;
            }

            /// Checks that required hazard pointer count \p nRequiredCount is less or equal then max hazard pointer count
            /**
                If \p nRequiredCount > getHazardPointerCount() then the exception \p too_many_hazard_ptr is thrown
            */
            static void checkHPCount( unsigned int nRequiredCount )
            {
                if ( instance().getHazardPointerCount() < nRequiredCount )
                    throw too_many_hazard_ptr();
            }

            /// Get current scan strategy
            scan_type getScanType() const
            {
                return m_nScanType;
            }

            /// Set current scan strategy
            /** @anchor hzp_gc_setScanType
                Scan strategy changing is allowed on the fly.
            */
            void setScanType(
                scan_type nScanType     ///< new scan strategy
            )
            {
                m_nScanType = nScanType;
            }

        public:    // Internals for threads

            /// Allocates Hazard Pointer GC record. For internal use only
            details::hp_record * alloc_hp_record();

            /// Free HP record. For internal use only
            void free_hp_record( details::hp_record * pRec );

            /// The main garbage collecting function
            /**
                This function is called internally by ThreadGC object when upper bound of thread's list of reclaimed pointers
                is reached.

                There are the following scan algorithm:
                - \ref hzp_gc_classic_scan "classic_scan" allocates memory for internal use
                - \ref hzp_gc_inplace_scan "inplace_scan" does not allocate any memory

                Use \ref hzp_gc_setScanType "setScanType" member function to setup appropriate scan algorithm.
            */
            void Scan( details::hp_record * pRec )
            {
                switch ( m_nScanType ) {
                    case inplace:
                        inplace_scan( pRec );
                        break;
                    default:
                        assert(false)   ;   // Forgotten something?..
                    case classic:
                        classic_scan( pRec );
                        break;
                }
            }

            /// Helper scan routine
            /**
                The function guarantees that every node that is eligible for reuse is eventually freed, barring
                thread failures. To do so, after executing Scan, a thread executes a HelpScan,
                where it checks every HP record. If an HP record is inactive, the thread moves all "lost" reclaimed pointers
                to thread's list of reclaimed pointers.

                The function is called internally by Scan.
            */
            void HelpScan( details::hp_record * pThis );

        protected:
            /// Classic scan algorithm
            /** @anchor hzp_gc_classic_scan
                Classical scan algorithm as described in Michael's paper.

                A scan includes four stages. The first stage involves scanning the array HP for non-null values.
                Whenever a non-null value is encountered, it is inserted in a local list of currently protected pointer.
                Only stage 1 accesses shared variables. The following stages operate only on private variables.

                The second stage of a scan involves sorting local list of protected pointers to allow
                binary search in the third stage.

                The third stage of a scan involves checking each reclaimed node
                against the pointers in local list of protected pointers. If the binary search yields
                no match, the node is freed. Otherwise, it cannot be deleted now and must kept in thread's list
                of reclaimed pointers.

                The forth stage prepares new thread's private list of reclaimed pointers
                that could not be freed during the current scan, where they remain until the next scan.

                This algorithm allocates memory for internal HP array.

                This function is called internally by ThreadGC object when upper bound of thread's list of reclaimed pointers
                is reached.
            */
            void classic_scan( details::hp_record * pRec );

            /// In-place scan algorithm
            /** @anchor hzp_gc_inplace_scan
                Unlike the \ref hzp_gc_classic_scan "classic_scan" algorithm, \p inplace_scan does not allocate any memory.
                All operations are performed in-place.
            */
            void inplace_scan( details::hp_record * pRec );
        };

        /// Thread's hazard pointer manager
        /**
            To use Hazard Pointer reclamation schema each thread object must be linked with the object of ThreadGC class
            that interacts with GarbageCollector global object. The linkage is performed by calling \ref cds_threading "cds::threading::Manager::attachThread()"
            on the start of each thread that uses HP GC. Before terminating the thread linked to HP GC it is necessary to call
            \ref cds_threading "cds::threading::Manager::detachThread()".
        */
        class ThreadGC
        {
            GarbageCollector&    m_HzpManager; ///< Hazard Pointer GC singleton
            details::hp_record * m_pHzpRec;    ///< Pointer to thread's HZP record

        public:
            /// Default constructor
            ThreadGC()
                : m_HzpManager( GarbageCollector::instance() ),
                m_pHzpRec( nullptr )
            {}

            /// The object is not copy-constructible
            ThreadGC( ThreadGC const& ) = delete;

            ~ThreadGC()
            {
                fini();
            }

            /// Checks if thread GC is initialized
            bool    isInitialized() const   { return m_pHzpRec != nullptr; }

            /// Initialization. Repeat call is available
            void init()
            {
                if ( !m_pHzpRec )
                    m_pHzpRec = m_HzpManager.alloc_hp_record();
            }

            /// Finalization. Repeat call is available
            void fini()
            {
                if ( m_pHzpRec ) {
                    details::hp_record * pRec = m_pHzpRec;
                    m_pHzpRec = nullptr;
                    m_HzpManager.free_hp_record( pRec );
                }
            }

            /// Initializes HP guard \p guard
            details::hp_guard& allocGuard()
            {
                assert( m_pHzpRec );
                return m_pHzpRec->m_hzp.alloc();
            }

            /// Frees HP guard \p guard
            void freeGuard( details::hp_guard& guard )
            {
                assert( m_pHzpRec );
                m_pHzpRec->m_hzp.free( guard );
            }

            /// Initializes HP guard array \p arr
            template <size_t Count>
            void allocGuard( details::hp_array<Count>& arr )
            {
                assert( m_pHzpRec );
                m_pHzpRec->m_hzp.alloc( arr );
            }

            /// Frees HP guard array \p arr
            template <size_t Count>
            void freeGuard( details::hp_array<Count>& arr )
            {
                assert( m_pHzpRec );
                m_pHzpRec->m_hzp.free( arr );
            }

            /// Places retired pointer \p and its deleter \p pFunc into thread's array of retired pointer for deferred reclamation
            template <typename T>
            void retirePtr( T * p, void (* pFunc)(T *) )
            {
                /*
                union {
                    T * p;
                    hazard_pointer hp;
                } cast_ptr;
                cast_ptr.p = p;

                union{
                    void( *pFunc )(T *);
                    free_retired_ptr_func hpFunc;
                } cast_func;
                cast_func.pFunc = pFunc;

                retirePtr( details::retired_ptr( cast_ptr.hp, cast_func.hpFunc ) );
                */
                retirePtr( details::retired_ptr( reinterpret_cast<void *>( p ), reinterpret_cast<free_retired_ptr_func>( pFunc )));
            }

            /// Places retired pointer \p into thread's array of retired pointer for deferred reclamation
            void retirePtr( details::retired_ptr const& p )
            {
                m_pHzpRec->m_arrRetired.push( p );

                if ( m_pHzpRec->m_arrRetired.isFull() ) {
                    // Max of retired pointer count is reached. Do scan
                    scan();
                }
            }

            /// Run retiring scan cycle
            void scan()
            {
                m_HzpManager.Scan( m_pHzpRec );
                m_HzpManager.HelpScan( m_pHzpRec );
            }

            void sync()
            {
                assert( m_pHzpRec != nullptr );
                m_pHzpRec->sync();
            }
        };

        /// Auto hp_guard.
        /**
            This class encapsulates Hazard Pointer guard to protect a pointer against deletion.
            It allocates one HP from thread's HP array in constructor and free the hazard pointer allocated
            in destructor.
        */
        class guard
        {
            details::hp_guard&  m_hp    ; ///< Hazard pointer guarded

        public:
            typedef details::hp_guard::hazard_ptr hazard_ptr ;  ///< Hazard pointer type

        public:
            /// Allocates HP guard
            guard(); // inline in hp_impl.h

            /// Allocates HP guard from \p gc and protects the pointer \p p of type \p T
            template <typename T>
            explicit guard( T * p ); // inline in hp_impl.h

            /// Frees HP guard. The pointer guarded may be deleted after this.
            ~guard(); // inline in hp_impl.h

            /// Protects the pointer \p p against reclamation (guards the pointer).
            template <typename T>
            T * operator =( T * p )
            {
                return m_hp = p;
            }

            //@cond
            std::nullptr_t operator =(std::nullptr_t)
            {
                return m_hp = nullptr;
            }
            //@endcond

            /// Get raw guarded pointer
            hazard_ptr get() const
            {
                return m_hp;
            }
        };

        /// Auto-managed array of hazard pointers
        /**
            This class is wrapper around cds::gc::hp::details::hp_array class.
            \p Count is the size of HP array
        */
        template <size_t Count>
        class array : public details::hp_array<Count>
        {
        public:
            /// Rebind array for other size \p COUNT2
            template <size_t Count2>
            struct rebind {
                typedef array<Count2>  other;   ///< rebinding result
            };

        public:
            /// Allocates array of HP guard
            array(); // inline in hp_impl.h

            /// Frees array of HP guard
            ~array(); //inline in hp_impl.h
        };

    }   // namespace hp
}}  // namespace cds::gc
//@endcond

//@cond
// Inlines
namespace cds {
    namespace gc { namespace hp { namespace details {

        inline retired_vector::retired_vector( const cds::gc::hp::GarbageCollector& HzpMgr )
            : m_arr( HzpMgr.getMaxRetiredPtrCount() ),
            m_nSize(0)
        {}

        inline hp_record::hp_record( const cds::gc::hp::GarbageCollector& HzpMgr )
            : m_hzp( HzpMgr.getHazardPointerCount() )
            , m_arrRetired( HzpMgr )
            , m_nSync( 0 )
        {}

    }}} // namespace gc::hp::details
} // namespace cds
//@endcond


#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma warning(pop)
#endif

#endif  // #ifndef CDSLIB_GC_DETAILS_HP_H

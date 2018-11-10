// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_URCU_DETAILS_BASE_H
#define CDSLIB_URCU_DETAILS_BASE_H

#include <cds/algo/atomic.h>
#include <cds/gc/details/retired_ptr.h>
#include <cds/details/allocator.h>
#include <cds/os/thread.h>
#include <cds/details/marked_ptr.h>

namespace cds {
    /// User-space Read-Copy Update (URCU) namespace
    /** @ingroup cds_garbage_collector
        @anchor cds_urcu_desc

        This namespace contains declarations for different types of Read-Copy Update (%RCU)
        synchronization primitive and data structures developed for RCU.
        In <b>libcds</b> %RCU is used as garbage collector.

        <b>Source papers</b>:
        - [2009] M.Desnoyers "Low-Impact Operating System Tracing" PhD Thesis,
          Chapter 6 "User-Level Implementations of Read-Copy Update"
        - [2011] M.Desnoyers, P.McKenney, A.Stern, M.Dagenias, J.Walpole "User-Level
          Implementations of Read-Copy Update"
        - [2012] M.Desnoyers, P.McKenney, A.Stern, M.Dagenias, J.Walpole "Supplementary
          Material for User-Level Implementations of Read-Copy Update"

        <b>Informal introduction to user-space %RCU</b>

        [<i>From Desnoyer's papers</i>] %RCU is a synchronization mechanism that was added to the
        Linux kernel in October of 2002. %RCU achieves scalability improvements by allowing
        reads to occur concurrently with updates. In contrast to conventional locking
        primitives that ensure mutual exclusion among concurrent threads regardless of whether
        they be readers or updaters, or with reader-writer locks that allow concurrent reads
        but not in the presence of updates, %RCU supports concurrency between multiple updaters
        and multiple readers. %RCU ensures that data are not freed up until all pre-existing
        critical sections complete. %RCU defines and uses efficient and scalable mechanisms
        for deferring reclamation of old data. These mechanisms distribute the work among read and update
        paths in such a way as to make read paths extremely fast.

        %RCU readers execute within %RCU read-side critical sections. Each such critical section begins with
        \p rcu_read_lock(), ends with \p rcu_read_unlock() (in \p libcds these primitives are the methods of
        GC class and are usually called \p access_lock and \p access_unlock respectively). Read-side
        critical sections can be nested.
        The performance benefits of %RCU are due to the fact that \p rcu_read_lock()
        and \p rcu_read_unlock() are exceedingly fast.

        When a thread is not in an %RCU read-side critical section, it is in a quiescent state.
        A quiescent state that persists for a significant time period is an extended quiescent state.
        Any time period during which every thread has been in at least one quiescent state
        is a grace period; this implies that every %RCU read-side critical section
        that starts before a grace period must end before that grace period does.
        Distinct grace periods may overlap, either partially or completely. Any time period
        that includes a grace period is by definition itself a grace period.
        Each grace period is guaranteed to complete as long as all read-side critical sections
        are finite in duration; thus even a constant flow of such critical sections is unable to
        extend an %RCU grace period indefinitely.

        Suppose that readers enclose each of their data-structure traversals in
        an %RCU read-side critical section. If an updater first removes an element
        from such a data structure and then waits for a grace period, there can be
        no more readers accessing that element. The updater can then carry out destructive
        operations, for example freeing the element, without disturbing any readers.

        The %RCU update is split into two phases, a removal phase and a reclamation phase.
        These two phases must be separated by a grace period, for example via the \p synchronize_rcu()
        primitive, which initiates a grace period and waits for it to finish.
        During the removal phase, the %RCU update removes elements from a shared data structure.
        The removed data elements will only be accessible to read-side critical sections
        that ran concurrently with the removal phase, which are guaranteed to complete before the
        grace period ends. Therefore the reclamation phase can safely free the data elements
        removed by the removal phase.

        Desnoyers describes several classes of user-space %RCU implementations:
        - The Quiescent-State-Based Reclamation (QSBR) %RCU implementation offers
          the best possible read-side performance, but requires that each thread periodically
          calls a function to announce that it is in a quiescent state, thus strongly
          constraining the application design. This type of %RCU is not implemented in \p libcds.
        - The general-purpose %RCU implementation places almost no constraints on the application's
          design, thus being appropriate for use within a general-purpose library, but it has
          relatively higher read-side overhead. The \p libcds contains several implementations of general-purpose
          %RCU: \ref general_instant, \ref general_buffered, \ref general_threaded.
        - \p signal_buffered: the signal-handling %RCU presents an implementation having low read-side overhead and
          requiring only that the application give up one POSIX signal to %RCU update processing.

        @note The signal-handled %RCU is defined only for UNIX-like systems, not for Windows.

        @anchor cds_urcu_type
        <b>RCU implementation type</b>

        There are several internal implementation of RCU (all declared in \p %cds::urcu namespace):
        - \ref general_instant - general purpose RCU with immediate reclamation
        - \ref general_buffered - general purpose RCU with deferred (buffered) reclamation
        - \ref general_threaded - general purpose RCU with special reclamation thread
        - \ref signal_buffered - signal-handling RCU with deferred (buffered) reclamation

        You cannot create an object of any of those classes directly.
        Instead, you should use wrapper classes.
        The wrapper simplifies creation and usage of RCU singleton objects
        and has the reacher interface that combines interfaces of wrapped class i.e. RCU global part like
        \p synchronize, and corresponding RCU thread-specific interface like \p access_lock, \p access_unlock and \p retire_ptr.

        @anchor cds_urcu_gc
        There are several wrapper classes (all declared in \p %cds::urcu namespace)
        - \ref cds_urcu_general_instant_gc "gc<general_instant>" - general purpose RCU with immediate reclamation,
            include file <tt><cds/urcu/general_instant.h></tt>
        - \ref cds_urcu_general_buffered_gc "gc<general_buffered>" - general purpose RCU with deferred (buffered) reclamation,
            include file <tt><cds/urcu/general_buffered.h></tt>
        - \ref cds_urcu_general_threaded_gc "gc<general_threaded>" - general purpose RCU with special reclamation thread
            include file <tt><cds/urcu/general_threaded.h></tt>
        - \ref cds_urcu_signal_buffered_gc "gc<signal_buffered>" - signal-handling RCU with deferred (buffered) reclamation
            include file <tt><cds/urcu/signal_buffered.h></tt>

        Any RCU-related container in \p libcds expects that its \p RCU template parameter is one of those wrapper.

        @anchor cds_urcu_tags
        For simplicity, in some algorithms instead of using RCU implementation type
        you should specify corresponding RCU tags (all declared in \p %cds::urcu namespace):
        - \ref general_instant_tag - for \ref general_instant
        - \ref general_buffered_tag - for \ref general_buffered
        - \ref general_threaded_tag - for \ref general_threaded
        - \ref signal_buffered_tag - for \ref signal_buffered

        @anchor cds_urcu_performance
        <b>Performance</b>

        As a result of our experiments we can range above %RCU implementation in such order,
        from high to low performance:
        - <tt>gc<general_buffered></tt> - high
        - <tt>gc<general_threaded></tt>
        - <tt>gc<signal_buffered></tt>
        - <tt>gc<general_instant></tt> - low

        This estimation is very rough and depends on many factors:
        type of payload - mostly read-only (seeking) or read-write (inserting and deleting), -
        a hardware, your application, and so on.

        @anchor cds_urcu_howto
        <b>How to use</b>

        Usually, in your application you use only one \ref cds_urcu_gc "type of RCU" that is the best for your needs.
        However, the library allows to apply several RCU singleton in one application.
        The only limitation is that only one object of each RCU type can be instantiated.
        Since each RCU type is a template class the creation of two object of one RCU type class
        with different template arguments is an error and is not supported.
        However, it is correct when your RCU objects relates to different RCU types.

        In \p libcds, many GC-based ordered list, set and map template classes have %RCU-related specializations
        that hide the %RCU specific details.

        RCU GC is initialized in usual way: you should declare an object of type \p cds::urcu::gc< RCU_type >,
        for example:
        \code
        #include <cds/urcu/general_buffered.h>

        typedef cds::urcu::gc< cds::urcu::general_buffered<> >    rcu_gpb;

        int main() {
            // Initialize libcds
            cds::Initialize();
            {
                // Initialize general_buffered RCU
                rcu_gpb   gpbRCU;

                // If main thread uses lock-free containers
                // the main thread should be attached to libcds infrastructure
                cds::threading::Manager::attachThread();

                // Now you can use RCU-based containers in the main thread
                //...
            }
            // Terminate libcds
            cds::Terminate();
        }
        \endcode

        Each thread that deals with RCU-based container should be initialized first:
        \code
        #include <cds/urcu/general_buffered.h>
        int myThreadEntryPoint(void *)
        {
            // Attach the thread to libcds infrastructure
            cds::threading::Manager::attachThread();

            // Now you can use RCU-based containers in the thread
            //...

            // Detach thread when terminating
            cds::threading::Manager::detachThread();
        }
        \endcode
    */
    namespace urcu {

#   if (CDS_OS_INTERFACE == CDS_OSI_UNIX || defined(CDS_DOXYGEN_INVOKED)) && !defined(CDS_THREAD_SANITIZER_ENABLED)
#       define CDS_URCU_SIGNAL_HANDLING_ENABLED 1
#   endif

        /// General-purpose URCU type
        struct general_purpose_rcu {
            //@cond
            static uint32_t const c_nControlBit = 0x80000000;
            static uint32_t const c_nNestMask   = c_nControlBit - 1;
            //@endcond
        };

#   ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        /// Signal-handling URCU type
        struct signal_handling_rcu {
            //@cond
            static uint32_t const c_nControlBit = 0x80000000;
            static uint32_t const c_nNestMask   = c_nControlBit - 1;
            //@endcond
        };
#   endif

        /// Tag for general_instant URCU
        struct general_instant_tag: public general_purpose_rcu {
            typedef general_purpose_rcu     rcu_class ; ///< The URCU type
        };

        /// Tag for general_buffered URCU
        struct general_buffered_tag: public general_purpose_rcu
        {
            typedef general_purpose_rcu     rcu_class ; ///< The URCU type
        };

        /// Tag for general_threaded URCU
        struct general_threaded_tag: public general_purpose_rcu {
            typedef general_purpose_rcu     rcu_class ; ///< The URCU type
        };

#   ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        /// Tag for signal_buffered URCU
        struct signal_buffered_tag: public signal_handling_rcu {
                typedef signal_handling_rcu     rcu_class ; ///< The URCU type
        };
#   endif

        ///@anchor cds_urcu_retired_ptr Retired pointer, i.e. pointer that ready for reclamation
        typedef cds::gc::details::retired_ptr   retired_ptr;
        using cds::gc::make_retired_ptr;

        /// Pointer to function to free (destruct and deallocate) retired pointer of specific type
        typedef cds::gc::details::free_retired_ptr_func free_retired_ptr_func;

        //@cond
        /// Implementation-specific URCU details
        namespace details {
            /// forward declarations
            template <typename RCUtag>
            struct thread_data;

            template <typename RCUtag>
            class thread_gc;

            template <typename RCUtag >
            class singleton;

            //@cond
            class singleton_vtbl {
            protected:
                virtual ~singleton_vtbl()
                {}
            public:
                virtual void retire_ptr( retired_ptr& p ) = 0;
            };

            class gc_common
            {
            public:
                template <typename MarkedPtr> using atomic_marked_ptr = atomics::atomic<MarkedPtr>;
            };
            //@endcond

            //@cond
            template <typename ThreadData>
            struct thread_list_record {
                ThreadData*  next_ = nullptr;   ///< Next item in thread list
                atomics::atomic<OS::ThreadId> thread_id_{ cds::OS::c_NullThreadId }; ///< Owner thread id; 0 - the record is free (not owned)

                thread_list_record() = default;

                explicit thread_list_record( OS::ThreadId owner )
                    : thread_id_( owner )
                {}

                ~thread_list_record()
                {}
            };
            //@endcond

            //@cond
            template <typename RCUtag, class Alloc = CDS_DEFAULT_ALLOCATOR >
            class thread_list {
            public:
                typedef thread_data<RCUtag>                             thread_record;
                typedef cds::details::Allocator< thread_record, Alloc > allocator_type;

            private:
                atomics::atomic<thread_record *> m_pHead;

            public:
                thread_list()
                    : m_pHead( nullptr )
                {}

                ~thread_list()
                {
                    destroy();
                }

                thread_record * alloc()
                {
                    thread_record * pRec;
                    cds::OS::ThreadId const nullThreadId = cds::OS::c_NullThreadId;
                    cds::OS::ThreadId const curThreadId  = cds::OS::get_current_thread_id();

                    // First, try to reuse a retired (non-active) HP record
                    for ( pRec = m_pHead.load( atomics::memory_order_acquire ); pRec; pRec = pRec->m_list.next_ ) {
                        cds::OS::ThreadId thId = nullThreadId;
                        if ( !pRec->m_list.thread_id_.compare_exchange_strong( thId, curThreadId, atomics::memory_order_acquire, atomics::memory_order_relaxed ))
                            continue;
                        return pRec;
                    }

                    // No records available for reuse
                    // Allocate and push a new record
                    pRec = allocator_type().New( curThreadId );

                    thread_record * pOldHead = m_pHead.load( atomics::memory_order_acquire );
                    do {
                        // Compiler barriers: assignment MUST BE inside the loop
                        CDS_COMPILER_RW_BARRIER;
                        pRec->m_list.next_ = pOldHead;
                        CDS_COMPILER_RW_BARRIER;
                    } while ( !m_pHead.compare_exchange_weak( pOldHead, pRec, atomics::memory_order_release, atomics::memory_order_acquire ));

                    return pRec;
                }

                void retire( thread_record * pRec )
                {
                    assert( pRec != nullptr );
                    pRec->m_list.thread_id_.store( cds::OS::c_NullThreadId, atomics::memory_order_release );
                }

                void detach_all()
                {
                    thread_record * pNext = nullptr;
                    cds::OS::ThreadId const nullThreadId = cds::OS::c_NullThreadId;

                    for ( thread_record * pRec = m_pHead.load( atomics::memory_order_acquire ); pRec; pRec = pNext ) {
                        pNext = pRec->m_list.next_;
                        if ( pRec->m_list.thread_id_.load( atomics::memory_order_acquire ) != nullThreadId ) {
                            retire( pRec );
                        }
                    }
                }

                thread_record * head( atomics::memory_order mo ) const
                {
                    return m_pHead.load( mo );
                }

            private:
                void destroy()
                {
                    allocator_type al;
                    CDS_DEBUG_ONLY( cds::OS::ThreadId const nullThreadId = cds::OS::c_NullThreadId; )
                    CDS_DEBUG_ONLY( cds::OS::ThreadId const mainThreadId = cds::OS::get_current_thread_id() ;)

                    thread_record * p = m_pHead.exchange( nullptr, atomics::memory_order_acquire );
                    while ( p ) {
                        thread_record * pNext = p->m_list.next_;

                        assert( p->m_list.thread_id_.load( atomics::memory_order_relaxed ) == nullThreadId
                            || p->m_list.thread_id_.load( atomics::memory_order_relaxed ) == mainThreadId
                            );

                        al.Delete( p );
                        p = pNext;
                    }
                }
            };
            //@endcond

            //@cond
            template <class ThreadGC>
            class scoped_lock {
            public:
                typedef ThreadGC                    thread_gc;
                typedef typename thread_gc::rcu_tag rcu_tag;

            public:
                scoped_lock()
                {
                    thread_gc::access_lock();
                }

                ~scoped_lock()
                {
                    thread_gc::access_unlock();
                }
            };
            //@endcond
        } // namespace details
        //@endcond

        // forwards
        //@cond
        template <typename RCUimpl> class gc;
        //@endcond

        /// Epoch-based retired ptr
        /**
            Retired pointer with additional epoch field that prevents early reclamation.
            This type of retired pointer is used in buffered RCU implementations.
        */
        struct epoch_retired_ptr: public retired_ptr
        {
            uint64_t    m_nEpoch;  ///< The epoch when the object has been retired

            //@cond
            epoch_retired_ptr()
            {}
            //@endcond

            /// Constructor creates a copy of \p rp retired pointer and saves \p nEpoch reclamation epoch.
            epoch_retired_ptr( retired_ptr const& rp, uint64_t nEpoch )
                : retired_ptr( rp )
                , m_nEpoch( nEpoch )
            {}
        };

    } // namespace urcu
} // namespace cds

#endif // #ifndef CDSLIB_URCU_DETAILS_BASE_H

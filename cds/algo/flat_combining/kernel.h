// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_ALGO_FLAT_COMBINING_KERNEL_H
#define CDSLIB_ALGO_FLAT_COMBINING_KERNEL_H

#include <cds/algo/flat_combining/defs.h>
#include <cds/algo/flat_combining/wait_strategy.h>

#include <cds/sync/spinlock.h>
#include <cds/details/allocator.h>
#include <cds/opt/options.h>
#include <cds/algo/int_algo.h>

namespace cds { namespace algo {

    /// @defgroup cds_flat_combining_intrusive Intrusive flat combining containers
    /// @defgroup cds_flat_combining_container Non-intrusive flat combining containers

    /// Flat combining
    /**
        @anchor cds_flat_combining_description
        Flat combining (FC) technique is invented by Hendler, Incze, Shavit and Tzafrir in their paper
        [2010] <i>"Flat Combining and the Synchronization-Parallelism Tradeoff"</i>.
        The technique converts a sequential data structure to its concurrent implementation.
        A few structures are added to the sequential implementation: a <i>global lock</i>,
        a <i>count</i> of the number of combining passes, and a pointer to the <i>head</i>
        of a <i>publication list</i>. The publication list is a list of thread-local records
        of a size proportional to the number of threads that are concurrently accessing the shared object.

        Each thread \p t accessing the structure to perform an invocation of some method \p f()
        on the shared object executes the following sequence of steps:
        <ol>
        <li>Write the invocation opcode and parameters (if any) of the method \p f() to be applied
        sequentially to the shared object in the <i>request</i> field of your thread local publication
        record (there is no need to use a load-store memory barrier). The <i>request</i> field will later
        be used to receive the response. If your thread local publication record is marked as active
        continue to step 2, otherwise continue to step 5.</li>
        <li>Check if the global lock is taken. If so (another thread is an active combiner), spin on the <i>request</i>
        field waiting for a response to the invocation (one can add a yield at this point to allow other threads
        on the same core to run). Once in a while while spinning check if the lock is still taken and that your
        record is active (you may use any of \p wait_strategy instead of spinning). If your record is inactive proceed to step 5.
        Once the response is available, reset the request field to null and return the response.</li>
        <li>If the lock is not taken, attempt to acquire it and become a combiner. If you fail,
        return to spinning in step 2.</li>
        <li>Otherwise, you hold the lock and are a combiner.
        <ul>
            <li>Increment the combining pass count by one.</li>
            <li>Execute a \p fc_apply() by traversing the publication list from the head,
            combining all non-null method call invocations, setting the <i>age</i> of each of these records
            to the current <i>count</i>, applying the combined method calls to the structure D, and returning
            responses to all the invocations. This traversal is guaranteed to be wait-free.</li>
            <li>If the <i>count</i> is such that a cleanup needs to be performed, traverse the publication
            list from the <i>head</i>. Starting from the second item (we always leave the item pointed to
            by the head in the list), remove from the publication list all records whose <i>age</i> is
            much smaller than the current <i>count</i>. This is done by removing the node and marking it
            as inactive.</li>
            <li>Release the lock.</li>
        </ul>
        <li>If you have no thread local publication record allocate one, marked as active. If you already
        have one marked as inactive, mark it as active. Execute a store-load memory barrier. Proceed to insert
        the record into the list with a successful CAS to the <i>head</i>. Then proceed to step 1.</li>
        </ol>

        As the test results show, the flat combining technique is suitable for non-intrusive containers
        like stack, queue, deque. For intrusive concurrent containers the flat combining demonstrates
        less impressive results.

        \ref cds_flat_combining_container "List of FC-based containers" in libcds.

        \ref cds_flat_combining_intrusive "List of intrusive FC-based containers" in libcds.
    */
    namespace flat_combining {

        /// Flat combining internal statistics
        template <typename Counter = cds::atomicity::event_counter >
        struct stat
        {
            typedef Counter counter_type;   ///< Event counter type

            counter_type    m_nOperationCount   ;   ///< How many operations have been performed
            counter_type    m_nCombiningCount   ;   ///< Combining call count
            counter_type    m_nCompactPublicationList; ///< Count of publication list compacting
            counter_type    m_nDeactivatePubRecord; ///< How many publication records were deactivated during compacting
            counter_type    m_nActivatePubRecord;   ///< Count of publication record activating
            counter_type    m_nPubRecordCreated ;   ///< Count of created publication records
            counter_type    m_nPubRecordDeleted ;   ///< Count of deleted publication records
            counter_type    m_nPassiveWaitCall;     ///< Count of passive waiting call (\p kernel::wait_for_combining())
            counter_type    m_nPassiveWaitIteration;///< Count of iteration inside passive waiting
            counter_type    m_nPassiveWaitWakeup;   ///< Count of forcing wake-up of passive wait cycle
            counter_type    m_nInvokeExclusive;     ///< Count of call \p kernel::invoke_exclusive()
            counter_type    m_nWakeupByNotifying;   ///< How many times the passive thread be waked up by a notification
            counter_type    m_nPassiveToCombiner;   ///< How many times the passive thread becomes the combiner

            /// Returns current combining factor
            /**
                Combining factor is how many operations perform in one combine pass:
                <tt>combining_factor := m_nOperationCount / m_nCombiningCount</tt>
            */
            double combining_factor() const
            {
                return m_nCombiningCount.get() ? double( m_nOperationCount.get()) / m_nCombiningCount.get() : 0.0;
            }

            //@cond
            void    onOperation()               { ++m_nOperationCount;          }
            void    onCombining()               { ++m_nCombiningCount;          }
            void    onCompactPublicationList()  { ++m_nCompactPublicationList;  }
            void    onDeactivatePubRecord()     { ++m_nDeactivatePubRecord;     }
            void    onActivatePubRecord()       { ++m_nActivatePubRecord;       }
            void    onCreatePubRecord()         { ++m_nPubRecordCreated;        }
            void    onDeletePubRecord()         { ++m_nPubRecordDeleted;        }
            void    onPassiveWait()             { ++m_nPassiveWaitCall;         }
            void    onPassiveWaitIteration()    { ++m_nPassiveWaitIteration;    }
            void    onPassiveWaitWakeup()       { ++m_nPassiveWaitWakeup;       }
            void    onInvokeExclusive()         { ++m_nInvokeExclusive;         }
            void    onWakeupByNotifying()       { ++m_nWakeupByNotifying;       }
            void    onPassiveToCombiner()       { ++m_nPassiveToCombiner;       }

            //@endcond
        };

        /// Flat combining dummy internal statistics
        struct empty_stat
        {
            //@cond
            void    onOperation()               const {}
            void    onCombining()               const {}
            void    onCompactPublicationList()  const {}
            void    onDeactivatePubRecord()     const {}
            void    onActivatePubRecord()       const {}
            void    onCreatePubRecord()         const {}
            void    onDeletePubRecord()         const {}
            void    onPassiveWait()             const {}
            void    onPassiveWaitIteration()    const {}
            void    onPassiveWaitWakeup()       const {}
            void    onInvokeExclusive()         const {}
            void    onWakeupByNotifying()       const {}
            void    onPassiveToCombiner()       const {}
            //@endcond
        };

        /// Type traits of \ref kernel class
        /**
            You can define different type traits for \ref kernel
            by specifying your struct based on \p %traits
            or by using \ref make_traits metafunction.
        */
        struct traits
        {
            typedef cds::sync::spin             lock_type;  ///< Lock type
            typedef cds::algo::flat_combining::wait_strategy::backoff< cds::backoff::delay_of<2>> wait_strategy; ///< Wait strategy
            typedef CDS_DEFAULT_ALLOCATOR       allocator;  ///< Allocator used for TLS data (allocating \p publication_record derivatives)
            typedef empty_stat                  stat;       ///< Internal statistics
            typedef opt::v::relaxed_ordering  memory_model; ///< /// C++ memory ordering model
        };

        /// Metafunction converting option list to traits
        /**
            \p Options are:
            - \p opt::lock_type - mutex type, default is \p cds::sync::spin
            - \p opt::wait_strategy - wait strategy, see \p wait_strategy namespace, default is \p wait_strategy::backoff.
            - \p opt::allocator - allocator type, default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::stat - internal statistics, possible type: \ref stat, \ref empty_stat (the default)
            - \p opt::memory_model - C++ memory ordering model.
                List of all available memory ordering see \p opt::memory_model.
                Default is \p cds::opt::v::relaxed_ordering
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

        /// The kernel of flat combining
        /**
            Template parameters:
            - \p PublicationRecord - a type derived from \ref publication_record
            - \p Traits - a type traits of flat combining, default is \p flat_combining::traits.
                \ref make_traits metafunction can be used to create type traits

            The kernel object should be a member of a container class. The container cooperates with flat combining
            kernel object. There are two ways to interact with the kernel:
            - One-by-one processing the active records of the publication list. This mode provides by \p combine() function:
              the container acquires its publication record by \p acquire_record(), fills its fields and calls
              \p combine() function of its kernel object. If the current thread becomes a combiner, the kernel
              calls \p fc_apply() function of the container for each active non-empty record. Then, the container
              should release its publication record by \p release_record(). Only one pass through the publication
              list is possible.
            - Batch processing - \p batch_combine() function. It this mode the container obtains access
              to entire publication list. This mode allows the container to perform an elimination, for example,
              the stack can collide \p push() and \p pop() requests. The sequence of invocations is the following:
              the container acquires its publication record by \p acquire_record(), fills its field and call
              \p batch_combine() function of its kernel object. If the current thread becomes a combiner,
              the kernel calls \p fc_process() function of the container passing two iterators pointing to
              the begin and the end of publication list (see \ref iterator class). The iterators allow
              multiple pass through active records of publication list. For each processed record the container
              should call \p operation_done() function. On the end, the container should release
              its record by \p release_record().
        */
        template <
            typename PublicationRecord
            ,typename Traits = traits
        >
        class kernel
        {
        public:
            typedef Traits   traits;                               ///< Type traits
            typedef typename traits::lock_type global_lock_type;   ///< Global lock type
            typedef typename traits::wait_strategy wait_strategy;  ///< Wait strategy type
            typedef typename traits::allocator allocator;          ///< Allocator type (used for allocating publication_record_type data)
            typedef typename traits::stat      stat;               ///< Internal statistics
            typedef typename traits::memory_model memory_model;    ///< C++ memory model

            typedef typename wait_strategy::template make_publication_record<PublicationRecord>::type publication_record_type; ///< Publication record type

        protected:
            //@cond
            typedef cds::details::Allocator< publication_record_type, allocator >   cxx11_allocator; ///< internal helper cds::details::Allocator
            typedef std::lock_guard<global_lock_type> lock_guard;
            //@endcond

        protected:
            atomics::atomic<unsigned int>  m_nCount;    ///< Total count of combining passes. Used as an age.
            publication_record_type*    m_pHead;        ///< Head of active publication list
            publication_record_type*    m_pAllocatedHead; ///< Head of allocated publication list
            boost::thread_specific_ptr< publication_record_type > m_pThreadRec;   ///< Thread-local publication record
            mutable global_lock_type    m_Mutex;        ///< Global mutex
            mutable stat                m_Stat;         ///< Internal statistics
            unsigned int const          m_nCompactFactor;    ///< Publication list compacting factor (the list will be compacted through \p %m_nCompactFactor combining passes)
            unsigned int const          m_nCombinePassCount; ///< Number of combining passes
            wait_strategy               m_waitStrategy;      ///< Wait strategy

        public:
            /// Initializes the object
            /**
                Compact factor = 1024

                Combiner pass count = 8
            */
            kernel()
                : kernel( 1024, 8 )
            {}

            /// Initializes the object
            kernel(
                unsigned int nCompactFactor  ///< Publication list compacting factor (the list will be compacted through \p nCompactFactor combining passes)
                ,unsigned int nCombinePassCount ///< Number of combining passes for combiner thread
                )
                : m_nCount(0)
                , m_pHead( nullptr )
                , m_pAllocatedHead( nullptr )
                , m_pThreadRec( tls_cleanup )
                , m_nCompactFactor( static_cast<unsigned>( cds::beans::ceil2( static_cast<size_t>( nCompactFactor )) - 1 ))   // binary mask
                , m_nCombinePassCount( nCombinePassCount )
            {
                assert( m_pThreadRec.get() == nullptr );
                publication_record_type* pRec = cxx11_allocator().New();
                m_pAllocatedHead =
                    m_pHead = pRec;
                m_pThreadRec.reset( pRec );
                m_Stat.onCreatePubRecord();
            }

            /// Destroys the object and all publication records
            ~kernel()
            {
                m_pThreadRec.reset();   // calls tls_cleanup()

                // delete all publication records
                for ( publication_record* p = m_pAllocatedHead; p; ) {
                    publication_record * pRec = p;
                    p = p->pNextAllocated.load( memory_model::memory_order_relaxed );
                    free_publication_record( static_cast<publication_record_type *>( pRec ));
                }
            }

            /// Gets publication list record for the current thread
            /**
                If there is no publication record for the current thread
                the function allocates it.
            */
            publication_record_type * acquire_record()
            {
                publication_record_type * pRec = m_pThreadRec.get();
                if ( !pRec ) {
                    // Allocate new publication record
                    pRec = cxx11_allocator().New();
                    m_pThreadRec.reset( pRec );
                    m_Stat.onCreatePubRecord();

                    // Insert in allocated list
                    assert( m_pAllocatedHead != nullptr );
                    publication_record* p = m_pAllocatedHead->pNextAllocated.load( memory_model::memory_order_relaxed );
                    do {
                        pRec->pNextAllocated.store( p, memory_model::memory_order_release );
                    } while ( !m_pAllocatedHead->pNextAllocated.compare_exchange_weak( p, pRec, memory_model::memory_order_release, atomics::memory_order_acquire ));

                    publish( pRec );
                }
                else if ( pRec->nState.load( memory_model::memory_order_acquire ) != active )
                    publish( pRec );

                assert( pRec->op() == req_EmptyRecord );

                return pRec;
            }

            /// Marks publication record for the current thread as empty
            void release_record( publication_record_type * pRec )
            {
                assert( pRec->is_done());
                pRec->nRequest.store( req_EmptyRecord, memory_model::memory_order_release );
            }

            /// Trying to execute operation \p nOpId
            /**
                \p pRec is the publication record acquiring by \ref acquire_record earlier.
                \p owner is a container that is owner of flat combining kernel object.
                As a result the current thread can become a combiner or can wait for
                another combiner performs \p pRec operation.

                If the thread becomes a combiner, the kernel calls \p owner.fc_apply
                for each active non-empty publication record.
            */
            template <class Container>
            void combine( unsigned int nOpId, publication_record_type * pRec, Container& owner )
            {
                assert( nOpId >= req_Operation );
                assert( pRec );

                pRec->nRequest.store( nOpId, memory_model::memory_order_release );
                m_Stat.onOperation();

                try_combining( owner, pRec );
            }

            /// Trying to execute operation \p nOpId in batch-combine mode
            /**
                \p pRec is the publication record acquiring by \p acquire_record() earlier.
                \p owner is a container that owns flat combining kernel object.
                As a result the current thread can become a combiner or can wait for
                another combiner performs \p pRec operation.

                If the thread becomes a combiner, the kernel calls \p owner.fc_process()
                giving the container the full access over publication list. This function
                is useful for an elimination technique if the container supports any kind of
                that. The container can perform multiple pass through publication list.

                \p owner.fc_process() has two arguments - forward iterators on begin and end of
                publication list, see \ref iterator class. For each processed record the container
                should call \p operation_done() function to mark the record as processed.

                On the end of \p %batch_combine the \p combine() function is called
                to process rest of publication records.
            */
            template <class Container>
            void batch_combine( unsigned int nOpId, publication_record_type* pRec, Container& owner )
            {
                assert( nOpId >= req_Operation );
                assert( pRec );

                pRec->nRequest.store( nOpId, memory_model::memory_order_release );
                m_Stat.onOperation();

                try_batch_combining( owner, pRec );
            }

            /// Invokes \p Func in exclusive mode
            /**
                Some operation in flat combining containers should be called in exclusive mode
                i.e the current thread should become the combiner to process the operation.
                The typical example is \p empty() function.

                \p %invoke_exclusive() allows do that: the current thread becomes the combiner,
                invokes \p f exclusively but unlike a typical usage the thread does not process any pending request.
                Instead, after end of \p f call the current thread wakes up a pending thread if any.
            */
            template <typename Func>
            void invoke_exclusive( Func f )
            {
                {
                    lock_guard l( m_Mutex );
                    f();
                }
                m_waitStrategy.wakeup( *this );
                m_Stat.onInvokeExclusive();
            }

            /// Marks \p rec as executed
            /**
                This function should be called by container if \p batch_combine() mode is used.
                For usual combining (see \p combine()) this function is excess.
            */
            void operation_done( publication_record& rec )
            {
                rec.nRequest.store( req_Response, memory_model::memory_order_release );
                m_waitStrategy.notify( *this, static_cast<publication_record_type&>( rec ));
            }

            /// Internal statistics
            stat const& statistics() const
            {
                return m_Stat;
            }

            //@cond
            // For container classes based on flat combining
            stat& internal_statistics() const
            {
                return m_Stat;
            }
            //@endcond

            /// Returns the compact factor
            unsigned int compact_factor() const
            {
                return m_nCompactFactor + 1;
            }

            /// Returns number of combining passes for combiner thread
            unsigned int combine_pass_count() const
            {
                return m_nCombinePassCount;
            }

        public:
            /// Publication list iterator
            /**
                Iterators are intended for batch processing by container's
                \p fc_process function.
                The iterator allows iterate through active publication list.
            */
            class iterator
            {
                //@cond
                friend class kernel;
                publication_record_type * m_pRec;
                //@endcond

            protected:
                //@cond
                iterator( publication_record_type * pRec )
                    : m_pRec( pRec )
                {
                    skip_inactive();
                }

                void skip_inactive()
                {
                    while ( m_pRec && (m_pRec->nState.load( memory_model::memory_order_acquire ) != active
                                    || m_pRec->op( memory_model::memory_order_relaxed) < req_Operation ))
                    {
                        m_pRec = static_cast<publication_record_type*>(m_pRec->pNext.load( memory_model::memory_order_acquire ));
                    }
                }
                //@endcond

            public:
                /// Initializes an empty iterator object
                iterator()
                    : m_pRec( nullptr )
                {}

                /// Copy ctor
                iterator( iterator const& src )
                    : m_pRec( src.m_pRec )
                {}

                /// Pre-increment
                iterator& operator++()
                {
                    assert( m_pRec );
                    m_pRec = static_cast<publication_record_type *>( m_pRec->pNext.load( memory_model::memory_order_acquire ));
                    skip_inactive();
                    return *this;
                }

                /// Post-increment
                iterator operator++(int)
                {
                    assert( m_pRec );
                    iterator it(*this);
                    ++(*this);
                    return it;
                }

                /// Dereference operator, can return \p nullptr
                publication_record_type* operator ->()
                {
                    return m_pRec;
                }

                /// Dereference operator, the iterator should not be an end iterator
                publication_record_type& operator*()
                {
                    assert( m_pRec );
                    return *m_pRec;
                }

                /// Iterator equality
                friend bool operator==( iterator it1, iterator it2 )
                {
                    return it1.m_pRec == it2.m_pRec;
                }

                /// Iterator inequality
                friend bool operator!=( iterator it1, iterator it2 )
                {
                    return !( it1 == it2 );
                }
            };

            /// Returns an iterator to the first active publication record
            iterator begin()    { return iterator(m_pHead); }

            /// Returns an iterator to the end of publication list. Should not be dereferenced.
            iterator end()      { return iterator(); }

        public:
            /// Gets current value of \p rec.nRequest
            /**
                This function is intended for invoking from a wait strategy
            */
            int get_operation( publication_record& rec )
            {
                return rec.op( memory_model::memory_order_acquire );
            }

            /// Wakes up any waiting thread
            /**
                This function is intended for invoking from a wait strategy
            */
            void wakeup_any()
            {
                publication_record* pRec = m_pHead;
                while ( pRec ) {
                    if ( pRec->nState.load( memory_model::memory_order_acquire ) == active
                      && pRec->op( memory_model::memory_order_acquire ) >= req_Operation )
                    {
                        m_waitStrategy.notify( *this, static_cast<publication_record_type&>( *pRec ));
                        break;
                    }
                    pRec = pRec->pNext.load( memory_model::memory_order_acquire );
                }
            }

        private:
            //@cond
            static void tls_cleanup( publication_record_type* pRec )
            {
                // Thread done
                // pRec that is TLS data should be excluded from publication list
                pRec->nState.store( removed, memory_model::memory_order_release );
            }

            void free_publication_record( publication_record_type* pRec )
            {
                cxx11_allocator().Delete( pRec );
                m_Stat.onDeletePubRecord();
            }

            void publish( publication_record_type* pRec )
            {
                assert( pRec->nState.load( memory_model::memory_order_relaxed ) == inactive );

                pRec->nAge.store( m_nCount.load(memory_model::memory_order_relaxed), memory_model::memory_order_relaxed );
                pRec->nState.store( active, memory_model::memory_order_relaxed );

                // Insert record to publication list
                if ( m_pHead != static_cast<publication_record *>(pRec)) {
                    publication_record * p = m_pHead->pNext.load( memory_model::memory_order_relaxed );
                    if ( p != static_cast<publication_record *>( pRec )) {
                        do {
                            pRec->pNext.store( p, memory_model::memory_order_release );
                            // Failed CAS changes p
                        } while ( !m_pHead->pNext.compare_exchange_weak( p, static_cast<publication_record *>(pRec),
                            memory_model::memory_order_release, atomics::memory_order_acquire ));
                        m_Stat.onActivatePubRecord();
                    }
                }
            }

            void republish( publication_record_type* pRec )
            {
                if ( pRec->nState.load( memory_model::memory_order_relaxed ) != active ) {
                    // The record has been excluded from publication list. Reinsert it
                    publish( pRec );
                }
            }

            template <class Container>
            void try_combining( Container& owner, publication_record_type* pRec )
            {
                if ( m_Mutex.try_lock()) {
                    // The thread becomes a combiner
                    lock_guard l( m_Mutex, std::adopt_lock_t());

                    // The record pRec can be excluded from publication list. Re-publish it
                    republish( pRec );

                    combining( owner );
                    assert( pRec->op( memory_model::memory_order_relaxed ) == req_Response );
                }
                else {
                    // There is another combiner, wait while it executes our request
                    if ( !wait_for_combining( pRec )) {
                        // The thread becomes a combiner
                        lock_guard l( m_Mutex, std::adopt_lock_t());

                        // The record pRec can be excluded from publication list. Re-publish it
                        republish( pRec );

                        combining( owner );
                        assert( pRec->op( memory_model::memory_order_relaxed ) == req_Response );
                    }
                }
            }

            template <class Container>
            void try_batch_combining( Container& owner, publication_record_type * pRec )
            {
                if ( m_Mutex.try_lock()) {
                    // The thread becomes a combiner
                    lock_guard l( m_Mutex, std::adopt_lock_t());

                    // The record pRec can be excluded from publication list. Re-publish it
                    republish( pRec );

                    batch_combining( owner );
                    assert( pRec->op( memory_model::memory_order_relaxed ) == req_Response );
                }
                else {
                    // There is another combiner, wait while it executes our request
                    if ( !wait_for_combining( pRec )) {
                        // The thread becomes a combiner
                        lock_guard l( m_Mutex, std::adopt_lock_t());

                        // The record pRec can be excluded from publication list. Re-publish it
                        republish( pRec );

                        batch_combining( owner );
                        assert( pRec->op( memory_model::memory_order_relaxed ) == req_Response );
                    }
                }
            }

            template <class Container>
            void combining( Container& owner )
            {
                // The thread is a combiner
                assert( !m_Mutex.try_lock());

                unsigned int const nCurAge = m_nCount.fetch_add( 1, memory_model::memory_order_relaxed ) + 1;

                unsigned int nEmptyPassCount = 0;
                unsigned int nUsefulPassCount = 0;
                for ( unsigned int nPass = 0; nPass < m_nCombinePassCount; ++nPass ) {
                    if ( combining_pass( owner, nCurAge ))
                        ++nUsefulPassCount;
                    else if ( ++nEmptyPassCount > nUsefulPassCount )
                        break;
                }

                m_Stat.onCombining();
                if ( ( nCurAge & m_nCompactFactor ) == 0 )
                    compact_list( nCurAge );
            }

            template <class Container>
            bool combining_pass( Container& owner, unsigned int nCurAge )
            {
                publication_record* p = m_pHead;
                bool bOpDone = false;
                while ( p ) {
                    switch ( p->nState.load( memory_model::memory_order_acquire )) {
                    case active:
                        if ( p->op( memory_model::memory_order_acquire ) >= req_Operation ) {
                            p->nAge.store( nCurAge, memory_model::memory_order_relaxed );
                            owner.fc_apply( static_cast<publication_record_type*>( p ));
                            operation_done( *p );
                            bOpDone = true;
                        }
                        break;
                    case inactive:
                        break;
                    case removed:
                        // Such record will be removed on compacting phase
                        break;
                    default:
                        /// ??? That is impossible
                        assert( false );
                    }
                    p = p->pNext.load( memory_model::memory_order_acquire );
                }
                return bOpDone;
            }

            template <class Container>
            void batch_combining( Container& owner )
            {
                // The thread is a combiner
                assert( !m_Mutex.try_lock());

                unsigned int const nCurAge = m_nCount.fetch_add( 1, memory_model::memory_order_relaxed ) + 1;

                for ( unsigned int nPass = 0; nPass < m_nCombinePassCount; ++nPass )
                    owner.fc_process( begin(), end());

                combining_pass( owner, nCurAge );
                m_Stat.onCombining();
                if ( ( nCurAge & m_nCompactFactor ) == 0 )
                    compact_list( nCurAge );
            }

            bool wait_for_combining( publication_record_type* pRec )
            {
                m_waitStrategy.prepare( *pRec );
                m_Stat.onPassiveWait();

                while ( pRec->op( memory_model::memory_order_acquire ) != req_Response ) {
                    // The record can be excluded from publication list. Reinsert it
                    republish( pRec );

                    m_Stat.onPassiveWaitIteration();

                    // Wait while operation processing
                    if ( m_waitStrategy.wait( *this, *pRec ))
                        m_Stat.onWakeupByNotifying();

                    if ( m_Mutex.try_lock()) {
                        if ( pRec->op( memory_model::memory_order_acquire ) == req_Response ) {
                            // Operation is done
                            m_Mutex.unlock();

                            // Wake up a pending threads
                            m_waitStrategy.wakeup( *this );
                            m_Stat.onPassiveWaitWakeup();

                            break;
                        }
                        // The thread becomes a combiner
                        m_Stat.onPassiveToCombiner();
                        return false;
                    }
                }
                return true;
            }

            void compact_list( unsigned int nCurAge )
            {
                // Compacts publication list
                // This function is called only by combiner thread

            try_again:
                publication_record * pPrev = m_pHead;
                for ( publication_record * p = pPrev->pNext.load( memory_model::memory_order_acquire ); p; ) {
                    switch ( p->nState.load( memory_model::memory_order_relaxed )) {
                    case active:
                        if ( p->nAge.load( memory_model::memory_order_relaxed ) + m_nCompactFactor < nCurAge )
                        {
                            publication_record * pNext = p->pNext.load( memory_model::memory_order_relaxed );
                            if ( pPrev->pNext.compare_exchange_strong( p, pNext,
                                memory_model::memory_order_acquire, atomics::memory_order_relaxed ))
                            {
                                p->nState.store( inactive, memory_model::memory_order_release );
                                p = pNext;
                                m_Stat.onDeactivatePubRecord();
                                continue;
                            }
                        }
                        break;

                    case removed:
                        publication_record * pNext = p->pNext.load( memory_model::memory_order_acquire );
                        if ( cds_likely( pPrev->pNext.compare_exchange_strong( p, pNext, memory_model::memory_order_acquire, atomics::memory_order_relaxed ))) {
                            p = pNext;
                            continue;
                        }
                        else {
                            // CAS can be failed only in beginning of list
                            assert( pPrev == m_pHead );
                            goto try_again;
                        }
                    }
                    pPrev = p;
                    p = p->pNext.load( memory_model::memory_order_acquire );
                }

                // Iterate over allocated list to find removed records
                pPrev = m_pAllocatedHead;
                for ( publication_record * p = pPrev->pNextAllocated.load( memory_model::memory_order_acquire ); p; ) {
                    if ( p->nState.load( memory_model::memory_order_relaxed ) == removed ) {
                        publication_record * pNext = p->pNextAllocated.load( memory_model::memory_order_relaxed );
                        if ( pPrev->pNextAllocated.compare_exchange_strong( p, pNext, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                            free_publication_record( static_cast<publication_record_type *>( p ));
                            p = pNext;
                            continue;
                        }
                    }

                    pPrev = p;
                    p = p->pNextAllocated.load( memory_model::memory_order_relaxed );
                }

                m_Stat.onCompactPublicationList();
            }
            //@endcond
        };

        //@cond
        class container
        {
        public:
            template <typename PubRecord>
            void fc_apply( PubRecord * )
            {
                assert( false );
            }

            template <typename Iterator>
            void fc_process( Iterator, Iterator )
            {
                assert( false );
            }
        };
        //@endcond

    } // namespace flat_combining
}} // namespace cds::algo

/*
  CppMem model  (http://svr-pes20-cppmem.cl.cam.ac.uk/cppmem/)

  // Combiner thread - slave (waiting) thread
int main() {
      atomic_int y = 0;  // pRec->op
      int x = 0;         // pRec->data
   {{{
      { // slave thread (not combiner)
        // Op data
        x = 1;
        // Annotate request (op)
        y.store(1, release);
        // Wait while request done
        y.load(acquire).readsvalue(2);
        // Read result
        r2=x;
      }
   |||
      { // Combiner thread
        // Read request (op)
        r1=y.load(acquire).readsvalue(1);
        // Execute request - change request data
        x = 2;
        // store "request processed" flag (pRec->op := req_Response)
        y.store(2, release);
      }
   }}};
   return 0;
}

*/

#endif // #ifndef CDSLIB_ALGO_FLAT_COMBINING_KERNEL_H

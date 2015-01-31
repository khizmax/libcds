//$$CDS-header$$

#ifndef CDSLIB_ALGO_FLAT_COMBINING_H
#define CDSLIB_ALGO_FLAT_COMBINING_H

#include <mutex>
#include <cds/algo/atomic.h>
#include <cds/details/allocator.h>
#include <cds/algo/backoff_strategy.h>
#include <cds/sync/spinlock.h>
#include <cds/opt/options.h>
#include <cds/algo/int_algo.h>
#include <boost/thread/tss.hpp>     // thread_specific_ptr

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

        Each thread \p t accessing the structure to perform an invocation of some method \p m
        on the shared object executes the following sequence of steps:
        <ol>
        <li>Write the invocation opcode and parameters (if any) of the method \p m to be applied
        sequentially to the shared object in the <i>request</i> field of your thread local publication
        record (there is no need to use a load-store memory barrier). The <i>request</i> field will later
        be used to receive the response. If your thread local publication record is marked as active
        continue to step 2, otherwise continue to step 5.</li>
        <li>Check if the global lock is taken. If so (another thread is an active combiner), spin on the <i>request</i>
        field waiting for a response to the invocation (one can add a yield at this point to allow other threads
        on the same core to run). Once in a while while spinning check if the lock is still taken and that your
        record is active. If your record is inactive proceed to step 5. Once the response is available,
        reset the request field to null and return the response.</li>
        <li>If the lock is not taken, attempt to acquire it and become a combiner. If you fail,
        return to spinning in step 2.</li>
        <li>Otherwise, you hold the lock and are a combiner.
        <ul>
            <li>Increment the combining pass count by one.</li>
            <li>Execute a \p fc_apply() by traversing the publication list from the head,
            combining all nonnull method call invocations, setting the <i>age</i> of each of these records
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

        /// Special values of publication_record::nRequest
        enum request_value
        {
            req_EmptyRecord,    ///< Publication record is empty
            req_Response,       ///< Operation is done

            req_Operation       ///< First operation id for derived classes
        };

        /// publication_record state
        enum record_state {
            inactive,       ///< Record is inactive
            active,         ///< Record is active
            removed         ///< Record should be removed
        };

        /// Record of publication list
        /**
            Each data structure based on flat combining contains a class derived from \p %publication_record
        */
        struct publication_record {
            atomics::atomic<unsigned int>    nRequest;   ///< Request field (depends on data structure)
            atomics::atomic<unsigned int>    nState;     ///< Record state: inactive, active, removed
            unsigned int                        nAge;       ///< Age of the record
            atomics::atomic<publication_record *> pNext; ///< Next record in publication list
            void *                              pOwner;    ///< [internal data] Pointer to \ref kernel object that manages the publication list

            /// Initializes publication record
            publication_record()
                : nRequest( req_EmptyRecord )
                , nState( inactive )
                , nAge(0)
                , pNext( nullptr )
                , pOwner( nullptr )
            {}

            /// Returns the value of \p nRequest field
            unsigned int op() const
            {
                return nRequest.load( atomics::memory_order_relaxed );
            }

            /// Checks if the operation is done
            bool is_done() const
            {
                return nRequest.load( atomics::memory_order_relaxed ) == req_Response;
            }
        };

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
            counter_type    m_nPubRecordDeteted ;   ///< Count of deleted publication records
            counter_type    m_nAcquirePubRecCount;  ///< Count of acquiring publication record
            counter_type    m_nReleasePubRecCount;  ///< Count on releasing publication record

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
            void    onOperation()               { ++m_nOperationCount; }
            void    onCombining()               { ++m_nCombiningCount; }
            void    onCompactPublicationList()  { ++m_nCompactPublicationList; }
            void    onDeactivatePubRecord()     { ++m_nDeactivatePubRecord; }
            void    onActivatPubRecord()        { ++m_nActivatePubRecord; }
            void    onCreatePubRecord()         { ++m_nPubRecordCreated; }
            void    onDeletePubRecord()         { ++m_nPubRecordDeteted; }
            void    onAcquirePubRecord()        { ++m_nAcquirePubRecCount; }
            void    onReleasePubRecord()        { ++m_nReleasePubRecCount; }
            //@endcond
        };

        /// Flat combining dummy internal statistics
        struct empty_stat
        {
            //@cond
            void    onOperation()               {}
            void    onCombining()               {}
            void    onCompactPublicationList()  {}
            void    onDeactivatePubRecord()     {}
            void    onActivatPubRecord()        {}
            void    onCreatePubRecord()         {}
            void    onDeletePubRecord()         {}
            void    onAcquirePubRecord()        {}
            void    onReleasePubRecord()        {}
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
            typedef cds::backoff::delay_of<2>   back_off;   ///< Back-off strategy
            typedef CDS_DEFAULT_ALLOCATOR       allocator;  ///< Allocator used for TLS data (allocating publication_record derivatives)
            typedef empty_stat                  stat;       ///< Internal statistics
            typedef opt::v::relaxed_ordering  memory_model; ///< /// C++ memory ordering model
        };

        /// Metafunction converting option list to traits
        /**
            \p Options are:
            - \p opt::lock_type - mutex type, default is \p cds::sync::spin
            - \p opt::back_off - back-off strategy, defalt is \p cds::backoff::delay_of<2>
            - \p opt::allocator - allocator type, default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::stat - internal statistics, possible type: \ref stat, \ref empty_stat (the default)
            - \p opt::memory_model - C++ memory ordering model.
                List of all available memory ordering see opt::memory_model.
                Default if cds::opt::v:relaxed_ordering
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
            - One-by-one processing the active records of the publication list. This mode provides \ref combine function:
              the container acquires its publication record by \ref acquire_record, fills its fields and calls
              \p combine function of its kernel object. If the current thread becomes a combiner, the kernel
              calls \p fc_apply function of the container for each active non-empty record. Then, the container
              should release its publication record by \ref release_record. Only one pass through the publication
              list is possible.
            - Batch processing (\ref batch_combine function). It this mode the container obtains access
              to entire publication list. This mode allows the container to perform an elimination, for example,
              the stack can collide \p push and \p pop requests. The sequence of invocations is the following:
              the container acquires its publication record by \ref acquire_record, fills its field and call
              \p batch_combine function of its kernel object. If the current thread becomes a combiner,
              the kernel calls \p fc_process function of the container passing two iterators pointing to
              begin and end of publication list (see \ref iterator class). The iterators allows
              multiple pass through active records of publication list. For each processed record the container
              should call \ref operation_done function. On the end, the container should release
              its record by \ref release_record.
        */
        template <
            typename PublicationRecord
            ,typename Traits = traits
        >
        class kernel
        {
        public:
            typedef PublicationRecord   publication_record_type;   ///< publication record type
            typedef Traits   traits;                               ///< Type traits
            typedef typename traits::lock_type global_lock_type;   ///< Global lock type
            typedef typename traits::back_off  back_off;           ///< back-off strategy type
            typedef typename traits::allocator allocator;          ///< Allocator type (used for allocating publication_record_type data)
            typedef typename traits::stat      stat;               ///< Internal statistics
            typedef typename traits::memory_model memory_model;    ///< C++ memory model

        protected:
            //@cond
            typedef cds::details::Allocator< publication_record_type, allocator >   cxx11_allocator; ///< internal helper cds::details::Allocator
            typedef std::lock_guard<global_lock_type> lock_guard;
            //@endcond

        protected:
            unsigned int                m_nCount;   ///< Count of combining passes
            publication_record_type *   m_pHead;    ///< Head of publication list
            boost::thread_specific_ptr< publication_record_type >   m_pThreadRec;   ///< Thread-local publication record
            mutable global_lock_type    m_Mutex;    ///< Global mutex
            mutable stat                m_Stat;     ///< Internal statistics
            unsigned int const          m_nCompactFactor; ///< Publication list compacting factor (the list will be compacted through \p %m_nCompactFactor combining passes)
            unsigned int const          m_nCombinePassCount; ///< Number of combining passes

        public:
            /// Initializes the object
            /**
                Compact factor = 64

                Combiner pass count = 8
            */
            kernel()
                : m_nCount(0)
                , m_pHead( nullptr )
                , m_pThreadRec( tls_cleanup )
                , m_nCompactFactor( 64 - 1 ) // binary mask
                , m_nCombinePassCount( 8 )
            {
                init();
            }

            /// Initializes the object
            kernel(
                unsigned int nCompactFactor  ///< Publication list compacting factor (the list will be compacted through \p nCompactFactor combining passes)
                ,unsigned int nCombinePassCount ///< Number of combining passes for combiner thread
                )
                : m_nCount(0)
                , m_pHead( nullptr )
                , m_pThreadRec( tls_cleanup )
                , m_nCompactFactor( (unsigned int)( cds::beans::ceil2( nCompactFactor ) - 1 ))   // binary mask
                , m_nCombinePassCount( nCombinePassCount )
            {
                init();
            }

            /// Destroys the objects and mark all publication records as inactive
            ~kernel()
            {
                // mark all publication record as detached
                for ( publication_record * p = m_pHead; p; p = p->pNext.load( memory_model::memory_order_relaxed ))
                    p->pOwner = nullptr;
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
                    pRec->pOwner = reinterpret_cast<void *>( this );
                    m_pThreadRec.reset( pRec );
                    m_Stat.onCreatePubRecord();
                }

                if ( pRec->nState.load( memory_model::memory_order_acquire ) != active )
                    publish( pRec );

                assert( pRec->nRequest.load( memory_model::memory_order_relaxed ) == req_EmptyRecord );

                m_Stat.onAcquirePubRecord();
                return pRec;
            }

            /// Marks publication record for the current thread as empty
            void release_record( publication_record_type * pRec )
            {
                assert( pRec->is_done() );
                pRec->nRequest.store( req_EmptyRecord, memory_model::memory_order_relaxed );
                m_Stat.onReleasePubRecord();
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
                //assert( pRec->nState.load( memory_model::memory_order_relaxed ) == active );
                pRec->nRequest.store( nOpId, memory_model::memory_order_release );

                m_Stat.onOperation();

                try_combining( owner, pRec );
            }

            /// Trying to execute operation \p nOpId in batch-combine mode
            /**
                \p pRec is the publication record acquiring by \ref acquire_record earlier.
                \p owner is a container that owns flat combining kernel object.
                As a result the current thread can become a combiner or can wait for
                another combiner performs \p pRec operation.

                If the thread becomes a combiner, the kernel calls \p owner.fc_process
                giving the container the full access over publication list. This function
                is useful for an elimination technique if the container supports any kind of
                that. The container can perform multiple pass through publication list.

                \p owner.fc_process has two arguments - forward iterators on begin and end of
                publication list, see \ref iterator class. For each processed record the container
                should call \ref operation_done function to mark the record as processed.

                On the end of \p %batch_combine the \ref combine function is called
                to process rest of publication records.
            */
            template <class Container>
            void batch_combine( unsigned int nOpId, publication_record_type * pRec, Container& owner )
            {
                assert( nOpId >= req_Operation );
                assert( pRec );
                //assert( pRec->nState.load( memory_model::memory_order_relaxed ) == active );
                pRec->nRequest.store( nOpId, memory_model::memory_order_release );

                m_Stat.onOperation();

                try_batch_combining( owner, pRec );
            }

            /// Waits for end of combining
            void wait_while_combining() const
            {
                lock_guard l( m_Mutex );
            }

            /// Marks \p rec as executed
            /**
                This function should be called by container if batch_combine mode is used.
                For usual combining (see \ref combine) this function is excess.
            */
            void operation_done( publication_record& rec )
            {
                rec.nRequest.store( req_Response, memory_model::memory_order_release );
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
                                    || m_pRec->nRequest.load( memory_model::memory_order_relaxed) < req_Operation ))
                    {
                        m_pRec = static_cast<publication_record_type *>(m_pRec->pNext.load( memory_model::memory_order_acquire ));
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
                publication_record_type * operator ->()
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

        private:
            //@cond
            static void tls_cleanup( publication_record_type * pRec )
            {
                // Thread done
                // pRec that is TLS data should be excluded from publication list
                if ( pRec ) {
                    if ( pRec->nState.load(memory_model::memory_order_relaxed) == active && pRec->pOwner ) {
                        // record is active and kernel is alive
                        unsigned int nState = active;
                        pRec->nState.compare_exchange_strong( nState, removed, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    }
                    else {
                        // record is not in publication list or kernel already deleted
                        cxx11_allocator().Delete( pRec );
                    }
                }
            }

            void init()
            {
                assert( m_pThreadRec.get() == nullptr );
                publication_record_type * pRec = cxx11_allocator().New();
                m_pHead = pRec;
                pRec->pOwner = this;
                m_pThreadRec.reset( pRec );
                m_Stat.onCreatePubRecord();
            }

            void publish( publication_record_type * pRec )
            {
                assert( pRec->nState.load( memory_model::memory_order_relaxed ) == inactive );

                pRec->nAge = m_nCount;
                pRec->nState.store( active, memory_model::memory_order_release );

                // Insert record to publication list
                if ( m_pHead != static_cast<publication_record *>(pRec) ) {
                    publication_record * p = m_pHead->pNext.load(memory_model::memory_order_relaxed);
                    if ( p != static_cast<publication_record *>( pRec )) {
                        do {
                            pRec->pNext = p;
                            // Failed CAS changes p
                        } while ( !m_pHead->pNext.compare_exchange_weak( p, static_cast<publication_record *>(pRec),
                            memory_model::memory_order_release, atomics::memory_order_relaxed ));
                        m_Stat.onActivatPubRecord();
                    }
                }
            }

            void republish( publication_record_type * pRec )
            {
                if ( pRec->nState.load( memory_model::memory_order_relaxed ) != active ) {
                    // The record has been excluded from publication list. Reinsert it
                    publish( pRec );
                }
            }

            template <class Container>
            void try_combining( Container& owner, publication_record_type * pRec )
            {
                if ( m_Mutex.try_lock() ) {
                    // The thread becomes a combiner
                    lock_guard l( m_Mutex, std::adopt_lock_t() );

                    // The record pRec can be excluded from publication list. Re-publish it
                    republish( pRec );

                    combining( owner );
                    assert( pRec->nRequest.load( memory_model::memory_order_relaxed ) == req_Response );
                }
                else {
                    // There is another combiner, wait while it executes our request
                    if ( !wait_for_combining( pRec ) ) {
                        // The thread becomes a combiner
                        lock_guard l( m_Mutex, std::adopt_lock_t() );

                        // The record pRec can be excluded from publication list. Re-publish it
                        republish( pRec );

                        combining( owner );
                        assert( pRec->nRequest.load( memory_model::memory_order_relaxed ) == req_Response );
                    }
                }
            }

            template <class Container>
            void try_batch_combining( Container& owner, publication_record_type * pRec )
            {
                if ( m_Mutex.try_lock() ) {
                    // The thread becomes a combiner
                    lock_guard l( m_Mutex, std::adopt_lock_t() );

                    // The record pRec can be excluded from publication list. Re-publish it
                    republish( pRec );

                    batch_combining( owner );
                    assert( pRec->nRequest.load( memory_model::memory_order_relaxed ) == req_Response );
                }
                else {
                    // There is another combiner, wait while it executes our request
                    if ( !wait_for_combining( pRec ) ) {
                        // The thread becomes a combiner
                        lock_guard l( m_Mutex, std::adopt_lock_t() );

                        // The record pRec can be excluded from publication list. Re-publish it
                        republish( pRec );

                        batch_combining( owner );
                        assert( pRec->nRequest.load( memory_model::memory_order_relaxed ) == req_Response );
                    }
                }
            }

            template <class Container>
            void combining( Container& owner )
            {
                // The thread is a combiner
                assert( !m_Mutex.try_lock() );

                unsigned int const nCurAge = ++m_nCount;

                for ( unsigned int nPass = 0; nPass < m_nCombinePassCount; ++nPass )
                    if ( !combining_pass( owner, nCurAge ))
                        break;

                m_Stat.onCombining();
                if ( (nCurAge & m_nCompactFactor) == 0 )
                    compact_list( nCurAge );
            }

            template <class Container>
            bool combining_pass( Container& owner, unsigned int nCurAge )
            {
                publication_record * pPrev = nullptr;
                publication_record * p = m_pHead;
                bool bOpDone = false;
                while ( p ) {
                    switch ( p->nState.load( memory_model::memory_order_acquire )) {
                        case active:
                            if ( p->op() >= req_Operation ) {
                                p->nAge = nCurAge;
                                owner.fc_apply( static_cast<publication_record_type *>(p) );
                                operation_done( *p );
                                bOpDone = true;
                            }
                            break;
                        case inactive:
                            // Only m_pHead can be inactive in the publication list
                            assert( p == m_pHead );
                            break;
                        case removed:
                            // The record should be removed
                            p = unlink_and_delete_record( pPrev, p );
                            continue;
                        default:
                            /// ??? That is impossible
                            assert(false);
                    }
                    pPrev = p;
                    p = p->pNext.load( memory_model::memory_order_acquire );
                }
                return bOpDone;
            }

            template <class Container>
            void batch_combining( Container& owner )
            {
                // The thread is a combiner
                assert( !m_Mutex.try_lock() );

                unsigned int const nCurAge = ++m_nCount;

                for ( unsigned int nPass = 0; nPass < m_nCombinePassCount; ++nPass )
                    owner.fc_process( begin(), end() );

                combining_pass( owner, nCurAge );
                m_Stat.onCombining();
                if ( (nCurAge & m_nCompactFactor) == 0 )
                    compact_list( nCurAge );
            }

            bool wait_for_combining( publication_record_type * pRec )
            {
                back_off bkoff;
                while ( pRec->nRequest.load( memory_model::memory_order_acquire ) != req_Response ) {

                    // The record can be excluded from publication list. Reinsert it
                    republish( pRec );

                    bkoff();

                    if ( m_Mutex.try_lock() ) {
                        if ( pRec->nRequest.load( memory_model::memory_order_acquire ) == req_Response ) {
                            m_Mutex.unlock();
                            break;
                        }
                        // The thread becomes a combiner
                        return false;
                    }
                }
                return true;
            }

            void compact_list( unsigned int const nCurAge )
            {
                // Thinning publication list
                publication_record * pPrev = nullptr;
                for ( publication_record * p = m_pHead; p; ) {
                    if ( p->nState.load( memory_model::memory_order_acquire ) == active && p->nAge + m_nCompactFactor < nCurAge ) {
                        if ( pPrev ) {
                            publication_record * pNext = p->pNext.load( memory_model::memory_order_acquire );
                            if ( pPrev->pNext.compare_exchange_strong( p, pNext,
                                memory_model::memory_order_release, atomics::memory_order_relaxed ))
                            {
                                p->nState.store( inactive, memory_model::memory_order_release );
                                p = pNext;
                                m_Stat.onDeactivatePubRecord();
                                continue;
                            }
                        }
                    }
                    pPrev = p;
                    p = p->pNext.load( memory_model::memory_order_acquire );
                }

                m_Stat.onCompactPublicationList();
            }

            publication_record * unlink_and_delete_record( publication_record * pPrev, publication_record * p )
            {
                if ( pPrev ) {
                    publication_record * pNext = p->pNext.load( memory_model::memory_order_acquire );
                    if ( pPrev->pNext.compare_exchange_strong( p, pNext,
                        memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    {
                        cxx11_allocator().Delete( static_cast<publication_record_type *>( p ));
                        m_Stat.onDeletePubRecord();
                    }
                    return pNext;
                }
                else {
                    m_pHead = static_cast<publication_record_type *>( p->pNext.load( memory_model::memory_order_acquire ));
                    cxx11_allocator().Delete( static_cast<publication_record_type *>( p ));
                    m_Stat.onDeletePubRecord();
                    return m_pHead;
                }
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

#endif // #ifndef CDSLIB_ALGO_FLAT_COMBINING_H

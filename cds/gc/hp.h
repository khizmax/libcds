// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_GC_HP_SMR_H
#define CDSLIB_GC_HP_SMR_H

#include <exception>
#include <cds/gc/details/hp_common.h>
#include <cds/threading/model.h>
#include <cds/details/throw_exception.h>
#include <cds/details/marked_ptr.h>
#include <cds/user_setup/cache_line.h>

/**
    @page cds_garbage_collectors_comparison Hazard Pointer SMR implementations
    @ingroup cds_garbage_collector

    <table>
        <tr>
            <th>Feature</th>
            <th>%cds::gc::HP</th>
            <th>%cds::gc::DHP</th>
        </tr>
        <tr>
            <td>Max number of guarded (hazard) pointers per thread</td>
            <td>limited (specified at construction time)</td>
            <td>unlimited (dynamically allocated when needed)</td>
        </tr>
        <tr>
            <td>Max number of retired pointers<sup>1</sup></td>
            <td>bounded, specified at construction time</td>
            <td>bounded, adaptive, depends on current thread count and number of hazard pointer for each thread</td>
        </tr>
        <tr>
            <td>Thread count</td>
            <td>bounded, upper bound is specified at construction time</td>
            <td>unbounded</td>
        </tr>
    </table>

    <sup>1</sup>Unbounded count of retired pointers means a possibility of memory exhaustion.
*/

namespace cds {
    /// @defgroup cds_garbage_collector Garbage collectors


    /// Different safe memory reclamation schemas (garbage collectors)
    /** @ingroup cds_garbage_collector

        This namespace specifies different safe memory reclamation (SMR) algorithms.
        See \ref cds_garbage_collector "Garbage collectors"
    */
    namespace gc {
    } // namespace gc

} // namespace cds


namespace cds { namespace gc {
    /// Hazard pointer implementation details
    namespace hp {
        namespace details {
            using namespace cds::gc::hp::common;

            /// \p smr::scan() strategy
            enum scan_type {
                classic,    ///< classic scan as described in Michael's works (see smr::classic_scan())
                inplace     ///< inplace scan without allocation (see smr::inplace_scan())
            };

            //@cond
            /// Hazard Pointer SMR (Safe Memory Reclamation)
            class basic_smr {
                template<typename TLSManager>
                friend class generic_smr;
                struct thread_record: thread_data
                {
                    // next hazard ptr record in list
                    thread_record*                      next_ = nullptr;
                    // Owner thread_record; nullptr - the record is free (not owned)
                    atomics::atomic<thread_record*>  owner_rec_;
                    // true if record is free (not owned)
                    atomics::atomic<bool>               free_{ false };

                    thread_record( guard* guards, size_t guard_count, retired_ptr* retired_arr, size_t retired_capacity )
                            : thread_data( guards, guard_count, retired_arr, retired_capacity ), owner_rec_(this)
                    {}
                };

            public:
                /// Returns the instance of Hazard Pointer \ref basic_smr
                static basic_smr &instance() {
#       ifdef CDS_DISABLE_SMR_EXCEPTION
                    assert( instance_ != nullptr );
#       else
                    if (!instance_)
                        CDS_THROW_EXCEPTION(not_initialized());
#       endif
                    return *instance_;
                }

                /// Creates Hazard Pointer SMR singleton
                /**
                    Hazard Pointer SMR is a singleton. If HP instance is not initialized then the function creates the instance.
                    Otherwise it does nothing.

                    The Michael's HP reclamation schema depends of three parameters:
                    - \p nHazardPtrCount - HP pointer count per thread. Usually it is small number (2-4) depending from
                        the data structure algorithms. By default, if \p nHazardPtrCount = 0,
                        the function uses maximum of HP count for CDS library
                    - \p nMaxThreadCount - max count of thread with using HP GC in your application. Default is 100.
                    - \p nMaxRetiredPtrCount - capacity of array of retired pointers for each thread. Must be greater than
                        <tt> nHazardPtrCount * nMaxThreadCount </tt>
                        Default is <tt>2 * nHazardPtrCount * nMaxThreadCount</tt>
                */
                static CDS_EXPORT_API void construct(
                        size_t nHazardPtrCount = 0,     ///< Hazard pointer count per thread
                        size_t nMaxThreadCount = 0,     ///< Max count of simultaneous working thread in your application
                        size_t nMaxRetiredPtrCount = 0, ///< Capacity of the array of retired objects for the thread
                        scan_type nScanType = inplace   ///< Scan type (see \ref scan_type enum)
                );

                // for back-copatibility
                static void Construct(
                        size_t nHazardPtrCount = 0,     ///< Hazard pointer count per thread
                        size_t nMaxThreadCount = 0,     ///< Max count of simultaneous working thread in your application
                        size_t nMaxRetiredPtrCount = 0, ///< Capacity of the array of retired objects for the thread
                        scan_type nScanType = inplace   ///< Scan type (see \ref scan_type enum)
                ) {
                    construct(nHazardPtrCount, nMaxThreadCount, nMaxRetiredPtrCount, nScanType);
                }

                /// Destroys global instance of \ref basic_smr
                /**
                    The parameter \p bDetachAll should be used carefully: if its value is \p true,
                    then the object destroyed automatically detaches all attached threads. This feature
                    can be useful when you have no control over the thread termination, for example,
                    when \p libcds is injected into existing external thread.
                */
                static CDS_EXPORT_API void destruct(
                        bool bDetachAll = false     ///< Detach all threads
                );

                // for back-compatibility
                static void Destruct(
                        bool bDetachAll = false     ///< Detach all threads
                ) {
                    destruct(bDetachAll);
                }

                /// Checks if global SMR object is constructed and may be used
                static bool isUsed() noexcept {
                    return instance_ != nullptr;
                }

                /// Set memory management functions
                /**
                    @note This function may be called <b>BEFORE</b> creating an instance
                    of Hazard Pointer SMR

                    SMR object allocates some memory for thread-specific data and for
                    creating SMR object.
                    By default, a standard \p new and \p delete operators are used for this.
                */
                static CDS_EXPORT_API void set_memory_allocator(
                        void *( *alloc_func )(size_t size),
                        void (*free_func )(void *p)
                );

                /// Returns max Hazard Pointer count per thread
                size_t get_hazard_ptr_count() const noexcept {
                    return hazard_ptr_count_;
                }

                /// Returns max thread count
                size_t get_max_thread_count() const noexcept {
                    return max_thread_count_;
                }

                /// Returns max size of retired objects array
                size_t get_max_retired_ptr_count() const noexcept {
                    return max_retired_ptr_count_;
                }

                /// Get current scan strategy
                scan_type get_scan_type() const {
                    return scan_type_;
                }

                /// Checks that required hazard pointer count \p nRequiredCount is less or equal then max hazard pointer count
                /**
                    If <tt> nRequiredCount > get_hazard_ptr_count()</tt> then the exception \p not_enough_hazard_ptr is thrown
                */
                static void check_hazard_ptr_count(size_t nRequiredCount) {
                    if (instance().get_hazard_ptr_count() < nRequiredCount) {
#       ifdef CDS_DISABLE_SMR_EXCEPTION
                        assert( false );    // not enough hazard ptr
#       else
                        CDS_THROW_EXCEPTION(not_enough_hazard_ptr());
#       endif
                    }
                }

                /// Get internal statistics
                CDS_EXPORT_API void statistics(stat &st);

            public: // for internal use only
                /// The main garbage collecting function
                /**
                    This function is called internally when upper bound of thread's list of reclaimed pointers
                    is reached.

                    There are the following scan algorithm:
                    - \ref hzp_gc_classic_scan "classic_scan" allocates memory for internal use
                    - \ref hzp_gc_inplace_scan "inplace_scan" does not allocate any memory

                    Use \p set_scan_type() member function to setup appropriate scan algorithm.
                */
                void scan(thread_data *pRec) {
                    pRec->sync();
                    (this->*scan_func_)(pRec);
                }

                /// Helper scan routine
                /**
                    The function guarantees that every node that is eligible for reuse is eventually freed, barring
                    thread failures. To do so, after executing \p scan(), a thread executes a \p %help_scan(),
                    where it checks every HP record. If an HP record is inactive, the thread moves all "lost" reclaimed pointers
                    to thread's list of reclaimed pointers.

                    The function is called internally by \p scan().
                */
                CDS_EXPORT_API void help_scan(thread_data *pThis);

            private:
                CDS_EXPORT_API basic_smr(
                        size_t nHazardPtrCount,     ///< Hazard pointer count per thread
                        size_t nMaxThreadCount,     ///< Max count of simultaneous working thread in your application
                        size_t nMaxRetiredPtrCount, ///< Capacity of the array of retired objects for the thread
                        scan_type nScanType         ///< Scan type (see \ref scan_type enum)
                );

                CDS_EXPORT_API ~basic_smr();

                CDS_EXPORT_API void detach_all_thread();

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
                CDS_EXPORT_API void classic_scan(thread_data *pRec);

                /// In-place scan algorithm
                /** @anchor hzp_gc_inplace_scan
                    Unlike the \p classic_scan() algorithm, \p %inplace_scan() does not allocate any memory.
                    All operations are performed in-place.
                */
                CDS_EXPORT_API void inplace_scan(thread_data *pRec);

            private:
                CDS_EXPORT_API thread_record *create_thread_data();

                static CDS_EXPORT_API void destroy_thread_data(thread_record *pRec);

                /// Allocates Hazard Pointer SMR thread private data
                CDS_EXPORT_API thread_record *alloc_thread_data();

                /// Free HP SMR thread-private data
                CDS_EXPORT_API void free_thread_data(thread_record *pRec, bool callHelpScan);

            private:
                static CDS_EXPORT_API basic_smr *instance_;

                atomics::atomic<thread_record *> thread_list_;   ///< Head of thread list

                size_t const hazard_ptr_count_;      ///< max count of thread's hazard pointer
                size_t const max_thread_count_;      ///< max count of thread
                size_t const max_retired_ptr_count_; ///< max count of retired ptr per thread
                scan_type const scan_type_;             ///< scan type (see \ref scan_type enum)
                void ( basic_smr::*scan_func_ )(thread_data *pRec);
            };
            //@endcond

            template<typename TLSManager>
            class generic_smr : public basic_smr 
            {
            public:

                /// TLS manager type
                typedef TLSManager  tls_manager;

                /// Returns thread-local data for the current thread
                static thread_data* tls()
                {
                    thread_data* data = tls_manager::getTLS();
                    assert( data != nullptr );
                    return data;
                }

                /// Attach current thread to HP
                static void attach_thread()
                {
                    if ( !tls_manager::getTLS() )
                        tls_manager::setTLS(instance().alloc_thread_data());
                }

                /// Detach current thread from HP
                static void detach_thread()
                {
                    thread_data* rec = tls_manager::getTLS();
                    if ( rec ) {
                        tls_manager::setTLS(nullptr);
                        instance().free_thread_data(static_cast<thread_record*>( rec ), true );
                    }
                }
            };

        } // namespace details

        //@cond
        // for backward compatibility
        typedef details::generic_smr<details::DefaultTLSManager> smr;
        typedef smr GarbageCollector;
        //@endcond

        template<typename TLSManager>
        using custom_smr = details::generic_smr<TLSManager>;
    } // namespace cds::gc::hp

    namespace details {
        /// Hazard Pointer SMR (Safe Memory Reclamation)
        /**  @ingroup cds_garbage_collector

            Implementation of classic Hazard Pointer SMR

            Sources:
                - [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-freeobjects using atomic reads and writes"
                - [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
                - [2004] Andrei Alexandrescy, Maged Michael "Lock-free Data Structures with Hazard Pointers"

            Hazard Pointer SMR is a singleton. The main user-level part of Hazard Pointer schema is
            \p %cds::gc::HP class and its nested classes. Before use any HP-related class you must initialize \p %HP
            by contructing \p %cds::gc::HP object in beginning of your \p main().
            See \ref cds_how_to_use "How to use" section for details how to apply SMR schema.
        */
        template<typename TLSManager>
        class generic_HP
        {
        public:
            /// TLS manager type
            using tls_manager = TLSManager;

        private:
            //@cond
            using hp_implementation = hp::details::generic_smr<tls_manager>;
            //@endcond

        public:
            /// Native guarded pointer type
            typedef hp::details::hazard_ptr guarded_pointer;

            /// Atomic reference
            template <typename T> using atomic_ref = atomics::atomic<T *>;

            /// Atomic marked pointer
            template <typename MarkedPtr> using atomic_marked_ptr = atomics::atomic<MarkedPtr>;

            /// Atomic type
            template <typename T> using atomic_type = atomics::atomic<T>;

            /// Exception "Not enough Hazard Pointer"
            typedef hp::details::not_enough_hazard_ptr not_enough_hazard_ptr_exception;

            /// Internal statistics
            typedef hp::details::stat stat;

            /// Hazard Pointer guard
            /**
                A guard is a hazard pointer.
                Additionally, the \p %Guard class manages allocation and deallocation of the hazard pointer.

                \p %Guard object is movable but not copyable.

                The guard object can be in two states:
                - unlinked - the guard is not linked with any internal hazard pointer.
                  In this state no operation except \p link() and move assignment is supported.
                - linked (default) - the guard allocates an internal hazard pointer and completely operable.

                Due to performance reason the implementation does not check state of the guard at runtime.

                @warning Move assignment transfers the guard in unlinked state, use with care.
            */
            class Guard
            {
            public:
                /// Default ctor allocates a guard (hazard pointer) from thread-private storage
                /**
                    @warning Can throw \p not_enough_hazard_ptr if internal hazard pointer objects are exhausted.
                */
                Guard()
                    : guard_(hp_implementation::tls()->hazards_.alloc())
                {}

                /// Initilalizes an unlinked guard i.e. the guard contains no hazard pointer. Used for move semantics support
                explicit Guard( std::nullptr_t ) noexcept
                    : guard_( nullptr )
                {}

                /// Move ctor - \p src guard becomes unlinked (transfer internal guard ownership)
                Guard( Guard&& src ) noexcept
                    : guard_( src.guard_ )
                {
                    src.guard_ = nullptr;
                }

                /// Move assignment: the internal guards are swapped between \p src and \p this
                /**
                    @warning \p src will become in unlinked state if \p this was unlinked on entry.
                */
                Guard& operator=( Guard&& src ) noexcept
                {
                    std::swap( guard_, src.guard_ );
                    return *this;
                }

                /// Copy ctor is prohibited - the guard is not copyable
                Guard( Guard const& ) = delete;

                /// Copy assignment is prohibited
                Guard& operator=( Guard const& ) = delete;

                /// Frees the internal hazard pointer if the guard is in linked state
                ~Guard()
                {
                    unlink();
                }

                /// Checks if the guard object linked with any internal hazard pointer
                bool is_linked() const
                {
                    return guard_ != nullptr;
                }

                /// Links the guard with internal hazard pointer if the guard is in unlinked state
                /**
                    @warning Can throw \p not_enough_hazard_ptr_exception if internal hazard pointer array is exhausted.
                */
                void link()
                {
                    if ( !guard_ )
                        guard_ = hp_implementation::tls()->hazards_.alloc();
                }

                /// Unlinks the guard from internal hazard pointer; the guard becomes in unlinked state
                void unlink()
                {
                    if ( guard_ ) {
                        hp_implementation::tls()->hazards_.free( guard_ );
                        guard_ = nullptr;
                    }
                }

                /// Protects a pointer of type \p atomic<T*>
                /**
                    Return the value of \p toGuard

                    The function tries to load \p toGuard and to store it
                    to the HP slot repeatedly until the guard's value equals \p toGuard

                    @warning The guard object should be in linked state, otherwise the result is undefined
                */
                template <typename T>
                T protect( atomics::atomic<T> const& toGuard )
                {
                    return protect(toGuard, [](T p) { return p; });
                }

                /// Protects a converted pointer of type \p atomic<T*>
                /**
                    Return the value of \p toGuard

                    The function tries to load \p toGuard and to store result of \p f functor
                    to the HP slot repeatedly until the guard's value equals \p toGuard.

                    The function is useful for intrusive containers when \p toGuard is a node pointer
                    that should be converted to a pointer to the value before protecting.
                    The parameter \p f of type Func is a functor that makes this conversion:
                    \code
                        struct functor {
                            value_type * operator()( T * p );
                        };
                    \endcode
                    Actually, the result of <tt> f( toGuard.load()) </tt> is assigned to the hazard pointer.

                    @warning The guard object should be in linked state, otherwise the result is undefined
                */
                template <typename T, class Func>
                T protect( atomics::atomic<T> const& toGuard, Func f )
                {
                    assert( guard_ != nullptr );

                    T pCur = toGuard.load(atomics::memory_order_relaxed);
                    T pRet;
                    do {
                        pRet = pCur;
                        assign( f( pCur ));
                        pCur = toGuard.load(atomics::memory_order_acquire);
                    } while ( pRet != pCur );
                    return pCur;
                }

                /// Store \p p to the guard
                /**
                    The function equals to a simple assignment the value \p p to guard, no loop is performed.
                    Can be used for a pointer that cannot be changed concurrently or if the pointer is already
                    guarded by another guard.

                    @warning The guard object should be in linked state, otherwise the result is undefined
                */
                template <typename T>
                T * assign( T* p )
                {
                    assert( guard_ != nullptr );

                    guard_->set( p );
                    hp_implementation::tls()->sync();
                    return p;
                }

                //@cond
                std::nullptr_t assign( std::nullptr_t )
                {
                    assert( guard_ != nullptr );

                    guard_->clear();
                    return nullptr;
                }
                //@endcond

                /// Copy a value guarded from \p src guard to \p this guard (valid only in linked state)
                void copy( Guard const& src )
                {
                    assign( src.get_native());
                }

                /// Store marked pointer \p p to the guard
                /**
                    The function equals to a simple assignment of <tt>p.ptr()</tt>, no loop is performed.
                    Can be used for a marked pointer that cannot be changed concurrently or if the marked pointer
                    is already guarded by another guard.

                    @warning The guard object should be in linked state, otherwise the result is undefined
                */
                template <typename T, int BITMASK>
                T * assign( cds::details::marked_ptr<T, BITMASK> p )
                {
                    return assign( p.ptr());
                }

                /// Clear value of the guard (valid only in linked state)
                void clear()
                {
                    assign( nullptr );
                }

                /// Get the value currently protected (valid only in linked state)
                template <typename T>
                T * get() const
                {
                    assert( guard_ != nullptr );
                    return guard_->get_as<T>();
                }

                /// Get native hazard pointer stored (valid only in linked state)
                guarded_pointer get_native() const
                {
                    assert( guard_ != nullptr );
                    return guard_->get();
                }

                //@cond
                hp::details::guard* release()
                {
                    hp::details::guard* g = guard_;
                    guard_ = nullptr;
                    return g;
                }

                hp::details::guard*& guard_ref()
                {
                    return guard_;
                }
                //@endcond

            private:
                //@cond
                hp::details::guard* guard_;
                //@endcond
            };

            /// Array of Hazard Pointer guards
            /**
                The class is intended for allocating an array of hazard pointer guards.
                Template parameter \p Count defines the size of the array.
            */
            template <size_t Count>
            class GuardArray
            {
            public:
                /// Rebind array for other size \p Count2
                template <size_t Count2>
                struct rebind {
                    typedef GuardArray<Count2>  other;   ///< rebinding result
                };

                /// Array capacity
                static constexpr const size_t c_nCapacity = Count;

            public:
                /// Default ctor allocates \p Count hazard pointers
                GuardArray()
                {
                    hp_implementation::tls()->hazards_.alloc( guards_ );
                }

                /// Move ctor is prohibited
                GuardArray( GuardArray&& ) = delete;

                /// Move assignment is prohibited
                GuardArray& operator=( GuardArray&& ) = delete;

                /// Copy ctor is prohibited
                GuardArray( GuardArray const& ) = delete;

                /// Copy assignment is prohibited
                GuardArray& operator=( GuardArray const& ) = delete;

                /// Frees allocated hazard pointers
                ~GuardArray()
                {
                    hp_implementation::tls()->hazards_.free( guards_ );
                }

                /// Protects a pointer of type \p atomic<T*>
                /**
                    Return the value of \p toGuard

                    The function tries to load \p toGuard and to store it
                    to the slot \p nIndex repeatedly until the guard's value equals \p toGuard
                */
                template <typename T>
                T protect( size_t nIndex, atomics::atomic<T> const& toGuard )
                {
                    return protect(nIndex, toGuard, [](T p) { return p; });
                }

                /// Protects a pointer of type \p atomic<T*>
                /**
                    Return the value of \p toGuard

                    The function tries to load \p toGuard and to store it
                    to the slot \p nIndex repeatedly until the guard's value equals \p toGuard

                    The function is useful for intrusive containers when \p toGuard is a node pointer
                    that should be converted to a pointer to the value type before guarding.
                    The parameter \p f of type Func is a functor that makes this conversion:
                    \code
                        struct functor {
                            value_type * operator()( T * p );
                        };
                    \endcode
                    Really, the result of <tt> f( toGuard.load()) </tt> is assigned to the hazard pointer.
                */
                template <typename T, class Func>
                T protect( size_t nIndex, atomics::atomic<T> const& toGuard, Func f )
                {
                    assert( nIndex < capacity());

                    T pRet;
                    do {
                        assign( nIndex, f( pRet = toGuard.load(atomics::memory_order_relaxed)));
                    } while ( pRet != toGuard.load(atomics::memory_order_acquire));

                    return pRet;
                }

                /// Store \p to the slot \p nIndex
                /**
                    The function equals to a simple assignment, no loop is performed.
                */
                template <typename T>
                T * assign( size_t nIndex, T * p )
                {
                    assert( nIndex < capacity());

                    guards_.set( nIndex, p );
                    hp_implementation::tls()->sync();
                    return p;
                }

                /// Store marked pointer \p p to the guard
                /**
                    The function equals to a simple assignment of <tt>p.ptr()</tt>, no loop is performed.
                    Can be used for a marked pointer that cannot be changed concurrently.
                */
                template <typename T, int BITMASK>
                T * assign( size_t nIndex, cds::details::marked_ptr<T, BITMASK> p )
                {
                    return assign( nIndex, p.ptr());
                }

                /// Copy guarded value from \p src guard to slot at index \p nIndex
                void copy( size_t nIndex, Guard const& src )
                {
                    assign( nIndex, src.get_native());
                }

                /// Copy guarded value from slot \p nSrcIndex to the slot \p nDestIndex
                void copy( size_t nDestIndex, size_t nSrcIndex )
                {
                    assign( nDestIndex, get_native( nSrcIndex ));
                }

                /// Clear value of the slot \p nIndex
                void clear( size_t nIndex )
                {
                    guards_.clear( nIndex );
                }

                /// Get current value of slot \p nIndex
                template <typename T>
                T * get( size_t nIndex ) const
                {
                    assert( nIndex < capacity());
                    return guards_[nIndex]->template get_as<T>();
                }

                /// Get native hazard pointer stored
                guarded_pointer get_native( size_t nIndex ) const
                {
                    assert( nIndex < capacity());
                    return guards_[nIndex]->get();
                }

                //@cond
                hp::details::guard* release( size_t nIndex ) noexcept
                {
                    return guards_.release( nIndex );
                }
                //@endcond

                /// Capacity of the guard array
                static constexpr size_t capacity()
                {
                    return c_nCapacity;
                }

            private:
                //@cond
                hp::details::guard_array<c_nCapacity> guards_;
                //@endcond
            };

            /// Guarded pointer
            /**
                A guarded pointer is a pair of a pointer and GC's guard.
                Usually, it is used for returning a pointer to an element of a lock-free container.
                The guard prevents the pointer to be early disposed (freed) by SMR.
                After destructing \p %guarded_ptr object the pointer can be disposed (freed) automatically at any time.

                Template arguments:
                - \p GuardedType - a type which the guard stores
                - \p ValueType - a value type
                - \p Cast - a functor for converting <tt>GuardedType*</tt> to <tt>ValueType*</tt>. Default is \p void (no casting).

                For intrusive containers, \p GuardedType is the same as \p ValueType and no casting is needed.
                In such case the \p %guarded_ptr is:
                @code
                typedef cds::gc::HP::guarded_ptr< foo > intrusive_guarded_ptr;
                @endcode

                For standard (non-intrusive) containers \p GuardedType is not the same as \p ValueType and casting is needed.
                For example:
                @code
                struct foo {
                    int const   key;
                    std::string value;
                };

                struct value_accessor {
                    std::string* operator()( foo* pFoo ) const
                    {
                        return &(pFoo->value);
                    }
                };

                // Guarded ptr
                typedef cds::gc::HP::guarded_ptr< Foo, std::string, value_accessor > nonintrusive_guarded_ptr;
                @endcode

                You don't need use this class directly.
                All set/map container classes from \p libcds declare the typedef for \p %guarded_ptr with appropriate casting functor.
            */
            template <typename GuardedType, typename ValueType=GuardedType, typename Cast=void >
            class guarded_ptr
            {
                //@cond
                struct trivial_cast {
                    ValueType * operator()( GuardedType * p ) const
                    {
                        return p;
                    }
                };

                template <typename GT, typename VT, typename C> friend class guarded_ptr;
                //@endcond

            public:
                typedef GuardedType guarded_type; ///< Guarded type
                typedef ValueType   value_type;   ///< Value type

                /// Functor for casting \p guarded_type to \p value_type
                typedef typename std::conditional< std::is_same<Cast, void>::value, trivial_cast, Cast >::type value_cast;

            public:
                /// Creates empty guarded pointer
                guarded_ptr() noexcept
                    : guard_(nullptr)
                {}

                //@cond
                explicit guarded_ptr( hp::details::guard* g ) noexcept
                    : guard_( g )
                {}

                /// Initializes guarded pointer with \p p
                explicit guarded_ptr( guarded_type* p ) noexcept
                    : guard_( nullptr )
                {
                    reset(p);
                }
                explicit guarded_ptr( std::nullptr_t ) noexcept
                    : guard_( nullptr )
                {}
                //@endcond

                /// Move ctor
                guarded_ptr( guarded_ptr&& gp ) noexcept
                    : guard_( gp.guard_ )
                {
                    gp.guard_ = nullptr;
                }

                /// Move ctor
                template <typename GT, typename VT, typename C>
                guarded_ptr( guarded_ptr<GT, VT, C>&& gp ) noexcept
                    : guard_( gp.guard_ )
                {
                    gp.guard_ = nullptr;
                }

                /// Ctor from \p Guard
                explicit guarded_ptr( Guard&& g ) noexcept
                    : guard_( g.release())
                {}

                /// The guarded pointer is not copy-constructible
                guarded_ptr( guarded_ptr const& gp ) = delete;

                /// Clears the guarded pointer
                /**
                    \ref release() is called if guarded pointer is not \ref empty()
                */
                ~guarded_ptr() noexcept
                {
                    release();
                }

                /// Move-assignment operator
                guarded_ptr& operator=( guarded_ptr&& gp ) noexcept
                {
                    std::swap( guard_, gp.guard_ );
                    return *this;
                }

                /// Move-assignment from \p Guard
                guarded_ptr& operator=( Guard&& g ) noexcept
                {
                    std::swap( guard_, g.guard_ref());
                    return *this;
                }

                /// The guarded pointer is not copy-assignable
                guarded_ptr& operator=(guarded_ptr const& gp) = delete;

                /// Returns a pointer to guarded value
                value_type * operator ->() const noexcept
                {
                    assert( !empty());
                    return value_cast()( guard_->get_as<guarded_type>());
                }

                /// Returns a reference to guarded value
                value_type& operator *() noexcept
                {
                    assert( !empty());
                    return *value_cast()( guard_->get_as<guarded_type>());
                }

                /// Returns const reference to guarded value
                value_type const& operator *() const noexcept
                {
                    assert( !empty());
                    return *value_cast()( guard_->get_as<guarded_type>());
                }

                /// Checks if the guarded pointer is \p nullptr
                bool empty() const noexcept
                {
                    return !guard_ || guard_->get( atomics::memory_order_relaxed ) == nullptr;
                }

                /// \p bool operator returns <tt>!empty()</tt>
                explicit operator bool() const noexcept
                {
                    return !empty();
                }

                /// Clears guarded pointer
                /**
                    If the guarded pointer has been released, the pointer can be disposed (freed) at any time.
                    Dereferncing the guarded pointer after \p release() is dangerous.
                */
                void release() noexcept
                {
                    free_guard();
                }

                //@cond
                // For internal use only!!!
                void reset(guarded_type * p) noexcept
                {
                    alloc_guard();
                    assert( guard_ );
                    guard_->set(p);
                }
                //@endcond

            private:
                //@cond
                void alloc_guard()
                {
                    if ( !guard_ )
                        guard_ = hp_implementation::tls()->hazards_.alloc();
                }

                void free_guard()
                {
                    if ( guard_ ) {
                        hp_implementation::tls()->hazards_.free( guard_ );
                        guard_ = nullptr;
                    }
                }
                //@endcond

            private:
                //@cond
                hp::details::guard* guard_;
                //@endcond
            };

        public:
            /// \p scan() type
            enum class scan_type {
                classic = hp::details::classic,    ///< classic scan as described in Michael's papers
                inplace = hp::details::inplace     ///< inplace scan without allocation
            };

            /// Initializes %HP singleton
            /**
                The constructor initializes Hazard Pointer SMR singleton with passed parameters.
                If the instance does not yet exist then the function creates the instance.
                Otherwise it does nothing.

                The Michael's %HP reclamation schema depends of three parameters:
                - \p nHazardPtrCount - hazard pointer count per thread. Usually it is small number (up to 10) depending from
                    the data structure algorithms. If \p nHazardPtrCount = 0, the defaul value 8 is used
                - \p nMaxThreadCount - max count of thread with using Hazard Pointer GC in your application. Default is 100.
                - \p nMaxRetiredPtrCount - capacity of array of retired pointers for each thread. Must be greater than
                    <tt> nHazardPtrCount * nMaxThreadCount </tt>. Default is <tt>2 * nHazardPtrCount * nMaxThreadCount </tt>.
            */
            generic_HP(
                size_t nHazardPtrCount = 0,     ///< Hazard pointer count per thread
                size_t nMaxThreadCount = 0,     ///< Max count of simultaneous working thread in your application
                size_t nMaxRetiredPtrCount = 0, ///< Capacity of the array of retired objects for the thread
                scan_type nScanType = scan_type::inplace   ///< Scan type (see \p scan_type enum)
            )
            {
                hp_implementation::construct(
                    nHazardPtrCount,
                    nMaxThreadCount,
                    nMaxRetiredPtrCount,
                    static_cast<hp::details::scan_type>(nScanType)
                );
            }

            /// Terminates GC singleton
            /**
                The destructor destroys %HP global object. After calling of this function you may \b NOT
                use CDS data structures based on \p %cds::gc::HP.
                Usually, %HP object is destroyed at the end of your \p main().
            */
            ~generic_HP()
            {
                hp_implementation::destruct( true );
            }

            /// Checks that required hazard pointer count \p nCountNeeded is less or equal then max hazard pointer count
            /**
                If <tt> nCountNeeded > get_hazard_ptr_count()</tt> then the exception \p not_enough_hazard_ptr is thrown
            */
            static void check_available_guards( size_t nCountNeeded )
            {
                hp_implementation::check_hazard_ptr_count( nCountNeeded );
            }

            /// Set memory management functions
            /**
                @note This function may be called <b>BEFORE</b> creating an instance
                of Hazard Pointer SMR

                SMR object allocates some memory for thread-specific data and for
                creating SMR object.
                By default, a standard \p new and \p delete operators are used for this.
            */
            static void set_memory_allocator(
                void* ( *alloc_func )( size_t size ),   ///< \p malloc() function
                void( *free_func )( void * p )          ///< \p free() function
            )
            {
                hp_implementation::set_memory_allocator( alloc_func, free_func );
            }

            /// Returns max Hazard Pointer count
            static size_t max_hazard_count()
            {
                return hp_implementation::instance().get_hazard_ptr_count();
            }

            /// Returns max count of thread
            static size_t max_thread_count()
            {
                return hp_implementation::instance().get_max_thread_count();
            }

            /// Returns capacity of retired pointer array
            static size_t retired_array_capacity()
            {
                return hp_implementation::instance().get_max_retired_ptr_count();
            }

            /// Retire pointer \p p with function \p func
            /**
                The function places pointer \p p to array of pointers ready for removing.
                (so called retired pointer array). The pointer can be safely removed when no hazard pointer points to it.
                \p func is a disposer: when \p p can be safely removed, \p func is called.
            */
            template <typename T>
            static void retire( T * p, void( *func )( void * ))
            {
                hp::details::thread_data* rec = hp_implementation::tls();
                if ( !rec->retired_.push( hp::details::retired_ptr( p, func )))
                    hp_implementation::instance().scan( rec );
            }

            /// Retire pointer \p p with functor of type \p Disposer
            /**
                The function places pointer \p p to array of pointers ready for removing.
                (so called retired pointer array). The pointer can be safely removed when no hazard pointer points to it.

                Deleting the pointer is an invocation of some object of type \p Disposer; the interface of \p Disposer is:
                \code
                template <typename T>
                struct disposer {
                    void operator()( T * p )    ;   // disposing operator
                };
                \endcode
                Since the functor call can happen at any time after \p retire() call, additional restrictions are imposed to \p Disposer type:
                - it should be stateless functor
                - it should be default-constructible
                - the result of functor call with argument \p p should not depend on where the functor will be called.

                \par Examples:
                Operator \p delete functor:
                \code
                template <typename T>
                struct disposer {
                    void operator ()( T * p ) {
                        delete p;
                    }
                };

                // How to call HP::retire method
                int * p = new int;

                // ... use p in lock-free manner

                cds::gc::HP::retire<disposer>( p ) ;   // place p to retired pointer array of HP GC
                \endcode

                Functor based on \p std::allocator :
                \code
                template <typename Alloc = std::allocator<int> >
                struct disposer {
                    template <typename T>
                    void operator()( T * p ) {
                        typedef typename Alloc::templare rebind<T>::other alloc_t;
                        alloc_t a;
                        a.destroy( p );
                        a.deallocate( p, 1 );
                    }
                };
                \endcode
            */
            template <class Disposer, typename T>
            static void retire( T * p )
            {
                if ( !hp_implementation::tls()->retired_.push( hp::details::retired_ptr( p, +[]( void* p ) { Disposer()( static_cast<T*>( p )); })))
                    scan();
            }

            /// Get current scan strategy
            static scan_type getScanType()
            {
                return static_cast<scan_type>( hp_implementation::instance().get_scan_type());
            }

            /// Checks if Hazard Pointer GC is constructed and may be used
            static bool isUsed()
            {
                return hp_implementation::isUsed();
            }

            /// Forces SMR call for current thread
            /**
                Usually, this function should not be called directly.
            */
            static void scan()
            {
                hp_implementation::instance().scan( hp_implementation::tls());
            }

            /// Synonym for \p scan()
            static void force_dispose()
            {
                scan();
            }

            /// Returns internal statistics
            /**
                The function clears \p st before gathering statistics.

                @note Internal statistics is available only if you compile
                \p libcds and your program with \p -DCDS_ENABLE_HPSTAT.
            */
            static void statistics( stat& st )
            {
                hp_implementation::instance().statistics( st );
            }

            /// Returns post-mortem statistics
            /**
                Post-mortem statistics is gathered in the \p %HP object destructor
                and can be accessible after destructing the global \p %HP object.

                @note Internal statistics is available only if you compile
                \p libcds and your program with \p -DCDS_ENABLE_HPSTAT.

                Usage:
                \code
                int main()
                {
                    cds::Initialize();
                    {
                        // Initialize HP SMR
                        cds::gc::HP hp;

                        // deal with HP-based data structured
                        // ...
                    }

                    // HP object destroyed
                    // Get total post-mortem statistics
                    cds::gc::HP::stat const& st = cds::gc::HP::postmortem_statistics();

                    printf( "HP statistics:\n"
                        "  thread count           = %llu\n"
                        "  guard allocated        = %llu\n"
                        "  guard freed            = %llu\n"
                        "  retired data count     = %llu\n"
                        "  free data count        = %llu\n"
                        "  scan() call count      = %llu\n"
                        "  help_scan() call count = %llu\n",
                        st.thread_rec_count,
                        st.guard_allocated, st.guard_freed,
                        st.retired_count, st.free_count,
                        st.scan_count, st.help_scan_count
                    );

                    cds::Terminate();
                }
                \endcode
            */
            static stat const& postmortem_statistics() {
                return cds::gc::hp::details::postmortem_statistics();
            }
        };
    } // namespace cds::gc::details

    /// Default Hazard Pointer schema with \p %hp::details::DefaultTLSManager
    typedef details::generic_HP<hp::details::DefaultTLSManager> HP;


    /// Custom Hazard Pointer schema with custom-provided \p TLSManager
    template<typename TLSManager>
    using custom_HP = details::generic_HP<TLSManager>;

}} // namespace cds::gc

#endif // #ifndef CDSLIB_GC_HP_SMR_H


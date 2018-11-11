// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_GC_DHP_SMR_H
#define CDSLIB_GC_DHP_SMR_H

#include <exception>
#include <cds/gc/details/hp_common.h>
#include <cds/threading/model.h>
#include <cds/intrusive/free_list_selector.h>
#include <cds/details/throw_exception.h>
#include <cds/details/marked_ptr.h>
#include <cds/user_setup/cache_line.h>

namespace cds { namespace gc {

    /// Dynamic (adaptive) Hazard Pointer implementation details
    namespace dhp {
        using namespace cds::gc::hp::common;

        /// Exception "Dynamic Hazard Pointer SMR is not initialized"
        class not_initialized: public std::runtime_error
        {
        public:
            //@cond
            not_initialized()
                : std::runtime_error( "Global DHP SMR object is not initialized" )
            {}
            //@endcond
        };

        //@cond
        struct guard_block: public cds::intrusive::FreeListImpl::node
        {
            guard_block*  next_block_ = nullptr;  // next block in the thread list

            guard* first()
            {
                return reinterpret_cast<guard*>( this + 1 );
            }
        };
        //@endcond

        //@cond
        /// \p guard_block allocator (global object)
        class hp_allocator
        {
            friend class smr;
        public:
            static hp_allocator& instance();

            CDS_EXPORT_API guard_block* alloc();
            void free( guard_block* block )
            {
                free_list_.put( block );
            }

        private:
            hp_allocator()
#ifdef CDS_ENABLE_HPSTAT
                : block_allocated_(0)
#endif
            {}
            CDS_EXPORT_API ~hp_allocator();

        private:
            cds::intrusive::FreeListImpl    free_list_; ///< list of free \p guard_block
#ifdef CDS_ENABLE_HPSTAT
        public:
            atomics::atomic<size_t>         block_allocated_;   ///< count of allocated blocks
#endif
        };
        //@endcond

        //@cond
        /// Per-thread hazard pointer storage
        class thread_hp_storage
        {
            friend class smr;
        public:
            thread_hp_storage( guard* arr, size_t nSize ) noexcept
                : free_head_( arr )
                , array_( arr )
                , initial_capacity_( nSize )
#       ifdef CDS_ENABLE_HPSTAT
                , alloc_guard_count_( 0 )
                , free_guard_count_( 0 )
                , extend_call_count_( 0 )
#       endif
            {
                // Initialize guards
                new( arr ) guard[nSize];
                extended_list_.store( nullptr, atomics::memory_order_release );
            }

            thread_hp_storage() = delete;
            thread_hp_storage( thread_hp_storage const& ) = delete;
            thread_hp_storage( thread_hp_storage&& ) = delete;

            ~thread_hp_storage()
            {
                clear();
            }

            guard* alloc()
            {
                if ( cds_unlikely( free_head_ == nullptr )) {
                    extend();
                    assert( free_head_ != nullptr );
                }

                guard* g = free_head_;
                free_head_ = g->next_;
                CDS_HPSTAT( ++alloc_guard_count_ );
                return g;
            }

            void free( guard* g ) noexcept
            {
                if ( g ) {
                    g->clear();
                    g->next_ = free_head_;
                    free_head_ = g;
                    CDS_HPSTAT( ++free_guard_count_ );
                }
            }

            template< size_t Capacity>
            size_t alloc( guard_array<Capacity>& arr )
            {
                for ( size_t i = 0; i < Capacity; ++i ) {
                    if ( cds_unlikely( free_head_ == nullptr ))
                        extend();
                    arr.reset( i, free_head_ );
                    free_head_ = free_head_->next_;
                }
                CDS_HPSTAT( alloc_guard_count_ += Capacity );
                return Capacity;
            }

            template <size_t Capacity>
            void free( guard_array<Capacity>& arr ) noexcept
            {
                guard* gList = free_head_;
                for ( size_t i = 0; i < Capacity; ++i ) {
                    guard* g = arr[i];
                    if ( g ) {
                        g->clear();
                        g->next_ = gList;
                        gList = g;
                        CDS_HPSTAT( ++free_guard_count_ );
                    }
                }
                free_head_ = gList;
            }

            void clear()
            {
                // clear array_
                for ( guard* cur = array_, *last = array_ + initial_capacity_; cur < last; ++cur )
                    cur->clear();

                // free all extended blocks
                hp_allocator& a = hp_allocator::instance();
                for ( guard_block* p = extended_list_.load( atomics::memory_order_relaxed ); p; ) {
                    guard_block* next = p->next_block_;
                    a.free( p );
                    p = next;
                }

                extended_list_.store( nullptr, atomics::memory_order_release );
            }

            void init()
            {
                assert( extended_list_.load(atomics::memory_order_relaxed) == nullptr );

                guard* p = array_;
                for ( guard* pEnd = p + initial_capacity_ - 1; p != pEnd; ++p )
                    p->next_ = p + 1;
                p->next_ = nullptr;
                free_head_ = array_;
            }

        private:
            void extend()
            {
                assert( free_head_ == nullptr );

                guard_block* block = hp_allocator::instance().alloc();
                block->next_block_ = extended_list_.load( atomics::memory_order_relaxed );
                extended_list_.store( block, atomics::memory_order_release );
                free_head_ = block->first();
                CDS_HPSTAT( ++extend_call_count_ );
            }

        private:
            guard*          free_head_;        ///< Head of free guard list
            atomics::atomic<guard_block*> extended_list_;    ///< Head of extended guard blocks allocated for the thread
            guard* const    array_;            ///< initial HP array
            size_t const    initial_capacity_; ///< Capacity of \p array_
#       ifdef CDS_ENABLE_HPSTAT
        public:
            size_t          alloc_guard_count_;
            size_t          free_guard_count_;
            size_t          extend_call_count_;
#       endif
        };
        //@endcond

        //@cond
        struct retired_block: public cds::intrusive::FreeListImpl::node
        {
            retired_block*  next_;  ///< Next block in thread-private retired array

            static size_t const c_capacity = 256;

            retired_block()
                : next_( nullptr )
            {}

            retired_ptr* first() const
            {
                return reinterpret_cast<retired_ptr*>( const_cast<retired_block*>( this ) + 1 );
            }

            retired_ptr* last() const
            {
                return first() + c_capacity;
            }
        };
        //@endcond

        //@cond
        class retired_allocator
        {
            friend class smr;
        public:
            static retired_allocator& instance();

            CDS_EXPORT_API retired_block* alloc();
            void free( retired_block* block )
            {
                block->next_ = nullptr;
                free_list_.put( block );
            }

        private:
            retired_allocator()
#ifdef CDS_ENABLE_HPSTAT
                : block_allocated_(0)
#endif
            {}
            CDS_EXPORT_API ~retired_allocator();

        private:
            cds::intrusive::FreeListImpl    free_list_; ///< list of free \p guard_block
#ifdef CDS_ENABLE_HPSTAT
        public:
            atomics::atomic<size_t> block_allocated_; ///< Count of allocated blocks
#endif
        };
        //@endcond

        //@cond
        /// Per-thread retired array
        class retired_array
        {
            friend class smr;
        public:
            retired_array() noexcept
                : current_block_( nullptr )
                , current_cell_( nullptr )
                , list_head_( nullptr )
                , list_tail_( nullptr )
                , block_count_(0)
#       ifdef CDS_ENABLE_HPSTAT
                , retire_call_count_( 0 )
                , extend_call_count_( 0 )
#       endif
            {}

            retired_array( retired_array const& ) = delete;
            retired_array( retired_array&& ) = delete;

            ~retired_array()
            {
                assert( empty());
                fini();
            }

            bool push( retired_ptr const& p ) noexcept
            {
                assert( current_block_ != nullptr );
                assert( current_block_->first() <= current_cell_ );
                assert( current_cell_ < current_block_->last());
                //assert( &p != current_cell_ );

                *current_cell_ = p;
                CDS_HPSTAT( ++retire_call_count_ );

                if ( ++current_cell_ == current_block_->last()) {
                    // goto next block if exists
                    if ( current_block_->next_ ) {
                        current_block_ = current_block_->next_;
                        current_cell_ = current_block_->first();
                        return true;
                    }

                    // no free block
                    // smr::scan() extend retired_array if needed
                    return false;
                }

                return true;
            }

            bool repush( retired_ptr* p ) noexcept
            {
                bool ret = push( *p );
                CDS_HPSTAT( --retire_call_count_ );
                assert( ret );
                return ret;
            }

        private: // called by smr
            void init()
            {
                if ( list_head_ == nullptr ) {
                    retired_block* block = retired_allocator::instance().alloc();
                    assert( block->next_ == nullptr );

                    current_block_ =
                        list_head_ =
                        list_tail_ = block;
                    current_cell_ = block->first();

                    block_count_ = 1;
                }
            }

            void fini()
            {
                retired_allocator& alloc = retired_allocator::instance();
                for ( retired_block* p = list_head_; p; ) {
                    retired_block* next = p->next_;
                    alloc.free( p );
                    p = next;
                }

                current_block_ =
                    list_head_ =
                    list_tail_ = nullptr;
                current_cell_ = nullptr;

                block_count_ = 0;
            }

            void extend()
            {
                assert( list_head_ != nullptr );
                assert( current_block_ == list_tail_ );
                assert( current_cell_ == current_block_->last());

                retired_block* block = retired_allocator::instance().alloc();
                assert( block->next_ == nullptr );

                current_block_ = list_tail_ = list_tail_->next_ = block;
                current_cell_ = block->first();
                ++block_count_;
                CDS_HPSTAT( ++extend_call_count_ );
            }

            bool empty() const
            {
                return current_block_ == nullptr
                    || ( current_block_ == list_head_ && current_cell_ == current_block_->first());
            }

        private:
            retired_block*          current_block_;
            retired_ptr*            current_cell_;  // in current_block_

            retired_block*          list_head_;
            retired_block*          list_tail_;
            size_t                  block_count_;
#       ifdef CDS_ENABLE_HPSTAT
        public:
            size_t  retire_call_count_;
            size_t  extend_call_count_;
#       endif
        };
        //@endcond

        /// Internal statistics
        struct stat {
            size_t  guard_allocated;    ///< Count of allocated HP guards
            size_t  guard_freed;        ///< Count of freed HP guards
            size_t  retired_count;      ///< Count of retired pointers
            size_t  free_count;         ///< Count of free pointers
            size_t  scan_count;         ///< Count of \p scan() call
            size_t  help_scan_count;    ///< Count of \p help_scan() call

            size_t  thread_rec_count;   ///< Count of thread records

            size_t  hp_block_count;         ///< Count of extended HP blocks allocated
            size_t  retired_block_count;    ///< Count of retired blocks allocated
            size_t  hp_extend_count;        ///< Count of hp array \p extend() call
            size_t  retired_extend_count;   ///< Count of retired array \p extend() call

                                        /// Default ctor
            stat()
            {
                clear();
            }

            /// Clears all counters
            void clear()
            {
                guard_allocated =
                    guard_freed =
                    retired_count =
                    free_count =
                    scan_count =
                    help_scan_count =
                    thread_rec_count =
                    hp_block_count =
                    retired_block_count =
                    hp_extend_count =
                    retired_extend_count = 0;
            }
        };

        //@cond
        /// Per-thread data
        struct thread_data {
            thread_hp_storage   hazards_;   ///< Hazard pointers private to the thread
            retired_array       retired_;   ///< Retired data private to the thread

            char pad1_[cds::c_nCacheLineSize];
            atomics::atomic<unsigned int> sync_; ///< dummy var to introduce synchronizes-with relationship between threads
            char pad2_[cds::c_nCacheLineSize];

#       ifdef CDS_ENABLE_HPSTAT
            size_t              free_call_count_;
            size_t              scan_call_count_;
            size_t              help_scan_call_count_;
#       endif

            // CppCheck warn: pad1_ and pad2_ is uninitialized in ctor
            // cppcheck-suppress uninitMemberVar
            thread_data( guard* guards, size_t guard_count )
                : hazards_( guards, guard_count )
                , sync_( 0 )
#       ifdef CDS_ENABLE_HPSTAT
                , free_call_count_(0)
                , scan_call_count_(0)
                , help_scan_call_count_(0)
#       endif
            {}

            thread_data() = delete;
            thread_data( thread_data const& ) = delete;
            thread_data( thread_data&& ) = delete;

            void sync()
            {
                sync_.fetch_add( 1, atomics::memory_order_acq_rel );
            }
        };
        //@endcond

        //@cond
        // Dynamic (adaptive) Hazard Pointer SMR (Safe Memory Reclamation)
        class smr
        {
            struct thread_record;

        public:
            /// Returns the instance of Hazard Pointer \ref smr
            static smr& instance()
            {
#       ifdef CDS_DISABLE_SMR_EXCEPTION
                assert( instance_ != nullptr );
#       else
                if ( !instance_ )
                    CDS_THROW_EXCEPTION( not_initialized());
#       endif
                return *instance_;
            }

            /// Creates Dynamic Hazard Pointer SMR singleton
            /**
                Dynamic Hazard Pointer SMR is a singleton. If DHP instance is not initialized then the function creates the instance.
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
                size_t nInitialHazardPtrCount = 16  ///< Initial number of hazard pointer per thread
            );

            // for back-copatibility
            static void Construct(
                size_t nInitialHazardPtrCount = 16  ///< Initial number of hazard pointer per thread
            )
            {
                construct( nInitialHazardPtrCount );
            }

            /// Destroys global instance of \ref smr
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
            )
            {
                destruct( bDetachAll );
            }

            /// Checks if global SMR object is constructed and may be used
            static bool isUsed() noexcept
            {
                return instance_ != nullptr;
            }

            /// Set memory management functions
            /**
                @note This function may be called <b>BEFORE</b> creating an instance
                of Dynamic Hazard Pointer SMR

                SMR object allocates some memory for thread-specific data and for
                creating SMR object.
                By default, a standard \p new and \p delete operators are used for this.
            */
            static CDS_EXPORT_API void set_memory_allocator(
                void* ( *alloc_func )( size_t size ),
                void( *free_func )( void * p )
            );

            /// Returns thread-local data for the current thread
            static CDS_EXPORT_API thread_data* tls();

            static CDS_EXPORT_API void attach_thread();
            static CDS_EXPORT_API void detach_thread();

            /// Get internal statistics
            CDS_EXPORT_API void statistics( stat& st );

        public: // for internal use only
            /// The main garbage collecting function
            CDS_EXPORT_API void scan( thread_data* pRec );

            /// Helper scan routine
            /**
                The function guarantees that every node that is eligible for reuse is eventually freed, barring
                thread failures. To do so, after executing \p scan(), a thread executes a \p %help_scan(),
                where it checks every HP record. If an HP record is inactive, the thread moves all "lost" reclaimed pointers
                to thread's list of reclaimed pointers.

                The function is called internally by \p scan().
            */
            CDS_EXPORT_API void help_scan( thread_data* pThis );

            hp_allocator& get_hp_allocator()
            {
                return hp_allocator_;
            }

            retired_allocator& get_retired_allocator()
            {
                return retired_allocator_;
            }

        private:
            CDS_EXPORT_API explicit smr(
                size_t nInitialHazardPtrCount
            );

            CDS_EXPORT_API ~smr();

            CDS_EXPORT_API void detach_all_thread();

        private:
            CDS_EXPORT_API thread_record* create_thread_data();
            static CDS_EXPORT_API void destroy_thread_data( thread_record* pRec );

            /// Allocates Hazard Pointer SMR thread private data
            CDS_EXPORT_API thread_record* alloc_thread_data();

            /// Free HP SMR thread-private data
            CDS_EXPORT_API void free_thread_data( thread_record* pRec, bool callHelpScan );

        private:
            static CDS_EXPORT_API smr* instance_;

            atomics::atomic< thread_record*>    thread_list_;   ///< Head of thread list
            size_t const        initial_hazard_count_;  ///< initial number of hazard pointers per thread
            hp_allocator        hp_allocator_;
            retired_allocator   retired_allocator_;

            // temporaries
            std::atomic<size_t> last_plist_size_;   ///< HP array size in last scan() call
        };
        //@endcond

        //@cond
        // for backward compatibility
        typedef smr GarbageCollector;


        // inlines
        inline hp_allocator& hp_allocator::instance()
        {
            return smr::instance().get_hp_allocator();
        }

        inline retired_allocator& retired_allocator::instance()
        {
            return smr::instance().get_retired_allocator();
        }
        //@endcond

    } // namespace dhp


    /// Dynamic (adaptie) Hazard Pointer SMR
    /**  @ingroup cds_garbage_collector

        Implementation of Dynamic (adaptive) Hazard Pointer SMR

        Sources:
            - [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-freeobjects using atomic reads and writes"
            - [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
            - [2004] Andrei Alexandrescy, Maged Michael "Lock-free Data Structures with Hazard Pointers"

        %DHP is an adaptive variant of classic \p cds::gc::HP, see @ref cds_garbage_collectors_comparison "Compare HP implementation"

        @note Internally, %DHP depends on free-list implementation. There are
        DCAS-based free-list \p cds::intrusive::TaggedFreeList and more complicated CAS-based free-list
        \p cds::intrusive::FreeList. For x86 architecture and GCC/clang, libcds selects appropriate free-list
        based on \p -mcx16 compiler flag. You may manually disable DCAS support specifying
        \p -DCDS_DISABLE_128BIT_ATOMIC for 64bit build or \p -DCDS_DISABLE_64BIT_ATOMIC for 32bit build
        in compiler command line. All your projects and libcds MUST be compiled with the same flags -
        either with DCAS support or without it.
        For MS VC++ compiler DCAS is not supported.

        See \ref cds_how_to_use "How to use" section for details how to apply SMR.
    */
    class DHP
    {
    public:
        /// Native guarded pointer type
        typedef void* guarded_pointer;

        /// Atomic reference
        template <typename T> using atomic_ref = atomics::atomic<T *>;

        /// Atomic type
        /**
            @headerfile cds/gc/dhp.h
        */
        template <typename T> using atomic_type = atomics::atomic<T>;

        /// Atomic marked pointer
        template <typename MarkedPtr> using atomic_marked_ptr = atomics::atomic<MarkedPtr>;

        /// Internal statistics
        typedef dhp::stat stat;

        /// Dynamic Hazard Pointer guard
        /**
            A guard is a hazard pointer.
            Additionally, the \p %Guard class manages allocation and deallocation of the hazard pointer

            \p %Guard object is movable but not copyable.

            The guard object can be in two states:
            - unlinked - the guard is not linked with any internal hazard pointer.
              In this state no operation except \p link() and move assignment is supported.
            - linked (default) - the guard allocates an internal hazard pointer and fully operable.

            Due to performance reason the implementation does not check state of the guard in runtime.

            @warning Move assignment can transfer the guard in unlinked state, use with care.
        */
        class Guard
        {
        public:
            /// Default ctor allocates a guard (hazard pointer) from thread-private storage
            Guard() noexcept
                : guard_( dhp::smr::tls()->hazards_.alloc())
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
            void link()
            {
                if ( !guard_ )
                    guard_ = dhp::smr::tls()->hazards_.alloc();
            }

            /// Unlinks the guard from internal hazard pointer; the guard becomes in unlinked state
            void unlink()
            {
                if ( guard_ ) {
                    dhp::smr::tls()->hazards_.free( guard_ );
                    guard_ = nullptr;
                }
            }

            /// Protects a pointer of type <tt> atomic<T*> </tt>
            /**
                Return the value of \p toGuard

                The function tries to load \p toGuard and to store it
                to the HP slot repeatedly until the guard's value equals \p toGuard
            */
            template <typename T>
            T protect( atomics::atomic<T> const& toGuard )
            {
                return protect(toGuard, [](T p) { return p; });
            }

            /// Protects a converted pointer of type <tt> atomic<T*> </tt>
            /**
                Return the value of \p toGuard

                The function tries to load \p toGuard and to store result of \p f functor
                to the HP slot repeatedly until the guard's value equals \p toGuard.

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
                The function is just an assignment, no loop is performed.
                Can be used for a pointer that cannot be changed concurrently
                or for already guarded pointer.
            */
            template <typename T>
            T* assign( T* p )
            {
                assert( guard_ != nullptr );

                guard_->set( p );
                dhp::smr::tls()->sync();
                return p;
            }

            //@cond
            std::nullptr_t assign( std::nullptr_t )
            {
                assert( guard_ != nullptr );

                clear();
                return nullptr;
            }
            //@endcond

            /// Store marked pointer \p p to the guard
            /**
                The function is just an assignment of <tt>p.ptr()</tt>, no loop is performed.
                Can be used for a marked pointer that cannot be changed concurrently
                or for already guarded pointer.
            */
            template <typename T, int BITMASK>
            T* assign( cds::details::marked_ptr<T, BITMASK> p )
            {
                return assign( p.ptr());
            }

            /// Copy from \p src guard to \p this guard
            void copy( Guard const& src )
            {
                assign( src.get_native());
            }

            /// Clears value of the guard
            void clear()
            {
                assert( guard_ != nullptr );

                guard_->clear();
            }

            /// Gets the value currently protected (relaxed read)
            template <typename T>
            T * get() const
            {
                assert( guard_ != nullptr );
                return guard_->get_as<T>();
            }

            /// Gets native guarded pointer stored
            void* get_native() const
            {
                assert( guard_ != nullptr );
                return guard_->get();
            }

            //@cond
            dhp::guard* release()
            {
                dhp::guard* g = guard_;
                guard_ = nullptr;
                return g;
            }

            dhp::guard*& guard_ref()
            {
                return guard_;
            }
            //@endcond

        private:
            //@cond
            dhp::guard* guard_;
            //@endcond
        };

        /// Array of Dynamic Hazard Pointer guards
        /**
            The class is intended for allocating an array of hazard pointer guards.
            Template parameter \p Count defines the size of the array.

            A \p %GuardArray object is not copy- and move-constructible
            and not copy- and move-assignable.
        */
        template <size_t Count>
        class GuardArray
        {
        public:
            /// Rebind array for other size \p OtherCount
            template <size_t OtherCount>
            struct rebind {
                typedef GuardArray<OtherCount>  other   ;   ///< rebinding result
            };

            /// Array capacity
            static constexpr const size_t c_nCapacity = Count;

        public:
            /// Default ctor allocates \p Count hazard pointers
            GuardArray()
            {
                dhp::smr::tls()->hazards_.alloc( guards_ );
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
                dhp::smr::tls()->hazards_.free( guards_ );
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
                The parameter \p f of type Func is a functor to make that conversion:
                \code
                    struct functor {
                        value_type * operator()( T * p );
                    };
                \endcode
                Actually, the result of <tt> f( toGuard.load()) </tt> is assigned to the hazard pointer.
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

            /// Store \p p to the slot \p nIndex
            /**
                The function is just an assignment, no loop is performed.
            */
            template <typename T>
            T * assign( size_t nIndex, T * p )
            {
                assert( nIndex < capacity());

                guards_.set( nIndex, p );
                dhp::smr::tls()->sync();
                return p;
            }

            /// Store marked pointer \p p to the guard
            /**
                The function is just an assignment of <tt>p.ptr()</tt>, no loop is performed.
                Can be used for a marked pointer that cannot be changed concurrently
                or for already guarded pointer.
            */
            template <typename T, int Bitmask>
            T * assign( size_t nIndex, cds::details::marked_ptr<T, Bitmask> p )
            {
                return assign( nIndex, p.ptr());
            }

            /// Copy guarded value from \p src guard to slot at index \p nIndex
            void copy( size_t nIndex, Guard const& src )
            {
                assign( nIndex, src.get_native());
            }

            /// Copy guarded value from slot \p nSrcIndex to slot at index \p nDestIndex
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

            /// Get native guarded pointer stored
            guarded_pointer get_native( size_t nIndex ) const
            {
                assert( nIndex < capacity());
                return guards_[nIndex]->get();
            }

            //@cond
            dhp::guard* release( size_t nIndex ) noexcept
            {
                return guards_.release( nIndex );
            }
            //@endcond

            /// Capacity of the guard array
            static constexpr size_t capacity()
            {
                return Count;
            }

        private:
            //@cond
            dhp::guard_array<c_nCapacity> guards_;
            //@endcond
        };

        /// Guarded pointer
        /**
            A guarded pointer is a pair of a pointer and GC's guard.
            Usually, it is used for returning a pointer to the item from an lock-free container.
            The guard prevents the pointer to be early disposed (freed) by GC.
            After destructing \p %guarded_ptr object the pointer can be disposed (freed) automatically at any time.

            Template arguments:
            - \p GuardedType - a type which the guard stores
            - \p ValueType - a value type
            - \p Cast - a functor for converting <tt>GuardedType*</tt> to <tt>ValueType*</tt>. Default is \p void (no casting).

            For intrusive containers, \p GuardedType is the same as \p ValueType and no casting is needed.
            In such case the \p %guarded_ptr is:
            @code
            typedef cds::gc::DHP::guarded_ptr< foo > intrusive_guarded_ptr;
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
            typedef cds::gc::DHP::guarded_ptr< Foo, std::string, value_accessor > nonintrusive_guarded_ptr;
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
                : guard_( nullptr )
            {}

            //@cond
            explicit guarded_ptr( dhp::guard* g ) noexcept
                : guard_( g )
            {}

            /// Initializes guarded pointer with \p p
            explicit guarded_ptr( guarded_type * p ) noexcept
                : guard_( nullptr )
            {
                reset( p );
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
                \ref release is called if guarded pointer is not \ref empty
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
                return *value_cast()(reinterpret_cast<guarded_type *>(guard_->get()));
            }

            /// Checks if the guarded pointer is \p nullptr
            bool empty() const noexcept
            {
                return guard_ == nullptr || guard_->get( atomics::memory_order_relaxed ) == nullptr;
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
                guard_->set( p );
            }

            //@endcond

        private:
            //@cond
            void alloc_guard()
            {
                if ( !guard_ )
                    guard_ = dhp::smr::tls()->hazards_.alloc();
            }

            void free_guard()
            {
                if ( guard_ ) {
                    dhp::smr::tls()->hazards_.free( guard_ );
                    guard_ = nullptr;
                }
            }
            //@endcond

        private:
            //@cond
            dhp::guard* guard_;
            //@endcond
        };

    public:
        /// Initializes %DHP memory manager singleton
        /**
            Constructor creates and initializes %DHP global object.
            %DHP object should be created before using CDS data structure based on \p %cds::gc::DHP. Usually,
            it is created in the beginning of \p main() function.
            After creating of global object you may use CDS data structures based on \p %cds::gc::DHP.

            \p nInitialThreadGuardCount - initial count of guard allocated for each thread.
                When a thread is initialized the GC allocates local guard pool for the thread from a common guard pool.
                By perforce the local thread's guard pool is grown automatically from common pool.
                When the thread terminated its guard pool is backed to common GC's pool.
        */
        explicit DHP(
            size_t nInitialHazardPtrCount = 16  ///< Initial number of hazard pointer per thread
        )
        {
            dhp::smr::construct( nInitialHazardPtrCount );
        }

        /// Destroys %DHP memory manager
        /**
            The destructor destroys %DHP global object. After calling of this function you may \b NOT
            use CDS data structures based on \p %cds::gc::DHP.
            Usually, %DHP object is destroyed at the end of your \p main().
        */
        ~DHP()
        {
            dhp::GarbageCollector::destruct( true );
        }

        /// Checks if count of hazard pointer is no less than \p nCountNeeded
        /**
            The function always returns \p true since the guard count is unlimited for
            \p %gc::DHP garbage collector.
        */
        static constexpr bool check_available_guards(
#ifdef CDS_DOXYGEN_INVOKED
            size_t nCountNeeded,
#else
            size_t
#endif
        )
        {
            return true;
        }

        /// Set memory management functions
        /**
            @note This function may be called <b>BEFORE</b> creating an instance
            of Dynamic Hazard Pointer SMR

            SMR object allocates some memory for thread-specific data and for creating SMR object.
            By default, a standard \p new and \p delete operators are used for this.
        */
        static void set_memory_allocator(
            void* ( *alloc_func )( size_t size ),   ///< \p malloc() function
            void( *free_func )( void * p )          ///< \p free() function
        )
        {
            dhp::smr::set_memory_allocator( alloc_func, free_func );
        }

        /// Retire pointer \p p with function \p pFunc
        /**
            The function places pointer \p p to array of pointers ready for removing.
            (so called retired pointer array). The pointer can be safely removed when no hazard pointer points to it.
            \p func is a disposer: when \p p can be safely removed, \p func is called.
        */
        template <typename T>
        static void retire( T * p, void (* func)(void *))
        {
            dhp::thread_data* rec = dhp::smr::tls();
            if ( !rec->retired_.push( dhp::retired_ptr( p, func )))
                dhp::smr::instance().scan( rec );
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

            cds::gc::DHP::retire<disposer>( p ) ;   // place p to retired pointer array of DHP SMR
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
        static void retire( T* p )
        {
            if ( !dhp::smr::tls()->retired_.push( dhp::retired_ptr( p, +[]( void* p ) { Disposer()( static_cast<T*>( p )); })))
                scan();
        }

        /// Checks if Dynamic Hazard Pointer GC is constructed and may be used
        static bool isUsed()
        {
            return dhp::smr::isUsed();
        }

        /// Forced GC cycle call for current thread
        /**
            Usually, this function should not be called directly.
        */
        static void scan()
        {
            dhp::smr::instance().scan( dhp::smr::tls());
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
            dhp::smr::instance().statistics( st );
        }

        /// Returns post-mortem statistics
        /**
            Post-mortem statistics is gathered in the \p %DHP object destructor
            and can be accessible after destructing the global \p %DHP object.

            @note Internal statistics is available only if you compile
            \p libcds and your program with \p -DCDS_ENABLE_HPSTAT.

            Usage:
            \code
            int main()
            {
                cds::Initialize();
                {
                    // Initialize DHP SMR
                    cds::gc::DHP dhp;

                    // deal with DHP-based data structured
                    // ...
                }

                // DHP object destroyed
                // Get total post-mortem statistics
                cds::gc::DHP::stat const& st = cds::gc::DHP::postmortem_statistics();

                printf( "DHP statistics:\n"
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
        CDS_EXPORT_API static stat const& postmortem_statistics();
    };

}} // namespace cds::gc

#endif // #ifndef CDSLIB_GC_DHP_SMR_H



// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_GC_DETAILS_HP_COMMON_H
#define CDSLIB_GC_DETAILS_HP_COMMON_H

#include <cds/algo/atomic.h>
#include <cds/gc/details/retired_ptr.h>
#include <cds/details/throw_exception.h>

#ifdef CDS_ENABLE_HPSTAT
#   define CDS_HPSTAT( expr ) expr
#else
#   define CDS_HPSTAT( expr )
#endif

//@cond
namespace cds { namespace gc { namespace hp { namespace common {

    /// Hazard pointer type
    typedef void*   hazard_ptr;

    /// Retired pointer
    using cds::gc::details::retired_ptr;
    using cds::gc::make_retired_ptr;

    /// Hazard pointer guard
    class guard
    {
    public:
        guard() noexcept
            : hp_( nullptr )
            , next_( nullptr )
        {}

        template <typename T>
        T* operator=( T* ptr ) noexcept
        {
            set( ptr );
            return ptr;
        }

        std::nullptr_t operator=( std::nullptr_t ) noexcept
        {
            clear();
            return nullptr;
        }

        hazard_ptr get() const noexcept
        {
            return hp_.load( atomics::memory_order_acquire );
        }

        hazard_ptr get( atomics::memory_order order ) const noexcept
        {
            return hp_.load( order );
        }

        template <typename T>
        T* get_as() const noexcept
        {
            return reinterpret_cast<T*>( get());
        }

        template <typename T>
        void set( T* ptr ) noexcept
        {
            hp_.store( reinterpret_cast<hazard_ptr>( ptr ), atomics::memory_order_release );
        }

        void clear( atomics::memory_order order ) noexcept
        {
            hp_.store( nullptr, order );
        }

        void clear() noexcept
        {
            clear( atomics::memory_order_release );
        }

    private:
        atomics::atomic<hazard_ptr>   hp_;

    public:
        guard* next_;   // free guard list
    };

    /// Array of guards
    template <size_t Capacity>
    class guard_array
    {
    public:
        static size_t const c_nCapacity = Capacity;

    public:
        guard_array()
            : arr_{ nullptr }
        {}

        static constexpr size_t capacity()
        {
            return c_nCapacity;
        }

        guard* operator[]( size_t idx ) const noexcept
        {
            assert( idx < capacity());
            return arr_[idx];
        }

        template <typename T>
        void set( size_t idx, T* ptr ) noexcept
        {
            assert( idx < capacity());
            assert( arr_[idx] != nullptr );

            arr_[idx]->set( ptr );
        }

        void clear( size_t idx ) noexcept
        {
            assert( idx < capacity());
            assert( arr_[idx] != nullptr );

            arr_[idx]->clear();
        }

        guard* release( size_t idx ) noexcept
        {
            assert( idx < capacity());

            guard* g = arr_[idx];
            arr_[idx] = nullptr;
            return g;
        }

        void reset( size_t idx, guard* g ) noexcept
        {
            assert( idx < capacity());
            assert( arr_[idx] == nullptr );

            arr_[idx] = g;
        }

    private:
        guard*  arr_[c_nCapacity];
    };


    /// Retired pointer disposer
    typedef void ( *disposer_func )( void* );

}}}} // namespace cds::gc::hp::common
//@endcond

namespace cds { namespace gc { namespace hp { namespace details {
    using namespace cds::gc::hp::common;

    /// Exception "Not enough Hazard Pointer"
    class not_enough_hazard_ptr : public std::length_error {
        //@cond
    public:
        not_enough_hazard_ptr()
                : std::length_error("Not enough Hazard Pointer") {}
        //@endcond
    };

    /// Exception "Hazard Pointer SMR is not initialized"
    class not_initialized : public std::runtime_error {
        //@cond
    public:
        not_initialized()
                : std::runtime_error("Global Hazard Pointer SMR object is not initialized") {}
        //@endcond
    };

    //@cond
    /// Per-thread hazard pointer storage
    class thread_hp_storage {
    public:
        thread_hp_storage(guard *arr, size_t nSize) noexcept
            : free_head_(arr), array_(arr), capacity_(nSize)
#       ifdef CDS_ENABLE_HPSTAT
            , alloc_guard_count_(0)
            , free_guard_count_(0)
#       endif
        {
            // Initialize guards
            new( arr ) guard[nSize];

            for ( guard *pEnd = arr + nSize - 1; arr < pEnd; ++arr )
                arr->next_ = arr + 1;
            arr->next_ = nullptr;
        }

        thread_hp_storage() = delete;

        thread_hp_storage(thread_hp_storage const &) = delete;

        thread_hp_storage(thread_hp_storage &&) = delete;

        size_t capacity() const noexcept {
            return capacity_;
        }

        bool full() const noexcept {
            return free_head_ == nullptr;
        }

        guard *alloc() {
#       ifdef CDS_DISABLE_SMR_EXCEPTION
            assert(!full());
#       else
            if ( full() )
                CDS_THROW_EXCEPTION(not_enough_hazard_ptr());
#       endif
            guard *g = free_head_;
            free_head_ = g->next_;
            CDS_HPSTAT(++alloc_guard_count_);
            return g;
        }

        void free(guard *g) noexcept {
            assert(g >= array_ && g < array_ + capacity());

            if ( g ) {
                g->clear();
                g->next_ = free_head_;
                free_head_ = g;
                CDS_HPSTAT(++free_guard_count_);
            }
        }

        template<size_t Capacity>
        size_t alloc(guard_array<Capacity> &arr) {
            size_t i;
            guard *g = free_head_;
            for ( i = 0; i < Capacity && g; ++i ) {
                arr.reset(i, g);
                g = g->next_;
            }

#       ifdef CDS_DISABLE_SMR_EXCEPTION
            assert(i == Capacity);
#       else
            if ( i != Capacity )
                CDS_THROW_EXCEPTION(not_enough_hazard_ptr());
#       endif
            free_head_ = g;
            CDS_HPSTAT(alloc_guard_count_ += Capacity);
            return i;
        }

        template<size_t Capacity>
        void free(guard_array<Capacity> &arr) noexcept {
            guard *gList = free_head_;
            for ( size_t i = 0; i < Capacity; ++i ) {
                guard *g = arr[i];
                if ( g ) {
                    g->clear();
                    g->next_ = gList;
                    gList = g;
                    CDS_HPSTAT(++free_guard_count_);
                }
            }
            free_head_ = gList;
        }

        // cppcheck-suppress functionConst
        void clear() {
            for ( guard *cur = array_, *last = array_ + capacity(); cur < last; ++cur )
                cur->clear();
        }

        guard &operator[](size_t idx) {
            assert(idx < capacity());

            return array_[idx];
        }

        static size_t calc_array_size(size_t capacity) {
            return sizeof(guard) * capacity;
        }

        guard *begin() const {
            return array_;
        }

        guard *end() const {
            return &array_[capacity_];
        }

    private:
        guard *free_head_; ///< Head of free guard list
        guard *const array_;     ///< HP array
        size_t const capacity_;  ///< HP array capacity
#       ifdef CDS_ENABLE_HPSTAT
    public:
        size_t          alloc_guard_count_;
        size_t          free_guard_count_;
#       endif
    };
    //@endcond

    //@cond
    /// Per-thread retired array
    class retired_array {
    public:
        retired_array(retired_ptr *arr, size_t capacity) noexcept
            : current_(arr), last_(arr + capacity), retired_(arr)
#       ifdef CDS_ENABLE_HPSTAT
            , retire_call_count_(0)
#       endif
        {}

        retired_array() = delete;

        retired_array(retired_array const &) = delete;

        retired_array(retired_array &&) = delete;

        size_t capacity() const noexcept {
            return last_ - retired_;
        }

        size_t size() const noexcept {
            return current_.load(atomics::memory_order_relaxed) - retired_;
        }

        bool push(retired_ptr &&p) noexcept {
            retired_ptr *cur = current_.load(atomics::memory_order_relaxed);
            *cur = p;
            CDS_HPSTAT(++retire_call_count_);
            current_.store(cur + 1, atomics::memory_order_relaxed);
            return cur + 1 < last_;
        }

        retired_ptr *first() const noexcept {
            return retired_;
        }

        retired_ptr *last() const noexcept {
            return current_.load(atomics::memory_order_relaxed);
        }

        void reset(size_t nSize) noexcept {
            current_.store(first() + nSize, atomics::memory_order_relaxed);
        }

        void interthread_clear() {
            current_.exchange(first(), atomics::memory_order_acq_rel);
        }

        bool full() const noexcept {
            return current_.load(atomics::memory_order_relaxed) == last_;
        }

        static size_t calc_array_size(size_t capacity) {
            return sizeof(retired_ptr) * capacity;
        }

    private:
        atomics::atomic<retired_ptr *> current_;
        retired_ptr *const last_;
        retired_ptr *const retired_;
#   ifdef CDS_ENABLE_HPSTAT
    public:
        size_t  retire_call_count_;
#   endif
    };
    //@endcond

    /// Internal statistics
    struct stat {
        size_t guard_allocated;    ///< Count of allocated HP guards
        size_t guard_freed;        ///< Count of freed HP guards
        size_t retired_count;      ///< Count of retired pointers
        size_t free_count;         ///< Count of free pointers
        size_t scan_count;         ///< Count of \p scan() call
        size_t help_scan_count;    ///< Count of \p help_scan() call

        size_t thread_rec_count;   ///< Count of thread records

        /// Default ctor
        stat() {
            clear();
        }

        /// Clears all counters
        void clear() {
            guard_allocated =
            guard_freed =
            retired_count =
            free_count =
            scan_count =
            help_scan_count =
            thread_rec_count = 0;
        }
    };

    stat const& postmortem_statistics();

    //@cond
    /// Per-thread data
    struct thread_data {
        thread_hp_storage hazards_;   ///< Hazard pointers private to the thread
        retired_array retired_;   ///< Retired data private to the thread

        char pad1_[cds::c_nCacheLineSize];
        atomics::atomic<unsigned int> sync_; ///< dummy var to introduce synchronizes-with relationship between threads
        char pad2_[cds::c_nCacheLineSize];

#   ifdef CDS_ENABLE_HPSTAT
        // Internal statistics:
        size_t              free_count_;
        size_t              scan_count_;
        size_t              help_scan_count_;
#   endif

        // CppCheck warn: pad1_ and pad2_ is uninitialized in ctor
        // cppcheck-suppress uninitMemberVar
        thread_data(guard *guards, size_t guard_count, retired_ptr *retired_arr, size_t retired_capacity)
            : hazards_(guards, guard_count), retired_(retired_arr, retired_capacity), sync_(0)
#       ifdef CDS_ENABLE_HPSTAT
            , free_count_(0)
            , scan_count_(0)
            , help_scan_count_(0)
#       endif
        {}

        thread_data() = delete;

        thread_data(thread_data const &) = delete;

        thread_data(thread_data &&) = delete;

        void sync() {
            sync_.fetch_add(1, atomics::memory_order_acq_rel);
        }
    };
    //@endcond

    /// Default TLS manager
    /**
        By default, HP stores its data in TLS.
        This class provides such behavoiur.
    */
    class DefaultTLSManager {
#ifndef CDS_DISABLE_CLASS_TLS_INLINE
        // GCC, CLang
    public:
        /// Get HP data for current thread
        static thread_data* getTLS()
        {
            return tls_;
        }
        /// Set HP data for current thread
        static void setTLS(thread_data* td)
        {
            tls_ = td;
        }
    private:
        //@cond
        static thread_local thread_data* tls_;
        //@endcond
#else   
        // MSVC
    public:
        static CDS_EXPORT_API thread_data* getTLS() noexcept;
        static CDS_EXPORT_API void setTLS(thread_data*) noexcept;
#endif
    };

    //@cond
    // Strange thread manager for testing purpose only!
    class StrangeTLSManager {
    public:
        static CDS_EXPORT_API thread_data* getTLS();
        static CDS_EXPORT_API void setTLS(thread_data*);
    };
    //@endcond
}}}} // namespace cds::gc::hp::details

#endif // #ifndef CDSLIB_GC_DETAILS_HP_COMMON_H



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

#ifndef CDSLIB_GC_IMPL_DHP_DECL_H
#define CDSLIB_GC_IMPL_DHP_DECL_H

#include <cds/gc/details/dhp.h>
#include <cds/details/marked_ptr.h>
#include <cds/details/static_functor.h>

namespace cds { namespace gc {

    /// Dynamic Hazard Pointer garbage collector
    /**  @ingroup cds_garbage_collector
        @headerfile cds/gc/dhp.h

        Implementation of Dynamic Hazard Pointer garbage collector.

        Sources:
            - [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-freeobjects using atomic reads and writes"
            - [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
            - [2004] Andrei Alexandrescy, Maged Michael "Lock-free Data Structures with Hazard Pointers"

        Dynamic Hazard Pointers SMR (safe memory reclamation) provides an unbounded number of hazard pointer per thread
        despite of classic Hazard Pointer SMR in which the count of the hazard pointef per thread is limited.

        See \ref cds_how_to_use "How to use" section for details how to apply garbage collector.
    */
    class DHP
    {
    public:
        /// Native guarded pointer type
        /**
            @headerfile cds/gc/dhp.h
        */
        typedef void * guarded_pointer;

        /// Atomic reference
        /**
            @headerfile cds/gc/dhp.h
        */
        template <typename T> using atomic_ref = atomics::atomic<T *>;

        /// Atomic type
        /**
            @headerfile cds/gc/dhp.h
        */
        template <typename T> using atomic_type = atomics::atomic<T>;

        /// Atomic marked pointer
        /**
            @headerfile cds/gc/dhp.h
        */
        template <typename MarkedPtr> using atomic_marked_ptr = atomics::atomic<MarkedPtr>;

        /// Thread GC implementation for internal usage
        /**
            @headerfile cds/gc/dhp.h
        */
        typedef dhp::ThreadGC   thread_gc_impl;

        /// Thread-level garbage collector
        /**
            @headerfile cds/gc/dhp.h
            This class performs automatically attaching/detaching Dynamic Hazard Pointer GC
            for the current thread.
        */
        class thread_gc: public thread_gc_impl
        {
            //@cond
            bool    m_bPersistent;
            //@endcond
        public:
            /// Constructor
            /**
                The constructor attaches the current thread to the Dynamic Hazard Pointer GC
                if it is not yet attached.
                The \p bPersistent parameter specifies attachment persistence:
                - \p true - the class destructor will not detach the thread from Dynamic Hazard Pointer GC.
                - \p false (default) - the class destructor will detach the thread from Dynamic Hazard Pointer GC.
            */
            thread_gc(
                bool    bPersistent = false
            )   ;   // inline in dhp_impl.h

            /// Destructor
            /**
                If the object has been created in persistent mode, the destructor does nothing.
                Otherwise it detaches the current thread from Dynamic Hazard Pointer GC.
            */
            ~thread_gc()    ;   // inline in dhp_impl.h

        public: // for internal use only!!!
            //@cond
            static void alloc_guard( cds::gc::dhp::details::guard& g ); // inline in dhp_impl.h
            static void free_guard( cds::gc::dhp::details::guard& g ); // inline in dhp_impl.h
            //@endcond
        };


        /// Dynamic Hazard Pointer guard
        /**
            @headerfile cds/gc/dhp.h

            A guard is the hazard pointer.
            Additionally, the \p %Guard class manages allocation and deallocation of the hazard pointer

            A \p %Guard object is not copy- and move-constructible
            and not copy- and move-assignable.
        */
        class Guard: public dhp::Guard
        {
            //@cond
            typedef dhp::Guard base_class;
            //@endcond

        public: // for internal use only
            //@cond
            typedef cds::gc::dhp::details::guard native_guard;
            //@endcond

        public:
            // Default ctor
            Guard()
            {}

            //@cond
            Guard( Guard const& ) = delete;
            Guard( Guard&& s ) = delete;
            Guard& operator=(Guard const&) = delete;
            Guard& operator=(Guard&&) = delete;
            //@endcond

            /// Protects a pointer of type <tt> atomic<T*> </tt>
            /**
                Return the value of \p toGuard

                The function tries to load \p toGuard and to store it
                to the HP slot repeatedly until the guard's value equals \p toGuard
            */
            template <typename T>
            T protect( atomics::atomic<T> const& toGuard )
            {
                T pCur = toGuard.load(atomics::memory_order_acquire);
                T pRet;
                do {
                    pRet = assign( pCur );
                    pCur = toGuard.load(atomics::memory_order_acquire);
                } while ( pRet != pCur );
                return pCur;
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
                Really, the result of <tt> f( toGuard.load() ) </tt> is assigned to the hazard pointer.
            */
            template <typename T, class Func>
            T protect( atomics::atomic<T> const& toGuard, Func f )
            {
                T pCur = toGuard.load(atomics::memory_order_acquire);
                T pRet;
                do {
                    pRet = pCur;
                    assign( f( pCur ) );
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
            T * assign( T * p )
            {
                return base_class::operator =(p);
            }

            //@cond
            std::nullptr_t assign( std::nullptr_t )
            {
                return base_class::operator =(nullptr);
            }
            //@endcond

            /// Store marked pointer \p p to the guard
            /**
                The function is just an assignment of <tt>p.ptr()</tt>, no loop is performed.
                Can be used for a marked pointer that cannot be changed concurrently
                or for already guarded pointer.
            */
            template <typename T, int BITMASK>
            T * assign( cds::details::marked_ptr<T, BITMASK> p )
            {
                return base_class::operator =( p.ptr() );
            }

            /// Copy from \p src guard to \p this guard
            void copy( Guard const& src )
            {
                assign( src.get_native() );
            }

            /// Clears value of the guard
            void clear()
            {
                base_class::clear();
            }

            /// Gets the value currently protected (relaxed read)
            template <typename T>
            T * get() const
            {
                return reinterpret_cast<T *>( get_native() );
            }

            /// Gets native guarded pointer stored
            guarded_pointer get_native() const
            {
                return base_class::get_guard()->pPost.load(atomics::memory_order_relaxed);
            }
        };

        /// Array of Dynamic Hazard Pointer guards
        /**
            @headerfile cds/gc/dhp.h
            The class is intended for allocating an array of hazard pointer guards.
            Template parameter \p Count defines the size of the array.

            A \p %GuardArray object is not copy- and move-constructible
            and not copy- and move-assignable.
        */
        template <size_t Count>
        class GuardArray: public dhp::GuardArray<Count>
        {
            //@cond
            typedef dhp::GuardArray<Count> base_class;
            //@endcond
        public:
            /// Rebind array for other size \p OtherCount
            template <size_t OtherCount>
            struct rebind {
                typedef GuardArray<OtherCount>  other   ;   ///< rebinding result
            };

        public:
            // Default ctor
            GuardArray()
            {}

            //@cond
            GuardArray( GuardArray const& ) = delete;
            GuardArray( GuardArray&& ) = delete;
            GuardArray& operator=(GuardArray const&) = delete;
            GuardArray& operator-(GuardArray&&) = delete;
            //@endcond

            /// Protects a pointer of type \p atomic<T*>
            /**
                Return the value of \p toGuard

                The function tries to load \p toGuard and to store it
                to the slot \p nIndex repeatedly until the guard's value equals \p toGuard
            */
            template <typename T>
            T protect( size_t nIndex, atomics::atomic<T> const& toGuard )
            {
                T pRet;
                do {
                    pRet = assign( nIndex, toGuard.load(atomics::memory_order_acquire) );
                } while ( pRet != toGuard.load(atomics::memory_order_relaxed));

                return pRet;
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
                Actually, the result of <tt> f( toGuard.load() ) </tt> is assigned to the hazard pointer.
            */
            template <typename T, class Func>
            T protect( size_t nIndex, atomics::atomic<T> const& toGuard, Func f )
            {
                T pRet;
                do {
                    assign( nIndex, f( pRet = toGuard.load(atomics::memory_order_acquire) ));
                } while ( pRet != toGuard.load(atomics::memory_order_relaxed));

                return pRet;
            }

            /// Store \p p to the slot \p nIndex
            /**
                The function is just an assignment, no loop is performed.
            */
            template <typename T>
            T * assign( size_t nIndex, T * p )
            {
                base_class::set(nIndex, p);
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
                return assign( nIndex, p.ptr() );
            }

            /// Copy guarded value from \p src guard to slot at index \p nIndex
            void copy( size_t nIndex, Guard const& src )
            {
                assign( nIndex, src.get_native() );
            }

            /// Copy guarded value from slot \p nSrcIndex to slot at index \p nDestIndex
            void copy( size_t nDestIndex, size_t nSrcIndex )
            {
                assign( nDestIndex, get_native( nSrcIndex ));
            }

            /// Clear value of the slot \p nIndex
            void clear( size_t nIndex )
            {
                base_class::clear( nIndex );
            }

            /// Get current value of slot \p nIndex
            template <typename T>
            T * get( size_t nIndex ) const
            {
                return reinterpret_cast<T *>( get_native( nIndex ) );
            }

            /// Get native guarded pointer stored
            guarded_pointer get_native( size_t nIndex ) const
            {
                return base_class::operator[](nIndex).get_guard()->pPost.load(atomics::memory_order_relaxed);
            }

            /// Capacity of the guard array
            static CDS_CONSTEXPR size_t capacity()
            {
                return Count;
            }
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
            //@endcond

        public:
            typedef GuardedType guarded_type; ///< Guarded type
            typedef ValueType   value_type;   ///< Value type

            /// Functor for casting \p guarded_type to \p value_type
            typedef typename std::conditional< std::is_same<Cast, void>::value, trivial_cast, Cast >::type value_cast;

            //@cond
            typedef cds::gc::dhp::details::guard native_guard;
            //@endcond

        private:
            //@cond
            native_guard    m_guard;
            //@endcond

        public:
            /// Creates empty guarded pointer
            guarded_ptr() CDS_NOEXCEPT
            {}

            //@cond
            /// Initializes guarded pointer with \p p
            explicit guarded_ptr( guarded_type * p ) CDS_NOEXCEPT
            {
                alloc_guard();
                assert( m_guard.is_initialized() );
                m_guard.set( p );
            }
            explicit guarded_ptr( std::nullptr_t ) CDS_NOEXCEPT
            {}
            //@endcond

            /// Move ctor
            guarded_ptr( guarded_ptr&& gp ) CDS_NOEXCEPT
            {
                m_guard.set_guard( gp.m_guard.release_guard() );
            }

            /// The guarded pointer is not copy-constructible
            guarded_ptr( guarded_ptr const& gp ) = delete;

            /// Clears the guarded pointer
            /**
                \ref release is called if guarded pointer is not \ref empty
            */
            ~guarded_ptr() CDS_NOEXCEPT
            {
                free_guard();
            }

            /// Move-assignment operator
            guarded_ptr& operator=( guarded_ptr&& gp ) CDS_NOEXCEPT
            {
                free_guard();
                m_guard.set_guard( gp.m_guard.release_guard() );
                return *this;
            }

            /// The guarded pointer is not copy-assignable
            guarded_ptr& operator=(guarded_ptr const& gp) = delete;

            /// Returns a pointer to guarded value
            value_type * operator ->() const CDS_NOEXCEPT
            {
                assert( !empty() );
                return value_cast()( reinterpret_cast<guarded_type *>(m_guard.get()));
            }

            /// Returns a reference to guarded value
            value_type& operator *() CDS_NOEXCEPT
            {
                assert( !empty());
                return *value_cast()(reinterpret_cast<guarded_type *>(m_guard.get()));
            }

            /// Returns const reference to guarded value
            value_type const& operator *() const CDS_NOEXCEPT
            {
                assert( !empty() );
                return *value_cast()(reinterpret_cast<guarded_type *>(m_guard.get()));
            }

            /// Checks if the guarded pointer is \p nullptr
            bool empty() const CDS_NOEXCEPT
            {
                return !m_guard.is_initialized() || m_guard.get( atomics::memory_order_relaxed ) == nullptr;
            }

            /// \p bool operator returns <tt>!empty()</tt>
            explicit operator bool() const CDS_NOEXCEPT
            {
                return !empty();
            }

            /// Clears guarded pointer
            /**
                If the guarded pointer has been released, the pointer can be disposed (freed) at any time.
                Dereferncing the guarded pointer after \p release() is dangerous.
            */
            void release() CDS_NOEXCEPT
            {
                free_guard();
            }

            //@cond
            // For internal use only!!!
            native_guard& guard() CDS_NOEXCEPT
            {
                alloc_guard();
                assert( m_guard.is_initialized() );
                return m_guard;
            }

            void reset(guarded_type * p) CDS_NOEXCEPT
            {
                alloc_guard();
                assert( m_guard.is_initialized() );
                m_guard.set(p);
            }

            //@endcond

        private:
            //@cond
            void alloc_guard()
            {
                if ( !m_guard.is_initialized() )
                    thread_gc::alloc_guard( m_guard );
            }

            void free_guard()
            {
                if ( m_guard.is_initialized() )
                    thread_gc::free_guard( m_guard );
            }
            //@endcond
        };

    public:
        /// Initializes %DHP memory manager singleton
        /**
            Constructor creates and initializes %DHP global object.
            %DHP object should be created before using CDS data structure based on \p %cds::gc::DHP GC. Usually,
            it is created in the \p main() function.
            After creating of global object you may use CDS data structures based on \p %cds::gc::DHP.

            \par Parameters
            - \p nLiberateThreshold - \p scan() threshold. When count of retired pointers reaches this value,
                the \p scan() member function would be called for freeing retired pointers.
            - \p nInitialThreadGuardCount - initial count of guard allocated for each thread.
                When a thread is initialized the GC allocates local guard pool for the thread from common guard pool.
                By perforce the local thread's guard pool is grown automatically from common pool.
                When the thread terminated its guard pool is backed to common GC's pool.
            - \p nEpochCount: internally, DHP memory manager uses epoch-based schema to solve
                ABA problem for internal data. \p nEpochCount specifies the epoch count,
                i.e. the count of simultaneously working threads that remove the elements
                of DHP-based concurrent data structure. Default value is 16.
        */
        DHP(
            size_t nLiberateThreshold = 1024
            , size_t nInitialThreadGuardCount = 8
            , size_t nEpochCount = 16
        )
        {
            dhp::GarbageCollector::Construct( nLiberateThreshold, nInitialThreadGuardCount, nEpochCount );
        }

        /// Destroys %DHP memory manager
        /**
            The destructor destroys %DHP global object. After calling of this function you may \b NOT
            use CDS data structures based on \p %cds::gc::DHP.
            Usually, %DHP object is destroyed at the end of your \p main().
        */
        ~DHP()
        {
            dhp::GarbageCollector::Destruct();
        }

        /// Checks if count of hazard pointer is no less than \p nCountNeeded
        /**
            The function always returns \p true since the guard count is unlimited for
            \p %gc::DHP garbage collector.
        */
        static CDS_CONSTEXPR bool check_available_guards(
#ifdef CDS_DOXYGEN_INVOKED
            size_t nCountNeeded,
#else
            size_t,
#endif
            bool /*bRaiseException*/ = true )
        {
            return true;
        }

        /// Retire pointer \p p with function \p pFunc
        /**
            The function places pointer \p p to array of pointers ready for removing.
            (so called retired pointer array). The pointer can be safely removed when no guarded pointer points to it.
            Deleting the pointer is the function \p pFunc call.
        */
        template <typename T>
        static void retire( T * p, void (* pFunc)(T *) )
        {
            dhp::GarbageCollector::instance().retirePtr( p, pFunc );
        }

        /// Retire pointer \p p with functor of type \p Disposer
        /**
            The function places pointer \p p to array of pointers ready for removing.
            (so called retired pointer array). The pointer can be safely removed when no guarded pointer points to it.

            See \p gc::HP::retire for \p Disposer requirements.
        */
        template <class Disposer, typename T>
        static void retire( T * p )
        {
            retire( p, cds::details::static_functor<Disposer, T>::call );
        }

        /// Checks if Dynamic Hazard Pointer GC is constructed and may be used
        static bool isUsed()
        {
            return dhp::GarbageCollector::isUsed();
        }

        /// Forced GC cycle call for current thread
        /**
            Usually, this function should not be called directly.
        */
        static void scan()  ;   // inline in dhp_impl.h

        /// Synonym for \ref scan()
        static void force_dispose()
        {
            scan();
        }
    };

}} // namespace cds::gc

#endif // #ifndef CDSLIB_GC_IMPL_DHP_DECL_H

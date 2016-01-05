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

#ifndef CDSLIB_GC_IMPL_HP_DECL_H
#define CDSLIB_GC_IMPL_HP_DECL_H

#include <stdexcept>    // overflow_error
#include <cds/gc/details/hp.h>
#include <cds/details/marked_ptr.h>

namespace cds { namespace gc {
    /// @defgroup cds_garbage_collector Garbage collectors

    /// Hazard Pointer garbage collector
    /**  @ingroup cds_garbage_collector
        @headerfile cds/gc/hp.h

        Implementation of classic Hazard Pointer garbage collector.

        Sources:
            - [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-freeobjects using atomic reads and writes"
            - [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
            - [2004] Andrei Alexandrescy, Maged Michael "Lock-free Data Structures with Hazard Pointers"

        Hazard Pointer garbage collector is a singleton. The main user-level part of Hazard Pointer schema is
        GC class \p %cds::gc::HP and its nested classes. Before use any HP-related class you must initialize HP garbage collector
        by contructing \p %cds::gc::HP object in beginning of your \p main().
        See \ref cds_how_to_use "How to use" section for details how to apply garbage collector.
    */
    class HP
    {
    public:
        /// Native guarded pointer type
        /**
            @headerfile cds/gc/hp.h
        */
        typedef gc::hp::hazard_pointer guarded_pointer;

        /// Atomic reference
        /**
            @headerfile cds/gc/hp.h
        */
        template <typename T> using atomic_ref = atomics::atomic<T *>;

        /// Atomic marked pointer
        /**
            @headerfile cds/gc/hp.h
        */
        template <typename MarkedPtr> using atomic_marked_ptr = atomics::atomic<MarkedPtr>;

        /// Atomic type
        /**
            @headerfile cds/gc/hp.h
        */
        template <typename T> using atomic_type = atomics::atomic<T>;

        /// Thread GC implementation for internal usage
        /**
            @headerfile cds/gc/hp.h
        */
        typedef hp::ThreadGC   thread_gc_impl;

        /// Wrapper for hp::ThreadGC class
        /**
            @headerfile cds/gc/hp.h
            This class performs automatically attaching/detaching Hazard Pointer GC
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
                The constructor attaches the current thread to the Hazard Pointer GC
                if it is not yet attached.
                The \p bPersistent parameter specifies attachment persistence:
                - \p true - the class destructor will not detach the thread from Hazard Pointer GC.
                - \p false (default) - the class destructor will detach the thread from Hazard Pointer GC.
            */
            thread_gc(
                bool    bPersistent = false
            ) ;     //inline in hp_impl.h

            /// Destructor
            /**
                If the object has been created in persistent mode, the destructor does nothing.
                Otherwise it detaches the current thread from Hazard Pointer GC.
            */
            ~thread_gc() ;  // inline in hp_impl.h

        public: // for internal use only!!!
            //@cond
            static cds::gc::hp::details::hp_guard& alloc_guard(); // inline in hp_impl.h
            static void free_guard( cds::gc::hp::details::hp_guard& g ); // inline in hp_impl.h
            //@endcond
        };

        /// Hazard Pointer guard
        /**
            @headerfile cds/gc/hp.h

            A guard is the hazard pointer.
            Additionally, the \p %Guard class manages allocation and deallocation of the hazard pointer

            A \p %Guard object is not copy- and move-constructible
            and not copy- and move-assignable.
        */
        class Guard : public hp::guard
        {
            //@cond
            typedef hp::guard base_class;
            //@endcond

        public:
            /// Default ctor
            Guard()
            {}

            //@cond
            Guard( Guard const& ) = delete;
            Guard( Guard&& s ) = delete;
            Guard& operator=(Guard const&) = delete;
            Guard& operator=(Guard&&) = delete;
            //@endcond

            /// Protects a pointer of type \p atomic<T*>
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
                The function equals to a simple assignment the value \p p to guard, no loop is performed.
                Can be used for a pointer that cannot be changed concurrently
            */
            template <typename T>
            T * assign( T * p );    // inline in hp_impl.h

            //@cond
            std::nullptr_t assign( std::nullptr_t )
            {
                return base_class::operator =(nullptr);
            }
            //@endcond

            /// Copy from \p src guard to \p this guard
            void copy( Guard const& src )
            {
                assign( src.get_native() );
            }

            /// Store marked pointer \p p to the guard
            /**
                The function equals to a simple assignment of <tt>p.ptr()</tt>, no loop is performed.
                Can be used for a marked pointer that cannot be changed concurrently.
            */
            template <typename T, int BITMASK>
            T * assign( cds::details::marked_ptr<T, BITMASK> p )
            {
                return base_class::operator =( p.ptr() );
            }

            /// Clear value of the guard
            void clear()
            {
                assign( nullptr );
            }

            /// Get the value currently protected
            template <typename T>
            T * get() const
            {
                return reinterpret_cast<T *>( get_native() );
            }

            /// Get native hazard pointer stored
            guarded_pointer get_native() const
            {
                return base_class::get();
            }
        };

        /// Array of Hazard Pointer guards
        /**
            @headerfile cds/gc/hp.h
            The class is intended for allocating an array of hazard pointer guards.
            Template parameter \p Count defines the size of the array.

            A \p %GuardArray object is not copy- and move-constructible
            and not copy- and move-assignable.
        */
        template <size_t Count>
        class GuardArray : public hp::array<Count>
        {
            //@cond
            typedef hp::array<Count> base_class;
            //@endcond
        public:
            /// Rebind array for other size \p Count2
            template <size_t Count2>
            struct rebind {
                typedef GuardArray<Count2>  other   ;   ///< rebinding result
            };

        public:
            /// Default ctor
            GuardArray()
            {}

            //@cond
            GuardArray( GuardArray const& ) = delete;
            GuardArray( GuardArray&& ) = delete;
            GuardArray& operator=(GuardArray const&) = delete;
            GuardArray& operator=(GuardArray&&) = delete;
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
                } while ( pRet != toGuard.load(atomics::memory_order_acquire));

                return pRet;
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
                Really, the result of <tt> f( toGuard.load() ) </tt> is assigned to the hazard pointer.
            */
            template <typename T, class Func>
            T protect( size_t nIndex, atomics::atomic<T> const& toGuard, Func f )
            {
                T pRet;
                do {
                    assign( nIndex, f( pRet = toGuard.load(atomics::memory_order_acquire) ));
                } while ( pRet != toGuard.load(atomics::memory_order_acquire));

                return pRet;
            }

            /// Store \p to the slot \p nIndex
            /**
                The function equals to a simple assignment, no loop is performed.
            */
            template <typename T>
            T * assign( size_t nIndex, T * p ); // inline in hp_impl.h

            /// Store marked pointer \p p to the guard
            /**
                The function equals to a simple assignment of <tt>p.ptr()</tt>, no loop is performed.
                Can be used for a marked pointer that cannot be changed concurrently.
            */
            template <typename T, int BITMASK>
            T * assign( size_t nIndex, cds::details::marked_ptr<T, BITMASK> p )
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

            /// Get native hazard pointer stored
            guarded_pointer get_native( size_t nIndex ) const
            {
                return base_class::operator[](nIndex).get();
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
            //@endcond

        public:
            typedef GuardedType guarded_type; ///< Guarded type
            typedef ValueType   value_type;   ///< Value type

            /// Functor for casting \p guarded_type to \p value_type
            typedef typename std::conditional< std::is_same<Cast, void>::value, trivial_cast, Cast >::type value_cast;

            //@cond
            typedef cds::gc::hp::details::hp_guard native_guard;
            //@endcond

        private:
            //@cond
            native_guard *  m_pGuard;
            //@endcond

        public:
            /// Creates empty guarded pointer
            guarded_ptr() CDS_NOEXCEPT
                : m_pGuard(nullptr)
            {
                alloc_guard();
            }

            //@cond
            /// Initializes guarded pointer with \p p
            explicit guarded_ptr( guarded_type * p ) CDS_NOEXCEPT
                : m_pGuard( nullptr )
            {
                reset(p);
            }
            explicit guarded_ptr( std::nullptr_t ) CDS_NOEXCEPT
                : m_pGuard( nullptr )
            {}
            //@endcond

            /// Move ctor
            guarded_ptr( guarded_ptr&& gp ) CDS_NOEXCEPT
                : m_pGuard( gp.m_pGuard )
            {
                gp.m_pGuard = nullptr;
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
                // Hazard Pointer array is organized as a stack
                if ( m_pGuard && m_pGuard > gp.m_pGuard ) {
                    m_pGuard->set( gp.m_pGuard->get(atomics::memory_order_relaxed) );
                    gp.free_guard();
                }
                else {
                    free_guard();
                    m_pGuard = gp.m_pGuard;
                    gp.m_pGuard = nullptr;
                }
                return *this;
            }

            /// The guarded pointer is not copy-assignable
            guarded_ptr& operator=(guarded_ptr const& gp) = delete;

            /// Returns a pointer to guarded value
            value_type * operator ->() const CDS_NOEXCEPT
            {
                assert( !empty() );
                return value_cast()( reinterpret_cast<guarded_type *>(m_pGuard->get()));
            }

            /// Returns a reference to guarded value
            value_type& operator *() CDS_NOEXCEPT
            {
                assert( !empty());
                return *value_cast()(reinterpret_cast<guarded_type *>(m_pGuard->get()));
            }

            /// Returns const reference to guarded value
            value_type const& operator *() const CDS_NOEXCEPT
            {
                assert( !empty() );
                return *value_cast()(reinterpret_cast<guarded_type *>(m_pGuard->get()));
            }

            /// Checks if the guarded pointer is \p nullptr
            bool empty() const CDS_NOEXCEPT
            {
                return !m_pGuard || m_pGuard->get( atomics::memory_order_relaxed ) == nullptr;
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
                assert( m_pGuard );
                return *m_pGuard;
            }

            void reset(guarded_type * p) CDS_NOEXCEPT
            {
                alloc_guard();
                assert( m_pGuard );
                m_pGuard->set(p);
            }
            //@endcond

        private:
            //@cond
            void alloc_guard()
            {
                if ( !m_pGuard )
                    m_pGuard = &thread_gc::alloc_guard();
            }

            void free_guard()
            {
                if ( m_pGuard ) {
                    thread_gc::free_guard( *m_pGuard );
                    m_pGuard = nullptr;
                }
            }
            //@endcond
        };

    public:
        /// \p scan() type
        enum class scan_type {
            classic = hp::classic,    ///< classic scan as described in Michael's papers
            inplace = hp::inplace     ///< inplace scan without allocation
        };
        /// Initializes %HP singleton
        /**
            The constructor initializes GC singleton with passed parameters.
            If GC instance is not exist then the function creates the instance.
            Otherwise it does nothing.

            The Michael's %HP reclamation schema depends of three parameters:
            - \p nHazardPtrCount - hazard pointer count per thread. Usually it is small number (up to 10) depending from
                the data structure algorithms. By default, if \p nHazardPtrCount = 0, the function
                uses maximum of the hazard pointer count for CDS library.
            - \p nMaxThreadCount - max count of thread with using Hazard Pointer GC in your application. Default is 100.
            - \p nMaxRetiredPtrCount - capacity of array of retired pointers for each thread. Must be greater than
                <tt> nHazardPtrCount * nMaxThreadCount </tt>. Default is <tt>2 * nHazardPtrCount * nMaxThreadCount </tt>.
        */
        HP(
            size_t nHazardPtrCount = 0,     ///< Hazard pointer count per thread
            size_t nMaxThreadCount = 0,     ///< Max count of simultaneous working thread in your application
            size_t nMaxRetiredPtrCount = 0, ///< Capacity of the array of retired objects for the thread
            scan_type nScanType = scan_type::inplace   ///< Scan type (see \p scan_type enum)
        )
        {
            hp::GarbageCollector::Construct(
                nHazardPtrCount,
                nMaxThreadCount,
                nMaxRetiredPtrCount,
                static_cast<hp::scan_type>(nScanType)
            );
        }

        /// Terminates GC singleton
        /**
            The destructor destroys %HP global object. After calling of this function you may \b NOT
            use CDS data structures based on \p %cds::gc::HP.
            Usually, %HP object is destroyed at the end of your \p main().
        */
        ~HP()
        {
            hp::GarbageCollector::Destruct( true );
        }

        /// Checks if count of hazard pointer is no less than \p nCountNeeded
        /**
            If \p bRaiseException is \p true (that is the default), the function raises
            an \p std::overflow_error exception "Too few hazard pointers"
            if \p nCountNeeded is more than the count of hazard pointer per thread.
        */
        static bool check_available_guards( size_t nCountNeeded, bool bRaiseException = true )
        {
            if ( hp::GarbageCollector::instance().getHazardPointerCount() < nCountNeeded ) {
                if ( bRaiseException )
                    throw std::overflow_error( "Too few hazard pointers" );
                return false;
            }
            return true;
        }

        /// Returns max Hazard Pointer count
        static size_t max_hazard_count()
        {
            return hp::GarbageCollector::instance().getHazardPointerCount();
        }

        /// Returns max count of thread
        static size_t max_thread_count()
        {
            return hp::GarbageCollector::instance().getMaxThreadCount();
        }

        /// Returns capacity of retired pointer array
        static size_t retired_array_capacity()
        {
            return hp::GarbageCollector::instance().getMaxRetiredPtrCount();
        }

        /// Retire pointer \p p with function \p pFunc
        /**
            The function places pointer \p p to array of pointers ready for removing.
            (so called retired pointer array). The pointer can be safely removed when no hazard pointer points to it.
            Deleting the pointer is the function \p pFunc call.
        */
        template <typename T>
        static void retire( T * p, void (* pFunc)(T *) );   // inline in hp_impl.h

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
            Since the functor call can happen at any time after \p retire call, additional restrictions are imposed to \p Disposer type:
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

            // How to call GC::retire method
            int * p = new int;

            // ... use p in lock-free manner

            cds::gc::HP::retire<disposer>( p ) ;   // place p to retired pointer array of HP GC
            \endcode

            Functor based on \p std::allocator :
            \code
            template <typename ALLOC = std::allocator<int> >
            struct disposer {
                template <typename T>
                void operator()( T * p ) {
                    typedef typename ALLOC::templare rebind<T>::other   alloc_t;
                    alloc_t a;
                    a.destroy( p );
                    a.deallocate( p, 1 );
                }
            };
            \endcode
        */
        template <class Disposer, typename T>
        static void retire( T * p );   // inline in hp_impl.h

        /// Get current scan strategy
        static scan_type getScanType()
        {
            return static_cast<scan_type>( hp::GarbageCollector::instance().getScanType());
        }

        /// Set current scan strategy
        static void setScanType(
            scan_type nScanType     ///< new scan strategy
        )
        {
            hp::GarbageCollector::instance().setScanType( static_cast<hp::scan_type>(nScanType) );
        }

        /// Checks if Hazard Pointer GC is constructed and may be used
        static bool isUsed()
        {
            return hp::GarbageCollector::isUsed();
        }

        /// Forced GC cycle call for current thread
        /**
            Usually, this function should not be called directly.
        */
        static void scan()  ;   // inline in hp_impl.h

        /// Synonym for \ref scan()
        static void force_dispose()
        {
            scan();
        }
    };
}}  // namespace cds::gc

#endif  // #ifndef CDSLIB_GC_IMPL_HP_DECL_H

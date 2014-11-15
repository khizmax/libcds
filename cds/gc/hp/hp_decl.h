//$$CDS-header$$

#ifndef __CDS_GC_HP_HP_DECL_H
#define __CDS_GC_HP_HP_DECL_H

#include <stdexcept>    // overflow_error
#include <cds/gc/details/hp.h>
#include <cds/details/marked_ptr.h>

namespace cds { namespace gc {
    /// @defgroup cds_garbage_collector Garbage collectors

    /// Hazard Pointer garbage collector
    /**  @ingroup cds_garbage_collector
        @headerfile cds/gc/hp.h

        This class realizes a wrapper for Hazard Pointer garbage collector internal implementation.

        Sources:
            - [2002] Maged M.Michael "Safe memory reclamation for dynamic lock-freeobjects using atomic reads and writes"
            - [2003] Maged M.Michael "Hazard Pointers: Safe memory reclamation for lock-free objects"
            - [2004] Andrei Alexandrescy, Maged Michael "Lock-free Data Structures with Hazard Pointers"

        See \ref cds_how_to_use "How to use" section for details of garbage collector applying.
    */
    class HP
    {
    public:
        /// Native guarded pointer type
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
        };

        /// Hazard Pointer guard
        /**
            @headerfile cds/gc/hp.h
            This class is a wrapper for \p hp::guard.
        */
        class Guard : public hp::guard
        {
            //@cond
            typedef hp::guard base_class;
            //@endcond

        public:
            //@cond
            Guard() ;   // inline in hp_impl.h
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
                T pCur = toGuard.load(atomics::memory_order_relaxed);
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
                that should be converted to a pointer to the value type before protecting.
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
                T pCur = toGuard.load(atomics::memory_order_relaxed);
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
            This class is a wrapper for \p hp::array template.
            Template parameter \p Count defines the size of HP array.
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
            //@cond
            GuardArray()    ;   // inline in hp_impl.h
            //@endcond
            /// Protects a pointer of type \p atomic<T*>
            /**
                Return the value of \p toGuard

                The function tries to load \p toGuard and to store it
                to the slot \p nIndex repeatedly until the guard's value equals \p toGuard
            */
            template <typename T>
            T protect(size_t nIndex, atomics::atomic<T> const& toGuard )
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
                The parameter \p f of type Func is a functor that makes this conversion:
                \code
                    struct functor {
                        value_type * operator()( T * p );
                    };
                \endcode
                Really, the result of <tt> f( toGuard.load() ) </tt> is assigned to the hazard pointer.
            */
            template <typename T, class Func>
            T protect(size_t nIndex, atomics::atomic<T> const& toGuard, Func f )
            {
                T pRet;
                do {
                    assign( nIndex, f( pRet = toGuard.load(atomics::memory_order_acquire) ));
                } while ( pRet != toGuard.load(atomics::memory_order_relaxed));

                return pRet;
            }

            /// Store \p to the slot \p nIndex
            /**
                The function equals to a simple assignment, no loop is performed.
            */
            template <typename T>
            T * assign( size_t nIndex, T * p )
            {
                base_class::set(nIndex, p);
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
            void clear( size_t nIndex)
            {
                base_class::clear( nIndex );
            }

            /// Get current value of slot \p nIndex
            template <typename T>
            T * get( size_t nIndex) const
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

    public:
        /// Initializes hp::GarbageCollector singleton
        /**
            The constructor initializes GC singleton with passed parameters.
            If GC instance is not exist then the function creates the instance.
            Otherwise it does nothing.

            The Michael's HP reclamation schema depends of three parameters:
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
            hp::scan_type nScanType = hp::inplace   ///< Scan type (see \ref hp::scan_type enum)
        )
        {
            hp::GarbageCollector::Construct(
                nHazardPtrCount,
                nMaxThreadCount,
                nMaxRetiredPtrCount,
                nScanType
            );
        }

        /// Terminates GC singleton
        /**
            The destructor calls \code hp::GarbageCollector::Destruct( true ) \endcode
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
        size_t max_hazard_count() const
        {
            return hp::GarbageCollector::instance().getHazardPointerCount();
        }

        /// Returns max count of thread
        size_t max_thread_count() const
        {
            return hp::GarbageCollector::instance().getMaxThreadCount();
        }

        /// Returns capacity of retired pointer array
        size_t retired_array_capacity() const
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
        static void retire( T * p, void (* pFunc)(T *) )    ;   // inline in hp_impl.h

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
        static void retire( T * p ) ;   // inline in hp_impl.h

        /// Get current scan strategy
        hp::scan_type getScanType() const
        {
            return hp::GarbageCollector::instance().getScanType();
        }

        /// Set current scan strategy
        void setScanType(
            hp::scan_type nScanType     ///< new scan strategy
        )
        {
            hp::GarbageCollector::instance().setScanType( nScanType );
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

#endif  // #ifndef __CDS_GC_HP_HP_DECL_H

//$$CDS-header$$

#ifndef __CDS_GC_DHP_DHP_DECL_H
#define __CDS_GC_DHP_DHP_DECL_H

#include <cds/gc/dhp/dhp.h>
#include <cds/details/marked_ptr.h>
#include <cds/details/static_functor.h>

namespace cds { namespace gc {

    /// Pass-the-Buck garbage collector
    /**  @ingroup cds_garbage_collector
        @headerfile cds/gc/dhp.h
        This class is a wrapper for Pass-the-Buck garbage collector internal implementation.

        Sources:
        - [2002] M. Herlihy, V. Luchangco, and M. Moir. The repeat offender problem: A mechanism for supporting
            dynamic-sized lockfree data structures. Technical Report TR-2002-112, Sun Microsystems Laboratories, 2002
        - [2002] M. Herlihy, V. Luchangco, P. Martin, and M. Moir. Dynamic-sized Lockfree Data Structures.
            Technical Report TR-2002-110, Sun Microsystems Laboratories, 2002
        - [2005] M. Herlihy, V. Luchangco, P. Martin, and M. Moir. Nonblocking Memory Management Support
            for Dynamic_Sized Data Structures. ACM Transactions on Computer Systems, Vol.23, No.2, May 2005

        See \ref cds_how_to_use "How to use" section for details of garbage collector applying.
    */
    class PTB
    {
    public:
        /// Native guarded pointer type
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
        typedef ptb::ThreadGC   thread_gc_impl;

        /// Wrapper for ptb::ThreadGC class
        /**
            @headerfile cds/gc/dhp.h
            This class performs automatically attaching/detaching Pass-the-Buck GC
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
                The constructor attaches the current thread to the Pass-the-Buck GC
                if it is not yet attached.
                The \p bPersistent parameter specifies attachment persistence:
                - \p true - the class destructor will not detach the thread from Pass-the-Buck GC.
                - \p false (default) - the class destructor will detach the thread from Pass-the-Buck GC.
            */
            thread_gc(
                bool    bPersistent = false
            )   ;   // inline in ptb_impl.h

            /// Destructor
            /**
                If the object has been created in persistent mode, the destructor does nothing.
                Otherwise it detaches the current thread from Pass-the-Buck GC.
            */
            ~thread_gc()    ;   // inline in ptb_impl.h
        };


        /// Pass-the-Buck guard
        /**
            @headerfile cds/gc/dhp.h
            This class is a wrapper for ptb::Guard.
        */
        class Guard: public ptb::Guard
        {
            //@cond
            typedef ptb::Guard base_class;
            //@endcond

        public:
            //@cond
            Guard() ;   // inline in ptb_impl.h
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
                T pCur = toGuard.load(atomics::memory_order_relaxed);
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
                The function equals to a simple assignment, no loop is performed.
                Can be used for a pointer that cannot be changed concurrently.
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
                The function equals to a simple assignment of <tt>p.ptr()</tt>, no loop is performed.
                Can be used for a marked pointer that cannot be changed concurrently.
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

            /// Clear value of the guard
            void clear()
            {
                base_class::clear();
            }

            /// Get the value currently protected (relaxed read)
            template <typename T>
            T * get() const
            {
                return reinterpret_cast<T *>( get_native() );
            }

            /// Get native guarded pointer stored
            guarded_pointer get_native() const
            {
                return base_class::get_guard()->pPost.load(atomics::memory_order_relaxed);
            }

        };

        /// Array of Pass-the-Buck guards
        /**
            @headerfile cds/gc/dhp.h
            This class is a wrapper for ptb::GuardArray template.
            Template parameter \p Count defines the size of PTB array.
        */
        template <size_t Count>
        class GuardArray: public ptb::GuardArray<Count>
        {
            //@cond
            typedef ptb::GuardArray<Count> base_class;
            //@endcond
        public:
            /// Rebind array for other size \p COUNT2
            template <size_t OtherCount>
            struct rebind {
                typedef GuardArray<OtherCount>  other   ;   ///< rebinding result
            };

        public:
            //@cond
            GuardArray()    ;   // inline in ptb_impl.h
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
                    pRet = assign( nIndex, toGuard.load(atomics::memory_order_relaxed) );
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
            T protect(size_t nIndex, atomics::atomic<T> const& toGuard, Func f )
            {
                T pRet;
                do {
                    assign( nIndex, f( pRet = toGuard.load(atomics::memory_order_relaxed) ));
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
                base_class::set(nIndex, p);
                return p;
            }

            /// Store marked pointer \p p to the guard
            /**
                The function equals to a simple assignment of <tt>p.ptr()</tt>, no loop is performed.
                Can be used for a marked pointer that cannot be changed concurrently.
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

    public:
        /// Initializes ptb::GarbageCollector singleton
        /**
            The constructor calls GarbageCollector::Construct with passed parameters.
            See ptb::GarbageCollector::Construct for explanation of parameters meaning.
        */
        PTB(
            size_t nLiberateThreshold = 1024
            , size_t nInitialThreadGuardCount = 8
        )
        {
            ptb::GarbageCollector::Construct(
                nLiberateThreshold,
                nInitialThreadGuardCount
            );
        }

        /// Terminates ptb::GarbageCollector singleton
        /**
            The destructor calls \code ptb::GarbageCollector::Destruct() \endcode
        */
        ~PTB()
        {
            ptb::GarbageCollector::Destruct();
        }

        /// Checks if count of hazard pointer is no less than \p nCountNeeded
        /**
            The function always returns \p true since the guard count is unlimited for
            PTB garbage collector.
        */
        static bool check_available_guards( size_t nCountNeeded, bool /*bRaiseException*/ = true )
        {
            CDS_UNUSED( nCountNeeded );
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
            ptb::GarbageCollector::instance().retirePtr( p, pFunc );
        }

        /// Retire pointer \p p with functor of type \p Disposer
        /**
            The function places pointer \p p to array of pointers ready for removing.
            (so called retired pointer array). The pointer can be safely removed when no guarded pointer points to it.

            See gc::HP::retire for \p Disposer requirements.
        */
        template <class Disposer, typename T>
        static void retire( T * p )
        {
            retire( p, cds::details::static_functor<Disposer, T>::call );
        }

        /// Checks if Pass-the-Buck GC is constructed and may be used
        static bool isUsed()
        {
            return ptb::GarbageCollector::isUsed();
        }

        /// Forced GC cycle call for current thread
        /**
            Usually, this function should not be called directly.
        */
        static void scan()  ;   // inline in ptb_impl.h

        /// Synonym for \ref scan()
        static void force_dispose()
        {
            scan();
        }
    };

}} // namespace cds::gc

#endif // #ifndef __CDS_GC_DHP_DHP_DECL_H

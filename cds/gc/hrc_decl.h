//$$CDS-header$$

#ifndef __CDS_GC_HRC_DECL_H
#define __CDS_GC_HRC_DECL_H

#include <cds/gc/hrc/hrc.h>
#include <cds/details/marked_ptr.h>

namespace cds { namespace gc {

    /// Gidenstam's garbage collector
    /** @ingroup cds_garbage_collector
        @headerfile cds/gc/hrc.h

        This class is a wrapper for Gidenstam's memory reclamation schema (HRC - Hazard pointer + Reference Counting)
        internal implementation.

        Sources:
        - [2006] A.Gidenstam "Algorithms for synchronization and consistency
            in concurrent system services", Chapter 5 "Lock-Free Memory Reclamation"
            Thesis for the degree of Doctor    of Philosophy
        - [2005] Anders Gidenstam, Marina Papatriantafilou and Philippas Tsigas "Allocating
            memory in a lock-free manner", Proceedings of the 13th Annual European
            Symposium on Algorithms (ESA 2005), Lecture Notes in Computer
            Science Vol. 3669, pages 229 – 242, Springer-Verlag, 2005

        Note that HRC schema does not support cyclic references that significantly limits the applicability of this GC.

        <h1>Usage</h1>
        In your \p main function you declare a object of class cds::gc::HRC. This declaration
        initializes internal hrc::GarbageCollector singleton.
        \code
        #include <cds/init.h>       // for cds::Initialize and cds::Terminate
        #include <cds/gc/hrc.h>

        int main(int argc, char** argv)
        {
            // Initialize libcds
            cds::Initialize();

            {
                // Initialize HRC singleton
                cds::gc::HRC hpGC();

                // Some useful work
                ...
            }

            // Terminate libcds
            cds::Terminate();
        }
        \endcode

        Each thread that uses cds::gc::HRC -based containers must be attached to HRC
        singleton. To make attachment you should declare a object of class HRC::thread_gc:
        \code
        #include <cds/gc/hrc.h>

        int myThreadEntryPoint()
        {
            // Attach the thread to HRC singleton
            cds::gc::HRC::thread_gc myThreadGC();

            // Do some work
            ...

            // The destructor of myThreadGC object detaches the thread from HRC singleton
        }
        \endcode

        In some cases, you should work in a external thread. For example, your application
        is a plug-in for a server that calls your code in the threads that has been created by server.
        In this case, you may use persistent mode of HRC::thread_gc. In this mode, the thread attaches
        to the HRC singleton only if it is not yet attached and never call detaching:
        \code
        #include <cds/gc/hrc.h>

        int myThreadEntryPoint()
        {
        // Attach the thread in persistent mode
        cds::gc::HRC::thread_gc myThreadGC( true );

        // Do some work
        ...

        // The destructor of myThreadGC object does NOT detach the thread from HRC singleton
        }
        \endcode

    */
    class HRC
    {
    public:

        /// Thread GC implementation for internal usage
        typedef hrc::ThreadGC   thread_gc_impl;

        /// Wrapper for hrc::ThreadGC class
        /**
            @headerfile cds/gc/hrc.h
            This class performs automatically attaching/detaching Gidenstam's GC
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
                The constructor attaches the current thread to the Gidenstam's GC
                if it is not yet attached.
                The \p bPersistent parameter specifies attachment persistence:
                - \p true - the class destructor will not detach the thread from Gidenstam's GC.
                - \p false (default) - the class destructor will detach the thread from Gidenstam's GC.
            */
            thread_gc(
                bool    bPersistent = false
            ) ; // inline in hrc_impl.h

            /// Destructor
            /**
                If the object has been created in persistent mode, the destructor does nothing.
                Otherwise it detaches the current thread from HRC GC.
            */
            ~thread_gc()    ;   // inline in hrc_impl.h
        };

        ///@anchor hrc_gc_HRC_container_node Base for container node
        typedef hrc::ContainerNode container_node;

        /// Native hazard pointer type
        typedef container_node * guarded_pointer;

        /// Atomic reference
        /**
            @headerfile cds/gc/hrc.h
        */
        template <typename T>
        class atomic_ref: protected atomics::atomic<T *>
        {
            //@cond
            typedef atomics::atomic<T *> base_class;
            //@endcond
        public:
            //@cond
            atomic_ref() = default;
            explicit CDS_CONSTEXPR atomic_ref(T * p) CDS_NOEXCEPT
                : base_class( p )
            {}
            //@endcond

            /// Read reference value
            T * load( atomics::memory_order order ) const CDS_NOEXCEPT
            {
                return base_class::load( order );
            }
            //@cond
            T * load( atomics::memory_order order ) const volatile CDS_NOEXCEPT
            {
                return base_class::load( order );
            }
            //@endcond

            /// Store new value to reference
            void store( T * pNew, atomics::memory_order order ) CDS_NOEXCEPT
            {
                before_store( pNew );
                T * pOld = base_class::exchange( pNew, order );
                after_store( pOld, pNew );
            }
            //@cond
            void store( T * pNew, atomics::memory_order order ) volatile CDS_NOEXCEPT
            {
                before_store( pNew );
                T * pOld = base_class::exchange( pNew, order );
                after_store( pOld, pNew );
            }
            //@endcond

            /// Updates atomic reference from current value \p pOld to new value \p pNew (strong CAS)
            /**
                May be used when concurrent updates are possible

                \p T - class derived from \ref hrc_gc_HRC_container_node "container_node" type
            */
            bool compare_exchange_strong( T *& pOld, T * pNew, atomics::memory_order mo_success, atomics::memory_order mo_fail ) CDS_NOEXCEPT
            {
                before_cas( pNew );
                bool bSuccess = base_class::compare_exchange_strong( pOld, pNew, mo_success, mo_fail );
                after_cas( bSuccess, pOld, pNew );
                return bSuccess;
            }
            //@cond
            bool compare_exchange_strong( T *& pOld, T * pNew, atomics::memory_order mo_success, atomics::memory_order mo_fail ) volatile CDS_NOEXCEPT
            {
                before_cas( pNew );
                bool bSuccess = base_class::compare_exchange_strong( pOld, pNew, mo_success, mo_fail );
                after_cas( bSuccess, pOld, pNew );
                return bSuccess;
            }
            bool compare_exchange_strong( T *& pOld, T * pNew, atomics::memory_order mo_success ) CDS_NOEXCEPT
            {
                return compare_exchange_strong( pOld, pNew, mo_success, atomics::memory_order_relaxed );
            }
            bool compare_exchange_strong( T *& pOld, T * pNew, atomics::memory_order mo_success ) volatile CDS_NOEXCEPT
            {
                return compare_exchange_strong( pOld, pNew, mo_success, atomics::memory_order_relaxed );
            }
            //@endcond

            /// Updates atomic reference from current value \p pOld to new value \p pNew (weak CAS)
            /**
                May be used when concurrent updates are possible

                \p T - class derived from \ref hrc_gc_HRC_container_node "container_node" type
            */
            bool compare_exchange_weak( T *& pOld, T * pNew, atomics::memory_order mo_success, atomics::memory_order mo_fail ) CDS_NOEXCEPT
            {
                before_cas( pNew );
                bool bSuccess = base_class::compare_exchange_weak( pOld, pNew, mo_success, mo_fail );
                after_cas( bSuccess, pOld, pNew );
                return bSuccess;
            }
            //@cond
            bool compare_exchange_weak( T *& pOld, T * pNew, atomics::memory_order mo_success, atomics::memory_order mo_fail ) volatile CDS_NOEXCEPT
            {
                before_cas( pNew );
                bool bSuccess = base_class::compare_exchange_weak( pOld, pNew, mo_success, mo_fail );
                after_cas( bSuccess, pOld, pNew );
                return bSuccess;
            }
            bool compare_exchange_weak( T *& pOld, T * pNew, atomics::memory_order mo_success ) CDS_NOEXCEPT
            {
                return compare_exchange_weak( pOld, pNew, mo_success, atomics::memory_order_relaxed );
            }
            bool compare_exchange_weak( T *& pOld, T * pNew, atomics::memory_order mo_success ) volatile CDS_NOEXCEPT
            {
                return compare_exchange_weak( pOld, pNew, mo_success, atomics::memory_order_relaxed );
            }
            //@endcond

        private:
            //@cond
            static void before_store( T * pNew ) CDS_NOEXCEPT
            {
                if ( pNew )
                    ++pNew->m_RC;
            }
            static void after_store( T * pOld, T * pNew ) CDS_NOEXCEPT
            {
                if ( pNew )
                    pNew->m_bTrace.store( false, atomics::memory_order_release );
                if ( pOld )
                    --pOld->m_RC;
            }
            static void before_cas( T * p ) CDS_NOEXCEPT
            {
                if ( p ) {
                    ++p->m_RC;
                    p->m_bTrace.store( false, atomics::memory_order_release );
                }
            }
            static void after_cas( bool bSuccess, T * pOld, T * pNew ) CDS_NOEXCEPT
            {
                if ( bSuccess ) {
                    if ( pOld )
                        --pOld->m_RC;
                }
                else {
                    if ( pNew )
                        --pNew->m_RC;
                }
            }
            //@endcond
        };

        /// Atomic marked pointer
        /**
            @headerfile cds/gc/hrc.h
        */
        template <typename MarkedPtr>
        class atomic_marked_ptr
        {
            //@cond
            atomics::atomic< MarkedPtr >     m_a;
            //@endcond
        public:
            /// Marked pointer type
            typedef MarkedPtr    marked_ptr;

            //@cond
            atomic_marked_ptr() CDS_NOEXCEPT
                : m_a( marked_ptr() )
            {}

            explicit CDS_CONSTEXPR atomic_marked_ptr( typename marked_ptr::value_type * p ) CDS_NOEXCEPT
                : m_a( marked_ptr(p) )
            {}

            atomic_marked_ptr( typename marked_ptr::value_type * ptr, int nMask ) CDS_NOEXCEPT
                : m_a( marked_ptr(ptr, nMask) )
            {}

            explicit atomic_marked_ptr( marked_ptr const&  ptr ) CDS_NOEXCEPT
                : m_a( ptr )
            {}
            //@endcond


            /// Read reference value
            marked_ptr load(atomics::memory_order order) const CDS_NOEXCEPT
            {
                return m_a.load(order);
            }

            /// Store new value to reference
            void store( marked_ptr pNew, atomics::memory_order order ) CDS_NOEXCEPT
            {
                before_store( pNew.ptr() );
                marked_ptr pOld = m_a.exchange( pNew, order );
                after_store( pOld.ptr(), pNew.ptr() );
            }

            /// Store new value to reference
            void store( typename marked_ptr::pointer_type pNew, atomics::memory_order order ) CDS_NOEXCEPT
            {
                before_store( pNew );
                marked_ptr pOld = m_a.exchange( marked_ptr(pNew), order );
                after_store( pOld.ptr(), pNew );
            }

            /// Updates atomic reference from current value \p pOld to new value \p pNew (weak CAS)
            /**
                May be used when concurrent updates are possible

                \p T - class derived from \ref hrc_gc_HRC_container_node "container_node" type
            */
            bool compare_exchange_weak( marked_ptr& pOld, marked_ptr pNew, atomics::memory_order mo_success, atomics::memory_order mo_fail ) CDS_NOEXCEPT
            {
                before_cas( pNew.ptr() );
                bool bSuccess = m_a.compare_exchange_weak( pOld, pNew, mo_success, mo_fail );
                after_cas( bSuccess, pOld.ptr(), pNew.ptr() );
                return bSuccess;
            }
            //@cond
            bool compare_exchange_weak( marked_ptr& pOld, marked_ptr pNew, atomics::memory_order mo_success ) CDS_NOEXCEPT
            {
                before_cas( pNew.ptr() );
                bool bSuccess = m_a.compare_exchange_weak( pOld, pNew, mo_success );
                after_cas( bSuccess, pOld.ptr(), pNew.ptr() );
                return bSuccess;
            }
            //@endcond

            /// Updates atomic reference from current value \p pOld to new value \p pNew (strong CAS)
            /**
                May be used when concurrent updates are possible

                \p T - class derived from \ref hrc_gc_HRC_container_node "container_node" type
            */
            bool compare_exchange_strong( marked_ptr& pOld, marked_ptr pNew, atomics::memory_order mo_success, atomics::memory_order mo_fail ) CDS_NOEXCEPT
            {
                // protect pNew
                before_cas( pNew.ptr() );
                bool bSuccess = m_a.compare_exchange_strong( pOld, pNew, mo_success, mo_fail );
                after_cas( bSuccess, pOld.ptr(), pNew.ptr() );
                return bSuccess;
            }
            //@cond
            bool compare_exchange_strong( marked_ptr& pOld, marked_ptr pNew, atomics::memory_order mo_success ) CDS_NOEXCEPT
            {
                before_cas( pNew.ptr() );
                bool bSuccess = m_a.compare_exchange_strong( pOld, pNew, mo_success );
                after_cas( bSuccess, pOld.ptr(), pNew.ptr() );
                return bSuccess;
            }
            //@endcond

        private:
            //@cond
            static void before_store( typename marked_ptr::pointer_type p ) CDS_NOEXCEPT
            {
                if ( p )
                    ++p->m_RC;
            }
            static void after_store( typename marked_ptr::pointer_type pOld, typename marked_ptr::pointer_type pNew ) CDS_NOEXCEPT
            {
                if ( pNew )
                    pNew->m_bTrace.store( false, atomics::memory_order_release );
                if ( pOld )
                    --pOld->m_RC;
            }
            static void before_cas( typename marked_ptr::pointer_type p ) CDS_NOEXCEPT
            {
                if ( p ) {
                    ++p->m_RC;
                    p->m_bTrace.store( false, atomics::memory_order_release );
                }
            }
            static void after_cas( bool bSuccess, typename marked_ptr::pointer_type pOld, typename marked_ptr::pointer_type pNew ) CDS_NOEXCEPT
            {
                if ( bSuccess ) {
                    if ( pOld )
                        --pOld->m_RC;
                }
                else {
                    if ( pNew )
                        --pNew->m_RC;
                }
            }
            //@endcond
        };

        /// HRC guard
        /**
            @headerfile cds/gc/hrc.h
            This class is a wrapper for hrc::AutoHPGuard.
        */
        class Guard: public hrc::AutoHPGuard
        {
            //@cond
            typedef hrc::AutoHPGuard base_class;
            //@endcond

        public:
            /// Default constructor
            Guard() ;   // inline in hrc_impl.h

            /// Protects atomic pointer
            /**
                Return the value of \p toGuard

                The function tries to load \p toGuard and to store it
                to the HP slot repeatedly until the guard's value equals \p toGuard
            */
            template <typename T>
            T * protect( atomic_ref<T> const& toGuard )
            {
                T * pCur = toGuard.load(atomics::memory_order_relaxed);
                T * pRet;
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
            T * protect( atomic_ref<T> const& toGuard, Func f )
            {
                T * pCur = toGuard.load(atomics::memory_order_relaxed);
                T * pRet;
                do {
                    pRet = pCur;
                    assign( f( pCur ) );
                    pCur = toGuard.load(atomics::memory_order_acquire);
                } while ( pRet != pCur );
                return pCur;
            }

            /// Protects a atomic marked reference \p link
            /**
                Returns current value of \p link.

                The function tries to load \p link and to store it
                to the guard repeatedly until the guard's value equals \p link
            */
            template <typename T>
            typename atomic_marked_ptr<T>::marked_ptr protect( atomic_marked_ptr<T> const& link )
            {
                typename atomic_marked_ptr<T>::marked_ptr p;
                do {
                    assign( ( p = link.load(atomics::memory_order_relaxed)).ptr() );
                } while ( p != link.load(atomics::memory_order_acquire) );
                return p;
            }

            /// Protects a atomic marked reference \p link
            /**
                Returns current value of \p link.

                The function tries to load \p link and to store it
                to the guard repeatedly until the guard's value equals \p link

                The function is useful for intrusive containers when \p link is a node pointer
                that should be converted to a pointer to the value type before guarding.
                The parameter \p f of type Func is a functor that makes this conversion:
                \code
                    struct functor {
                        value_type * operator()( T p );
                    };
                \endcode
                Really, the result of <tt> f( link.load() ) </tt> is assigned to the hazard pointer.
            */
            template <typename T, typename Func>
            typename atomic_marked_ptr<T>::marked_ptr protect( atomic_marked_ptr<T> const& link, Func f )
            {
                typename atomic_marked_ptr<T>::marked_ptr pCur;
                do {
                    pCur = link.load(atomics::memory_order_relaxed);
                    assign( f( pCur ));
                } while ( pCur != link.load(atomics::memory_order_acquire) );
                return pCur;
            }

            /// Stores \p p to the guard
            /**
                The function equals to a simple assignment, no loop is performed.
                Can be used for a pointer that cannot be changed concurrently.
            */
            template <typename T>
            T * assign( T * p )
            {
                return base_class::operator =(p);
            }

            /// Stores marked pointer \p p to the guard
            /**
                The function equals to a simple assignment of <tt>p.ptr()</tt>, no loop is performed.
                Can be used for a marked pointer that cannot be changed concurrently.
            */
            template <typename T, int Bitmask>
            T * assign( cds::details::marked_ptr<T, Bitmask> p )
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

            /// Get the value currently protected
            template <typename T>
            T * get() const
            {
                return static_cast<T *>( get_native());
            }

            /// Get native hazard pointer stored
            guarded_pointer get_native() const
            {
                return base_class::get();
            }
        };

        /// Array of guards
        /**
            @headerfile cds/gc/hrc.h
            This class is a wrapper for AutoHPArray template.
            Template parameter \p Limit defines the size of HP array.
        */
        template <size_t Limit>
        class GuardArray: public hrc::AutoHPArray<Limit>
        {
            //@cond
            typedef hrc::AutoHPArray<Limit> base_class;
            //@endcond
        public:
            /// Rebind array for other size \p OtherLimit
            template <size_t OtherLimit>
            struct rebind {
                typedef GuardArray<OtherLimit>  other   ;   ///< rebinding result
            };

        public:
            //@cond
            GuardArray()    ;   // inline in hrc_impl.h
            GuardArray( thread_gc_impl& threadGC )
                : base_class( threadGC )
            {}
            //@endcond

            /// Protects an atomic reference \p link in slot \p nIndex
            /**
                Returns current value of \p link.

                The function tries to load \p pToGuard and to store it
                to the slot \p nIndex repeatedly until the guard's value equals \p pToGuard
            */
            template <typename T>
            T * protect( size_t nIndex, atomic_ref<T> const& link )
            {
                T * p;
                do {
                    p = assign( nIndex, link.load(atomics::memory_order_relaxed) );
                } while ( p != link.load(atomics::memory_order_acquire) );
                return p;
            }

            /// Protects a atomic marked reference \p link in slot \p nIndex
            /**
                Returns current value of \p link.

                The function tries to load \p link and to store it
                to the slot \p nIndex repeatedly until the guard's value equals \p link
            */
            template <typename T>
            typename atomic_marked_ptr<T>::marked_ptr protect( size_t nIndex, atomic_marked_ptr<T> const& link )
            {
                typename atomic_marked_ptr<T>::marked_ptr p;
                do {
                    assign( nIndex, ( p = link.load(atomics::memory_order_relaxed)).ptr() );
                } while ( p != link.load(atomics::memory_order_acquire) );
                return p;
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
            T * protect(size_t nIndex, atomic_ref<T> const& toGuard, Func f )
            {
                T * pRet;
                do {
                    assign( nIndex, f( pRet = toGuard.load(atomics::memory_order_relaxed) ));
                } while ( pRet != toGuard.load(atomics::memory_order_acquire));

                return pRet;
            }

            /// Protects a atomic marked reference \p link in slot \p nIndex
            /**
                Returns current value of \p link.

                The function tries to load \p link and to store it
                to the slot \p nIndex repeatedly until the guard's value equals \p link

                The function is useful for intrusive containers when \p link is a node pointer
                that should be converted to a pointer to the value type before guarding.
                The parameter \p f of type Func is a functor that makes this conversion:
                \code
                    struct functor {
                        value_type * operator()( T p );
                    };
                \endcode
                Really, the result of <tt> f( link.load() ) </tt> is assigned to the hazard pointer.
            */
            template <typename T, typename Func>
            typename atomic_marked_ptr<T>::marked_ptr protect( size_t nIndex, atomic_marked_ptr<T> const& link, Func f )
            {
                typename atomic_marked_ptr<T>::marked_ptr p;
                do {
                    p = link.load(atomics::memory_order_relaxed);
                    assign( nIndex, f( p ) );
                } while ( p != link.load(atomics::memory_order_acquire) );
                return p;
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
                return base_class::set( nIndex, p.ptr() );
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
                return static_cast<T *>( get_native( nIndex ) );
            }

            /// Get native hazard pointer stored
            guarded_pointer get_native( size_t nIndex ) const
            {
                return base_class::operator[](nIndex).get();
            }

            /// Capacity of the guard array
            static CDS_CONSTEXPR size_t capacity()
            {
                return Limit;
            }
        };

    public:
        /// Initializes hrc::GarbageCollector singleton
        /**
            The constructor calls hrc::GarbageCollector::Construct with passed parameters.
            See hrc::GarbageCollector::Construct for explanation of parameters meaning.
        */
        HRC(
            size_t nHazardPtrCount = 0,     ///< number of hazard pointers
            size_t nMaxThreadCount = 0,     ///< max threads count
            size_t nMaxNodeLinkCount = 0,   ///< max number of links a @ref hrc::ContainerNode can contain
            size_t nMaxTransientLinks = 0   ///< max number of links in live nodes that may transiently point to a deleted node
        )
        {
            hrc::GarbageCollector::Construct(
                nHazardPtrCount,
                nMaxThreadCount,
                nMaxNodeLinkCount,
                nMaxTransientLinks
            );
        }

        /// Terminates hrc::GarbageCollector singleton
        /**
            The destructor calls \code hrc::GarbageCollector::Destruct() \endcode
        */
        ~HRC()
        {
            hrc::GarbageCollector::Destruct();
        }

        /// Checks if count of hazard pointer is no less than \p nCountNeeded
        /**
            If \p bRaiseException is \p true (that is the default), the function raises an exception gc::too_few_hazard_pointers
            if \p nCountNeeded is more than the count of hazard pointer per thread.
        */
        static bool check_available_guards( size_t nCountNeeded, bool bRaiseException = true )
        {
            if ( hrc::GarbageCollector::instance().getHazardPointerCount() < nCountNeeded ) {
                if ( bRaiseException )
                    throw cds::gc::too_few_hazard_pointers();
                return false;
            }
            return true;
        }

        /// Retire pointer \p p with function \p pFunc
        /**
            The function places pointer \p p to array of pointers ready for removing.
            (so called retired pointer array). The pointer can be safely removed when no guarded pointer points to it.
            Deleting the pointer is the function \p pFunc call.
        */
        template <typename T>
        static void retire( T * p, void (* pFunc)(T *) )    ;   // inline in hrc_impl.h

        /// Retire pointer \p p with functor of type \p Disposer
        /**
            The function places pointer \p p to array of pointers ready for removing.
            (so called retired pointer array). The pointer can be safely removed when no guard points to it.

            See gc::HP::retire for \p Disposer requirements.
        */
        template <class Disposer, typename T>
        static void retire( T * p ) ;   // inline in hrc_impl.h

        /// Checks if HRC GC is constructed and may be used
        static bool isUsed()
        {
            return hrc::GarbageCollector::isUsed();
        }

        /// Forced GC cycle call for current thread
        /**
            Usually, this function should not be called directly.
        */
        static void scan()  ;   // inline in hrc_impl.h

        /// Synonym for \ref scan()
        static void force_dispose()
        {
            scan();
        }
    };
}} // namespace cds::gc

#endif // #ifndef __CDS_GC_HRC_DECL_H

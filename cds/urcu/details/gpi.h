// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_URCU_DETAILS_GPI_H
#define CDSLIB_URCU_DETAILS_GPI_H

#include <mutex>
#include <cds/urcu/details/gp.h>
#include <cds/algo/backoff_strategy.h>

namespace cds { namespace urcu {

    /// User-space general-purpose RCU with immediate reclamation
    /**
        @headerfile cds/urcu/general_instant.h

        This is simplest general-purpose RCU implementation. When a thread calls \ref retire_ptr function
        the RCU \p synchronize function is called that waits until all reader/updater threads end up
        their read-side critical sections, i.e. until the RCU quiescent state will come.
        After that the retired object is freed immediately.
        Thus, the implementation blocks for any retired object

        There is a wrapper \ref cds_urcu_general_instant_gc "gc<general_instant>" for \p %general_instant class
        that provides unified RCU interface. You should use this wrapper class instead \p %general_instant

        Template arguments:
        - \p Lock - mutex type, default is \p std::mutex
        - \p Backoff - back-off schema, default is cds::backoff::Default
    */
    template <
        class Lock = std::mutex
       ,class Backoff = cds::backoff::Default
    >
    class general_instant: public details::gp_singleton< general_instant_tag >
    {
        //@cond
        typedef details::gp_singleton< general_instant_tag > base_class;
        //@endcond

    public:
        typedef general_instant_tag rcu_tag ;   ///< RCU tag
        typedef Lock    lock_type   ;           ///< Lock type
        typedef Backoff back_off    ;           ///< Back-off schema type

        typedef typename base_class::thread_gc  thread_gc ;   ///< Thread-side RCU part
        typedef typename thread_gc::scoped_lock scoped_lock ; ///< Access lock class

        //@cond
        static bool const c_bBuffered = false ; ///< No buffers
        //@endcond

    protected:
        //@cond
        typedef details::gp_singleton_instance< rcu_tag >    singleton_ptr;
        //@endcond

    protected:
        //@cond
        lock_type   m_Lock;
        //@endcond

    public:
        /// Returns singleton instance
        static general_instant * instance()
        {
            return static_cast<general_instant *>( base_class::instance());
        }

        /// Checks if the singleton is created and ready to use
        static bool isUsed()
        {
            return singleton_ptr::s_pRCU != nullptr;
        }

    protected:
        //@cond
        general_instant()
        {}

        ~general_instant()
        {}

        void flip_and_wait()
        {
            back_off bkoff;
            base_class::flip_and_wait( bkoff );
        }
        //@endcond

    public:
        /// Creates singleton object
        static void Construct()
        {
            if ( !singleton_ptr::s_pRCU )
                singleton_ptr::s_pRCU = new general_instant();
        }

        /// Destroys singleton object
        static void Destruct( bool bDetachAll = false )
        {
            if ( isUsed()) {
                if ( bDetachAll )
                    instance()->m_ThreadList.detach_all();
                delete instance();
                singleton_ptr::s_pRCU = nullptr;
            }
        }

    public:
        /// Retires \p p pointer
        /**
            The method calls \p synchronize() to wait for the end of grace period
            and calls \p p disposer.
        */
        virtual void retire_ptr( retired_ptr& p ) override
        {
            synchronize();
            if ( p.m_p )
                p.free();
        }

        /// Retires the pointer chain [\p itFirst, \p itLast)
        template <typename ForwardIterator>
        void batch_retire( ForwardIterator itFirst, ForwardIterator itLast )
        {
            if ( itFirst != itLast ) {
                synchronize();
                while ( itFirst != itLast ) {
                    retired_ptr p( *itFirst );
                    ++itFirst;
                    if ( p.m_p )
                        p.free();
                }
            }
        }

        /// Retires the pointer chain until \p Func returns \p nullptr retired pointer
        template <typename Func>
        void batch_retire( Func e )
        {
            retired_ptr p{ e() };
            if ( p.m_p ) {
                synchronize();
                while ( p.m_p ) {
                    retired_ptr pr( p );
                    p = e();
                    pr.free();
                }
            }
        }

        /// Waits to finish a grace period
        void synchronize()
        {
            assert( !thread_gc::is_locked());
            std::unique_lock<lock_type> sl( m_Lock );
            flip_and_wait();
            flip_and_wait();
        }

        //@cond
        // Added for uniformity
        size_t constexpr capacity() const
        {
            return 1;
        }
        //@endcond
    };

    /// User-space general-purpose RCU with immediate reclamation (stripped version)
    /**
        @headerfile cds/urcu/general_instant.h

        This short version of \p general_instant is intended for stripping debug info.
        If you use \p %general_instant with default template arguments you may use
        this stripped version. All functionality of both classes are identical.
    */
    class general_instant_stripped: public general_instant<>
    {};

}} // namespace cds::urcu

#endif // #ifndef CDSLIB_URCU_DETAILS_GPI_H

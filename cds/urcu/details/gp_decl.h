// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_URCU_DETAILS_GP_DECL_H
#define CDSLIB_URCU_DETAILS_GP_DECL_H

#include <cds/urcu/details/base.h>
#include <cds/user_setup/cache_line.h>

//@cond
namespace cds { namespace urcu { namespace details {

    // We could derive thread_data from thread_list_record
    // but in this case m_nAccessControl would have offset != 0
    // that is not so efficiently
#   define CDS_GPURCU_DECLARE_THREAD_DATA(tag_) \
    template <> struct thread_data<tag_> { \
        atomics::atomic<uint32_t>        m_nAccessControl ; \
        thread_list_record< thread_data >   m_list ; \
        char pad_[cds::c_nCacheLineSize]; \
        thread_data(): m_nAccessControl(0) {} \
        explicit thread_data( OS::ThreadId owner ): m_nAccessControl(0), m_list(owner) {} \
        ~thread_data() {} \
    }

    CDS_GPURCU_DECLARE_THREAD_DATA( general_instant_tag );
    CDS_GPURCU_DECLARE_THREAD_DATA( general_buffered_tag );
    CDS_GPURCU_DECLARE_THREAD_DATA( general_threaded_tag );

#   undef CDS_GPURCU_DECLARE_THREAD_DATA

    template <typename RCUtag>
    struct gp_singleton_instance
    {
        static CDS_EXPORT_API singleton_vtbl *     s_pRCU;
    };
#if !( CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS))
    template<> CDS_EXPORT_API singleton_vtbl * gp_singleton_instance< general_instant_tag >::s_pRCU;
    template<> CDS_EXPORT_API singleton_vtbl * gp_singleton_instance< general_buffered_tag >::s_pRCU;
    template<> CDS_EXPORT_API singleton_vtbl * gp_singleton_instance< general_threaded_tag >::s_pRCU;
#endif

    template <typename GPRCUtag>
    class gp_thread_gc
    {
    public:
        typedef GPRCUtag                    rcu_tag;
        typedef typename rcu_tag::rcu_class rcu_class;
        typedef thread_data< rcu_tag >      thread_record;
        typedef cds::urcu::details::scoped_lock< gp_thread_gc > scoped_lock;

    protected:
        static thread_record * get_thread_record();

    public:
        gp_thread_gc();
        ~gp_thread_gc();
    public:
        static void access_lock();
        static void access_unlock();
        static bool is_locked();

        /// Retire pointer \p by the disposer \p Disposer
        template <typename Disposer, typename T>
        static void retire( T * p )
        {
            retire( p, +[]( void* p ) { Disposer()( static_cast<T*>( p )); });
        }

        /// Retire pointer \p by the disposer \p pFunc
        template <typename T>
        static void retire( T * p, void (* pFunc)(T *))
        {
            retired_ptr rp( reinterpret_cast<void *>( p ), reinterpret_cast<free_retired_ptr_func>( pFunc ));
            retire( rp );
        }

        /// Retire pointer \p
        static void retire( retired_ptr& p )
        {
            assert( gp_singleton_instance< rcu_tag >::s_pRCU );
            gp_singleton_instance< rcu_tag >::s_pRCU->retire_ptr( p );
        }
    };

#   define CDS_GP_RCU_DECLARE_THREAD_GC( tag_ ) template <> class thread_gc<tag_>: public gp_thread_gc<tag_> {}

    CDS_GP_RCU_DECLARE_THREAD_GC( general_instant_tag  );
    CDS_GP_RCU_DECLARE_THREAD_GC( general_buffered_tag );
    CDS_GP_RCU_DECLARE_THREAD_GC( general_threaded_tag );

#   undef CDS_GP_RCU_DECLARE_THREAD_GC

    template <class RCUtag>
    class gp_singleton: public singleton_vtbl
    {
    public:
        typedef RCUtag  rcu_tag;
        typedef cds::urcu::details::thread_gc< rcu_tag >   thread_gc;

    protected:
        typedef typename thread_gc::thread_record   thread_record;
        typedef gp_singleton_instance< rcu_tag >    rcu_instance;

    protected:
        atomics::atomic<uint32_t>    m_nGlobalControl;
        thread_list< rcu_tag >          m_ThreadList;

    protected:
        gp_singleton()
            : m_nGlobalControl(1)
        {}

        ~gp_singleton()
        {}

    public:
        static gp_singleton * instance()
        {
            return static_cast< gp_singleton *>( rcu_instance::s_pRCU );
        }

        static bool isUsed()
        {
            return rcu_instance::s_pRCU != nullptr;
        }

    public:
        virtual void retire_ptr( retired_ptr& p ) = 0;

    public: // thread_gc interface
        thread_record * attach_thread()
        {
            return m_ThreadList.alloc();
        }

        void detach_thread( thread_record * pRec )
        {
            m_ThreadList.retire( pRec );
        }

        uint32_t global_control_word( atomics::memory_order mo ) const
        {
            return m_nGlobalControl.load( mo );
        }

    protected:
        bool check_grace_period( thread_record * pRec ) const;

        template <class Backoff>
        void flip_and_wait( Backoff& bkoff );
    };

#   define CDS_GP_RCU_DECLARE_SINGLETON( tag_ ) \
    template <> class singleton< tag_ > { \
    public: \
        typedef tag_  rcu_tag ; \
        typedef cds::urcu::details::thread_gc< rcu_tag >   thread_gc ; \
    protected: \
        typedef thread_gc::thread_record            thread_record ; \
        typedef gp_singleton_instance< rcu_tag >    rcu_instance  ; \
        typedef gp_singleton< rcu_tag >             rcu_singleton ; \
    public: \
        static bool isUsed() { return rcu_singleton::isUsed() ; } \
        static rcu_singleton * instance() { assert( rcu_instance::s_pRCU ); return static_cast<rcu_singleton *>( rcu_instance::s_pRCU ); } \
        static thread_record * attach_thread() { return instance()->attach_thread() ; } \
        static void detach_thread( thread_record * pRec ) { return instance()->detach_thread( pRec ) ; } \
        static uint32_t global_control_word( atomics::memory_order mo ) { return instance()->global_control_word( mo ) ; } \
    }

    CDS_GP_RCU_DECLARE_SINGLETON( general_instant_tag  );
    CDS_GP_RCU_DECLARE_SINGLETON( general_buffered_tag );
    CDS_GP_RCU_DECLARE_SINGLETON( general_threaded_tag );

#   undef CDS_GP_RCU_DECLARE_SINGLETON

}}} // namespace cds::urcu::details
//@endcond

#endif // #ifndef CDSLIB_URCU_DETAILS_GP_DECL_H

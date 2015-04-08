//$$CDS-header$$

#ifndef CDSLIB_URCU_DETAILS_GP_H
#define CDSLIB_URCU_DETAILS_GP_H

#include <cds/urcu/details/gp_decl.h>
#include <cds/threading/model.h>

//@cond
namespace cds { namespace urcu { namespace details {

    // Inlines

    // gp_thread_gc
    template <typename RCUtag>
    inline gp_thread_gc<RCUtag>::gp_thread_gc()
    {
        if ( !threading::Manager::isThreadAttached() )
            cds::threading::Manager::attachThread();
    }

    template <typename RCUtag>
    inline gp_thread_gc<RCUtag>::~gp_thread_gc()
    {
        cds::threading::Manager::detachThread();
    }

    template <typename RCUtag>
    inline typename gp_thread_gc<RCUtag>::thread_record * gp_thread_gc<RCUtag>::get_thread_record()
    {
        return cds::threading::getRCU<RCUtag>();
    }

    template <typename RCUtag>
    inline void gp_thread_gc<RCUtag>::access_lock()
    {
        thread_record * pRec = get_thread_record();
        assert( pRec != nullptr );

        uint32_t tmp = pRec->m_nAccessControl.load( atomics::memory_order_relaxed );
        if ( (tmp & rcu_class::c_nNestMask) == 0 ) {
            pRec->m_nAccessControl.store( gp_singleton<RCUtag>::instance()->global_control_word(atomics::memory_order_relaxed),
                atomics::memory_order_relaxed );
            atomics::atomic_thread_fence( atomics::memory_order_acquire );
            //CDS_COMPILER_RW_BARRIER;
        }
        else {
            pRec->m_nAccessControl.fetch_add( 1, atomics::memory_order_relaxed );
        }
    }

    template <typename RCUtag>
    inline void gp_thread_gc<RCUtag>::access_unlock()
    {
        thread_record * pRec = get_thread_record();
        assert( pRec != nullptr );

        //CDS_COMPILER_RW_BARRIER;
        pRec->m_nAccessControl.fetch_sub( 1, atomics::memory_order_release );
    }

    template <typename RCUtag>
    inline bool gp_thread_gc<RCUtag>::is_locked()
    {
        thread_record * pRec = get_thread_record();
        assert( pRec != nullptr );

        return (pRec->m_nAccessControl.load( atomics::memory_order_relaxed ) & rcu_class::c_nNestMask) != 0;
    }


    // gp_singleton
    template <typename RCUtag>
    inline bool gp_singleton<RCUtag>::check_grace_period( typename gp_singleton<RCUtag>::thread_record * pRec ) const
    {
        uint32_t const v = pRec->m_nAccessControl.load( atomics::memory_order_relaxed );
        return (v & general_purpose_rcu::c_nNestMask)
            && ((( v ^ m_nGlobalControl.load( atomics::memory_order_relaxed )) & ~general_purpose_rcu::c_nNestMask ));
    }

    template <typename RCUtag>
    template <class Backoff>
    inline void gp_singleton<RCUtag>::flip_and_wait( Backoff& bkoff )
    {
        OS::ThreadId const nullThreadId = OS::c_NullThreadId;
        m_nGlobalControl.fetch_xor( general_purpose_rcu::c_nControlBit, atomics::memory_order_seq_cst );

        for ( thread_record * pRec = m_ThreadList.head( atomics::memory_order_acquire); pRec; pRec = pRec->m_list.m_pNext ) {
            while ( pRec->m_list.m_idOwner.load( atomics::memory_order_acquire) != nullThreadId && check_grace_period( pRec ) ) {
                bkoff();
                CDS_COMPILER_RW_BARRIER;
            }
            bkoff.reset();
        }
    }


}}} // namespace cds:urcu::details
//@endcond

#endif // #ifndef CDSLIB_URCU_DETAILS_GP_H

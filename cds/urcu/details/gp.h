/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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
        if ( !threading::Manager::isThreadAttached())
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

            atomics::atomic_thread_fence( atomics::memory_order_seq_cst );
        }
        else {
            // nested lock
            pRec->m_nAccessControl.store( tmp + 1, atomics::memory_order_relaxed );
        }
    }

    template <typename RCUtag>
    inline void gp_thread_gc<RCUtag>::access_unlock()
    {
        thread_record * pRec = get_thread_record();
        assert( pRec != nullptr );

        uint32_t tmp = pRec->m_nAccessControl.load( atomics::memory_order_relaxed );
        assert( (tmp & rcu_class::c_nNestMask) > 0 );

#if CDS_COMPILER == CDS_COMPILER_CLANG && CDS_COMPILER_VERSION < 30800
        // CLang 3.6-3.7: some tests of intrusive::FeldmanHashSet based on general-purpose RCU 
        // are failed even in single-threaded mode (unit tests) without magic compiler barrier below
        CDS_COMPILER_RW_BARRIER;
#endif
        pRec->m_nAccessControl.store( tmp - 1, atomics::memory_order_release );
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
        uint32_t const v = pRec->m_nAccessControl.load( atomics::memory_order_acquire );
        return (v & general_purpose_rcu::c_nNestMask)
            && (( v ^ m_nGlobalControl.load( atomics::memory_order_relaxed )) & ~general_purpose_rcu::c_nNestMask );
    }

    template <typename RCUtag>
    template <class Backoff>
    inline void gp_singleton<RCUtag>::flip_and_wait( Backoff& bkoff )
    {
        OS::ThreadId const nullThreadId = OS::c_NullThreadId;
        m_nGlobalControl.fetch_xor( general_purpose_rcu::c_nControlBit, atomics::memory_order_seq_cst );

        for ( thread_record * pRec = m_ThreadList.head( atomics::memory_order_acquire ); pRec; pRec = pRec->m_list.m_pNext ) {
            while ( pRec->m_list.m_idOwner.load( atomics::memory_order_acquire ) != nullThreadId && check_grace_period( pRec )) {
                bkoff();
                CDS_COMPILER_RW_BARRIER;
            }
            bkoff.reset();
        }
    }


}}} // namespace cds:urcu::details
//@endcond

#endif // #ifndef CDSLIB_URCU_DETAILS_GP_H

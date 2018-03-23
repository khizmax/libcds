// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/urcu/details/sh.h>

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
namespace cds { namespace urcu { namespace details {

    template<> CDS_EXPORT_API singleton_vtbl * sh_singleton_instance< signal_buffered_tag >::s_pRCU = nullptr;

    template <>
    void sh_singleton<signal_buffered_tag>::signal_handler( int /*signo*/, siginfo_t * /*sigInfo*/, void * /*context*/ )
    {
        thread_record * pRec = cds::threading::getRCU<signal_buffered_tag>();
        if ( pRec ) {
            atomics::atomic_signal_fence( atomics::memory_order_acquire );
            pRec->m_bNeedMemBar.store( false, atomics::memory_order_relaxed );
            atomics::atomic_signal_fence( atomics::memory_order_release );
        }
    }

}}} // namespace cds::urcu::details

#endif //#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED

// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_THREADING_MODEL_H
#define CDSLIB_THREADING_MODEL_H

#include <cds/threading/details/_common.h>
#include <cds/user_setup/threading.h>
#include <cds/threading/details/auto_detect.h>

namespace cds { namespace threading {

    /// Returns thread specific data of \p GC garbage collector
    template <class GC> typename GC::thread_gc_impl&  getGC();

    /// Returns RCU thread specific data (thread GC) for current thread
    /**
        Template argument \p RCUtag is one of \ref cds_urcu_tags "RCU tags"
    */
    template <typename RCUtag> cds::urcu::details::thread_data<RCUtag> * getRCU();

    //@cond
    template<>
    inline cds::urcu::details::thread_data<cds::urcu::general_instant_tag> * getRCU<cds::urcu::general_instant_tag>()
    {
        return Manager::thread_data()->m_pGPIRCU;
    }
    template<>
    inline cds::urcu::details::thread_data<cds::urcu::general_buffered_tag> * getRCU<cds::urcu::general_buffered_tag>()
    {
        return Manager::thread_data()->m_pGPBRCU;
    }
    template<>
    inline cds::urcu::details::thread_data<cds::urcu::general_threaded_tag> * getRCU<cds::urcu::general_threaded_tag>()
    {
        return Manager::thread_data()->m_pGPTRCU;
    }
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    template<>
    inline cds::urcu::details::thread_data<cds::urcu::signal_buffered_tag> * getRCU<cds::urcu::signal_buffered_tag>()
    {
        ThreadData * p = Manager::thread_data();
        return p ? p->m_pSHBRCU : nullptr;
    }
#endif

    static inline cds::algo::elimination::record& elimination_record()
    {
        return Manager::thread_data()->m_EliminationRec;
    }
    //@endcond

}} // namespace cds::threading

#endif // #ifndef CDSLIB_THREADING_MODEL_H

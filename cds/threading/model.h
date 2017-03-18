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

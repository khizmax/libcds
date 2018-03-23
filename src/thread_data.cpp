// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/threading/details/_common.h>
#include <cds/gc/hp.h>
#include <cds/gc/dhp.h>

namespace cds { namespace threading {

    CDS_EXPORT_API atomics::atomic<size_t> ThreadData::s_nLastUsedProcNo( 0 );
    CDS_EXPORT_API size_t ThreadData::s_nProcCount = 1;

    CDS_EXPORT_API void ThreadData::init()
    {
        if ( m_nAttachCount++ == 0 ) {
            if ( cds::gc::HP::isUsed())
                cds::gc::hp::smr::attach_thread();
            if ( cds::gc::DHP::isUsed())
                cds::gc::dhp::smr::attach_thread();

            if ( cds::urcu::details::singleton<cds::urcu::general_instant_tag>::isUsed())
                m_pGPIRCU = cds::urcu::details::singleton<cds::urcu::general_instant_tag>::attach_thread();
            if ( cds::urcu::details::singleton<cds::urcu::general_buffered_tag>::isUsed())
                m_pGPBRCU = cds::urcu::details::singleton<cds::urcu::general_buffered_tag>::attach_thread();
            if ( cds::urcu::details::singleton<cds::urcu::general_threaded_tag>::isUsed())
                m_pGPTRCU = cds::urcu::details::singleton<cds::urcu::general_threaded_tag>::attach_thread();
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            if ( cds::urcu::details::singleton<cds::urcu::signal_buffered_tag>::isUsed())
                m_pSHBRCU = cds::urcu::details::singleton<cds::urcu::signal_buffered_tag>::attach_thread();
#endif
        }
    }

    CDS_EXPORT_API bool ThreadData::fini()
    {
        if ( --m_nAttachCount == 0 ) {
            if ( cds::gc::DHP::isUsed())
                cds::gc::dhp::smr::detach_thread();
            if ( cds::gc::HP::isUsed())
                cds::gc::hp::smr::detach_thread();

            if ( cds::urcu::details::singleton<cds::urcu::general_instant_tag>::isUsed()) {
                cds::urcu::details::singleton<cds::urcu::general_instant_tag>::detach_thread( m_pGPIRCU );
                m_pGPIRCU = nullptr;
            }
            if ( cds::urcu::details::singleton<cds::urcu::general_buffered_tag>::isUsed()) {
                cds::urcu::details::singleton<cds::urcu::general_buffered_tag>::detach_thread( m_pGPBRCU );
                m_pGPBRCU = nullptr;
            }
            if ( cds::urcu::details::singleton<cds::urcu::general_threaded_tag>::isUsed()) {
                cds::urcu::details::singleton<cds::urcu::general_threaded_tag>::detach_thread( m_pGPTRCU );
                m_pGPTRCU = nullptr;
            }
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            if ( cds::urcu::details::singleton<cds::urcu::signal_buffered_tag>::isUsed()) {
                cds::urcu::details::singleton<cds::urcu::signal_buffered_tag>::detach_thread( m_pSHBRCU );
                m_pSHBRCU = nullptr;
            }
#endif
            return true;
        }
        return false;
    }


}} // namespace cds::threading

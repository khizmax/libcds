// Copyright (c) 2020-2020 Alexander Gaev
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/gc/hp.h>
#include <map>

namespace cds { namespace gc { namespace hp { namespace details {

#ifdef CDS_DISABLE_CLASS_TLS_INLINE
    // MSVC
    namespace {
        // for DefaultTLS Manager
        thread_local thread_data* default_tls_manager_ = nullptr;
        generic_smr<DefaultTLSManager>* default_tls_instance_ = nullptr;
    } // namespace

    /*static*/ CDS_EXPORT_API thread_data* DefaultTLSManager::getTLS() noexcept
    {
        return default_tls_manager_;
    }

    /*static*/ CDS_EXPORT_API void DefaultTLSManager::setTLS( thread_data* td ) noexcept
    {
        default_tls_manager_ = td;
    }

    /*static*/ CDS_EXPORT_API generic_smr<DefaultTLSManager>* DefaultTLSManager::getInstance() noexcept
    {
        return default_tls_instance_;
    }

    /*static*/ CDS_EXPORT_API void DefaultTLSManager::setInstance(generic_smr<DefaultTLSManager>* new_instance) noexcept
    {
        default_tls_instance_ = new_instance;
    }
#else
    // for DefaultTLS Manager
    // GCC, CLang
    thread_local thread_data* DefaultTLSManager::tls_ = nullptr;
    generic_smr<DefaultTLSManager>* DefaultTLSManager::instance_ = nullptr;
#endif

    // for StrangeTLSManager
    thread_local std::pair<thread_data *, thread_data *> *tls2_ = new std::pair<thread_data *, thread_data *>(
            nullptr,
            nullptr);
    generic_smr<StrangeTLSManager>* strange_tls_instance_ = nullptr;

    /*static*/ CDS_EXPORT_API generic_smr<StrangeTLSManager>* StrangeTLSManager::getInstance()
    {
        return strange_tls_instance_;
    }

    /*static*/ CDS_EXPORT_API void StrangeTLSManager::setInstance(generic_smr<StrangeTLSManager>* new_instance)
    {
        strange_tls_instance_ = new_instance;
    }

    /*static*/ CDS_EXPORT_API thread_data *StrangeTLSManager::getTLS() {
        if (cds::OS::get_current_thread_id() % 2 == 0) { // % 2 with ThreadId structure?
            return tls2_->second;
        } else {
            return tls2_->first;
        }
    }

    /*static*/ CDS_EXPORT_API void StrangeTLSManager::setTLS(thread_data *new_tls) {
        if (cds::OS::get_current_thread_id() % 2 == 0) { // % 2 with ThreadId structure?
            tls2_->second = new_tls;
        } else {
            tls2_->first = new_tls;
        }
    }

}}}} // namespace cds::gc::hp::details

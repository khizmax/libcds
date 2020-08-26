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
        // for Default Data Holder
        thread_local thread_data* default_data_holder_tls_ = nullptr;
        generic_smr<DefaultDataHolder>* default_data_holder_instance_ = nullptr;
    } // namespace

    /*static*/ CDS_EXPORT_API thread_data* DefaultDataHolder::getTLS() noexcept
    {
        return default_data_holder_tls_;
    }

    /*static*/ CDS_EXPORT_API void DefaultDataHolder::setTLS( thread_data* td ) noexcept
    {
        default_data_holder_tls_ = td;
    }

    /*static*/ CDS_EXPORT_API generic_smr<DefaultDataHolder>* DefaultDataHolder::getInstance() noexcept
    {
        return default_data_holder_instance_;
    }

    /*static*/ CDS_EXPORT_API void DefaultDataHolder::setInstance(generic_smr<DefaultDataHolder>* new_instance) noexcept
    {
        default_data_holder_instance_ = new_instance;
    }
#else
    // for Default Data Holder
    // GCC, CLang
    thread_local thread_data* DefaultDataHolder::tls_ = nullptr;
    generic_smr<DefaultDataHolder>* DefaultDataHolder::instance_ = nullptr;
#endif

    // for StrangeDataHolder
    thread_local std::pair<thread_data *, thread_data *> *strange_data_holder_tls_ = new std::pair<thread_data *, thread_data *>(
            nullptr, nullptr);
    generic_smr<StrangeDataHolder>* strange_data_holder_instance_ = nullptr;

    /*static*/ CDS_EXPORT_API generic_smr<StrangeDataHolder>* StrangeDataHolder::getInstance()
    {
        return strange_data_holder_instance_;
    }

    /*static*/ CDS_EXPORT_API void StrangeDataHolder::setInstance(generic_smr<StrangeDataHolder>* new_instance)
    {
        strange_data_holder_instance_ = new_instance;
    }

    /*static*/ CDS_EXPORT_API thread_data *StrangeDataHolder::getTLS() {
        if (cds::OS::get_current_thread_id() % 2 == 0) { // % 2 with ThreadId structure?
            return strange_data_holder_tls_->second;
        } else {
            return strange_data_holder_tls_->first;
        }
    }

    /*static*/ CDS_EXPORT_API void StrangeDataHolder::setTLS(thread_data *new_tls) {
        if (cds::OS::get_current_thread_id() % 2 == 0) { // % 2 with ThreadId structure?
            strange_data_holder_tls_->second = new_tls;
        } else {
            strange_data_holder_tls_->first = new_tls;
        }
    }

}}}} // namespace cds::gc::hp::details

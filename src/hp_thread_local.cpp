// Copyright (c) 2020-2020 Alexander Gaev
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/gc/hp.h>
#include <map>

namespace cds {
    namespace gc {
        namespace hp {
            namespace details {
                thread_local thread_data *tls_ = nullptr;

                /*static*/ CDS_EXPORT_API thread_data *DefaultTLSManager::getTLS() {
                    return tls_;
                }

                /*static*/ CDS_EXPORT_API void DefaultTLSManager::setTLS(thread_data *new_tls) {
                    tls_ = new_tls;
                }
            }
        }
    }
}

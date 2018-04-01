// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DETAILS_TLS_HOLDER_H
#define CDSLIB_DETAILS_TLS_HOLDER_H

#include <cds/details/defs.h>

//@cond
namespace cds { namespace details {

    template <typename T, typename Tag = void>
    class tls_holder
    {
        static T* tls()
        {
            thread_local T s_tls;
            return &s_tls;
        }

    public:
        static void set( T val )
        {
            *tls() = val;
        }

        static T get()
        {
            return *tls();
        }
    };

}}  // namespace cds::details
//@endcond

#endif // #ifndef CDSLIB_DETAILS_TLS_HOLDER_H

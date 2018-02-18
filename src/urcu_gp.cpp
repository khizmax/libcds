// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/urcu/details/gp.h>

namespace cds { namespace urcu { namespace details {

    template<> CDS_EXPORT_API singleton_vtbl * gp_singleton_instance< general_instant_tag >::s_pRCU = nullptr;
    template<> CDS_EXPORT_API singleton_vtbl * gp_singleton_instance< general_buffered_tag >::s_pRCU = nullptr;
    template<> CDS_EXPORT_API singleton_vtbl * gp_singleton_instance< general_threaded_tag >::s_pRCU = nullptr;

}}} // namespace cds::urcu::details

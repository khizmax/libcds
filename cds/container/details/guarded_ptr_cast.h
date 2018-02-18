// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_GUARDED_PTR_CAST_H
#define CDSLIB_CONTAINER_DETAILS_GUARDED_PTR_CAST_H
//@cond

#include <cds/details/defs.h>

namespace cds { namespace container { namespace details {

    template <typename Node, typename T>
    struct guarded_ptr_cast_set {
        T * operator()(Node* pNode ) const noexcept
        {
            return &(pNode->m_Value);
        }
    };

    template <typename Node, typename T>
    struct guarded_ptr_cast_map {
        T * operator()(Node* pNode ) const noexcept
        {
            return &(pNode->m_Data);
        }
    };

}}} // namespace cds::container::details

//@endcond
#endif // #ifndef CDSLIB_CONTAINER_DETAILS_GUARDED_PTR_CAST_H

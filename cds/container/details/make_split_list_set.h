// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_SPLIT_LIST_SET_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_SPLIT_LIST_SET_H

#include <cds/container/details/split_list_base.h>
#include <cds/details/allocator.h>
#include <cds/details/binary_functor_wrapper.h>

//@cond
namespace cds { namespace container {

    // Forward declaration
    struct michael_list_tag;
    struct lazy_list_tag;

}} // namespace cds::container
//@endcond


#ifdef CDSLIB_CONTAINER_DETAILS_MICHAEL_LIST_BASE_H
#   include <cds/container/details/make_split_list_set_michael_list.h>
#endif

#ifdef CDSLIB_CONTAINER_DETAILS_LAZY_LIST_BASE_H
#   include <cds/container/details/make_split_list_set_lazy_list.h>
#endif

#ifdef CDSLIB_CONTAINER_DETAILS_ITERABLE_LIST_BASE_H
#   include <cds/container/details/make_split_list_set_iterable_list.h>
#endif

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_SPLIT_LIST_SET_H

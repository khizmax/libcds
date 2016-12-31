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

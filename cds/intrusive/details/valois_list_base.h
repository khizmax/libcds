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

#include <type_traits>
#include <cds/intrusive/details/base.h>
#include <cds/opt/compare.h>
#include <cds/algo/atomic.h>
#include <cds/details/marked_ptr.h>
#include <cds/urcu/options.h>

#ifndef CDSLIB_INTRUSIVE_DETAILS_VALOIS_LIST_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_VALOIS_LIST_BASE_H

namespace cds { namespace intrusive {

    namespace valois_list{
        struct traits
        {
            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// Specifies binary predicate used for key compare.
            /**
                Default is \p std::less<T>
            */
            typedef opt::none                       less;

            /// Node allocator
            typedef CDS_DEFAULT_ALLOCATOR           node_allocator;

            /// Back-off strategy
            typedef cds::backoff::Default           back_off;

            /// Disposer for removing items
            typedef opt::v::empty_disposer          disposer;


            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter or \p atomicity::cache_friendly_item_counter to enable item counting
            typedef atomicity::empty_item_counter   item_counter;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;
        };

        template <typename T>
        struct node
        {
            typedef T value_type; ///< Value type
            typedef cds::details::marked_ptr<T, 1>   marked_data_ptr; ///< marked pointer to the value

            atomics::atomic< node* >            next;  ///< pointer to next node in the list
            atomics::atomic< marked_data_ptr >  data;  ///< pointer to user data, \p nullptr if the node is free

            node()
            {
                next.store( nullptr, atomics::memory_order_release );
                data.store( marked_data_ptr(), atomics::memory_order_release );
            }

            node( value_type * pVal )
            {
                next.store( nullptr, atomics::memory_order_release );
                data.store( marked_data_ptr( pVal ), atomics::memory_order_release );
            }

        };
    }
}}

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_VALOIS_LIST_BASE_H

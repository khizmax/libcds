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

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_ITERABLE_KVLIST_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_ITERABLE_KVLIST_H

#include <cds/details/binary_functor_wrapper.h>
#include <cds/opt/compare.h>

namespace cds { namespace container {

    //@cond
    namespace details {

        template <class GC, typename K, typename T, class Traits>
        struct make_iterable_kvlist
        {
            typedef Traits original_type_traits;

            typedef GC       gc;
            typedef K        key_type;
            typedef T        mapped_type;
            typedef std::pair<key_type const, mapped_type> value_type;

            typedef typename original_type_traits::allocator::template rebind<value_type>::other data_allocator_type;
            typedef cds::details::Allocator< value_type, data_allocator_type > cxx_data_allocator;

            typedef typename original_type_traits::memory_model memory_model;

            struct data_disposer
            {
                void operator ()( value_type * pData )
                {
                    cxx_data_allocator().Delete( pData );
                }
            };

            struct key_field_accessor {
                key_type const& operator()( value_type const& data )
                {
                    return data.first;
                }
            };

            template <typename Less>
            struct less_wrapper
            {
                template <typename Q>
                bool operator()( value_type const& lhs, Q const& rhs ) const
                {
                    return Less()( lhs.first, rhs );
                }

                template <typename Q>
                bool operator()( Q const& lhs, value_type const& rhs ) const
                {
                    return Less()( lhs, rhs.first );
                }
            };

            typedef typename opt::details::make_comparator< key_type, original_type_traits >::type key_comparator;

            struct base_traits: public original_type_traits
            {
                typedef data_disposer disposer;
                typedef cds::details::compare_wrapper< value_type, key_comparator, key_field_accessor > compare;
            };

            typedef container::IterableList<gc, value_type, base_traits> type;
        };
    }   // namespace details
    //@endcond

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_ITERABLE_KVLIST_H

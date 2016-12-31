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

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_MICHAEL_LIST_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_MICHAEL_LIST_H

#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace container {

    //@cond
    namespace details {

        template <class GC, typename T, class Traits>
        struct make_michael_list
        {
            typedef GC      gc;
            typedef T       value_type;

            struct node_type : public intrusive::michael_list::node<gc>
            {
                value_type  m_Value;

                node_type()
                {}

                template <typename Q>
                node_type( Q const& v )
                    : m_Value(v)
                {}

                template <typename... Args>
                node_type( Args&&... args )
                    : m_Value( std::forward<Args>(args)... )
                {}
            };

            typedef Traits original_traits;

            typedef typename original_traits::allocator::template rebind<node_type>::other allocator_type;
            typedef cds::details::Allocator< node_type, allocator_type > cxx_allocator;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };

            typedef typename opt::details::make_comparator< value_type, original_traits >::type key_comparator;

            struct value_accessor
            {
                value_type const & operator()( node_type const& node ) const
                {
                    return node.m_Value;
                }
            };

            template <typename Less>
            struct less_wrapper {
                typedef cds::details::compare_wrapper< node_type, cds::opt::details::make_comparator_from_less<Less>, value_accessor > type;
            };

            struct intrusive_traits: public original_traits
            {
                typedef intrusive::michael_list::base_hook< opt::gc<gc> > hook;
                typedef node_deallocator disposer;
                typedef cds::details::compare_wrapper< node_type, key_comparator, value_accessor > compare;
                static CDS_CONSTEXPR const opt::link_check_type link_checker = cds::intrusive::michael_list::traits::link_checker;
            };

            typedef intrusive::MichaelList<gc, node_type, intrusive_traits>  type;
        };
    }   // namespace details
    //@endcond

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_MICHAEL_LIST_H

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

#ifndef CDSLIB_DETAILS_BINARY_FUNCTOR_WRAPPER_H
#define CDSLIB_DETAILS_BINARY_FUNCTOR_WRAPPER_H

#include <cds/details/defs.h>

//@cond
namespace cds { namespace details {

    template <typename ReturnType, typename Functor, typename ArgType, typename Accessor>
    struct binary_functor_wrapper {
        typedef ReturnType  return_type;
        typedef Functor     functor_type;
        typedef ArgType     argument_type;
        typedef Accessor    accessor;

        return_type operator()( argument_type const& a1, argument_type const& a2 ) const
        {
            return functor_type()( accessor()( a1 ), accessor()( a2 ));
        }

        template <typename Q>
        return_type operator()( argument_type const& a, Q const& q ) const
        {
            return functor_type()( accessor()(a), q );
        }

        template <typename Q>
        return_type operator()( Q const& q, argument_type const& a ) const
        {
            return functor_type()( q, accessor()(a));
        }

        template <typename Q1, typename Q2>
        return_type operator()( Q1 const& q1, Q2 const& q2 ) const
        {
            return functor_type()( q1, q2 );
        }
    };

    struct trivial_accessor
    {
        template <typename T>
        T const& operator()( T const& p ) const
        {
            return p;
        }

        template <typename T>
        T& operator()( T& p ) const
        {
            return p;
        }
    };

    template <typename ArgType, typename Predicate, typename Accessor>
    using predicate_wrapper = binary_functor_wrapper< bool, Predicate, ArgType, Accessor>;

    template <typename ArgType, typename Compare, typename Accessor>
    using compare_wrapper = binary_functor_wrapper< int, Compare, ArgType, Accessor>;

}} // namespace cds::details

//@endcond

#endif // #ifndef CDSLIB_DETAILS_BINARY_FUNCTOR_WRAPPER_H

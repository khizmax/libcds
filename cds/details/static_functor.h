// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DETAILS_STATIC_FUNCTOR_H
#define CDSLIB_DETAILS_STATIC_FUNCTOR_H

//@cond
namespace cds { namespace details {

    template <class Functor, typename T>
    struct static_functor
    {
        static void call( void* p )
        {
            Functor()( reinterpret_cast<T*>( p ));
        }
    };

}} // namespace cds::details
//@endcond

#endif // #ifndef CDSLIB_DETAILS_STATIC_FUNCTOR_H

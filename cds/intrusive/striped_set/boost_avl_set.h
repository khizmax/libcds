// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_AVL_SET_ADAPTER_H
#define CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_AVL_SET_ADAPTER_H

#include <boost/intrusive/avl_set.hpp>
#include <cds/intrusive/striped_set/adapter.h>

//@cond
namespace cds { namespace intrusive { namespace striped_set {

#if CDS_COMPILER == CDS_COMPILER_INTEL && CDS_COMPILER_VERSION <= 1500
    template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename... Options>
    class adapt< boost::intrusive::avl_set< T, P1, P2, P3, P4, P5 >, Options... >
    {
    public:
        typedef boost::intrusive::avl_set< T, P1, P2, P3, P4, P5 >  container_type;   ///< underlying intrusive container type

    public:
        typedef details::boost_intrusive_set_adapter<container_type>   type;  ///< Result of the metafunction
    };
#else
    template <typename T, typename... BIOptons, typename... Options>
    class adapt< boost::intrusive::avl_set< T, BIOptons... >, Options... >
    {
    public:
        typedef boost::intrusive::avl_set< T, BIOptons... >  container_type  ;   ///< underlying intrusive container type

    public:
        typedef details::boost_intrusive_set_adapter<container_type>   type ;  ///< Result of the metafunction
    };
#endif

}}} // namespace cds::intrusive::striped_set
//@endcond

#endif // #ifndef CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_AVL_SET_ADAPTER_H

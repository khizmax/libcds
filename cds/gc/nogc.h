// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_GC_NOGC_H
#define CDSLIB_GC_NOGC_H

namespace cds { namespace gc {

    /// No garbage collecting
    /**  @ingroup cds_garbage_collector
        This empty class is used in \p libcds to mark that a template specialization implements
        the container without any garbage collector schema.

        Usually, the container with this "GC" does not support the item removal.
    */
    class nogc
    {
    public:
        //@cond
        /// Faked scan
        static void scan()
        {}
        static void force_dispose()
        {}
        //@endcond
    };

}}  // namespace cds::gc

#endif // #define CDSLIB_GC_NOGC_H

/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSLIB_CONTAINER_DETAILS_MICHAEL_MAP_BASE_H
#define CDSLIB_CONTAINER_DETAILS_MICHAEL_MAP_BASE_H

#include <cds/container/details/michael_set_base.h>

namespace cds { namespace container {

    /// MichaelHashMap related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace michael_map {
        /// \p MichaelHashMap traits
        typedef container::michael_set::traits  traits;

        /// Metafunction converting option list to \p michael_map::traits
        template <typename... Options>
        using make_traits = cds::intrusive::michael_set::make_traits< Options... >;

        //@cond
        namespace details {
            using michael_set::details::init_hash_bitmask;
        }
        //@endcond

    }   // namespace michael_map

    //@cond
    // Forward declarations
    template <class GC, class OrderedList, class Traits = michael_map::traits>
    class MichaelHashMap;
    //@endcond

}}  // namespace cds::container


#endif  // ifndef CDSLIB_CONTAINER_DETAILS_MICHAEL_MAP_BASE_H

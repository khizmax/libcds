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

#ifndef CDSLIB_CONTAINER_DETAILS_BASE_H
#define CDSLIB_CONTAINER_DETAILS_BASE_H

#include <cds/intrusive/details/base.h>

namespace cds {

/// Standard (non-intrusive) containers
/**
    @ingroup cds_nonintrusive_containers
    This namespace contains implementations of non-intrusive (std-like) lock-free containers.
*/
namespace container {

    /// Common options for non-intrusive containers
    /** @ingroup cds_nonintrusive_helper
        This namespace contains options for non-intrusive containers that is, in general, the same as for the intrusive containers.
        It imports all definitions from cds::opt and cds::intrusive::opt namespaces
    */
    namespace opt {
        using namespace cds::intrusive::opt;
    }   // namespace opt

    /// @defgroup cds_nonintrusive_containers Non-intrusive containers
    /** @defgroup cds_nonintrusive_helper Helper structs for non-intrusive containers
        @ingroup cds_nonintrusive_containers
    */

    /** @defgroup cds_nonintrusive_stack Stack
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_queue Queue
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_deque Deque
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_priority_queue Priority queue
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_map Map
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_set Set
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_list List
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_tree Tree
        @ingroup cds_nonintrusive_containers
    */

}   // namespace container
}   // namespace cds

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_BASE_H

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

#ifndef CDSLIB_GC_HP_H
#define CDSLIB_GC_HP_H

#include <cds/gc/impl/hp_decl.h>
#include <cds/gc/impl/hp_impl.h>
#include <cds/details/lib.h>

/**
    @page cds_garbage_collectors_comparison GC comparison
    @ingroup cds_garbage_collector

    <table>
        <tr>
            <th>Feature</th>
            <th>%cds::gc::HP</th>
            <th>%cds::gc::DHP</th>
        </tr>
        <tr>
            <td>Max number of guarded (hazard) pointers per thread</td>
            <td>limited (specifies in GC object ctor)</td>
            <td>unlimited (dynamically allocated when needed)</td>
        </tr>
        <tr>
            <td>Max number of retired pointers<sup>1</sup></td>
            <td>bounded</td>
            <td>bounded</td>
        </tr>
        <tr>
            <td>Array of retired pointers</td>
            <td>preallocated for each thread, size is limited</td>
            <td>global for the entire process, unlimited (dynamically allocated when needed)</td>
        </tr>
    </table>

    <sup>1</sup>Unbounded count of retired pointer means a possibility of memory exhaustion.
*/

namespace cds {

    /// Different safe memory reclamation schemas (garbage collectors)
    /** @ingroup cds_garbage_collector

        This namespace specifies different safe memory reclamation (SMR) algorithms.
        See \ref cds_garbage_collector "Garbage collectors"
    */
    namespace gc {
    } // namespace gc

} // namespace cds


#endif  // #ifndef CDSLIB_GC_HP_H

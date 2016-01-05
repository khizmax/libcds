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

#ifndef CDSLIB_ALGO_ELIMINATION_H
#define CDSLIB_ALGO_ELIMINATION_H

#include <cds/algo/elimination_tls.h>
#include <cds/algo/elimination_opt.h>
#include <cds/algo/atomic.h>
#include <cds/threading/model.h>

namespace cds { namespace algo {

    /// Elimination technique
    /** @anchor cds_elimination_description
        Elimination technique allows highly distributed coupling and execution of operations with reverse
        semantics like the pushes and pops on a stack. If a push followed by a pop are performed
        on a stack, the data structure's state does not change (similarly for a pop followed by a push).
        This means that if one can cause pairs of pushes and pops to meet and pair up in
        separate locations, the threads can exchange values without having to touch a centralized structure
        since they have anyhow "eliminated" each other's effect on it. Elimination can be implemented
        by using a collision array in which threads pick random locations in order to try and collide.
        Pairs of threads that "collide" in some location run through a synchronization protocol,
        and all such disjoint collisions can be performed in parallel. If a thread has not met another
        in the selected location or if it met a thread with an operation that cannot be eliminated
        (such as two push operations), an alternative scheme must be used.
    */
    namespace elimination {

        /// Base class describing an operation for eliminating
        /**
            This class contains some debugng info.
            Actual operation descriptor depends on real container and its interface.
        */
        struct operation_desc
        {
            record * pOwner;    ///< Owner of the descriptor
        };

        /// Acquires elimination record for the current thread
        template <typename OperationDesc>
        static inline record * init_record( OperationDesc& op )
        {
            record& rec = cds::threading::elimination_record();
            assert( rec.is_free());
            op.pOwner = &rec;
            rec.pOp = static_cast<operation_desc *>( &op );
            return &rec;
        }

        /// Releases elimination record for the current thread
        static inline void clear_record()
        {
            cds::threading::elimination_record().pOp = nullptr;
        }
    } // namespace elimination
}} // namespace cds::algo

#endif // CDSLIB_ALGO_ELIMINATION_H

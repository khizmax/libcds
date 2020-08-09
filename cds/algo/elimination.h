// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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

        For each thread an struct \p cds::algo::elimination::record stores current elimination record. 
        TLS-based \p cds::algo::elimination::storage class manages per-thread elimination record.
    */
    namespace elimination {

        /// Base class describing an operation for eliminating
        /**
            This class contains some debug info.
            Actual operation descriptor depends on real container and its interface
            and must be inherited from \p operation_desc.
        */
        struct operation_desc
        {
            record * pOwner = nullptr;    ///< Owner of the descriptor
        };

        /// Acquires elimination record for the current thread
        /**
            \p OperationDesc must be inherited from \p cds::algo::elimination::operation_desc.

            \p Storage - class managing elimination \p cds::algo::elimination::record, 
            for example, TLS-based \p cds::algo::elimination::storage.
        */
        template <class Storage, typename OperationDesc>
        static inline record * init_record( OperationDesc& op )
        {
            record& rec = Storage::get();
            assert( rec.is_free());
            op.pOwner = &rec;
            rec.pOp = static_cast<operation_desc *>( &op );
            return &rec;
        }

        /// Releases elimination record for the current thread
        template <class Storage>
        static inline void clear_record()
        {
            Storage::get().pOp = nullptr;
        }
    } // namespace elimination
}} // namespace cds::algo

#endif // CDSLIB_ALGO_ELIMINATION_H

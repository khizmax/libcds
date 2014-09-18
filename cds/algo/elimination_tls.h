//$$CDS-header$$

#ifndef __CDS_ALGO_ELIMINATION_TLS_H
#define __CDS_ALGO_ELIMINATION_TLS_H

#include <cds/algo/base.h>

namespace cds { namespace algo { namespace elimination {

    // Forwards
    struct operation_desc;

    /// Per-thread elimination record
    /** @headerfile cds/algo/elimination.h
    */
    struct record
    {
        operation_desc *    pOp ;   ///< Operation descriptor

        /// Initialization
        record()
            : pOp( null_ptr<operation_desc *>() )
        {}

        /// Checks if the record is free
        bool is_free() const
        {
            return pOp == null_ptr<operation_desc *>();
        }
    };

}}} // cds::algo::elimination

#endif // #ifndef __CDS_ALGO_ELIMINATION_TLS_H

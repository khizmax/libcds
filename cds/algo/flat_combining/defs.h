// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_ALGO_FLAT_COMBINING_DEFS_H
#define CDSLIB_ALGO_FLAT_COMBINING_DEFS_H

#include <cds/algo/atomic.h>


namespace cds { namespace algo { namespace flat_combining {

    /// Special values of \p publication_record::nRequest
    enum request_value
    {
        req_EmptyRecord,    ///< Publication record is empty
        req_Response,       ///< Operation is done

        req_Operation       ///< First operation id for derived classes
    };

    /// \p publication_record state
    enum record_state {
        inactive,       ///< Record is inactive
        active,         ///< Record is active
        removed         ///< Record should be removed
    };

    /// Record of publication list
    /**
        Each data structure based on flat combining contains a class derived from \p %publication_record
    */
    struct publication_record {
        atomics::atomic<unsigned int>           nRequest;   ///< Request field (depends on data structure)
        atomics::atomic<unsigned int>           nState;     ///< Record state: inactive, active, removed
        atomics::atomic<unsigned int>           nAge;       ///< Age of the record
        atomics::atomic<publication_record *>   pNext;      ///< Next record in active publication list
        atomics::atomic<publication_record *>   pNextAllocated; ///< Next record in allocated publication list

        /// Initializes publication record
        publication_record()
            : nRequest( req_EmptyRecord )
            , nAge( 0 )
            , pNext( nullptr )
            , pNextAllocated( nullptr )
        {
            nState.store( inactive, atomics::memory_order_release );
        }

        /// Returns the value of \p nRequest field
        unsigned int op( atomics::memory_order mo = atomics::memory_order_relaxed ) const
        {
            return nRequest.load( mo );
        }

        /// Checks if the operation is done
        bool is_done() const
        {
            return nRequest.load( atomics::memory_order_relaxed ) == req_Response;
        }
    };


}}} // namespace cds::algo::flat_combining

#endif // CDSLIB_ALGO_FLAT_COMBINING_DEFS_H

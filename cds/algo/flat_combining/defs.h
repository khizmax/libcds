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
        atomics::atomic<publication_record *>   pNext;      ///< Next record in publication list
        void *                                  pOwner;     ///< [internal data] Pointer to \ref kernel object that manages the publication list

        /// Initializes publication record
        publication_record()
            : nRequest( req_EmptyRecord )
            , nState( inactive )
            , nAge( 0 )
            , pNext( nullptr )
            , pOwner( nullptr )
        {}

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

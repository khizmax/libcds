// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_ALGO_ELIMINATION_TLS_H
#define CDSLIB_ALGO_ELIMINATION_TLS_H

#include <cds/algo/base.h>

namespace cds { namespace algo { namespace elimination {

    // Forwards
    struct operation_desc;

    /// Per-thread elimination record
    /** @headerfile cds/algo/elimination.h
    */
    struct record
    {
        operation_desc *    pOp = nullptr;   ///< Operation descriptor

        /// Checks if the record is free
        bool is_free() const
        {
            return pOp == nullptr;
        }
    };

    /// \p thread_local storage for elimination record for current thread
    /** @headerfile cds/algo/elimination.h
    */
    class storage
    {
#ifndef CDS_DISABLE_CLASS_TLS_INLINE
        // GCC, CLang
    public:
        /// Get elimination \p cds::algo::elimination::record for the current thread
        static record& get() noexcept
        {
            return tls_;
        }

    private:
        //@cond
        static thread_local record tls_;
        //@endcond
#else   //MSVC
    public:
        static CDS_EXPORT_API record& get() noexcept;
#endif  //#ifndef CDS_DISABLE_CLASS_TLS_INLINE
    };

}}} // cds::algo::elimination

#endif // #ifndef CDSLIB_ALGO_ELIMINATION_TLS_H

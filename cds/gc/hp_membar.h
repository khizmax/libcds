// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/details/defs.h>
#include <atomic>

namespace cds { namespace gc { namespace hp {

    class seq_qst_membar {
    public:
        static void sync_fast_path()
        {
            std::atomic_thread_fence( std::memory_order_seq_cst );
        }

        static void sync_slow_path()
        {
            sync_fast_path();
        }

        //@cond
        static void init()
        {}
        //@endcond
    };

    class shared_var_membar {
    private:
        static std::atomic<unsigned> shared_var_;

    public:
        static void sync_fast_path()
        {
            shared_var_.fetch_add( 1, std::memory_order_acq_rel );
        }

        static void sync_slow_path()
        {
            sync_fast_path();
        }

        //@cond
        static void init()
        {}
        //@endcond
    };

    typedef seq_qst_membar default_membar;

    class asymmetric_membar {
#   if CDS_OS_TYPE == CDS_OS_LINUX
        static bool membarrier_available_;
        static void call_membarrier();
        static void check_membarrier_available();
#   endif

    public:
        static void sync_fast_path()
        {
#       if CDS_OS_TYPE == CDS_OS_LINUX
            if ( membarrier_available_ )
                CDS_COMPILER_RW_BARRIER;
            else 
                default_membar::sync_fast_path();
#       else
            default_membar::sync_fast_path();
#       endif
        }

        static void sync_slow_path()
        {
#       if CDS_OS_TYPE == CDS_OS_LINUX
            if ( membarrier_available_ )
                call_membarrier();
            else
                default_membar::sync_fast_path();
#       else
            default_membar::sync_fast_path();
#       endif
        }

        //@cond
        static void init()
        {
#       if CDS_OS_TYPE == CDS_OS_LINUX
            check_membarrier_available();
#       endif
        }
        //@endcond
    };

    class asymmetric_global_membar {
#   if CDS_OS_TYPE == CDS_OS_LINUX
        static bool membarrier_available_;
        static void call_membarrier();
        static void check_membarrier_available();
#   endif

    public:
        static void sync_fast_path()
        {
#       if CDS_OS_TYPE == CDS_OS_LINUX
            if ( membarrier_available_ )
                CDS_COMPILER_RW_BARRIER;
            else
                default_membar::sync_fast_path();
#       else
            default_membar::sync_fast_path();
#       endif
        }

        static void sync_slow_path()
        {
#       if CDS_OS_TYPE == CDS_OS_LINUX
            if ( membarrier_available_ )
                call_membarrier();
            else
                default_membar::sync_fast_path();
#       else
            default_membar::sync_fast_path();
#       endif
        }

        //@cond
        static void init()
        {
#       if CDS_OS_TYPE == CDS_OS_LINUX
            check_membarrier_available();
#       endif
        }
        //@endcond
    };

}}} // namespace cds::gc::hp

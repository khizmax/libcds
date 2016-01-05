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

#ifndef CDSLIB_URCU_DETAILS_CHECK_DEADLOCK_H
#define CDSLIB_URCU_DETAILS_CHECK_DEADLOCK_H

#include <cds/urcu/options.h>

//@cond
namespace cds { namespace urcu { namespace details {

    template <class RCU, typename PolicyType>
    struct check_deadlock_policy
    {
        static void check()
        {
            assert( !RCU::is_locked() );
            if ( RCU::is_locked() )
                throw cds::urcu::rcu_deadlock();
        }
    };

    template <class RCU>
    struct check_deadlock_policy< RCU, cds::opt::v::rcu_no_check_deadlock>
    {
        static void check()
        {}
    };

    template <class RCU>
    struct check_deadlock_policy< RCU, cds::opt::v::rcu_assert_deadlock>
    {
        static void check()
        {
            assert( !RCU::is_locked() );
        }
    };

}}} // namespace cds::urcu::details
//@endcond

#endif // #ifndef CDSLIB_URCU_DETAILS_CHECK_DEADLOCK_H

/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#ifndef CDSLIB_URCU_OPTIONS_H
#define CDSLIB_URCU_OPTIONS_H

#include <cds/details/defs.h>

namespace cds { namespace urcu {
    /// Exception "RCU deadlock detected"
    /**@anchor cds_urcu_rcu_deadlock
        This exception is raised when \p cds::opt::v::rcu_throw_deadlock deadlock checking policy
        is used, see \p cds::opt::rcu_check_deadlock option.
    */
    class rcu_deadlock: public std::logic_error
    {
        //@cond
    public:
        rcu_deadlock()
            : std::logic_error( "RCU deadlock detected" )
        {}
        //@endcond
    };
}} // namespace cds::urcu


namespace cds { namespace opt {

    /// [type-option] RCU check deadlock option setter
    /**
        The RCU containers can check if a deadlock between read-side critical section
        and \p synchronize call is possible.
        This option specifies a policy for checking this situation.
        Possible \p Type is:
        - \p opt::v::rcu_no_check_deadlock - no deadlock checking
        - \p opt::v::rcu_assert_deadlock - call \p assert in debug mode only
        - \p opt::v::rcu_throw_deadlock - throw an \p cds::urcu::rcu_deadlock exception when a deadlock
            is encountered

        Usually, the default \p Type for this option is \p opt::v::rcu_throw_deadlock.
    */
    template <typename Type>
    struct rcu_check_deadlock
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type rcu_check_deadlock;
        };
        //@endcond
    };

    namespace v {
        /// \ref opt::rcu_check_deadlock option value: no deadlock checking
        struct rcu_no_check_deadlock {};

        /// \ref opt::rcu_check_deadlock option value: call \p assert in debug mode only
        struct rcu_assert_deadlock {};

        /// \ref opt::rcu_check_deadlock option value: throw a cds::urcu::rcu_deadlock exception when a deadlock detected
        struct rcu_throw_deadlock {};
    }
}}  // namespace cds::opt


#endif  // #ifndef CDSLIB_URCU_OPTIONS_H

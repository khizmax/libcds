// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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

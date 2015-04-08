//$$CDS-header$$

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

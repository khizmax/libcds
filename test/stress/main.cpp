// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds_test/stress_test.h>

#include <cds/init.h>
#include <cds/gc/hp.h>
#include <cds/gc/dhp.h>
#ifdef CDSUNIT_USE_URCU
#   include <cds/urcu/general_instant.h>
#   include <cds/urcu/general_buffered.h>
#   include <cds/urcu/general_threaded.h>
#   include <cds/urcu/signal_buffered.h>
#endif

#ifdef CDS_ENABLE_HPSTAT
#   include <cds_test/stat_hp_out.h>
#   include <cds_test/stat_dhp_out.h>
#   include <iostream>
#endif
#include <random>


/*static*/ std::random_device cds_test::fixture::random_dev_;
/*static*/ std::mt19937 cds_test::fixture::random_gen_( random_dev_());

int main( int argc, char **argv )
{
    int result;
    cds::Initialize();
    {
        // Read test config file
        cds_test::init_config( argc, argv );

        std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << "\n";

        // Init Google test
        ::testing::InitGoogleTest( &argc, argv );

        cds_test::config const& general_cfg = cds_test::stress_fixture::get_config( "General" );

        // Init SMR
        cds::gc::HP hzpGC(
            general_cfg.get_size_t( "hazard_pointer_count", 16 ),
            general_cfg.get_size_t( "hp_max_thread_count", 0 ),
            general_cfg.get_size_t( "hp_retired_ptr_count", 0 ),
            general_cfg.get( "hp_scan_strategy", "inplace" ) == "inplace" ? cds::gc::HP::scan_type::inplace : cds::gc::HP::scan_type::classic
        );

        cds::gc::DHP dhpGC(
            general_cfg.get_size_t( "dhp_init_guard_count", 16 )
        );

#ifdef CDSUNIT_USE_URCU
        size_t rcu_buffer_size = general_cfg.get_size_t( "rcu_buffer_size", 256 );

        // RCU varieties
        typedef cds::urcu::gc< cds::urcu::general_instant<> >    rcu_gpi;
        rcu_gpi   gpiRCU;

        typedef cds::urcu::gc< cds::urcu::general_buffered<> >    rcu_gpb;
        rcu_gpb   gpbRCU( rcu_buffer_size );

        typedef cds::urcu::gc< cds::urcu::general_threaded<> >    rcu_gpt;
        rcu_gpt   gptRCU( rcu_buffer_size );

#   ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cds::urcu::gc< cds::urcu::signal_buffered<> >    rcu_shb;
        rcu_shb   shbRCU( rcu_buffer_size, SIGUSR1 );
#   endif
#endif // CDSUNIT_USE_URCU

        cds::threading::Manager::attachThread();
        cds::gc::hp::custom_smr<cds::gc::hp::details::HeapTLSManager>::attach_thread();

        result =  RUN_ALL_TESTS();

        cds::gc::hp::custom_smr<cds::gc::hp::details::HeapTLSManager>::detach_thread();
        cds::threading::Manager::detachThread();
    }

#ifdef CDS_ENABLE_HPSTAT
    {
        cds::gc::HP::stat const& st = cds::gc::HP::postmortem_statistics();
        EXPECT_EQ( st.guard_allocated, st.guard_freed );
        EXPECT_EQ( st.retired_count, st.free_count );
        std::cout << st;
    }
    {
        cds::gc::DHP::stat const& st = cds::gc::DHP::postmortem_statistics();
        EXPECT_EQ( st.guard_allocated, st.guard_freed );
        EXPECT_EQ( st.retired_count, st.free_count );
        std::cout << st;
    }
#endif

    cds::Terminate();

    return result;
}

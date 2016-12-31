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

#include <cds_test/stress_test.h>

#include <cds/init.h>
#include <cds/gc/hp.h>
#include <cds/gc/dhp.h>
#ifdef CDSUNIT_USE_URCU
#   include <cds/urcu/general_instant.h>
#   include <cds/urcu/general_buffered.h>
#   include <cds/urcu/general_threaded.h>
#   include <cds/urcu/signal_buffered.h>
#   include <cds/urcu/signal_threaded.h>
#endif

int main( int argc, char **argv )
{
    int result;
    cds::Initialize();
    {
        // Read test config file
        cds_test::init_config( argc, argv );

        // Get detail level for stress test
        cds_test::stress_fixture::init_detail_level( argc, argv );

        // Init Google test
        ::testing::InitGoogleTest( &argc, argv );

        cds_test::config const& general_cfg = cds_test::stress_fixture::get_config( "General" );

        // Init SMR
        cds::gc::HP hzpGC( general_cfg.get_size_t( "hazard_pointer_count", 16 ));
        hzpGC.setScanType( general_cfg.get( "HZP_scan_strategy", "inplace" ) == "inplace" ? cds::gc::HP::scan_type::inplace : cds::gc::HP::scan_type::classic );

        cds::gc::DHP dhpGC(
            general_cfg.get_size_t( "dhp_liberate_threshold", 1024 ),
            general_cfg.get_size_t( "dhp_init_guard_count", 16 ),
            general_cfg.get_size_t( "dhp_epoch_count", 16 )
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

        typedef cds::urcu::gc< cds::urcu::signal_threaded<> >    rcu_sht;
        rcu_sht   shtRCU( rcu_buffer_size, SIGUSR2 );
#   endif
#endif // CDSUNIT_USE_URCU

        cds::threading::Manager::attachThread();

        result =  RUN_ALL_TESTS();

        cds::threading::Manager::detachThread();
    }
    cds::Terminate();
    return result;
}

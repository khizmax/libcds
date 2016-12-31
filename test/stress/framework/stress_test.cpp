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

#include <fstream>
#include <iostream>
#include <cds_test/stress_test.h>

namespace cds_test {

    static std::string s_stat_prefix( "stat" );

    /*static*/ std::string const& property_stream::stat_prefix()
    {
        return s_stat_prefix;
    }

    /*static*/ void property_stream::set_stat_prefix( char const* prefix )
    {
        if ( prefix && prefix[0] )
            s_stat_prefix = prefix;
        else
            s_stat_prefix = "stat";
    }

    /*static*/ property_stream& stress_fixture::propout()
    {
        static property_stream s_prop_stream;
        return s_prop_stream;
    }

    /*static*/ std::vector<std::string> stress_fixture::load_dictionary()
    {
        std::vector<std::string> arrString;

        std::ifstream s;
        char const* filename = "./dictionary.txt";
        s.open( filename );
        if ( !s.is_open()) {
            std::cerr << "WARNING: Cannot open test file " << filename << std::endl;
            return arrString;
        }

        std::string line;
        std::getline( s, line );

        arrString.reserve( std::stoul( line ));

        while ( !s.eof()) {
            std::getline( s, line );
            if ( !line.empty())
                arrString.push_back( std::move( line ));
        }

        s.close();

        return arrString;
    }


    static int s_nDetailLevel = 0;

    /*static*/ void stress_fixture::init_detail_level( int argc, char **argv )
    {
        bool found = false;
        for ( int i = 0; i < argc; ++i ) {
            char * arg = argv[i];
            char * eq = strchr( arg, '=' );
            if ( eq ) {
                if ( strncmp( arg, "--detail_level", eq - arg ) == 0 || strncmp( arg, "--detail-level", eq - arg ) == 0 ) {
                    s_nDetailLevel = atoi( eq + 1 );
                    found = true;
                }
            }
        }

        if ( !found ) {
            // Get detail level from environment variable
            char const * env = getenv( "CDSTEST_DETAIL_LEVEL" );
            if ( env && env[0] )
                s_nDetailLevel = atoi( env );
        }

        std::cout << "Stress test detail level=" << s_nDetailLevel << std::endl;
    }

    /*static*/ bool stress_fixture::check_detail_level( int nLevel )
    {
        if ( nLevel <= s_nDetailLevel )
            return true;

        std::cout << "Skipped (detail level=" << nLevel << ")" << std::endl;
        propout() << std::make_pair( "skipped", 1 );
        return false;
    }

} // namespace

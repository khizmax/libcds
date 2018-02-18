// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <iostream>
#include <cds_test/stress_test.h>

#ifdef CDS_ENABLE_HPSTAT
#   include <cds_test/stat_hp_out.h>
#   include <cds_test/stat_dhp_out.h>
#endif

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

    /*static*/ void stress_fixture::print_hp_stat()
    {
#ifdef CDS_ENABLE_HPSTAT
        {
            cds::gc::HP::stat st;
            cds::gc::HP::statistics( st );
            propout() << st;
        }
        {
            cds::gc::DHP::stat st;
            cds::gc::DHP::statistics( st );
            propout() << st;
        }
#endif
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
} // namespace

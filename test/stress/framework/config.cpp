// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <iostream>

#include <cds_test/stress_test.h>

namespace cds_test {

    class config_file
    {
        std::map< std::string, config>  m_cfg;
        config                          m_emptyCfg;

    public:
        void load( const char * fileName );

        config const& operator[]( const std::string& testName ) const
        {
            auto it = m_cfg.find( testName );
            if ( it != m_cfg.end())
                return it->second;
            return m_emptyCfg;
        }
    };

    void config_file::load( const char * fileName )
    {
        std::ifstream s;
        s.open( fileName );
        if ( !s.is_open()) {
            std::cerr << "WARNING: Cannot open test cfg file " << fileName
                << "\n\tUse default settings"
                << std::endl;
            return;
        }

        std::cout << "Using test config file: " << fileName << std::endl;

        char buf[4096];

        config * pMap = nullptr;
        while ( !s.eof()) {
            s.getline( buf, sizeof( buf ) / sizeof( buf[0] ));
            char * pszStr = buf;
            // trim left
            while ( *pszStr != 0 && (*pszStr == ' ' || *pszStr == '\t')) ++pszStr;
            // trim right
            char * pszEnd = strchr( pszStr, 0 );
            if ( pszEnd == pszStr )    // empty srtring
                continue;
            --pszEnd;
            while ( pszEnd != pszStr && (*pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\n' || *pszEnd == '\r')) --pszEnd;

            if ( pszStr == pszEnd )    // empty string
                continue;

            pszEnd[1] = 0;

            if ( *pszStr == '#' )    // comment
                continue;

            if ( *pszStr == '[' && *pszEnd == ']' ) {    // chapter header
                *pszEnd = 0;
                pMap = &(m_cfg[pszStr + 1]);
                continue;
            }

            if ( !pMap )
                continue;

            char * pszEq = strchr( pszStr, '=' );
            if ( !pszEq )
                continue;
            if ( pszEq == pszStr )
                continue;

            pszEnd = pszEq;
            while ( pszStr <= --pszEnd && (*pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\n' || *pszEnd == '\r'));

            if ( pszEnd <= pszStr )
                continue;
            pszEnd[1] = 0;
            pMap->m_Cfg[pszStr] = pszEq + 1;
        }
        s.close();
    }

    static config_file s_cfg;

    void init_config( int argc, char **argv )
    {
#if defined(_DEBUG) || !defined(NDEBUG)
        char const * default_cfg_file = "./test-debug.conf";
#else
        char const * default_cfg_file = "./test.conf";
#endif
        char const * cfg_file = NULL;
        for ( int i = 0; i < argc; ++i ) {
            char * arg = argv[i];
            char * eq = strchr( arg, '=' );
            if ( eq ) {
                if ( strncmp( arg, "--cfg", eq - arg ) == 0 )
                    cfg_file = eq + 1;
            }
        }

        if ( !cfg_file ) {
            // Get cfg filename from environment variable
            cfg_file = getenv( "CDSTEST_CFG" );
        }

        if ( !cfg_file || *cfg_file == 0 )
            cfg_file = default_cfg_file;

        ::testing::Test::RecordProperty( "config_file", cfg_file );
        s_cfg.load( cfg_file );
    }

    /*static*/ config const& stress_fixture::get_config( char const * slot )
    {
        return s_cfg[std::string( slot )];
    }

    /*static*/ config const& stress_fixture::get_config( std::string const& slot )
    {
        return s_cfg[ slot ];
    }

} // namespace cds_test

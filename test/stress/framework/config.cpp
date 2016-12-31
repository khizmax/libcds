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

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

#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <vector>

namespace map2 {

#define TEST_CASE(TAG, X)  void X();

    class Map_find_string: public CppUnitMini::TestCase
    {
    public:
        size_t c_nThreadCount = 8;     // thread count
        size_t c_nMapSize = 10000000;  // map size (count of searching item)
        size_t c_nPercentExists = 50;  // percent of existing keys in searching sequence
        size_t c_nPassCount = 2;
        size_t c_nMaxLoadFactor = 8;   // maximum load factor
        bool   c_bPrintGCState = true;

        size_t c_nCuckooInitialSize = 1024;// initial size for CuckooMap
        size_t c_nCuckooProbesetSize = 16; // CuckooMap probeset size (only for list-based probeset)
        size_t c_nCuckooProbesetThreshold = 0; // CUckooMap probeset threshold (o - use default)

        size_t c_nFeldmanMap_HeadBits = 10;
        size_t c_nFeldmanMap_ArrayBits = 4;

        size_t  c_nLoadFactor;  // current load factor

    private:
        typedef std::string  key_type;
        struct value_type {
            std::string const * pKey;
            bool        bExists ;   // true - key in map, false - key not in map
        };

        typedef std::vector<value_type> ValueVector;
        ValueVector             m_Arr;

        template <typename Iterator, typename Map>
        static bool check_result( Iterator const& it, Map const& map )
        {
            return it != map.end();
        }
        template <typename Map>
        static bool check_result( bool b, Map const& )
        {
            return b;
        }

        template <class Map>
        class TestThread: public CppUnitMini::TestThread
        {
            Map&     m_Map;

            virtual TestThread *    clone()
            {
                return new TestThread( *this );
            }
        public:
            struct Stat {
                size_t      nSuccess;
                size_t      nFailed;

                Stat()
                    : nSuccess(0)
                    , nFailed(0)
                {}
            };

            Stat    m_KeyExists;
            Stat    m_KeyNotExists;

        public:
            TestThread( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            TestThread( TestThread& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_find_string&  getTest()
            {
                return reinterpret_cast<Map_find_string&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                ValueVector& arr = getTest().m_Arr;
                size_t const nPassCount = getTest().c_nPassCount;

                Map& rMap = m_Map;
                for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                    if ( m_nThreadNo & 1 ) {
                        ValueVector::const_iterator itEnd = arr.end();
                        for ( ValueVector::const_iterator it = arr.begin(); it != itEnd; ++it ) {
                            auto bFound = rMap.contains( *(it->pKey) );
                            if ( it->bExists ) {
                                if ( check_result(bFound, rMap))
                                    ++m_KeyExists.nSuccess;
                                else
                                    ++m_KeyExists.nFailed;
                            }
                            else {
                                if ( check_result(bFound, rMap))
                                    ++m_KeyNotExists.nFailed;
                                else
                                    ++m_KeyNotExists.nSuccess;
                            }
                        }
                    }
                    else {
                        ValueVector::const_reverse_iterator itEnd = arr.rend();
                        for ( ValueVector::const_reverse_iterator it = arr.rbegin(); it != itEnd; ++it ) {
                            auto bFound = rMap.contains( *(it->pKey) );
                            if ( it->bExists ) {
                                if ( check_result(bFound, rMap))
                                    ++m_KeyExists.nSuccess;
                                else
                                    ++m_KeyExists.nFailed;
                            }
                            else {
                                if ( check_result( bFound, rMap ))
                                    ++m_KeyNotExists.nFailed;
                                else
                                    ++m_KeyNotExists.nSuccess;
                            }
                        }
                    }
                }
            }
        };

    public:
        Map_find_string()
            : c_nLoadFactor( 2 )
        {}

    protected:

        void generateSequence();

        template <class Map>
        void find_string_test( Map& testMap )
        {
            typedef TestThread<Map>     Thread;
            cds::OS::Timer    timer;

            CPPUNIT_MSG( "Map size=" << c_nMapSize << " find key loop=" << m_Arr.size() << " (" << c_nPercentExists << "% success)" );
            CPPUNIT_MSG( "Thread count=" << c_nThreadCount << " Pass count=" << c_nPassCount );

            // Fill the map
            CPPUNIT_MSG( "  Fill map...");
            timer.reset();
            for ( size_t i = 0; i < m_Arr.size(); ++i ) {
                // All keys in arrData are unique, insert() must be successful
                if ( m_Arr[i].bExists )
                    CPPUNIT_ASSERT( check_result( testMap.insert( *(m_Arr[i].pKey), m_Arr[i] ), testMap ));
            }
            CPPUNIT_MSG( "   Duration=" << timer.duration() );

            CPPUNIT_MSG( "  Searching...");
            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread( pool, testMap ), c_nThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            // Postcondition: the number of success searching == the number of map item
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread * pThread = static_cast<Thread *>( *it );
                CPPUNIT_CHECK( pThread->m_KeyExists.nSuccess == c_nMapSize * c_nPassCount );
                CPPUNIT_CHECK( pThread->m_KeyExists.nFailed == 0 );
                CPPUNIT_CHECK( pThread->m_KeyNotExists.nSuccess == (m_Arr.size() - c_nMapSize) * c_nPassCount );
                CPPUNIT_CHECK( pThread->m_KeyNotExists.nFailed == 0 );
            }

            check_before_cleanup( testMap );

            testMap.clear();
            additional_check( testMap );
            print_stat( testMap );
            additional_cleanup( testMap );
        }

        template <class Map>
        void run_test()
        {
            if ( Map::c_bLoadFactorDepended ) {
                for ( c_nLoadFactor = 1; c_nLoadFactor <= c_nMaxLoadFactor; c_nLoadFactor *= 2 ) {
                    CPPUNIT_MSG( "Load factor=" << c_nLoadFactor );
                    Map testMap( *this );
                    find_string_test( testMap );
                    if ( c_bPrintGCState )
                        print_gc_state();
                }
            }
            else {
                Map testMap( *this );
                find_string_test( testMap );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg );

#   include "map2/map_defs.h"
        CDSUNIT_DECLARE_MichaelMap
        CDSUNIT_DECLARE_MichaelMap_nogc
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_SplitList_nogc
        CDSUNIT_DECLARE_SkipListMap
        CDSUNIT_DECLARE_SkipListMap_nogc
        CDSUNIT_DECLARE_EllenBinTreeMap
        CDSUNIT_DECLARE_BronsonAVLTreeMap
        CDSUNIT_DECLARE_FeldmanHashMap_city
        CDSUNIT_DECLARE_StripedMap
        CDSUNIT_DECLARE_RefinableMap
        CDSUNIT_DECLARE_CuckooMap
        CDSUNIT_DECLARE_StdMap
        CDSUNIT_DECLARE_StdMap_NoLock

        CPPUNIT_TEST_SUITE(Map_find_string)
            CDSUNIT_TEST_MichaelMap
            CDSUNIT_TEST_MichaelMap_nogc
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SplitList_nogc
            CDSUNIT_TEST_SkipListMap
            CDSUNIT_TEST_SkipListMap_nogc
            CDSUNIT_TEST_EllenBinTreeMap
            CDSUNIT_TEST_BronsonAVLTreeMap
            CDSUNIT_TEST_FeldmanHashMap_city
            CDSUNIT_TEST_CuckooMap
            CDSUNIT_TEST_StripedMap
            CDSUNIT_TEST_RefinableMap
            CDSUNIT_TEST_StdMap
            CDSUNIT_TEST_StdMap_NoLock
        CPPUNIT_TEST_SUITE_END();
    };
} // namespace map2

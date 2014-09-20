//$$CDS-header$$

#include "misc/michael_allocator.h"
#include <cds/os/timer.h>
#include <cds/details/allocator.h>

#include "cppunit/cppunit_proxy.h"

namespace misc {

    static size_t s_nPassCount = 10;
    static unsigned long long s_nAllocPerPass = 1024 * 1024 * 1024;

    static size_t s_nConstructCount = 0;
    static size_t s_nDestructCount = 0;

    class Allocator_test : public CppUnitMini::TestCase
    {
        static const size_t s_nArrSizeSize = 64 * 1024;
        unsigned int    m_arrSize[s_nArrSizeSize];

        template <typename ALLOC>
        void alloc_free()
        {
            ALLOC a;

            for ( size_t nPass = 0; nPass < s_nPassCount; ++nPass ) {
                unsigned long long nTotalAllocated = 0;
                size_t nCurIdx = 0;
                while ( nTotalAllocated < s_nAllocPerPass ) {
                    size_t nSize = m_arrSize[nCurIdx] + 4;
                    char * p = a.allocate( nSize, nullptr );
                    CPPUNIT_ASSERT( p != nullptr );
                    memset( p, 0x96, nSize );
                    nTotalAllocated += nSize;
                    a.deallocate( p, 1 );
                    if ( ++nCurIdx > s_nArrSizeSize )
                        nCurIdx = 0;
                }
            }
        }

        void alloc_free_michael()
        {
            std::cout << "\n\tMichael allocator" << std::flush;
            cds::OS::Timer    timer;
            alloc_free<MichaelHeap_NoStat<char> >();
            double fDur = timer.duration();
            std::cout << "\tduration=" << fDur << std::endl;

            //cds::memory::michael_allocator::statistics st;
            //s_MichaelAlloc.get_statistics( st );
        }
        void alloc_free_std()
        {
            std::cout << "\n\tstd::allocator" << std::flush;
            cds::OS::Timer    timer;
            alloc_free<std::allocator<char> >();
            double fDur = timer.duration();
            std::cout << "\tduration=" << fDur << std::endl;
        }

        template <typename ALLOC>
        void alloc_all_free_all()
        {
            ALLOC a;

            for ( size_t nPass = 0; nPass < s_nPassCount; ++nPass ) {
                unsigned long long nTotalAllocated = 0;
                char * pHead = a.allocate( sizeof(void *), nullptr );
                CPPUNIT_ASSERT( pHead != nullptr );
                char * pCur = pHead;
                size_t nCurIdx = 0;
                while ( nTotalAllocated < s_nAllocPerPass ) {
                    size_t nSize = m_arrSize[nCurIdx] + sizeof(void *);
                    char * p = a.allocate( nSize, nullptr );
                    CPPUNIT_ASSERT( p != nullptr );
                    memset( p, 0x96, nSize );
                    *((char **) pCur) = p;
                    pCur = p;
                    nTotalAllocated += nSize;
                    if ( ++nCurIdx > s_nArrSizeSize )
                        nCurIdx = 0;
                }
                *((char **) pCur) = nullptr;

                pCur = pHead;
                while ( pCur != nullptr ) {
                    char * pNext = *((char **) pCur);
                    a.deallocate( pCur, 0 );
                    pCur = pNext;
                }
            }
        }

        void alloc_all_free_all_michael()
        {
            std::cout << "\n\tMichael allocator" << std::flush;
            cds::OS::Timer    timer;
            alloc_all_free_all<MichaelHeap_NoStat<char> >();
            double fDur = timer.duration();
            std::cout << "\tduration=" << fDur << std::endl;

            //cds::memory::michael_allocator::statistics st;
            //s_MichaelAlloc.get_statistics( st );
        }
        void alloc_all_free_all_std()
        {
            std::cout << "\n\tstd::allocator" << std::flush;
            cds::OS::Timer    timer;
            alloc_all_free_all<std::allocator<char> >();
            double fDur = timer.duration();
            std::cout << "\tduration=" << fDur << std::endl;
        }

        struct SimpleStruct
        {
            int     n;

            SimpleStruct()
            {
                ++s_nConstructCount;
            }

            ~SimpleStruct()
            {
                ++s_nDestructCount;
            }
        };

        void test_array()
        {
            size_t const nArraySize = 10;

            SimpleStruct * pArr;
            cds::details::Allocator<SimpleStruct>  a;
            pArr = a.NewArray( nArraySize );
            a.Delete( pArr, nArraySize );

            CPPUNIT_ASSERT( s_nConstructCount == nArraySize );
            CPPUNIT_ASSERT( s_nConstructCount == s_nDestructCount );
        }


        void setUpParams( const CppUnitMini::TestCfg& cfg )
        {
            s_nPassCount = cfg.getULong( "PassCount", 10 );
            s_nAllocPerPass = cfg.getULong( "AllocPerPass", 1024 ) * 1024 * 1024;
        }

    public:
        Allocator_test()
        {
            CPPUNIT_ASSERT( s_nArrSizeSize == sizeof(m_arrSize) / sizeof(m_arrSize[0]) );
            for ( size_t i = 0; i < s_nArrSizeSize; ++i )
                m_arrSize[i] = rand();
        }

        CPPUNIT_TEST_SUITE(Allocator_test);
            CPPUNIT_TEST(test_array)
            CPPUNIT_TEST(alloc_free_michael)
            CPPUNIT_TEST(alloc_free_std)
            CPPUNIT_TEST(alloc_all_free_all_michael)
            CPPUNIT_TEST(alloc_all_free_all_std)
        CPPUNIT_TEST_SUITE_END();
    };
}   // namespace memory
CPPUNIT_TEST_SUITE_REGISTRATION( misc::Allocator_test );

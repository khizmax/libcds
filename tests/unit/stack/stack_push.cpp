//$$CDS-header$$

#include "cppunit/thread.h"
#include "stack/stack_type.h"

// Multi-threaded stack test for push operation
namespace stack {

#define TEST_CASE( Q ) void Q()          { test_unbounded< Types<SimpleValue>::Q >(); }
#define TEST_ELIMINATION( Q ) void Q()   { test_elimination< Types<SimpleValue>::Q >(); }
#define TEST_BOUNDED( Q ) void Q()       { test_bounded< Types<SimpleValue>::Q >(); }

    namespace {
        static size_t s_nThreadCount = 8;
        static size_t s_nStackSize = 10000000;
        static size_t s_nEliminationSize = 4;

        struct SimpleValue {
            size_t      nNo;
            size_t      nThread;

            SimpleValue(): nNo(0), nThread(0) {}
            SimpleValue( size_t n ): nNo(n), nThread(0) {}
            size_t getNo() const { return  nNo; }
        };
    }

    class Stack_Push: public CppUnitMini::TestCase
    {
        template <class Stack>
        class Thread: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Thread( *this );
            }
        public:
            Stack&              m_Stack;
            double              m_fTime;
            size_t              m_nStartItem;
            size_t              m_nEndItem;
            size_t              m_nPushError;

        public:
            Thread( CppUnitMini::ThreadPool& pool, Stack& s )
                : CppUnitMini::TestThread( pool )
                , m_Stack( s )
            {}
            Thread( Thread& src )
                : CppUnitMini::TestThread( src )
                , m_Stack( src.m_Stack )
            {}

            Stack_Push&  getTest()
            {
                return reinterpret_cast<Stack_Push&>( m_Pool.m_Test );
            }

            virtual void init()
            {
                cds::threading::Manager::attachThread();
            }
            virtual void fini()
            {
                cds::threading::Manager::detachThread();
            }

            virtual void test()
            {
                m_fTime = m_Timer.duration();

                m_nPushError = 0;
                SimpleValue v;
                v.nThread = m_nThreadNo;
                for ( v.nNo = m_nStartItem; v.nNo < m_nEndItem; ++v.nNo ) {
                    if ( !m_Stack.push( v ))
                        ++m_nPushError;
                }

                m_fTime = m_Timer.duration() - m_fTime;
            }
        };

    protected:
        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nThreadCount = cfg.getULong("ThreadCount", 8 );
            s_nStackSize = cfg.getULong("StackSize", 10000000 );
            s_nEliminationSize = cfg.getULong("EliminationSize", 4 );
        }

        template <class Stack>
        void analyze( CppUnitMini::ThreadPool& pool, Stack& testStack  )
        {
            size_t nThreadItems = s_nStackSize / s_nThreadCount;
            std::vector<size_t> aThread;
            aThread.resize(s_nThreadCount);

            double fTime = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread<Stack> * pThread = reinterpret_cast<Thread<Stack> *>(*it);
                fTime += pThread->m_fTime;
                if ( pThread->m_nPushError != 0 )
                    CPPUNIT_MSG("     ERROR: thread push error count=" << pThread->m_nPushError );
                aThread[ pThread->m_nThreadNo] = pThread->m_nEndItem - 1;
            }
            CPPUNIT_MSG( "     Duration=" << (fTime / s_nThreadCount) );
            CPPUNIT_ASSERT( !testStack.empty() )

            size_t * arr = new size_t[ s_nStackSize ];
            memset(arr, 0, sizeof(arr[0]) * s_nStackSize );

            cds::OS::Timer      timer;
            CPPUNIT_MSG( "   Pop (single-threaded)..." );
            size_t nPopped = 0;
            SimpleValue val;
            while ( testStack.pop( val )) {
                nPopped++;
                ++arr[ val.getNo() ];
                CPPUNIT_ASSERT( val.nThread < s_nThreadCount);
                CPPUNIT_ASSERT( aThread[val.nThread] == val.nNo );
                aThread[val.nThread]--;
            }
            CPPUNIT_MSG( "     Duration=" << timer.duration() );

            size_t nTotalItems = nThreadItems * s_nThreadCount;
            size_t nError = 0;
            for ( size_t i = 0; i < nTotalItems; ++i ) {
                if ( arr[i] != 1 ) {
                    CPPUNIT_MSG( "   ERROR: Item " << i << " has not been pushed" );
                    CPPUNIT_ASSERT( ++nError > 10 );
                }
            }

            delete [] arr;
        }

        // Unbounded stack test
        template <class Stack>
        void test_unbounded()
        {
            Stack testStack;
            test( testStack );
        }

        // Unbounded elimination stack test
        template <class Stack>
        void test_elimination()
        {
            Stack testStack( s_nEliminationSize );
            test( testStack );
            check_elimination_stat( testStack.statistics() );
        }
        void check_elimination_stat( cds::container::treiber_stack::empty_stat const& )
        {}
        void check_elimination_stat( cds::container::treiber_stack::stat<> const& s )
        {
            CPPUNIT_CHECK( s.m_PushCount.get() == s.m_PopCount.get() );
        }

        // Bounded stack test
        template <class Stack>
        void test_bounded()
        {
            Stack testStack( s_nStackSize );
            test( testStack );
        }

        template <class Stack>
        void test( Stack& testStack )
        {
            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread<Stack>( pool, testStack ), s_nThreadCount );

            size_t nStart = 0;
            size_t nThreadItemCount = s_nStackSize / s_nThreadCount;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread<Stack> * pThread = reinterpret_cast<Thread<Stack> *>(*it);
                pThread->m_nStartItem = nStart;
                nStart += nThreadItemCount;
                pThread->m_nEndItem = nStart;
            }

            CPPUNIT_MSG( "   Push test, thread count=" << s_nThreadCount
                << " items=" << (nThreadItemCount * s_nThreadCount)
                << "...");
            pool.run();

            analyze( pool, testStack );
            CPPUNIT_MSG( testStack.statistics() );
        }

    protected:
#   include "stack/stack_defs.h"
        CDSUNIT_DECLARE_TreiberStack
        CDSUNIT_DECLARE_EliminationStack
        CDSUNIT_DECLARE_FCStack
        CDSUNIT_DECLARE_FCDeque
        CDSUNIT_DECLARE_StdStack

        CPPUNIT_TEST_SUITE(Stack_Push)
            CDSUNIT_TEST_TreiberStack
            CDSUNIT_TEST_EliminationStack
            CDSUNIT_TEST_FCStack
            CDSUNIT_TEST_FCDeque
            CDSUNIT_TEST_StdStack
        CPPUNIT_TEST_SUITE_END();
    };
} // namespace stack

CPPUNIT_TEST_SUITE_REGISTRATION(stack::Stack_Push);

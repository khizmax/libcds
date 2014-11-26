//$$CDS-header$$

#include "cppunit/thread.h"
#include "stack/stack_type.h"

// Multi-threaded stack test for push/pop operation
namespace stack {

#define TEST_CASE( Q ) void Q()          { test_unbounded< Types<SimpleValue>::Q >(); }
#define TEST_ELIMINATION( Q ) void Q()   { test_elimination< Types<SimpleValue>::Q >(); }
#define TEST_BOUNDED( Q ) void Q()       { test_bounded< Types<SimpleValue>::Q >(); }

    namespace {
        static size_t s_nPushThreadCount = 4;
        static size_t s_nPopThreadCount = 4;
        static size_t s_nStackSize = 1000000;
        static size_t s_nEliminationSize = 4;

        struct SimpleValue {
            size_t      nNo;
            size_t      nThread;

            SimpleValue(): nNo(0), nThread(0) {}
            SimpleValue( size_t n ): nNo(n), nThread(0) {}
            size_t getNo() const { return  nNo; }
        };
    }

    class Stack_PushPop: public CppUnitMini::TestCase
    {
        atomics::atomic<size_t>  m_nWorkingProducers;
        static size_t const c_nValArraySize = 1024;

        template <class Stack>
        class Pusher: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Pusher( *this );
            }
        public:
            Stack&              m_Stack;
            size_t              m_nItemCount;
            size_t              m_nPushError;
            size_t              m_arrPush[c_nValArraySize];

        public:
            Pusher( CppUnitMini::ThreadPool& pool, Stack& s )
                : CppUnitMini::TestThread( pool )
                , m_Stack( s )
            {}
            Pusher( Pusher& src )
                : CppUnitMini::TestThread( src )
                , m_Stack( src.m_Stack )
            {}

            Stack_PushPop&  getTest()
            {
                return reinterpret_cast<Stack_PushPop&>( m_Pool.m_Test );
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
                m_nPushError = 0;
                memset( m_arrPush, 0, sizeof(m_arrPush));

                SimpleValue v;
                v.nThread = m_nThreadNo;
                for ( size_t i = 0; i < m_nItemCount; ++i ) {
                    v.nNo = i % c_nValArraySize;
                    if ( m_Stack.push( v ))
                        ++m_arrPush[v.nNo];
                    else
                        ++m_nPushError;
                }


                getTest().m_nWorkingProducers.fetch_sub(1, atomics::memory_order_release);
            }
        };

        template <class Stack>
        class Popper: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Popper( *this );
            }
        public:
            Stack&              m_Stack;
            size_t              m_nPopCount;
            size_t              m_nPopEmpty;
            size_t              m_arrPop[c_nValArraySize];
            size_t              m_nDirtyPop;
        public:
            Popper( CppUnitMini::ThreadPool& pool, Stack& s )
                : CppUnitMini::TestThread( pool )
                , m_Stack( s )
            {}
            Popper( Popper& src )
                : CppUnitMini::TestThread( src )
                , m_Stack( src.m_Stack )
            {}

            Stack_PushPop&  getTest()
            {
                return reinterpret_cast<Stack_PushPop&>( m_Pool.m_Test );
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
                m_nPopEmpty = 0;
                m_nPopCount = 0;
                m_nDirtyPop = 0;
                memset( m_arrPop, 0, sizeof(m_arrPop));

                SimpleValue v;
                while ( !(getTest().m_nWorkingProducers.load(atomics::memory_order_acquire) == 0 && m_Stack.empty()) ) {
                    if ( m_Stack.pop( v )) {
                        ++m_nPopCount;
                        if ( v.nNo < sizeof(m_arrPop)/sizeof(m_arrPop[0]) )
                            ++m_arrPop[v.nNo];
                        else
                            ++m_nDirtyPop;
                    }
                    else
                        ++m_nPopEmpty;
                }
            }
        };

    protected:
        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nPushThreadCount = cfg.getULong("PushThreadCount", 4 );
            s_nPopThreadCount = cfg.getULong("PopThreadCount", 4 );
            s_nStackSize = cfg.getULong("StackSize", 1000000 );
            s_nEliminationSize = cfg.getULong("EliminationSize", 4 );
        }

        template <class Stack>
        void analyze( CppUnitMini::ThreadPool& pool, Stack& /*testStack*/  )
        {
            size_t nPushError = 0;
            size_t nPopEmpty = 0;
            size_t nPopCount = 0;
            size_t arrVal[c_nValArraySize];
            memset( arrVal, 0, sizeof(arrVal));
            size_t nDirtyPop = 0;

            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                CppUnitMini::TestThread * pThread = *it;
                Pusher<Stack> * pPusher = dynamic_cast< Pusher<Stack> *>( pThread );
                if ( pPusher ) {
                    nPushError += pPusher->m_nPushError;
                    for ( size_t i = 0; i < sizeof(arrVal)/sizeof(arrVal[0]); ++i )
                        arrVal[i] += pPusher->m_arrPush[i];
                }
                else {
                    Popper<Stack> * pPopper = dynamic_cast<Popper<Stack> *>( pThread );
                    assert( pPopper );
                    nPopEmpty += pPopper->m_nPopEmpty;
                    nPopCount += pPopper->m_nPopCount;
                    nDirtyPop += pPopper->m_nDirtyPop;
                    for ( size_t i = 0; i < sizeof(arrVal)/sizeof(arrVal[0]); ++i )
                        arrVal[i] -= pPopper->m_arrPop[i];
                }
            }

            CPPUNIT_MSG( "Push count=" << s_nStackSize
                << " push error=" << nPushError
                << " pop count=" << nPopCount
                << " pop empty=" << nPopEmpty
                << " dirty pop=" << nDirtyPop
                );
            CPPUNIT_CHECK( nPopCount == s_nStackSize );
            CPPUNIT_CHECK( nDirtyPop == 0 );
            for ( size_t i = 0; i < sizeof(arrVal)/sizeof(arrVal[0]); ++i ) {
                CPPUNIT_CHECK_EX( arrVal[i] == 0, "arrVal[" << i << "]=" << long(arrVal[i]) );
            }
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
            CPPUNIT_CHECK( s.m_PushCount.get() + s.m_ActivePushCollision.get() + s.m_PassivePushCollision.get() == s_nStackSize );
            CPPUNIT_CHECK( s.m_PopCount.get() + s.m_ActivePopCollision.get() + s.m_PassivePopCollision.get() == s_nStackSize );
            CPPUNIT_CHECK( s.m_PushCount.get() == s.m_PopCount.get() );
            CPPUNIT_CHECK( s.m_ActivePopCollision.get() == s.m_PassivePushCollision.get() );
            CPPUNIT_CHECK( s.m_ActivePushCollision.get() == s.m_PassivePopCollision.get() );
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
            m_nWorkingProducers.store(s_nPushThreadCount, atomics::memory_order_release);
            size_t const nPushCount = s_nStackSize / s_nPushThreadCount;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Pusher<Stack>( pool, testStack ), s_nPushThreadCount );
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it )
                static_cast<Pusher<Stack>* >( *it )->m_nItemCount = nPushCount;
            pool.add( new Popper<Stack>( pool, testStack ), s_nPopThreadCount );

            CPPUNIT_MSG( "   Push/Pop test, push thread count=" << s_nPushThreadCount
                << " pop thread count=" << s_nPopThreadCount
                << " items=" << (nPushCount * s_nPushThreadCount)
                << "...");
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            s_nStackSize = nPushCount * s_nPushThreadCount;
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

        CPPUNIT_TEST_SUITE(Stack_PushPop)
            CDSUNIT_TEST_TreiberStack
            CDSUNIT_TEST_EliminationStack
            CDSUNIT_TEST_FCStack
            CDSUNIT_TEST_FCDeque
            CDSUNIT_TEST_StdStack
        CPPUNIT_TEST_SUITE_END();
    };
} // namespace stack

CPPUNIT_TEST_SUITE_REGISTRATION(stack::Stack_PushPop);

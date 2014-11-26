//$$CDS-header$$

#include "cppunit/thread.h"
#include "stack/intrusive_stack_type.h"

// Multi-threaded stack test for push/pop operation
namespace istack {

#define TEST_CASE( Q, HOOK ) void Q()          { test_unbounded< Types<SimpleValue<HOOK> >::Q >();   }
#define TEST_ELIMINATION( Q, HOOK ) void Q()   { test_elimination< Types<SimpleValue<HOOK> >::Q >(); }
#define TEST_BOUNDED( Q, HOOK ) void Q()       { test_bounded< Types<SimpleValue<HOOK> >::Q >();     }
#define TEST_FCSTACK( Q, HOOK ) void Q()       { test_fcstack< Types<SimpleValue<HOOK> >::Q >();     }
#define TEST_STDSTACK( Q ) void Q()            { test_stdstack< Types<SimpleValue<> >::Q >();       }

    namespace {
        static size_t s_nPushThreadCount = 4;
        static size_t s_nPopThreadCount = 4;
        static size_t s_nStackSize = 10000000;
        static size_t s_nEliminationSize = 4;
        static bool s_bFCIterative = false;
        static unsigned int s_nFCCombinePassCount = 64;
        static unsigned int s_nFCCompactFactor = 1024;

        struct empty {};

        template <typename Base = empty >
        struct SimpleValue: public Base
        {
            size_t      nNo;
            size_t      nProducer;
            size_t      nConsumer;

            SimpleValue() {}
            SimpleValue( size_t n ): nNo(n) {}
            size_t getNo() const { return  nNo; }
        };

    } // namespace

    class IntrusiveStack_PushPop: public CppUnitMini::TestCase
    {
        atomics::atomic<size_t>  m_nWorkingProducers;
        static CDS_CONSTEXPR const size_t c_nValArraySize = 1024;
        static CDS_CONSTEXPR const size_t c_nBadConsumer = 0xbadc0ffe;

        template <class Stack>
        class Producer: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Producer( *this );
            }
        public:
            Stack&              m_Stack;
            size_t              m_nPushError;
            size_t              m_arrPush[c_nValArraySize];

            // Interval in m_arrValue
            typename Stack::value_type *       m_pStart;
            typename Stack::value_type *       m_pEnd;

        public:
            Producer( CppUnitMini::ThreadPool& pool, Stack& s )
                : CppUnitMini::TestThread( pool )
                , m_Stack( s )
            {}
            Producer( Producer& src )
                : CppUnitMini::TestThread( src )
                , m_Stack( src.m_Stack )
            {}

            IntrusiveStack_PushPop&  getTest()
            {
                return static_cast<IntrusiveStack_PushPop&>( m_Pool.m_Test );
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

                size_t i = 0;
                for ( typename Stack::value_type * p = m_pStart; p < m_pEnd; ++p, ++i ) {
                    p->nProducer = m_nThreadNo;
                    p->nNo = i % c_nValArraySize;
                    if ( m_Stack.push( *p ))
                        ++m_arrPush[ p->nNo ];
                    else
                        ++m_nPushError;
                }

                getTest().m_nWorkingProducers.fetch_sub( 1, atomics::memory_order_release );
            }
        };

        template <class Stack>
        class Consumer: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Consumer( *this );
            }
        public:
            Stack&              m_Stack;
            size_t              m_nPopCount;
            size_t              m_nPopEmpty;
            size_t              m_arrPop[c_nValArraySize];
            size_t              m_nDirtyPop;
        public:
            Consumer( CppUnitMini::ThreadPool& pool, Stack& s )
                : CppUnitMini::TestThread( pool )
                , m_Stack( s )
            {}
            Consumer( Consumer& src )
                : CppUnitMini::TestThread( src )
                , m_Stack( src.m_Stack )
            {}

            IntrusiveStack_PushPop&  getTest()
            {
                return static_cast<IntrusiveStack_PushPop&>( m_Pool.m_Test );
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

                while ( !(getTest().m_nWorkingProducers.load(atomics::memory_order_acquire) == 0 && m_Stack.empty()) ) {
                    typename Stack::value_type * p = m_Stack.pop();
                    if ( p ) {
                        p->nConsumer = m_nThreadNo;
                        ++m_nPopCount;
                        if ( p->nNo < sizeof(m_arrPop)/sizeof(m_arrPop[0]) )
                            ++m_arrPop[p->nNo];
                        else
                            ++m_nDirtyPop;
                    }
                    else
                        ++m_nPopEmpty;
                }
            }
        };

        template <typename T>
        class value_array
        {
            T * m_pArr;
        public:
            value_array( size_t nSize )
                : m_pArr( new T[nSize] )
            {}

            ~value_array()
            {
                delete [] m_pArr;
            }

            T * get() const { return m_pArr; }
        };

    protected:
        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nPushThreadCount = cfg.getULong("PushThreadCount", 4 );
            s_nPopThreadCount = cfg.getULong("PopThreadCount", 4 );
            s_nStackSize = cfg.getULong("StackSize", 1000000 );
            s_nEliminationSize = cfg.getULong("EliminationSize", 4 );
            s_bFCIterative = cfg.getBool( "FCIterate", false );
            s_nFCCombinePassCount = cfg.getUInt( "FCCombinePassCount", 64 );
            s_nFCCompactFactor = cfg.getUInt( "FCCompactFactor", 1024 );
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
                Producer<Stack> * pPusher = dynamic_cast< Producer<Stack> *>( pThread );
                if ( pPusher ) {
                    nPushError += pPusher->m_nPushError;
                    for ( size_t i = 0; i < sizeof(arrVal)/sizeof(arrVal[0]); ++i )
                        arrVal[i] += pPusher->m_arrPush[i];
                }
                else {
                    Consumer<Stack> * pPopper = dynamic_cast<Consumer<Stack> *>( pThread );
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

        template <class Stack>
        void test( Stack& testStack, value_array<typename Stack::value_type>& arrValue )
        {
            m_nWorkingProducers.store( s_nPushThreadCount, atomics::memory_order_release );
            size_t const nPushCount = s_nStackSize / s_nPushThreadCount;

            typename Stack::value_type * pValStart = arrValue.get();
            typename Stack::value_type * pValEnd = pValStart + s_nStackSize;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Producer<Stack>( pool, testStack ), s_nPushThreadCount );
            {
                for ( typename Stack::value_type * it = pValStart; it != pValEnd; ++it )
                    it->nConsumer = c_nBadConsumer;

                typename Stack::value_type * pStart = pValStart;
                for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                    static_cast<Producer<Stack>* >( *it )->m_pStart = pStart;
                    pStart += nPushCount;
                    static_cast<Producer<Stack>* >( *it )->m_pEnd = pStart;
                }
            }
            pool.add( new Consumer<Stack>( pool, testStack ), s_nPopThreadCount );

            CPPUNIT_MSG( "   Push/Pop test, push thread count=" << s_nPushThreadCount
                << " pop thread count=" << s_nPopThreadCount
                << " items=" << (nPushCount * s_nPushThreadCount)
                << "...");
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            s_nStackSize = nPushCount * s_nPushThreadCount;

            {
                typename Stack::value_type * pEnd = pValStart + s_nStackSize;
                for ( typename Stack::value_type * it = pValStart; it != pEnd; ++it  )
                    CPPUNIT_CHECK( it->nConsumer != c_nBadConsumer );
            }

            analyze( pool, testStack );
            CPPUNIT_MSG( testStack.statistics() );
        }

    public:
        // Unbounded stack test
        template <class Stack>
        void test_unbounded()
        {
            value_array<typename Stack::value_type> arrValue( s_nStackSize );
            {
                Stack testStack;
                test( testStack, arrValue );
            }
            Stack::gc::force_dispose();
        }

        template <class Stack>
        void test_fcstack()
        {
            if ( s_bFCIterative ) {
                for (unsigned int nCompactFactor = 1; nCompactFactor <= s_nFCCompactFactor; nCompactFactor *= 2 ) {
                    for ( unsigned int nPass = 1; nPass <= s_nFCCombinePassCount; nPass *= 2 ) {
                        CPPUNIT_MSG( "Compact factor=" << nCompactFactor << ", combine pass count=" << nPass );
                        value_array<typename Stack::value_type> arrValue( s_nStackSize );
                        Stack testStack( nCompactFactor, nPass );
                        test( testStack, arrValue );
                    }
                }
            }
            else {
                if ( s_nFCCompactFactor && s_nFCCombinePassCount ) {
                    CPPUNIT_MSG( "Compact factor=" << s_nFCCompactFactor << ", combine pass count=" << s_nFCCombinePassCount );
                    value_array<typename Stack::value_type> arrValue( s_nStackSize );
                    Stack testStack( s_nFCCompactFactor, s_nFCCombinePassCount );
                    test( testStack, arrValue );
                }
                else {
                    value_array<typename Stack::value_type> arrValue( s_nStackSize );
                    Stack testStack;
                    test( testStack, arrValue );
                }
            }
        }

        // Unbounded elimination stack test
        template <class Stack>
        void test_elimination()
        {
            value_array<typename Stack::value_type> arrValue( s_nStackSize );
            {
                Stack testStack( s_nEliminationSize );
                test( testStack, arrValue );
                check_elimination_stat( testStack.statistics() );
            }
            Stack::gc::force_dispose();
        }
        void check_elimination_stat( cds::intrusive::treiber_stack::empty_stat const& )
        {}
        void check_elimination_stat( cds::intrusive::treiber_stack::stat<> const& s )
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
            value_array<typename Stack::value_type> arrValue( s_nStackSize );
            Stack testStack( s_nStackSize );
            test( testStack, arrValue );
        }

        template <class Stack>
        void test_stdstack()
        {
            value_array<typename Stack::value_type> arrValue( s_nStackSize );
            Stack testStack;
            test( testStack, arrValue );
        }

    protected:
#   include "stack/intrusive_stack_defs.h"
        CDSUNIT_DECLARE_TreiberStack
        CDSUNIT_DECLARE_EliminationStack
        CDSUNIT_DECLARE_FCStack
        CDSUNIT_DECLARE_StdStack

        CPPUNIT_TEST_SUITE(IntrusiveStack_PushPop)
            CDSUNIT_TEST_TreiberStack
            CDSUNIT_TEST_EliminationStack
            CDSUNIT_TEST_FCStack
            CDSUNIT_TEST_StdStack
        CPPUNIT_TEST_SUITE_END();
    };
} // namespace istack

CPPUNIT_TEST_SUITE_REGISTRATION(istack::IntrusiveStack_PushPop);

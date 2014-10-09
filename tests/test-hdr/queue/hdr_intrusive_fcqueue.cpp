//$$CDS-header$$

#include "cppunit/cppunit_proxy.h"
#include <cds/intrusive/fcqueue.h>

namespace queue {

    class TestIntrusiveFCQueue: public CppUnitMini::TestCase
    {
        template <typename Hook>
        struct base_hook_item: public Hook
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount(0)
            {}
        };

        template <typename Hook>
        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            Hook hMember;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };

        struct disposer
        {
            template <typename T>
            void operator ()( T * p )
            {
                ++p->nDisposeCount;
            }
        };

        template <typename Queue>
        void test()
        {
            Queue q;
            test_with(q);
        }

        template <class Queue>
        void test_with( Queue& q )
        {
            typedef typename Queue::value_type value_type;
            value_type v1, v2, v3;
            CPPUNIT_ASSERT( q.empty() );

            v1.nVal = 1;
            v2.nVal = 2;
            v3.nVal = 3;
            CPPUNIT_ASSERT( q.push(v1));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.push(v2));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.push(v3));
            CPPUNIT_ASSERT( !q.empty() );

            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            value_type * pv;
            pv = q.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 1 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            pv = q.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 2 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            pv = q.dequeue();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 3 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            pv = q.dequeue();
            CPPUNIT_ASSERT( pv == nullptr );
            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            CPPUNIT_ASSERT( q.enqueue(v1));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.enqueue(v2));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.enqueue(v3));
            CPPUNIT_ASSERT( !q.empty() );

            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );
            q.clear( true );    // call disposer
            CPPUNIT_CHECK( q.empty() );
            CPPUNIT_CHECK( v1.nDisposeCount == 1 );
            CPPUNIT_CHECK( v2.nDisposeCount == 1 );
            CPPUNIT_CHECK( v3.nDisposeCount == 1 );

            CPPUNIT_ASSERT( q.push(v1));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.push(v2));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.push(v3));
            CPPUNIT_ASSERT( !q.empty() );
            q.clear( false );   // no disposer called
            CPPUNIT_CHECK( q.empty() );
            CPPUNIT_CHECK( v1.nDisposeCount == 1 );
            CPPUNIT_CHECK( v2.nDisposeCount == 1 );
            CPPUNIT_CHECK( v3.nDisposeCount == 1 );
        }

        void FCQueue_base();
        void FCQueue_base_stat();
        void FCQueue_base_elimination();
        void FCQueue_base_elimination_mutex();
        void FCQueue_base_seqcst();
        void FCQueue_member();
        void FCQueue_member_stat();
        void FCQueue_member_elimination();
        void FCQueue_member_elimination_mutex();
        void FCQueue_member_seqcst();

        CPPUNIT_TEST_SUITE(TestIntrusiveFCQueue)
            CPPUNIT_TEST(FCQueue_base)
            CPPUNIT_TEST(FCQueue_base_stat)
            CPPUNIT_TEST(FCQueue_base_elimination)
            CPPUNIT_TEST(FCQueue_base_elimination_mutex)
            CPPUNIT_TEST(FCQueue_base_seqcst)
            CPPUNIT_TEST(FCQueue_member)
            CPPUNIT_TEST(FCQueue_member_stat)
            CPPUNIT_TEST(FCQueue_member_elimination)
            CPPUNIT_TEST(FCQueue_member_elimination_mutex)
            CPPUNIT_TEST(FCQueue_member_seqcst)
        CPPUNIT_TEST_SUITE_END()
    };


    void TestIntrusiveFCQueue::FCQueue_base()
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void TestIntrusiveFCQueue::FCQueue_base_stat()
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        struct queue_traits : public cds::intrusive::fcqueue::traits
        {
            typedef TestIntrusiveFCQueue::disposer disposer;
            typedef cds::intrusive::fcqueue::stat<> stat;
        };
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >, queue_traits > queue_type;

        test<queue_type>();
    }

    void TestIntrusiveFCQueue::FCQueue_base_elimination()
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        struct queue_traits : public
            cds::intrusive::fcqueue::make_traits <
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::enable_elimination < true >
            > ::type
        {};
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >, queue_traits > queue_type;

        test<queue_type>();
    }

    void TestIntrusiveFCQueue::FCQueue_base_elimination_mutex()
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::enable_elimination< true >
                ,cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void TestIntrusiveFCQueue::FCQueue_base_seqcst()
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void TestIntrusiveFCQueue::FCQueue_member()
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void TestIntrusiveFCQueue::FCQueue_member_stat()
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::stat< cds::intrusive::fcqueue::stat<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void TestIntrusiveFCQueue::FCQueue_member_elimination()
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::enable_elimination< true >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void TestIntrusiveFCQueue::FCQueue_member_elimination_mutex()
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::enable_elimination< true >
                ,cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void TestIntrusiveFCQueue::FCQueue_member_seqcst()
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent >
            >::type
        > queue_type;

        test<queue_type>();
    }

} // namespace queue

CPPUNIT_TEST_SUITE_REGISTRATION(queue::TestIntrusiveFCQueue);

// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_THREAD_H
#define CDSTEST_THREAD_H

#include <cds_test/ext_gtest.h>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <cds/threading/model.h>

namespace cds_test {

    // Forwards
    class thread;
    class thread_pool;

    // Test thread
    class thread
    {
        void run();

    protected: // thread_pool interface
        thread( thread const& sample );

        virtual ~thread()
        {}

    protected:
        virtual thread * clone() = 0;
        virtual void test() = 0;

        virtual void SetUp()
        {
            cds::threading::Manager::attachThread();
        }

        virtual void TearDown()
        {
            cds::threading::Manager::detachThread();
        }

    public:
        explicit thread( thread_pool& master, int type = 0 );

        thread_pool& pool() { return m_pool; }
        int type() const { return m_type; }
        size_t id() const { return m_id;  }
        bool time_elapsed() const;

    private:
        friend class thread_pool;

        thread_pool&    m_pool;
        int const       m_type;
        size_t const    m_id;
    };

    // Pool of test threads
    class thread_pool
    {
        class barrier
        {
        public:
            barrier()
                : m_count( 0 )
            {}

            void reset( size_t count )
            {
                std::unique_lock< std::mutex > lock( m_mtx );
                m_count = count;
            }

            bool wait()
            {
                std::unique_lock< std::mutex > lock( m_mtx );
                if ( --m_count == 0 ) {
                    m_cv.notify_all();
                    return true;
                }

                while ( m_count != 0 )
                    m_cv.wait( lock );

                return false;
            }

        private:
            size_t      m_count;
            std::mutex  m_mtx;
            std::condition_variable m_cv;
        };

        class initial_gate
        {
        public:
            initial_gate()
                : m_ready( false )
            {}

            void wait()
            {
                std::unique_lock< std::mutex > lock( m_mtx );
                while ( !m_ready )
                    m_cv.wait( lock );
            }

            void ready()
            {
                std::unique_lock< std::mutex > lock( m_mtx );
                m_ready = true;
                m_cv.notify_all();
            }

            void reset()
            {
                std::unique_lock< std::mutex > lock( m_mtx );
                m_ready = false;
            }

        private:
            std::mutex  m_mtx;
            std::condition_variable m_cv;
            bool        m_ready;
        };

    public:
        explicit thread_pool( ::testing::Test& fx )
            : m_fixture( fx )
            , m_bTimeElapsed( false )
        {}

        ~thread_pool()
        {
            clear();
        }

        void add( thread * what )
        {
            m_workers.push_back( what );
        }

        void add( thread * what, size_t count )
        {
            add( what );
            for ( size_t i = 1; i < count; ++i ) {
                thread * p = what->clone();
                add( p );
            }
        }

        std::chrono::milliseconds run()
        {
            return run( std::chrono::seconds::zero());
        }

        std::chrono::milliseconds run( std::chrono::seconds duration )
        {
            m_startBarrier.reset( m_workers.size() + 1 );
            m_stopBarrier.reset( m_workers.size() + 1 );

            // Create threads
            std::vector< std::thread > threads;
            threads.reserve( m_workers.size());
            for ( auto w : m_workers )
                threads.emplace_back( &thread::run, w );

            // The pool is intialized
            m_startPoint.ready();

            m_bTimeElapsed.store( false, std::memory_order_release );

            auto native_duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration);

            // The pool is ready to start all workers
            m_startBarrier.wait();

            auto time_start = std::chrono::steady_clock::now();
            auto const expected_end = time_start + native_duration;

            if ( duration != std::chrono::seconds::zero()) {
                for ( ;; ) {
                    std::this_thread::sleep_for( native_duration );
                    auto time_now = std::chrono::steady_clock::now();
                    if ( time_now >= expected_end )
                        break;
                    native_duration = expected_end - time_now;
                }
            }
            m_bTimeElapsed.store( true, std::memory_order_release );

            // Waiting for all workers done
            m_stopBarrier.wait();

            auto time_end = std::chrono::steady_clock::now();

            for ( auto& t : threads )
                t.join();

            return m_testDuration = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
        }

        size_t size() const             { return m_workers.size(); }
        thread& get( size_t idx ) const { return *m_workers.at( idx ); }

        template <typename Fixture>
        Fixture& fixture()
        {
            return static_cast<Fixture&>(m_fixture);
        }

        std::chrono::milliseconds duration() const { return m_testDuration; }

        void clear()
        {
            for ( auto t : m_workers )
                delete t;
            m_workers.clear();
            m_startPoint.reset();
        }

        void reset()
        {
            clear();
        }

    protected: // thread interface
        size_t get_next_id()
        {
            return m_workers.size();
        }

        void ready_to_start( thread& /*who*/ )
        {
            // Called from test thread

            // Wait until the pool is ready
            m_startPoint.wait();

            // Wait until all thread ready
            m_startBarrier.wait();
        }

        void thread_done( thread& /*who*/ )
        {
            // Called from test thread
            m_stopBarrier.wait();
        }

    private:
        friend class thread;

        ::testing::Test&        m_fixture;
        std::vector<thread *>   m_workers;

        initial_gate            m_startPoint;
        barrier                 m_startBarrier;
        barrier                 m_stopBarrier;

        std::atomic<bool> m_bTimeElapsed;
        std::chrono::milliseconds m_testDuration;
    };

    inline thread::thread( thread_pool& master, int type /*= 0*/ )
        : m_pool( master )
        , m_type( type )
        , m_id( master.get_next_id())
    {}

    inline thread::thread( thread const& sample )
        : m_pool( sample.m_pool )
        , m_type( sample.m_type )
        , m_id( m_pool.get_next_id())
    {}

    inline void thread::run()
    {
        SetUp();
        m_pool.ready_to_start( *this );
        test();
        m_pool.thread_done( *this );
        TearDown();
    }

    inline bool thread::time_elapsed() const
    {
        return m_pool.m_bTimeElapsed.load( std::memory_order_acquire );
    }

} // namespace cds_test

#endif // CDSTEST_THREAD_H

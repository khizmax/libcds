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

#ifndef CDSTEST_THREAD_H
#define CDSTEST_THREAD_H

#include <gtest/gtest.h>
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

        void join()
        {
            m_impl.join();
        }

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
        int             m_type;
        size_t          m_id;
        std::thread     m_impl;
    };

    // Pool of test threads
    class thread_pool
    {
    public:
        explicit thread_pool( ::testing::Test& fixture )
            : m_fixture( fixture )
            , m_bRunning( false )
            , m_bStopped( false )
            , m_doneCount( 0 )
            , m_bTimeElapsed( false )
            , m_readyCount( 0 )
        {}

        ~thread_pool()
        {
            clear();
        }

        void add( thread * what )
        {
            m_threads.push_back( what );
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
            return run( std::chrono::seconds::zero() );
        }

        std::chrono::milliseconds run( std::chrono::seconds duration )
        {
            m_bStopped = false;
            m_doneCount = 0;

            while ( m_readyCount.load() != m_threads.size() )
                std::this_thread::yield();

            m_bTimeElapsed.store( false, std::memory_order_release );

            auto native_duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration);
            auto time_start = std::chrono::steady_clock::now();
            auto const expected_end = time_start + native_duration;

            {
                scoped_lock l( m_cvMutex );
                m_bRunning = true;
                m_cvStart.notify_all();
            }

            if ( duration != std::chrono::seconds::zero() ) {
                for ( ;; ) {
                    std::this_thread::sleep_for( native_duration );
                    auto time_now = std::chrono::steady_clock::now();
                    if ( time_now >= expected_end )
                        break;
                    native_duration = expected_end - time_now;
                } 
            }
            m_bTimeElapsed.store( true, std::memory_order_release );

            {
                scoped_lock l( m_cvMutex );
                while ( m_doneCount != m_threads.size() )
                    m_cvDone.wait( l );
                m_bStopped = true;
            }
            auto time_end = std::chrono::steady_clock::now();

            m_cvStop.notify_all();

            for ( auto t : m_threads )
                t->join();

            return m_testDuration = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
        }

        size_t size() const             { return m_threads.size(); }
        thread& get( size_t idx ) const { return *m_threads.at( idx ); }

        template <typename Fixture>
        Fixture& fixture()
        {
            return static_cast<Fixture&>(m_fixture);
        }

        std::chrono::milliseconds duration() const { return m_testDuration; }

        void clear()
        {
            for ( auto t : m_threads )
                delete t;
            m_threads.clear();
            m_bRunning = false;
            m_bStopped = false;
            m_doneCount = 0;
            m_readyCount = 0;
        }

    protected: // thread interface
        size_t get_next_id()
        {
            return m_threads.size();
        }

        void    ready_to_start( thread& /*who*/ )
        {
            // Called from test thread

            // Wait for all thread created
            scoped_lock l( m_cvMutex );
            m_readyCount.fetch_add( 1 );
            while ( !m_bRunning )
                m_cvStart.wait( l );
        }

        void    thread_done( thread& /*who*/ )
        {
            // Called from test thread

            {
                scoped_lock l( m_cvMutex );
                ++m_doneCount;

                // Tell pool that the thread is done
                m_cvDone.notify_all();

                // Wait for all thread done
                while ( !m_bStopped )
                    m_cvStop.wait( l );
            }
        }

    private:
        friend class thread;

        ::testing::Test&        m_fixture;
        std::vector<thread *>   m_threads;

        typedef std::unique_lock<std::mutex> scoped_lock;
        std::mutex              m_cvMutex;
        std::condition_variable m_cvStart;
        std::condition_variable m_cvStop;
        std::condition_variable m_cvDone;

        volatile bool   m_bRunning;
        volatile bool   m_bStopped;
        volatile size_t m_doneCount;
        std::atomic<bool> m_bTimeElapsed;
        std::atomic<size_t> m_readyCount;

        std::chrono::milliseconds m_testDuration;
    };

    inline thread::thread( thread_pool& master, int type /*= 0*/ )
        : m_pool( master )
        , m_type( type )
        , m_id( master.get_next_id())
        , m_impl( &thread::run, this )
    {}

    inline thread::thread( thread const& sample )
        : m_pool( sample.m_pool )
        , m_type( sample.m_type )
        , m_id( m_pool.get_next_id() )
        , m_impl( &thread::run, this )
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

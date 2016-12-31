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

#include <cds_test/stress_test.h>

#include <cds/intrusive/free_list.h>
#include <cds/intrusive/free_list_cached.h>
#ifdef CDS_DCAS_SUPPORT
#   include <cds/intrusive/free_list_tagged.h>
#endif

namespace {
    class put_get: public cds_test::stress_fixture
    {
    protected:
        static size_t s_nThreadCount;
        static size_t s_nPassCount;
        static size_t const c_nArraySize = 100;

        template <typename FreeList >
        struct value_type: public FreeList::node
        {
            size_t  counter;

            value_type()
                : counter(0)
            {}
        };

        template <class FreeList>
        class Worker: public cds_test::thread
        {
            typedef cds_test::thread base_class;
        public:
            FreeList&           m_FreeList;
            size_t              m_nSuccess = 0;

        public:
            Worker( cds_test::thread_pool& pool, FreeList& s )
                : base_class( pool )
                , m_FreeList( s )
            {}

            Worker( Worker& src )
                : base_class( src )
                , m_FreeList( src.m_FreeList )
            {}

            virtual thread * clone()
            {
                return new Worker( *this );
            }

            virtual void test()
            {
                typedef value_type<FreeList> item_type;
                item_type* arr[ c_nArraySize ];

                for ( size_t pass = 0; pass < s_nPassCount; ++pass ) {
                    size_t n = 0;
                    item_type* p;

                    while ( (p = static_cast<item_type*>( m_FreeList.get())) != nullptr ) {
                        CDS_TSAN_ANNOTATE_IGNORE_RW_BEGIN;
                        p->counter++;
                        CDS_TSAN_ANNOTATE_IGNORE_RW_END;
                        arr[n] = p;
                        ++m_nSuccess;
                        ++n;
                    }

                    for ( size_t i = 0; i < n; ++i )
                        m_FreeList.put( arr[i] );
                }
            }
        };

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "free_list" );

            s_nThreadCount = cfg.get_size_t( "ThreadCount", s_nThreadCount );
            s_nPassCount = cfg.get_size_t( "PassCount", s_nPassCount );

            if ( s_nThreadCount == 0 )
                s_nThreadCount = 1;
            if ( s_nPassCount == 0 )
                s_nPassCount = 1000;
        }
        //static void TearDownTestCase();

    protected:

        template <typename FreeList>
        void test( FreeList& list )
        {
            cds_test::thread_pool& pool = get_pool();

            value_type<FreeList> arr[c_nArraySize];

            for ( auto& i : arr )
                list.put( &i );

            pool.add( new Worker<FreeList>( pool, list ), s_nThreadCount );

            propout() << std::make_pair( "work_thread", s_nThreadCount )
                      << std::make_pair( "pass_count", s_nPassCount );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            // analyze result
            size_t nTotal = 0;
            for ( auto const& i : arr )
                nTotal += i.counter;

            size_t nSuccess = 0;
            for ( size_t threadNo = 0; threadNo < pool.size(); ++threadNo )
                nSuccess += static_cast<Worker<FreeList>&>( pool.get( threadNo )).m_nSuccess;

            EXPECT_EQ( nSuccess, nTotal );

            list.clear( []( typename FreeList::node* ) {} );
        }
    };

    size_t put_get::s_nThreadCount = 4;
    size_t put_get::s_nPassCount = 100000;

#define CDSSTRESS_FREELIST_F( name, freelist_type ) \
    TEST_F( put_get, name ) \
    { \
        freelist_type fl; \
        test( fl ); \
    }

    CDSSTRESS_FREELIST_F( FreeList, cds::intrusive::FreeList )

    typedef cds::intrusive::CachedFreeList<cds::intrusive::FreeList> cached_free_list;
    CDSSTRESS_FREELIST_F( CachedFreeList, cached_free_list )

#ifdef CDS_DCAS_SUPPORT
    TEST_F( put_get, TaggetFreeList )
    {
        struct tagged_ptr {
            void* p;
            uintptr_t tag;
        };

        atomics::atomic<tagged_ptr> tp;
        if ( tp.is_lock_free()) {
            cds::intrusive::TaggedFreeList fl;
            test( fl );
        }
        else
            std::cout << "Double-width CAS is not supported\n";
    }
#endif

} // namespace

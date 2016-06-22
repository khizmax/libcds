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

#include "set_type.h"

namespace set {

    class Set_InsDel_func: public cds_test::stress_fixture
    {
    public:
        static size_t s_nSetSize;               // set size
        static size_t s_nInsertThreadCount;     // count of insertion thread
        static size_t s_nDeleteThreadCount;     // count of deletion thread
        static size_t s_nUpdateThreadCount;     // count of updating thread
        static size_t s_nThreadPassCount;       // pass count for each thread
        static size_t s_nMaxLoadFactor;         // maximum load factor

        static size_t s_nCuckooInitialSize;     // initial size for CuckooSet
        static size_t s_nCuckooProbesetSize;    // CuckooSet probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold; // CUckooSet probeset threshold (0 - use default)

        static size_t s_nFeldmanSet_HeadBits;
        static size_t s_nFeldmanSet_ArrayBits;

        static size_t s_nLoadFactor;

        static void SetUpTestCase();
        //static void TearDownTestCase();

    public:
        typedef size_t  key_type;

        struct value {
            size_t      nKey;
            size_t      nData;
            atomics::atomic<size_t> nUpdateCall;
            bool volatile   bInitialized;
            cds::OS::ThreadId          threadId     ;   // insert thread id

            typedef cds::sync::spin_lock< cds::backoff::pause > lock_type;
            mutable lock_type   m_access;

            value()
                : nKey(0)
                , nData(0)
                , nUpdateCall(0)
                , bInitialized( false )
                , threadId( cds::OS::get_current_thread_id() )
            {}

            value( value const& s )
                : nKey(s.nKey)
                , nData(s.nData)
                , nUpdateCall(s.nUpdateCall.load(atomics::memory_order_relaxed))
                , bInitialized( s.bInitialized )
                , threadId( cds::OS::get_current_thread_id() )
                , m_access()
            {}

            // boost::container::flat_map requires operator =
            value& operator=( value const& v )
            {
                nKey = v.nKey;
                nData = v.nData;
                threadId = v.threadId;
                nUpdateCall.store( v.nUpdateCall.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed );
                bInitialized = v.bInitialized;

                return *this;
            }
        };

        size_t *    m_pKeyFirst;
        size_t *    m_pKeyLast;
        size_t *    m_pKeyArr;

        enum {
            insert_thread,
            update_thread,
            delete_thread
        };

        template <class Set>
        class Inserter: public cds_test::thread
        {
            typedef cds_test::thread base_class;

            Set&     m_Set;
            typedef typename Set::value_type keyval_type;

            struct insert_functor {
                size_t nTestFunctorRef;

                insert_functor()
                    : nTestFunctorRef(0)
                {}
                insert_functor( insert_functor const& ) = delete;

                void operator()( keyval_type& val )
                {
                    std::unique_lock< typename value::lock_type> ac( val.val.m_access );

                    val.val.nKey  = val.key;
                    val.val.nData = val.key * 8;

                    ++nTestFunctorRef;
                    val.val.bInitialized = true;
                }
            };

        public:
            size_t  m_nInsertSuccess = 0;
            size_t  m_nInsertFailed = 0;
            size_t  m_nTestFunctorRef = 0;

        public:
            Inserter( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, insert_thread )
                , m_Set( set )
            {}

            Inserter( Inserter& src )
                : base_class( src )
                , m_Set( src.m_Set )
            {}

            virtual thread * clone()
            {
                return new Inserter( *this );
            }

            virtual void test()
            {
                Set& rSet = m_Set;
                Set_InsDel_func& fixture = pool().template fixture<Set_InsDel_func>();

                size_t * pKeyFirst      = fixture.m_pKeyFirst;
                size_t * pKeyLast       = fixture.m_pKeyLast;
                size_t const nPassCount = fixture.s_nThreadPassCount;

                // func is passed by reference
                insert_functor  func;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t * p = pKeyFirst; p < pKeyLast; ++p ) {
                            if ( rSet.insert( *p, std::ref( func )))
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t * p = pKeyLast - 1; p >= pKeyFirst; --p ) {
                            if ( rSet.insert( *p, std::ref( func )))
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }

                m_nTestFunctorRef = func.nTestFunctorRef;
            }
        };

        template <class Set>
        class Updater: public cds_test::thread
        {
            typedef cds_test::thread base_class;

            Set&     m_Set;
            typedef typename Set::value_type keyval_type;

            struct update_functor {
                size_t  nCreated = 0;
                size_t  nModified = 0;

                update_functor() {}
                update_functor( const update_functor& ) = delete;

                void operator()( bool bNew, keyval_type& val, size_t /*nKey*/ )
                {
                    std::unique_lock<typename value::lock_type> ac( val.val.m_access );
                    if ( !val.val.bInitialized )
                    {
                        val.val.nKey = val.key;
                        val.val.nData = val.key * 8;
                        val.val.bInitialized = true;
                    }

                    if ( bNew ) {
                        ++nCreated;
                    }
                    else {
                        val.val.nUpdateCall.fetch_add( 1, atomics::memory_order_relaxed );
                        ++nModified;
                    }
                }

                void operator()( keyval_type& cur, keyval_type * old )
                {
                    operator()( old == nullptr, cur, 0 );
                }
            };

        public:
            size_t  m_nUpdateFailed = 0;
            size_t  m_nUpdateCreated = 0;
            size_t  m_nUpdateExisted = 0;
            size_t  m_nFunctorCreated = 0;
            size_t  m_nFunctorModified = 0;

        public:
            Updater( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, update_thread )
                , m_Set( set )
            {}

            Updater( Updater& src )
                : base_class( src )
                , m_Set( src.m_Set )
            {}

            virtual thread * clone()
            {
                return new Updater( *this );
            }

            virtual void test()
            {
                Set& rSet = m_Set;

                Set_InsDel_func& fixture = pool().template fixture<Set_InsDel_func>();
                size_t * pKeyFirst = fixture.m_pKeyFirst;
                size_t * pKeyLast = fixture.m_pKeyLast;
                size_t const nPassCount = fixture.s_nThreadPassCount;

                update_functor func;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t * p = pKeyFirst; p < pKeyLast; ++p ) {
                            std::pair<bool, bool> ret = rSet.update( *p, std::ref( func ), true );
                            if ( ret.first  ) {
                                if ( ret.second )
                                    ++m_nUpdateCreated;
                                else
                                    ++m_nUpdateExisted;
                            }
                            else
                                ++m_nUpdateFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t * p = pKeyLast - 1 ; p >= pKeyFirst; --p ) {
                            std::pair<bool, bool> ret = rSet.update( *p, std::ref( func ), true );
                            if ( ret.first  ) {
                                if ( ret.second )
                                    ++m_nUpdateCreated;
                                else
                                    ++m_nUpdateExisted;
                            }
                            else
                                ++m_nUpdateFailed;
                        }
                    }
                }

                m_nFunctorCreated = func.nCreated;
                m_nFunctorModified = func.nModified;
            }
        };

        template <class Set>
        class Deleter: public cds_test::thread
        {
            typedef cds_test::thread base_class;

            Set&     m_Set;
            typedef typename Set::value_type keyval_type;

            struct value_container
            {
                size_t      nKeyExpected;

                size_t      nSuccessItem = 0;
                size_t      nFailedItem = 0;
            };

            struct erase_functor {
                value_container     m_cnt;

                void operator ()( keyval_type const& itm )
                {
                    keyval_type& item = const_cast<keyval_type&>(itm);
                    while ( true ) {
                        bool bBkoff = false;
                        {
                            std::unique_lock< typename value::lock_type> ac( item.val.m_access );
                            if ( item.val.bInitialized ) {
                                if ( m_cnt.nKeyExpected == item.val.nKey && m_cnt.nKeyExpected * 8 == item.val.nData )
                                    ++m_cnt.nSuccessItem;
                                else
                                    ++m_cnt.nFailedItem;
                                item.val.nData++;
                                item.val.nKey = 0;
                                break;
                            }
                            else
                                bBkoff = true;
                        }
                        if ( bBkoff )
                            cds::backoff::yield()();
                    }
                }
            };

        public:
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;
            size_t  m_nValueSuccess = 0;
            size_t  m_nValueFailed = 0;

        public:
            Deleter( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, delete_thread )
                , m_Set( set )
            {}

            Deleter( Deleter& src )
                : base_class( src )
                , m_Set( src.m_Set )
            {}

            virtual thread * clone()
            {
                return new Deleter( *this );
            }

            virtual void test()
            {
                Set& rSet = m_Set;

                Set_InsDel_func& fixture = pool().template fixture<Set_InsDel_func>();
                size_t * pKeyFirst      = fixture.m_pKeyFirst;
                size_t * pKeyLast       = fixture.m_pKeyLast;
                size_t const nPassCount = fixture.s_nThreadPassCount;

                erase_functor   func;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t * p = pKeyFirst; p < pKeyLast; ++p ) {
                            func.m_cnt.nKeyExpected = *p;
                            if ( rSet.erase( *p, std::ref( func )))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t * p = pKeyLast - 1; p >= pKeyFirst; --p ) {
                            func.m_cnt.nKeyExpected = *p;
                            if ( rSet.erase( *p, std::ref( func )))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }

                m_nValueSuccess = func.m_cnt.nSuccessItem;
                m_nValueFailed = func.m_cnt.nFailedItem;
            }
        };

    protected:

        template <class Set>
        void run_test( Set& testSet )
        {
            typedef Inserter<Set>       InserterThread;
            typedef Deleter<Set>        DeleterThread;
            typedef Updater<Set>        UpdaterThread;

            m_pKeyArr = new size_t[ s_nSetSize ];
            m_pKeyFirst = m_pKeyArr;
            m_pKeyLast = m_pKeyFirst + s_nSetSize;
            for ( size_t i = 0; i < s_nSetSize; ++i )
                m_pKeyArr[i] = i;
            shuffle( m_pKeyFirst, m_pKeyLast );

            cds_test::thread_pool& pool = get_pool();
            pool.add( new InserterThread( pool, testSet ), s_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testSet ),  s_nDeleteThreadCount );
            pool.add( new UpdaterThread( pool, testSet ),  s_nUpdateThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsertThreadCount )
                << std::make_pair( "update_thread_count", s_nUpdateThreadCount )
                << std::make_pair( "delete_thread_count", s_nDeleteThreadCount )
                << std::make_pair( "thread_pass_count", s_nThreadPassCount )
                << std::make_pair( "set_size", s_nSetSize );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            delete [] m_pKeyArr;

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nDelValueSuccess = 0;
            size_t nDelValueFailed = 0;
            size_t nUpdateFailed = 0;
            size_t nUpdateCreated = 0;
            size_t nUpdateModified = 0;
            size_t nEnsFuncCreated = 0;
            size_t nEnsFuncModified = 0;
            size_t nTestFunctorRef = 0;

            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                switch ( thr.type() ) {
                case insert_thread:
                    {
                        InserterThread& inserter = static_cast<InserterThread&>( thr );
                        nInsertSuccess  += inserter.m_nInsertSuccess;
                        nInsertFailed   += inserter.m_nInsertFailed;
                        nTestFunctorRef += inserter.m_nTestFunctorRef;
                    }
                    break;
                case update_thread:
                    {
                        UpdaterThread& updater = static_cast<UpdaterThread&>(thr);
                        nUpdateCreated   += updater.m_nUpdateCreated;
                        nUpdateModified  += updater.m_nUpdateExisted;
                        nUpdateFailed    += updater.m_nUpdateFailed;
                        nEnsFuncCreated  += updater.m_nFunctorCreated;
                        nEnsFuncModified += updater.m_nFunctorModified;
                    }
                    break;
                case delete_thread:
                    {
                        DeleterThread& deleter = static_cast<DeleterThread&>(thr);
                        nDeleteSuccess   += deleter.m_nDeleteSuccess;
                        nDeleteFailed    += deleter.m_nDeleteFailed;
                        nDelValueSuccess += deleter.m_nValueSuccess;
                        nDelValueFailed  += deleter.m_nValueFailed;
                    }
                    break;
                }
            }

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_failed", nDeleteFailed )
                << std::make_pair( "update_created", nUpdateCreated )
                << std::make_pair( "update_modified", nUpdateModified )
                << std::make_pair( "update_failed", nUpdateFailed )
                << std::make_pair( "final_set_size", testSet.size() );


            EXPECT_EQ( nDelValueFailed, 0 );
            EXPECT_EQ( nDelValueSuccess, nDeleteSuccess );

            EXPECT_EQ( nUpdateFailed, 0 );
            EXPECT_EQ( nUpdateCreated, nEnsFuncCreated );
            EXPECT_EQ( nUpdateModified, nEnsFuncModified );

            // nTestFunctorRef is call count of insert functor
            EXPECT_EQ( nTestFunctorRef, nInsertSuccess );

            testSet.clear();
            EXPECT_TRUE( testSet.empty() );

            additional_check( testSet );
            print_stat( propout(), testSet  );

            additional_cleanup( testSet );
        }

        template <class Set>
        void run_test()
        {
            Set s( *this );
            run_test( s );
        }
    };

    class Set_InsDel_func_LF: public Set_InsDel_func
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Set>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Set_InsDel_func::run_test<Set>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace set

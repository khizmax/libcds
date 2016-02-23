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

#include "cppunit/cppunit_proxy.h"
#include "cppunit/test_beans.h"
#include <cds/container/timestamped_deque.h>
#include <boost/container/deque.hpp>

namespace deque {

    class HdrTSDeque: public CppUnitMini::TestCase
    {
        template <class Deque>
        void test_with( Deque& dq )
        {
            size_t const c_nSize = 100;

            // push_front/pop_front
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_front( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            size_t nCount = 0;
            int val;
            while ( !dq.empty() ) {
                CPPUNIT_CHECK( dq.pop_front( val ) );
                ++nCount;
                CPPUNIT_CHECK( static_cast<int>(c_nSize - nCount) == val );
            }
            CPPUNIT_CHECK( nCount == c_nSize );

            // push_back/pop_back
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_back( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            nCount = 0;
            while ( !dq.empty() ) {
                CPPUNIT_CHECK( dq.pop_back( val ) );
                ++nCount;
                CPPUNIT_CHECK( static_cast<int>(c_nSize - nCount) == val );
            }
            CPPUNIT_CHECK( nCount == c_nSize );

            // push_back/pop_front
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_back( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            nCount = 0;
            while ( !dq.empty() ) {
                CPPUNIT_CHECK( dq.pop_front( val ) );
                CPPUNIT_CHECK( static_cast<int>( nCount ) == val );
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == c_nSize );

            // push_front/pop_back
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_front( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            nCount = 0;
            while ( !dq.empty() ) {
                CPPUNIT_CHECK( dq.pop_back( val ) );
                CPPUNIT_CHECK( static_cast<int>( nCount ) == val );
                ++nCount;
            }
            CPPUNIT_CHECK( nCount == c_nSize );

            // clear
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                CPPUNIT_CHECK( dq.push_front( i ) );
            CPPUNIT_CHECK( dq.size() == c_nSize );

            CPPUNIT_CHECK( !dq.empty() );
            dq.clear();
            CPPUNIT_CHECK( dq.empty() );


        }

        template <class Deque>
        void test()
        {
            Deque dq;
            test_with( dq );
        }

        void tsDeque()
        {
            struct traits_TSDeque_ic : public cds::container::timestamped_deque::traits
            {
                typedef cds::atomicity::item_counter item_counter;
      
            };
            typedef cds::container::Timestamped_deque<int, traits_TSDeque_ic> deque_type;

            test<deque_type>();
        }


        CPPUNIT_TEST_SUITE(HdrTSDeque)
            CPPUNIT_TEST(tsDeque)
        CPPUNIT_TEST_SUITE_END()
    };

} // namespace deque

CPPUNIT_TEST_SUITE_REGISTRATION(deque::HdrTSDeque);

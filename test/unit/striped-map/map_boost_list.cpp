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

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

#include <boost/version.hpp>
#include <cds/details/defs.h>
#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == CDS_COMPILER_MSVC12 && BOOST_VERSION < 105500
    // Skipped; for Microsoft Visual C++ 2013 and boost::container::list you should use boost version 1.55 or above" );
#elif BOOST_VERSION >= 104800

#include <cds/container/striped_map/boost_list.h> 
#include "test_striped_map.h"

namespace {
    struct test_traits
    {
        typedef boost::container::list< std::pair< cds_test::striped_map_fixture::key_type const, cds_test::striped_map_fixture::value_type >> container_type;

        struct copy_policy {
            typedef container_type::iterator iterator;

            void operator()( container_type& list, iterator itInsert, iterator itWhat )
            {
                list.insert( itInsert, *itWhat );
            }
        };

        static bool const c_hasFindWith = true;
        static bool const c_hasEraseWith = true;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostList, StripedMap, test_traits );
    INSTANTIATE_TYPED_TEST_CASE_P( BoostList, RefinableMap, test_traits );

} // namespace

#else // BOOST_VERSION < 104800
// Skipped; for boost::container::list you should use boost version 1.48 or above
#endif  // BOOST_VERSION 

#endif // #ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

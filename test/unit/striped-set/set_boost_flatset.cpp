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

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

#include <boost/version.hpp>
#include <cds/details/defs.h>
#if CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION == CDS_COMPILER_MSVC12 && BOOST_VERSION < 105600
    // Skipped; for Microsoft Visual C++ 2013 and boost::container::flat_set you should use boost version 1.56 or above" );
#elif BOOST_VERSION >= 104800

#include <cds/container/striped_set/boost_flat_set.h>
#include "test_striped_set.h"

namespace {
    struct test_traits
    {
        typedef boost::container::flat_set< cds_test::container_set::int_item, cds_test::container_set::less > container_type;

        struct copy_policy {
            typedef container_type::iterator iterator;

            void operator()( container_type& set, iterator itWhat )
            {
                set.insert( *itWhat );
            }
        };

        static bool const c_hasFindWith = false;
        static bool const c_hasEraseWith = false;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostFlatSet, StripedSet, test_traits );
    INSTANTIATE_TYPED_TEST_CASE_P( BoostFlatSet, RefinableSet, test_traits );

} // namespace

#else // BOOST_VERSION < 104800
    // Skipped; for boost::container::flat_set you should use boost version 1.48 or above
#endif  // BOOST_VERSION

#endif // CDSUNIT_ENABLE_BOOST_CONTAINER

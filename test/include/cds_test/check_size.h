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

#ifndef CDSTEST_CHECK_SIZE_H
#define CDSTEST_CHECK_SIZE_H

#include <gtest/gtest.h>
#include <cds/algo/atomic.h>

namespace cds_test {

    template <typename ItemCounter>
    struct size_checker {
        template <typename Container>
        bool operator()( Container const& c, size_t nSize ) const
        {
            return c.size() == nSize;
        }
    };

    template<>
    struct size_checker< cds::atomicity::empty_item_counter > {
        template <typename Container>
        bool operator()( Container const& /*c*/, size_t /*nSize*/ ) const
        {
            return true;
        }
    };

    template <class Container>
    static inline bool check_size( Container const& cont, size_t nSize )
    {
        return size_checker<typename Container::item_counter>()( cont, nSize );
    }

} // namespace cds_test

#define ASSERT_CONTAINER_SIZE( container, expected ) \
    ASSERT_TRUE( cds_test::check_size(container, expected)) << "container size=" << container.size() << ", expected=" << expected

#define EXPECT_CONTAINER_SIZE( container, expected ) \
    EXPECT_TRUE( cds_test::check_size(container, expected)) << "container size=" << container.size() << ", expected=" << expected

#endif // #ifndef CDSTEST_CHECK_SIZE_H

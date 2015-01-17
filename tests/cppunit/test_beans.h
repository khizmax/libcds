/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

// Forward declarations
namespace cds {
    namespace intrusive {}
    namespace opt {}
}

// Including this header is a bad thing for header testing. How to avoid it?..
#include <cds/algo/atomic.h>   // for cds::atomicity::empty_item_counter

namespace test_beans {
    template <typename ItemCounter>
    struct check_item_counter {
        bool operator()( size_t nReal, size_t nExpected )
        {
            return nReal == nExpected;
        }
    };

    template <>
    struct check_item_counter<cds::atomicity::empty_item_counter>
    {
        bool operator()( size_t nReal, size_t /*nExpected*/ )
        {
            return nReal == 0;
        }
    };
} // namespace beans

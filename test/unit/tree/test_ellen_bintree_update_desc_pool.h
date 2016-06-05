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

#ifndef CDSUNIT_TREE_TEST_ELLEN_BINTREE_UPDATE_DESC_POOL_H
#define CDSUNIT_TREE_TEST_ELLEN_BINTREE_UPDATE_DESC_POOL_H

#include <cds/memory/vyukov_queue_pool.h>
#include <cds/memory/pool_allocator.h>

namespace cds_test {

    typedef cds::intrusive::ellen_bintree::update_desc< void, void > update_desc;

    // update_desc pools
    struct pool_traits: public cds::memory::vyukov_queue_pool_traits
    {
        typedef cds::opt::v::uninitialized_static_buffer< update_desc, 256 > buffer;
    };
    typedef cds::memory::vyukov_queue_pool< update_desc, pool_traits > pool_type;
    typedef cds::memory::lazy_vyukov_queue_pool< update_desc, pool_traits > lazy_pool_type;

    extern pool_type      s_Pool;
    extern lazy_pool_type s_LazyPool;

    struct pool_accessor
    {
        typedef pool_type::value_type value_type;

        pool_type& operator()() const
        {
            return s_Pool;
        }
    };

    struct lazy_pool_accessor
    {
        typedef lazy_pool_type::value_type value_type;

        lazy_pool_type& operator()() const
        {
            return s_LazyPool;
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_TREE_TEST_ELLEN_BINTREE_UPDATE_DESC_POOL_H

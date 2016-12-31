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

#include "test_intrusive_striped_set.h"
#include <cds/intrusive/striped_set/boost_splay_set.h>

namespace {
    namespace ci = cds::intrusive;
    namespace bi = boost::intrusive;

    struct test_traits
    {
#if BOOST_VERSION < 105600
        typedef bi::splay_set_member_hook<> splay_set_member_hook;
        typedef bi::splay_set_base_hook<> splay_set_base_hook;
#else
        typedef bi::bs_set_base_hook<> splay_set_base_hook;
        typedef bi::bs_set_member_hook<> splay_set_member_hook;
#endif
        typedef cds_test::intrusive_set::base_int_item< splay_set_base_hook > base_item;
        typedef cds_test::intrusive_set::member_int_item< splay_set_member_hook > member_item;

        typedef bi::splay_set< base_item,
            bi::compare< cds_test::intrusive_set::less< base_item >>
        > base_hook_container;

        typedef bi::splay_set< member_item,
            bi::member_hook< member_item, splay_set_member_hook, &member_item::hMember>,
            bi::compare< cds_test::intrusive_set::less< member_item >>
        > member_hook_container;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostSplaySet, IntrusiveStripedSet, test_traits );

} // namespace

#endif // CDSUNIT_ENABLE_BOOST_CONTAINER

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

#ifndef CDSUNIT_SET_TEST_INTRUSIVE_STRIPED_SET_H
#define CDSUNIT_SET_TEST_INTRUSIVE_STRIPED_SET_H

#include "test_intrusive_set.h"

#include <cds/intrusive/striped_set.h>

namespace {
    namespace ci = cds::intrusive;

    template <typename ContainerPair>
    class IntrusiveStripedSet : public cds_test::intrusive_set
    {
    protected:
        typedef cds_test::intrusive_set base_class;

        typedef typename ContainerPair::base_item   base_item;
        typedef typename ContainerPair::member_item member_item;

        typedef typename ContainerPair::base_hook_container   base_hook_container;
        typedef typename ContainerPair::member_hook_container member_hook_container;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    TYPED_TEST_CASE_P( IntrusiveStripedSet );

// ****************************************************************
// striped base hook

    TYPED_TEST_P( IntrusiveStripedSet, striped_basehook_compare )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::base_item >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, striped_basehook_less )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::base_item >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, striped_basehook_cmpmix )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::base_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::base_item >>,
            ci::opt::resizing_policy< ci::striped_set::load_factor_resizing< 8 >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64 );
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, striped_basehook_resizing_threshold )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::base_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::base_item >>,
            ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<8>>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, striped_basehook_resizing_threshold_rt )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash2 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::base_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::base_item >>,
            ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<0>>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>( 4 ));
            this->test( s, data );
        }
    }

// ****************************************************************
// striped member hook

    TYPED_TEST_P( IntrusiveStripedSet, striped_memberhook_compare )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::member_item >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, striped_memberhook_less )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::member_item >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 32 );
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, striped_memberhook_cmpmix )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::member_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::member_item >>,
            ci::opt::resizing_policy< ci::striped_set::load_factor_resizing< 8 >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64 );
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, striped_memberhook_resizing_threshold )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::member_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::member_item >>,
            ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<8>>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, striped_memberhook_resizing_threshold_rt )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash2 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::member_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::member_item >>,
            ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<0>>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>( 4 ));
            this->test( s, data );
        }
    }


// ****************************************************************
// refinable base hook

    TYPED_TEST_P( IntrusiveStripedSet, refinable_basehook_compare )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::base_item >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, refinable_basehook_less )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::base_item >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 32 );
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, refinable_basehook_cmpmix )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::base_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::base_item >>,
            ci::opt::resizing_policy< ci::striped_set::load_factor_resizing< 8 >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64 );
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, refinable_basehook_resizing_threshold )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::base_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::base_item >>,
            ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<8>>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, refinable_basehook_resizing_threshold_rt )
    {
        typedef ci::StripedSet<
            typename TestFixture::base_hook_container,
            ci::opt::hash< typename TestFixture::hash2 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::base_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::base_item >>,
            ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<0>>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>( 4 ));
            this->test( s, data );
        }
    }

// ****************************************************************
// refinable member hook

    TYPED_TEST_P( IntrusiveStripedSet, refinable_memberhook_compare )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::member_item >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, refinable_memberhook_less )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::member_item >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 32 );
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, refinable_memberhook_cmpmix )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::member_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::member_item >>,
            ci::opt::resizing_policy< ci::striped_set::load_factor_resizing< 8 >>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64 );
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, refinable_memberhook_resizing_threshold )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash1 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::member_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::member_item >>,
            ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<8>>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TYPED_TEST_P( IntrusiveStripedSet, refinable_memberhook_resizing_threshold_rt )
    {
        typedef ci::StripedSet<
            typename TestFixture::member_hook_container,
            ci::opt::hash< typename TestFixture::hash2 >,
            ci::opt::less< typename TestFixture::template less< typename TestFixture::member_item >>,
            ci::opt::compare< typename TestFixture::template cmp< typename TestFixture::member_item >>,
            ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<0>>
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>( 4 ));
            this->test( s, data );
        }
    }

    REGISTER_TYPED_TEST_CASE_P( IntrusiveStripedSet,
        striped_basehook_compare, striped_basehook_less, striped_basehook_cmpmix, striped_basehook_resizing_threshold, striped_basehook_resizing_threshold_rt, striped_memberhook_compare, striped_memberhook_less, striped_memberhook_cmpmix, striped_memberhook_resizing_threshold, striped_memberhook_resizing_threshold_rt, refinable_basehook_compare, refinable_basehook_less, refinable_basehook_cmpmix, refinable_basehook_resizing_threshold, refinable_basehook_resizing_threshold_rt, refinable_memberhook_compare, refinable_memberhook_less, refinable_memberhook_cmpmix, refinable_memberhook_resizing_threshold, refinable_memberhook_resizing_threshold_rt
        );

} // namespace

#endif // CDSUNIT_SET_TEST_INTRUSIVE_STRIPED_SET_H

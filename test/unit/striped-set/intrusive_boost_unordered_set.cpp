// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TEST_INTRUSIVE_BOOST_UNORDERED_SET_H
#define CDSUNIT_SET_TEST_INTRUSIVE_BOOST_UNORDERED_SET_H

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

#include "test_intrusive_set.h"

#include <cds/intrusive/striped_set/boost_unordered_set.h>
#include <cds/intrusive/striped_set.h>

namespace {
    namespace ci = cds::intrusive;
    namespace bi = boost::intrusive;

    class IntrusiveStripedBoostUnorderedSet: public cds_test::intrusive_set
    {
    public:
        typedef base_int_item< bi::unordered_set_base_hook<>> base_item;
        typedef member_int_item< bi::unordered_set_member_hook<>> member_item;
    };

    template <size_t Capacity, typename T, class Alloc = CDS_DEFAULT_ALLOCATOR>
    struct dyn_buffer: public ci::opt::v::initialized_dynamic_buffer< T, Alloc >
    {
        typedef ci::opt::v::initialized_dynamic_buffer< T, Alloc >   base_class;
    public:
        template <typename Q>
        struct rebind {
            typedef dyn_buffer<Capacity, Q, Alloc> other;  ///< Rebinding result type
        };

        dyn_buffer()
            : base_class( Capacity )
        {}
    };

    TEST_F( IntrusiveStripedBoostUnorderedSet, basehook )
    {
        typedef ci::StripedSet<
            bi::unordered_set< base_item
                , bi::hash< hash1 >
                , bi::equal< equal_to<base_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,cds::intrusive::opt::hash< hash2 >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, basehook_resize_bucket_threshold )
    {
        typedef ci::StripedSet<
            bi::unordered_set< base_item
                , bi::hash< hash1 >
                , bi::equal< equal_to<base_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,cds::intrusive::opt::hash< hash2 >
            ,cds::intrusive::opt::buffer< cds::intrusive::opt::v::initialized_static_buffer< cds::any_type, 64 > >
            ,cds::intrusive::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, basehook_resize_bucket_threshold_rt )
    {
        typedef ci::StripedSet<
            bi::unordered_set< base_item
                , bi::hash< hash1 >
                , bi::equal< equal_to<base_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,ci::opt::hash< hash2 >
            ,ci::opt::buffer< dyn_buffer<64, cds::any_type>>
            ,ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>( 128 ));
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, memberhook )
    {
        typedef ci::StripedSet<
            bi::unordered_set< member_item
                , bi::member_hook< member_item, bi::unordered_set_member_hook<>, &member_item::hMember>
                , bi::hash< hash1 >
                , bi::equal< equal_to<member_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,cds::intrusive::opt::hash< hash2 >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, memberhook_resize_bucket_threshold )
    {
        typedef ci::StripedSet<
            bi::unordered_set< member_item
                , bi::member_hook< member_item, bi::unordered_set_member_hook<>, &member_item::hMember>
                , bi::hash< hash1 >
                , bi::equal< equal_to<member_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,cds::intrusive::opt::hash< hash2 >
            ,cds::intrusive::opt::buffer< cds::intrusive::opt::v::initialized_static_buffer< cds::any_type, 64 > >
            ,cds::intrusive::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, memberhook_resize_bucket_threshold_rt )
    {
        typedef ci::StripedSet<
            bi::unordered_set< member_item
                , bi::member_hook< member_item, bi::unordered_set_member_hook<>, &member_item::hMember>
                , bi::hash< hash1 >
                , bi::equal< equal_to<member_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,ci::opt::hash< hash2 >
            ,ci::opt::buffer< dyn_buffer<64, cds::any_type>>
            ,ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>( 128 ));
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, refinable_basehook )
    {
        typedef ci::StripedSet<
            bi::unordered_set< base_item
                , bi::hash< hash1 >
                , bi::equal< equal_to<base_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,cds::intrusive::opt::hash< hash2 >
            ,ci::opt::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, refinable_basehook_resize_bucket_threshold )
    {
        typedef ci::StripedSet<
            bi::unordered_set< base_item
                , bi::hash< hash1 >
                , bi::equal< equal_to<base_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,ci::opt::mutex_policy< ci::striped_set::refinable<> >
            ,cds::intrusive::opt::hash< hash2 >
            ,cds::intrusive::opt::buffer< cds::intrusive::opt::v::initialized_static_buffer< cds::any_type, 64 > >
            ,cds::intrusive::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, refinable_basehook_resize_bucket_threshold_rt )
    {
        typedef ci::StripedSet<
            bi::unordered_set< base_item
                , bi::hash< hash1 >
                , bi::equal< equal_to<base_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,ci::opt::hash< hash2 >
            ,ci::opt::mutex_policy< ci::striped_set::refinable<> >
            ,ci::opt::buffer< dyn_buffer<64, cds::any_type>>
            ,ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>( 128 ));
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, refinable_memberhook )
    {
        typedef ci::StripedSet<
            bi::unordered_set< member_item
                , bi::member_hook< member_item, bi::unordered_set_member_hook<>, &member_item::hMember>
                , bi::hash< hash1 >
                , bi::equal< equal_to<member_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,cds::intrusive::opt::hash< hash2 >
            ,ci::opt::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, refinable_memberhook_resize_bucket_threshold )
    {
        typedef ci::StripedSet<
            bi::unordered_set< member_item
                , bi::member_hook< member_item, bi::unordered_set_member_hook<>, &member_item::hMember>
                , bi::hash< hash1 >
                , bi::equal< equal_to<member_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,cds::intrusive::opt::hash< hash2 >
            ,ci::opt::mutex_policy< ci::striped_set::refinable<> >
            ,cds::intrusive::opt::buffer< cds::intrusive::opt::v::initialized_static_buffer< cds::any_type, 64 > >
            ,cds::intrusive::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s;
            this->test( s, data );
        }
    }

    TEST_F( IntrusiveStripedBoostUnorderedSet, refinable_memberhook_resize_bucket_threshold_rt )
    {
        typedef ci::StripedSet<
            bi::unordered_set< member_item
                , bi::member_hook< member_item, bi::unordered_set_member_hook<>, &member_item::hMember>
                , bi::hash< hash1 >
                , bi::equal< equal_to<member_item> >
                , bi::power_2_buckets<true>
                , bi::incremental<true>
            >
            ,ci::opt::hash< hash2 >
            ,ci::opt::mutex_policy< ci::striped_set::refinable<> >
            ,ci::opt::buffer< dyn_buffer<64, cds::any_type>>
            ,ci::opt::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        std::vector< typename set_type::value_type > data;
        {
            set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>( 128 ));
            this->test( s, data );
        }
    }

} // namespace

#endif // #ifdef CDSUNIT_ENABLE_BOOST_CONTAINER
#endif // CDSUNIT_SET_TEST_INTRUSIVE_BOOST_UNORDERED_SET_H

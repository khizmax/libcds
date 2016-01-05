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

#include "set/hdr_feldman_hashset.h"
#include <cds/urcu/signal_buffered.h>
#include <cds/container/feldman_hashset_rcu.h>
#include "unit/print_feldman_hashset_stat.h"

namespace set {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc<cds::urcu::signal_buffered<>> rcu_type;
    } // namespace
#endif

    void FeldmanHashSetHdrTest::rcu_shb_nohash()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t key_type;

        struct traits : public cc::feldman_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!");
        test_rcu<set_type, nohash<key_type>>(4, 2);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<key_type>,
            typename cc::feldman_hashset::make_traits<
            cc::feldman_hashset::hash_accessor< get_key<key_type>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(4, 2);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_stdhash()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(4, 2);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(4, 2);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_hash128()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef hash128 hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef hash128::less less;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash128!!!" );
        test_rcu<set_type, hash128::make>(4, 2);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                , co::less< hash_type::less >
            >::type
        > set_type2;
        test_rcu<set_type2, hash128::make>(4, 2);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_nohash_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t key_type;

        struct traits : public cc::feldman_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!");
        test_rcu<set_type, nohash<key_type>>(4, 2);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<key_type>,
            typename cc::feldman_hashset::make_traits<
            cc::feldman_hashset::hash_accessor< get_key<key_type>>
            , co::stat< cc::feldman_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(4, 2);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_stdhash_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(4, 2);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                ,co::stat< cc::feldman_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(4, 2);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_hash128_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef hash128 hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef hash128::cmp  compare;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_rcu<set_type, hash_type::make>(4, 2);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                ,co::stat< cc::feldman_hashset::stat<>>
                ,co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_rcu<set_type2, hash_type::make>(4, 2);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_nohash_5_3()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t key_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, nohash<key_type>>(5, 3);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<key_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_key<key_type>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(5, 3);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_stdhash_5_3()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(5, 3);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(5, 3);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_hash128_4_3()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef hash128 hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_rcu<set_type, hash128::make >(4, 3);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        > set_type2;
        test_rcu<set_type2, hash128::make >(4, 3);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_nohash_5_3_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t key_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, nohash<key_type>>(5, 3);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<key_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_key<key_type>>
                ,co::stat< cc::feldman_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(5, 3);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_stdhash_5_3_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(5, 3);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                ,co::stat< cc::feldman_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(5, 3);
#endif
    }

    void FeldmanHashSetHdrTest::rcu_shb_hash128_4_3_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef hash128 hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
            typedef hash128::less less;
            typedef hash128::cmp compare;
        };
        typedef cc::FeldmanHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_rcu<set_type, hash_type::make>(4, 3);

        typedef cc::FeldmanHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                , co::stat< cc::feldman_hashset::stat<>>
                , co::less< hash_type::less >
                , co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_rcu<set_type2, hash_type::make>(4, 3);
#endif
    }
} // namespace set

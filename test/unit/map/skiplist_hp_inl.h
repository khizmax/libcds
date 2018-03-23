// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)


TEST_F( CDSTEST_FIXTURE_NAME, compare )
{
    typedef cc::SkipListMap< gc_type, key_type, value_type,
        typename cc::skip_list::make_traits<
            cds::opt::compare< cmp >
        >::type
    > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, less )
{
    typedef cc::SkipListMap< gc_type, key_type, value_type,
        typename cc::skip_list::make_traits<
            cds::opt::less< base_class::less >
        >::type
    > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, cmpmix )
{
    typedef cc::SkipListMap< gc_type, key_type, value_type,
        typename cc::skip_list::make_traits<
            cds::opt::less< base_class::less >
            ,cds::opt::compare< cmp >
        >::type
    > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, item_counting )
{
    struct map_traits: public cc::skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, backoff )
{
    struct map_traits: public cc::skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
    };
    typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, stat )
{
    struct map_traits: public cc::skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
        typedef cc::skip_list::stat<> stat;
    };
    typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, xorshift32 )
{
    struct map_traits: public cc::skip_list::traits
    {
        typedef base_class::less less;
        typedef cc::skip_list::xorshift32 random_level_generator;
    };
    typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, xorshift24 )
{
    struct map_traits: public cc::skip_list::traits
    {
        typedef base_class::less less;
        typedef cc::skip_list::stat<> stat;
        typedef cc::skip_list::xorshift24 random_level_generator;
    };
    typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, xorshift16 )
{
    struct map_traits: public cc::skip_list::traits
    {
        typedef cmp compare;
        typedef cc::skip_list::xorshift16 random_level_generator;
    };
    typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, turbo32 )
{
    struct map_traits: public cc::skip_list::traits
    {
        typedef cmp compare;
        typedef cc::skip_list::turbo32 random_level_generator;
    };
    typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, turbo24 )
{
    struct map_traits: public cc::skip_list::traits
    {
        typedef cmp compare;
        typedef cc::skip_list::turbo24 random_level_generator;
    };
    typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

    map_type m;
    test( m );
}

TEST_F( CDSTEST_FIXTURE_NAME, turbo16 )
{
    struct map_traits: public cc::skip_list::traits
    {
        typedef cmp compare;
        typedef cc::skip_list::turbo16 random_level_generator;
    };
    typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

    map_type m;
    test( m );
}

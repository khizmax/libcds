TEST_F( CDSTEST_FIXTURE_NAME, compare )
{
    typedef cc::LazySkipListSet< gc_type, int_item,
        typename cc::lazy_skip_list::make_traits<
            cds::opt::compare< cmp >
        >::type
    > set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, less )
{
    typedef cc::LazySkipListSet< gc_type, int_item,
        typename cc::lazy_skip_list::make_traits<
            cds::opt::less< base_class::less >
        >::type
    > set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, cmpmix )
{
    typedef cc::LazySkipListSet< gc_type, int_item,
        typename cc::lazy_skip_list::make_traits<
            cds::opt::less< base_class::less >
            ,cds::opt::compare< cmp >
        >::type
    > set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, item_counting )
{
    struct set_traits: public cc::lazy_skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::LazySkipListSet< gc_type, int_item, set_traits >set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, backoff )
{
    struct set_traits: public cc::lazy_skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
    };
    typedef cc::LazySkipListSet< gc_type, int_item, set_traits >set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, stat )
{
    struct set_traits: public cc::lazy_skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
        typedef cc::lazy_skip_list::stat<> stat;
    };
    typedef cc::LazySkipListSet< gc_type, int_item, set_traits >set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, xorshift32 )
{
    struct set_traits: public cc::lazy_skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::lazy_skip_list::stat<> stat;
        typedef cc::lazy_skip_list::xorshift32 random_level_generator;
    };
    typedef cc::LazySkipListSet< gc_type, int_item, set_traits >set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, xorshift24 )
{
    struct set_traits: public cc::lazy_skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::lazy_skip_list::stat<> stat;
        typedef cc::lazy_skip_list::xorshift24 random_level_generator;
    };
    typedef cc::LazySkipListSet< gc_type, int_item, set_traits >set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, xorshift16 )
{
    struct set_traits: public cc::lazy_skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::lazy_skip_list::stat<> stat;
        typedef cc::lazy_skip_list::xorshift16 random_level_generator;
    };
    typedef cc::LazySkipListSet< gc_type, int_item, set_traits >set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, turbo32 )
{
    struct set_traits: public cc::lazy_skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::lazy_skip_list::stat<> stat;
        typedef cc::lazy_skip_list::turbo32 random_level_generator;
    };
    typedef cc::LazySkipListSet< gc_type, int_item, set_traits >set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, turbo24 )
{
    struct set_traits: public cc::lazy_skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::lazy_skip_list::stat<> stat;
        typedef cc::lazy_skip_list::turbo24 random_level_generator;
    };
    typedef cc::LazySkipListSet< gc_type, int_item, set_traits >set_type;

    set_type s;
    test( s );
}

TEST_F( CDSTEST_FIXTURE_NAME, turbo16 )
{
    struct set_traits: public cc::lazy_skip_list::traits
    {
        typedef cmp compare;
        typedef base_class::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::lazy_skip_list::stat<> stat;
        typedef cc::lazy_skip_list::turbo16 random_level_generator;
    };
    typedef cc::LazySkipListSet< gc_type, int_item, set_traits >set_type;

    set_type s;
    test( s );
}

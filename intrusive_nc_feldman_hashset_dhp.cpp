#include "test_intrusive_nc_feldman_hashset_hp.h"

#include <cds/intrusive/nc_feldman_hashset_dhp.h>
#include <cds/container/iterable_list_hp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveFeldmanHashSet_DHP : public cds_test::intrusive_nc_feldman_hashset_hp
    {
    protected:
        typedef cds_test::intrusive_nc_feldman_hashset_hp base_class;

        struct traits : public ci::iterable_list::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef base_class::less less;
        };

        typedef ci::IterableList<gc_type, int_item, traits> list;        

    protected:

        void SetUp()
        {
            struct traits : public ci::feldman_hashset::traits
            {
                typedef base_class::hash_accessor hash_accessor;
                typedef base_class::equal value_equal;
                typedef cmp compare;
                typedef mock_disposer disposer;
                typedef list list_type;
            };

            typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

            cds::gc::dhp::smr::construct( set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };

    TEST_F( IntrusiveFeldmanHashSet_DHP, compare )
    {        
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef base_class::equal value_equal;
            typedef cmp compare;
            typedef mock_disposer disposer;
            typedef list list_type;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s;
        test( s );
    }


    TEST_F( IntrusiveFeldmanHashSet_DHP, less )
    {
 
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef base_class::equal value_equal;
            typedef std::less<int> less;
            typedef mock_disposer disposer;
            typedef list list_type;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 2, 5 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_DHP, cmpmix )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef base_class::equal value_equal;
            typedef cmp compare;
            typedef std::less<int> less;
            typedef mock_disposer disposer;
            typedef simple_item_counter item_counter;
            typedef list list_type;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 2, 5 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_DHP, backoff )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef base_class::equal value_equal;
            typedef cmp compare;
            typedef mock_disposer disposer;
            typedef cds::backoff::empty back_off;
            typedef ci::opt::v::sequential_consistent memory_model;
            typedef list list_type;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 8, 3 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_DHP, stat )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef base_class::equal value_equal;
            typedef cmp compare;
            typedef mock_disposer disposer;
            typedef ci::feldman_hashset::stat<> stat;
            typedef list list_type;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 8, 3 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_DHP, byte_cut )
    {
        struct traits: public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef base_class::equal value_equal;
            typedef cds::algo::byte_splitter< int > hash_splitter;
            typedef cmp compare;
            typedef std::less<int> less;
            typedef mock_disposer disposer;
            typedef simple_item_counter item_counter;
            typedef list list_type;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 8, 8 );
        test( s );
    }

} // namespace

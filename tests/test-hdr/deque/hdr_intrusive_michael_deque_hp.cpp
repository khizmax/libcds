//$$CDS-header$$

#include "hdr_intrusive_deque.h"
#include <cds/gc/hp.h>
#include <cds/intrusive/michael_deque.h>

namespace deque {
#define TEST(X) void IntrusiveDequeHeaderTest::test_##X() { test<X>(); }

    namespace {
        typedef cds::gc::HP HP;
        typedef ci::michael_deque::node<HP> node_hp;

        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::base_hook_item< node_hp >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
        > MichaelDeque_HP_default;

        /// HP + item counter
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::base_hook_item< node_hp >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::sequential_consistent >
        > MichaelDeque_HP_default_ic;

        /// HP + stat
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::base_hook_item< node_hp >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::stat< ci::michael_deque::stat<> >
        > MichaelDeque_HP_default_stat;

        // HP base hook
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::base_hook_item< node_hp >
            ,ci::opt::hook<
                ci::michael_deque::base_hook< ci::opt::gc<HP> >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
        > MichaelDeque_HP_base;

        // HP member hook
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::member_hook_item< node_hp >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_hp >, hMember),
                    ci::opt::gc<HP>
                >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
        > MichaelDeque_HP_member;

        /// HP base hook + item counter
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::base_hook_item< node_hp >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::michael_deque::base_hook< ci::opt::gc<HP> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::relaxed_ordering >
        > MichaelDeque_HP_base_ic;

        // HP member hook + item counter
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::member_hook_item< node_hp >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_hp >, hMember),
                    ci::opt::gc<HP>
                >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
        > MichaelDeque_HP_member_ic;

        // HP base hook + stat
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::base_hook_item< node_hp >
            ,ci::opt::hook<
                ci::michael_deque::base_hook< ci::opt::gc<HP> >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::stat< ci::michael_deque::stat<> >
        > MichaelDeque_HP_base_stat;

        // HP member hook + stat
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::member_hook_item< node_hp >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_hp >, hMember),
                    ci::opt::gc<HP>
                >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::stat< ci::michael_deque::stat<> >
        > MichaelDeque_HP_member_stat;

        // HP base hook + alignment
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::base_hook_item< node_hp >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::michael_deque::base_hook< ci::opt::gc<HP> >
            >
            ,co::alignment< 32 >
        > MichaelDeque_HP_base_align;

        // HP member hook + alignment
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::member_hook_item< node_hp >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_hp >, hMember),
                    ci::opt::gc<HP>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
        > MichaelDeque_HP_member_align;

        // HP base hook + no alignment
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::base_hook_item< node_hp >
            ,ci::opt::hook<
                ci::michael_deque::base_hook< ci::opt::gc<HP> >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > MichaelDeque_HP_base_noalign;

        // HP member hook + no alignment
        typedef ci::MichaelDeque< HP,
            IntrusiveDequeHeaderTest::member_hook_item< node_hp >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_hp >, hMember),
                    ci::opt::gc<HP>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > MichaelDeque_HP_member_noalign;

    }   // namespace

    TEST( MichaelDeque_HP_default)
    TEST( MichaelDeque_HP_default_ic)
    TEST( MichaelDeque_HP_default_stat)
    TEST( MichaelDeque_HP_base)
    TEST( MichaelDeque_HP_member)
    TEST( MichaelDeque_HP_base_ic)
    TEST( MichaelDeque_HP_member_ic)
    TEST( MichaelDeque_HP_base_stat)
    TEST( MichaelDeque_HP_member_stat)
    TEST( MichaelDeque_HP_base_align)
    TEST( MichaelDeque_HP_member_align)
    TEST( MichaelDeque_HP_base_noalign)
    TEST( MichaelDeque_HP_member_noalign)

} // namespace deque

CPPUNIT_TEST_SUITE_REGISTRATION(deque::IntrusiveDequeHeaderTest);

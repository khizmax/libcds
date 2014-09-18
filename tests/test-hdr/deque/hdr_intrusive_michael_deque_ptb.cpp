//$$CDS-header$$

#include "hdr_intrusive_deque.h"
#include <cds/gc/ptb.h>
#include <cds/intrusive/michael_deque.h>

namespace deque {
#define TEST(X) void IntrusiveDequeHeaderTest::test_##X() { test<X>(); }

    namespace {
        typedef cds::gc::PTB PTB;
        typedef ci::michael_deque::node<PTB> node_ptb;

        // PTB base hook
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::base_hook_item< node_ptb >
            ,ci::opt::hook<
            ci::michael_deque::base_hook< ci::opt::gc<PTB> >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
        > MichaelDeque_PTB_base;

        // PTB member hook
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::member_hook_item< node_ptb >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_ptb >, hMember),
                    ci::opt::gc<PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
        > MichaelDeque_PTB_member;

        /// PTB base hook + item counter
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::base_hook_item< node_ptb >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::michael_deque::base_hook< ci::opt::gc<PTB> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::relaxed_ordering >
        > MichaelDeque_PTB_base_ic;

        // PTB member hook + item counter
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::member_hook_item< node_ptb >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_ptb >, hMember),
                    ci::opt::gc<PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
        > MichaelDeque_PTB_member_ic;

        // PTB base hook + stat
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::base_hook_item< node_ptb >
            ,ci::opt::hook<
                ci::michael_deque::base_hook< ci::opt::gc<PTB> >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::stat< ci::michael_deque::stat<> >
        > MichaelDeque_PTB_base_stat;

        // PTB member hook + stat
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::member_hook_item< node_ptb >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_ptb >, hMember),
                    ci::opt::gc<PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::stat< ci::michael_deque::stat<> >
        > MichaelDeque_PTB_member_stat;

        // PTB base hook + alignment
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::base_hook_item< node_ptb >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::michael_deque::base_hook< ci::opt::gc<PTB> >
            >
            ,co::alignment< 32 >
        > MichaelDeque_PTB_base_align;

        // PTB member hook + alignment
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::member_hook_item< node_ptb >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_ptb >, hMember),
                    ci::opt::gc<PTB>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
        > MichaelDeque_PTB_member_align;

        // PTB base hook + no alignment
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::base_hook_item< node_ptb >
            ,ci::opt::hook<
                ci::michael_deque::base_hook< ci::opt::gc<PTB> >
            >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > MichaelDeque_PTB_base_noalign;

        // PTB member hook + no alignment
        typedef ci::MichaelDeque< PTB,
            IntrusiveDequeHeaderTest::member_hook_item< node_ptb >
            ,ci::opt::disposer< IntrusiveDequeHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::michael_deque::member_hook<
                    offsetof(IntrusiveDequeHeaderTest::member_hook_item< node_ptb >, hMember),
                    ci::opt::gc<PTB>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > MichaelDeque_PTB_member_noalign;
    }   // namespace

    TEST( MichaelDeque_PTB_base)
    TEST( MichaelDeque_PTB_member)
    TEST( MichaelDeque_PTB_base_ic)
    TEST( MichaelDeque_PTB_member_ic)
    TEST( MichaelDeque_PTB_base_stat)
    TEST( MichaelDeque_PTB_member_stat)
    TEST( MichaelDeque_PTB_base_align)
    TEST( MichaelDeque_PTB_member_align)
    TEST( MichaelDeque_PTB_base_noalign)
    TEST( MichaelDeque_PTB_member_noalign)


} // namespace deque


//
// Created by pasha on 24.12.17.
//

#ifndef MDP_ELLEN_BEEN_TREE_TEST_INTRUSIVE_ELLEN_BINTREE_NOGC_H
#define MDP_ELLEN_BEEN_TREE_TEST_INTRUSIVE_ELLEN_BINTREE_NOGC_H
#include "cds/intrusive/ellen_bintree_nogc.h"
#include "test_intrusive_tree_nogc.h"


namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::nogc gc_type;
    class IntrusiveEllenBinTreeNogc: public cds_test::intrusive_tree_nogc
    {
    public:
        typedef intrusive_tree base_class;

        typedef base_class::key_type key_type;
        typedef typename base_class::base_int_item< ci::ellen_bintree::node<gc_type>> base_item_type;
        typedef typename base_class::member_int_item< ci::ellen_bintree::node<gc_type>> member_item_type;

        struct generic_traits: public ci::ellen_bintree::traits
        {
            typedef base_class::key_extractor key_extractor;
            typedef mock_disposer disposer;
        };
    };

        TEST_F( IntrusiveEllenBinTreeNogc, base_cmp )
        {
            typedef ci::EllenBinTree<gc_type, key_type, base_item_type,
                    ci::ellen_bintree::make_traits<
                            ci::opt::type_traits< generic_traits >
                            ,ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >>>
                            ,ci::opt::compare< cmp<base_item_type>>
            >::type
            > tree_type;

            tree_type t;
            test( t );
        }

}

#endif //MDP_ELLEN_BEEN_TREE_TEST_INTRUSIVE_ELLEN_BINTREE_NOGC_H

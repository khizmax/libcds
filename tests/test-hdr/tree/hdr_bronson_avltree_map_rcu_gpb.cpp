//$$CDS-header$$

#include "tree/hdr_bronson_avltree_map.h"
#include <cds/urcu/general_buffered.h>
#include <cds/container/bronson_avltree_map_rcu.h>

#include "unit/print_bronsonavltree_stat.h"

namespace tree {
    namespace cc = cds::container;
    namespace co = cds::opt;
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;

        struct print_stat {
            template <typename Tree>
            void operator()( Tree const& t )
            {
                std::cout << t.statistics();
            }
        };
    } // namespace

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_gpb_less()
    {
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type,
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
            >::type
        > map_type;

        test<map_type, print_stat>();

    }

} // namespace tree

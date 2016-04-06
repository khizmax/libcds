CDS_TESTHDR_TREE := \
    tests/test-hdr/tree/hdr_tree_reg.cpp \
    tests/test-hdr/tree/hdr_intrusive_ellen_bintree_hp.cpp \
    tests/test-hdr/tree/hdr_intrusive_ellen_bintree_dhp.cpp \
    tests/test-hdr/tree/hdr_intrusive_ellen_bintree_rcu_gpb.cpp \
    tests/test-hdr/tree/hdr_intrusive_ellen_bintree_rcu_gpi.cpp \
    tests/test-hdr/tree/hdr_intrusive_ellen_bintree_rcu_gpt.cpp \
    tests/test-hdr/tree/hdr_intrusive_ellen_bintree_rcu_shb.cpp \
    tests/test-hdr/tree/hdr_intrusive_ellen_bintree_rcu_sht.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_map_hp.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_map_dhp.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_map_rcu_gpb.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_map_rcu_gpi.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_map_rcu_gpt.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_map_rcu_shb.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_map_rcu_sht.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_set_hp.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_set_dhp.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_set_rcu_gpb.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_set_rcu_gpi.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_set_rcu_gpt.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_set_rcu_shb.cpp \
    tests/test-hdr/tree/hdr_ellenbintree_set_rcu_sht.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_gpb.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_gpb_pool_monitor.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_gpi.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_gpi_pool_monitor.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_gpt.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_gpt_pool_monitor.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_shb.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_shb_pool_monitor.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_sht.cpp \
    tests/test-hdr/tree/hdr_bronson_avltree_map_rcu_sht_pool_monitor.cpp \
    

CDS_TESTHDR_MISC := \
    tests/test-hdr/misc/cxx11_atomic_class.cpp \
    tests/test-hdr/misc/cxx11_atomic_func.cpp \
    tests/test-hdr/misc/find_option.cpp \
    tests/test-hdr/misc/allocator_test.cpp \
    tests/test-hdr/misc/michael_allocator.cpp \
    tests/test-hdr/misc/hash_tuple.cpp \
    tests/test-hdr/misc/bitop_st.cpp \
    tests/test-hdr/misc/split_bitstring.cpp \
    tests/test-hdr/misc/permutation_generator.cpp \
    tests/test-hdr/misc/thread_init_fini.cpp

CDS_TESTHDR_SOURCES := \
    $(CDS_TESTHDR_TREE) \
    $(CDS_TESTHDR_MISC)
    

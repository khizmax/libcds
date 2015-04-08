CDS_TESTHDR_MAP := \
    tests/test-hdr/map/hdr_michael_map_hp.cpp \
    tests/test-hdr/map/hdr_michael_map_dhp.cpp \
    tests/test-hdr/map/hdr_michael_map_rcu_gpi.cpp \
    tests/test-hdr/map/hdr_michael_map_rcu_gpb.cpp \
    tests/test-hdr/map/hdr_michael_map_rcu_gpt.cpp \
    tests/test-hdr/map/hdr_michael_map_rcu_shb.cpp \
    tests/test-hdr/map/hdr_michael_map_rcu_sht.cpp \
    tests/test-hdr/map/hdr_michael_map_nogc.cpp \
    tests/test-hdr/map/hdr_michael_map_lazy_hp.cpp \
    tests/test-hdr/map/hdr_michael_map_lazy_dhp.cpp \
    tests/test-hdr/map/hdr_michael_map_lazy_rcu_gpi.cpp \
    tests/test-hdr/map/hdr_michael_map_lazy_rcu_gpb.cpp \
    tests/test-hdr/map/hdr_michael_map_lazy_rcu_gpt.cpp \
    tests/test-hdr/map/hdr_michael_map_lazy_rcu_shb.cpp \
    tests/test-hdr/map/hdr_michael_map_lazy_rcu_sht.cpp \
    tests/test-hdr/map/hdr_michael_map_lazy_nogc.cpp \
    tests/test-hdr/map/hdr_refinable_hashmap_hashmap_std.cpp \
    tests/test-hdr/map/hdr_refinable_hashmap_boost_list.cpp \
    tests/test-hdr/map/hdr_refinable_hashmap_list.cpp \
    tests/test-hdr/map/hdr_refinable_hashmap_map.cpp \
    tests/test-hdr/map/hdr_refinable_hashmap_boost_map.cpp \
    tests/test-hdr/map/hdr_refinable_hashmap_boost_flat_map.cpp \
    tests/test-hdr/map/hdr_refinable_hashmap_boost_unordered_map.cpp \
    tests/test-hdr/map/hdr_refinable_hashmap_slist.cpp \
    tests/test-hdr/map/hdr_skiplist_map_hp.cpp \
    tests/test-hdr/map/hdr_skiplist_map_dhp.cpp \
    tests/test-hdr/map/hdr_skiplist_map_rcu_gpi.cpp \
    tests/test-hdr/map/hdr_skiplist_map_rcu_gpb.cpp \
    tests/test-hdr/map/hdr_skiplist_map_rcu_gpt.cpp \
    tests/test-hdr/map/hdr_skiplist_map_rcu_shb.cpp \
    tests/test-hdr/map/hdr_skiplist_map_rcu_sht.cpp \
    tests/test-hdr/map/hdr_skiplist_map_nogc.cpp \
    tests/test-hdr/map/hdr_splitlist_map_hp.cpp \
    tests/test-hdr/map/hdr_splitlist_map_dhp.cpp \
    tests/test-hdr/map/hdr_splitlist_map_nogc.cpp \
    tests/test-hdr/map/hdr_splitlist_map_rcu_gpi.cpp \
    tests/test-hdr/map/hdr_splitlist_map_rcu_gpb.cpp \
    tests/test-hdr/map/hdr_splitlist_map_rcu_gpt.cpp \
    tests/test-hdr/map/hdr_splitlist_map_rcu_shb.cpp \
    tests/test-hdr/map/hdr_splitlist_map_rcu_sht.cpp \
    tests/test-hdr/map/hdr_splitlist_map_lazy_hp.cpp \
    tests/test-hdr/map/hdr_splitlist_map_lazy_dhp.cpp \
    tests/test-hdr/map/hdr_splitlist_map_lazy_nogc.cpp \
    tests/test-hdr/map/hdr_splitlist_map_lazy_rcu_gpi.cpp \
    tests/test-hdr/map/hdr_splitlist_map_lazy_rcu_gpb.cpp \
    tests/test-hdr/map/hdr_splitlist_map_lazy_rcu_gpt.cpp \
    tests/test-hdr/map/hdr_splitlist_map_lazy_rcu_sht.cpp \
    tests/test-hdr/map/hdr_splitlist_map_lazy_rcu_shb.cpp \
    tests/test-hdr/map/hdr_striped_hashmap_hashmap_std.cpp \
    tests/test-hdr/map/hdr_striped_hashmap_boost_list.cpp \
    tests/test-hdr/map/hdr_striped_hashmap_list.cpp \
    tests/test-hdr/map/hdr_striped_hashmap_map.cpp \
    tests/test-hdr/map/hdr_striped_hashmap_boost_map.cpp \
    tests/test-hdr/map/hdr_striped_hashmap_boost_flat_map.cpp \
    tests/test-hdr/map/hdr_striped_hashmap_boost_unordered_map.cpp \
    tests/test-hdr/map/hdr_striped_hashmap_slist.cpp \
    tests/test-hdr/map/hdr_striped_map_reg.cpp

CDS_TESTHDR_DEQUE := \
    tests/test-hdr/deque/hdr_fcdeque.cpp

CDS_TESTHDR_LIST := \
    tests/test-hdr/list/hdr_lazy_dhp.cpp \
    tests/test-hdr/list/hdr_lazy_hp.cpp \
    tests/test-hdr/list/hdr_lazy_nogc.cpp \
    tests/test-hdr/list/hdr_lazy_nogc_unord.cpp \
    tests/test-hdr/list/hdr_lazy_rcu_gpi.cpp \
    tests/test-hdr/list/hdr_lazy_rcu_gpb.cpp \
    tests/test-hdr/list/hdr_lazy_rcu_gpt.cpp \
    tests/test-hdr/list/hdr_lazy_rcu_shb.cpp \
    tests/test-hdr/list/hdr_lazy_rcu_sht.cpp \
    tests/test-hdr/list/hdr_lazy_kv_dhp.cpp \
    tests/test-hdr/list/hdr_lazy_kv_hp.cpp \
    tests/test-hdr/list/hdr_lazy_kv_nogc.cpp \
    tests/test-hdr/list/hdr_lazy_kv_nogc_unord.cpp \
    tests/test-hdr/list/hdr_lazy_kv_rcu_gpb.cpp \
    tests/test-hdr/list/hdr_lazy_kv_rcu_gpi.cpp \
    tests/test-hdr/list/hdr_lazy_kv_rcu_gpt.cpp \
    tests/test-hdr/list/hdr_lazy_kv_rcu_shb.cpp \
    tests/test-hdr/list/hdr_lazy_kv_rcu_sht.cpp \
    tests/test-hdr/list/hdr_michael_dhp.cpp \
    tests/test-hdr/list/hdr_michael_hp.cpp \
    tests/test-hdr/list/hdr_michael_nogc.cpp \
    tests/test-hdr/list/hdr_michael_rcu_gpi.cpp \
    tests/test-hdr/list/hdr_michael_rcu_gpb.cpp \
    tests/test-hdr/list/hdr_michael_rcu_gpt.cpp \
    tests/test-hdr/list/hdr_michael_rcu_shb.cpp \
    tests/test-hdr/list/hdr_michael_rcu_sht.cpp \
    tests/test-hdr/list/hdr_michael_kv_dhp.cpp \
    tests/test-hdr/list/hdr_michael_kv_hp.cpp \
    tests/test-hdr/list/hdr_michael_kv_nogc.cpp \
    tests/test-hdr/list/hdr_michael_kv_rcu_gpi.cpp \
    tests/test-hdr/list/hdr_michael_kv_rcu_gpb.cpp \
    tests/test-hdr/list/hdr_michael_kv_rcu_gpt.cpp \
    tests/test-hdr/list/hdr_michael_kv_rcu_shb.cpp \
    tests/test-hdr/list/hdr_michael_kv_rcu_sht.cpp

CDS_TESTHDR_PQUEUE := \
    tests/test-hdr/priority_queue/hdr_intrusive_mspqueue_dyn.cpp \
    tests/test-hdr/priority_queue/hdr_intrusive_mspqueue_static.cpp \
    tests/test-hdr/priority_queue/hdr_mspqueue_dyn.cpp \
    tests/test-hdr/priority_queue/hdr_mspqueue_static.cpp \
    tests/test-hdr/priority_queue/hdr_fcpqueue_boost_stable_vector.cpp \
    tests/test-hdr/priority_queue/hdr_fcpqueue_deque.cpp \
    tests/test-hdr/priority_queue/hdr_fcpqueue_vector.cpp \
    tests/test-hdr/priority_queue/hdr_priority_queue_reg.cpp

CDS_TESTHDR_QUEUE := \
    tests/test-hdr/queue/hdr_queue_register.cpp \
    tests/test-hdr/queue/hdr_intrusive_fcqueue.cpp \
    tests/test-hdr/queue/hdr_intrusive_segmented_queue_hp.cpp \
    tests/test-hdr/queue/hdr_intrusive_segmented_queue_dhp.cpp \
    tests/test-hdr/queue/hdr_intrusive_tsigas_cycle_queue.cpp \
    tests/test-hdr/queue/hdr_intrusive_vyukovmpmc_cycle_queue.cpp \
    tests/test-hdr/queue/hdr_basketqueue_hp.cpp \
    tests/test-hdr/queue/hdr_basketqueue_dhp.cpp \
    tests/test-hdr/queue/hdr_fcqueue.cpp \
    tests/test-hdr/queue/hdr_moirqueue_hp.cpp \
    tests/test-hdr/queue/hdr_moirqueue_dhp.cpp \
    tests/test-hdr/queue/hdr_msqueue_hp.cpp \
    tests/test-hdr/queue/hdr_msqueue_dhp.cpp \
    tests/test-hdr/queue/hdr_optimistic_hp.cpp \
    tests/test-hdr/queue/hdr_optimistic_dhp.cpp \
    tests/test-hdr/queue/hdr_rwqueue.cpp \
    tests/test-hdr/queue/hdr_segmented_queue_hp.cpp \
    tests/test-hdr/queue/hdr_segmented_queue_dhp.cpp \
    tests/test-hdr/queue/hdr_tsigas_cycle_queue.cpp \
    tests/test-hdr/queue/hdr_vyukov_mpmc_cyclic.cpp

CDS_TESTHDR_SET := \
    tests/test-hdr/set/hdr_intrusive_refinable_hashset_avlset.cpp \
    tests/test-hdr/set/hdr_intrusive_refinable_hashset_list.cpp \
    tests/test-hdr/set/hdr_intrusive_refinable_hashset_set.cpp \
    tests/test-hdr/set/hdr_intrusive_refinable_hashset_sgset.cpp \
    tests/test-hdr/set/hdr_intrusive_refinable_hashset_slist.cpp \
    tests/test-hdr/set/hdr_intrusive_refinable_hashset_splayset.cpp \
    tests/test-hdr/set/hdr_intrusive_refinable_hashset_treapset.cpp \
    tests/test-hdr/set/hdr_intrusive_refinable_hashset_uset.cpp \
    tests/test-hdr/set/hdr_intrusive_skiplist_hp.cpp \
    tests/test-hdr/set/hdr_intrusive_skiplist_dhp.cpp \
    tests/test-hdr/set/hdr_intrusive_skiplist_rcu_gpb.cpp \
    tests/test-hdr/set/hdr_intrusive_skiplist_rcu_gpi.cpp \
    tests/test-hdr/set/hdr_intrusive_skiplist_rcu_gpt.cpp \
    tests/test-hdr/set/hdr_intrusive_skiplist_rcu_shb.cpp \
    tests/test-hdr/set/hdr_intrusive_skiplist_rcu_sht.cpp \
    tests/test-hdr/set/hdr_intrusive_skiplist_nogc.cpp \
    tests/test-hdr/set/hdr_intrusive_striped_hashset_avlset.cpp \
    tests/test-hdr/set/hdr_intrusive_striped_hashset_list.cpp \
    tests/test-hdr/set/hdr_intrusive_striped_hashset_set.cpp \
    tests/test-hdr/set/hdr_intrusive_striped_hashset_sgset.cpp \
    tests/test-hdr/set/hdr_intrusive_striped_hashset_slist.cpp \
    tests/test-hdr/set/hdr_intrusive_striped_hashset_splayset.cpp \
    tests/test-hdr/set/hdr_intrusive_striped_hashset_treapset.cpp \
    tests/test-hdr/set/hdr_intrusive_striped_hashset_uset.cpp \
    tests/test-hdr/set/hdr_intrusive_striped_set.cpp \
    tests/test-hdr/set/hdr_michael_set_hp.cpp \
    tests/test-hdr/set/hdr_michael_set_dhp.cpp \
    tests/test-hdr/set/hdr_michael_set_rcu_gpi.cpp \
    tests/test-hdr/set/hdr_michael_set_rcu_gpb.cpp \
    tests/test-hdr/set/hdr_michael_set_rcu_gpt.cpp \
    tests/test-hdr/set/hdr_michael_set_rcu_shb.cpp \
    tests/test-hdr/set/hdr_michael_set_rcu_sht.cpp \
    tests/test-hdr/set/hdr_michael_set_nogc.cpp \
    tests/test-hdr/set/hdr_michael_set_lazy_hp.cpp \
    tests/test-hdr/set/hdr_michael_set_lazy_dhp.cpp \
    tests/test-hdr/set/hdr_michael_set_lazy_rcu_gpi.cpp \
    tests/test-hdr/set/hdr_michael_set_lazy_rcu_gpb.cpp \
    tests/test-hdr/set/hdr_michael_set_lazy_rcu_gpt.cpp \
    tests/test-hdr/set/hdr_michael_set_lazy_rcu_shb.cpp \
    tests/test-hdr/set/hdr_michael_set_lazy_rcu_sht.cpp \
    tests/test-hdr/set/hdr_michael_set_lazy_nogc.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_hashset_std.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_boost_flat_set.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_boost_list.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_boost_set.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_boost_stable_vector.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_boost_unordered_set.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_boost_vector.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_list.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_set.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_slist.cpp \
    tests/test-hdr/set/hdr_refinable_hashset_vector.cpp \
    tests/test-hdr/set/hdr_skiplist_set_hp.cpp \
    tests/test-hdr/set/hdr_skiplist_set_dhp.cpp \
    tests/test-hdr/set/hdr_skiplist_set_rcu_gpi.cpp \
    tests/test-hdr/set/hdr_skiplist_set_rcu_gpb.cpp \
    tests/test-hdr/set/hdr_skiplist_set_rcu_gpt.cpp \
    tests/test-hdr/set/hdr_skiplist_set_rcu_shb.cpp \
    tests/test-hdr/set/hdr_skiplist_set_rcu_sht.cpp \
    tests/test-hdr/set/hdr_skiplist_set_nogc.cpp \
    tests/test-hdr/set/hdr_splitlist_set_hp.cpp \
    tests/test-hdr/set/hdr_splitlist_set_nogc.cpp \
    tests/test-hdr/set/hdr_splitlist_set_dhp.cpp \
    tests/test-hdr/set/hdr_splitlist_set_rcu_gpi.cpp \
    tests/test-hdr/set/hdr_splitlist_set_rcu_gpb.cpp \
    tests/test-hdr/set/hdr_splitlist_set_rcu_gpt.cpp \
    tests/test-hdr/set/hdr_splitlist_set_rcu_shb.cpp \
    tests/test-hdr/set/hdr_splitlist_set_rcu_sht.cpp \
    tests/test-hdr/set/hdr_splitlist_set_lazy_hp.cpp \
    tests/test-hdr/set/hdr_splitlist_set_lazy_nogc.cpp \
    tests/test-hdr/set/hdr_splitlist_set_lazy_dhp.cpp \
    tests/test-hdr/set/hdr_splitlist_set_lazy_rcu_gpi.cpp \
    tests/test-hdr/set/hdr_splitlist_set_lazy_rcu_gpb.cpp \
    tests/test-hdr/set/hdr_splitlist_set_lazy_rcu_gpt.cpp \
    tests/test-hdr/set/hdr_splitlist_set_lazy_rcu_shb.cpp \
    tests/test-hdr/set/hdr_splitlist_set_lazy_rcu_sht.cpp \
    tests/test-hdr/set/hdr_striped_hashset_hashset_std.cpp \
    tests/test-hdr/set/hdr_striped_hashset_boost_flat_set.cpp \
    tests/test-hdr/set/hdr_striped_hashset_boost_list.cpp \
    tests/test-hdr/set/hdr_striped_hashset_boost_set.cpp \
    tests/test-hdr/set/hdr_striped_hashset_boost_stable_vector.cpp \
    tests/test-hdr/set/hdr_striped_hashset_boost_unordered_set.cpp \
    tests/test-hdr/set/hdr_striped_hashset_boost_vector.cpp \
    tests/test-hdr/set/hdr_striped_hashset_list.cpp \
    tests/test-hdr/set/hdr_striped_hashset_set.cpp \
    tests/test-hdr/set/hdr_striped_hashset_slist.cpp \
    tests/test-hdr/set/hdr_striped_hashset_vector.cpp 

CDS_TESTHDR_STACK := \
    tests/test-hdr/stack/hdr_intrusive_fcstack.cpp \
    tests/test-hdr/stack/hdr_treiber_stack_hp.cpp \
    tests/test-hdr/stack/hdr_treiber_stack_dhp.cpp \
    tests/test-hdr/stack/hdr_elimination_stack_hp.cpp \
    tests/test-hdr/stack/hdr_elimination_stack_dhp.cpp \
    tests/test-hdr/stack/hdr_fcstack.cpp

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
    tests/test-hdr/misc/permutation_generator.cpp \
    tests/test-hdr/misc/thread_init_fini.cpp

CDS_TESTHDR_SOURCES := \
    $(CDS_TESTHDR_QUEUE) \
    $(CDS_TESTHDR_PQUEUE) \
    $(CDS_TESTHDR_STACK) \
    $(CDS_TESTHDR_MAP) \
    $(CDS_TESTHDR_DEQUE) \
    $(CDS_TESTHDR_LIST) \
    $(CDS_TESTHDR_SET) \
    $(CDS_TESTHDR_TREE) \
    $(CDS_TESTHDR_MISC)
    

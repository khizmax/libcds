set(PACKAGE_NAME test-hdr)

set(CDS_TESTHDR_TREE
    tree/hdr_tree_reg.cpp
    tree/hdr_intrusive_ellen_bintree_hp.cpp
    tree/hdr_intrusive_ellen_bintree_dhp.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_gpb.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_gpi.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_gpt.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_shb.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_sht.cpp
    tree/hdr_ellenbintree_map_hp.cpp
    tree/hdr_ellenbintree_map_dhp.cpp
    tree/hdr_ellenbintree_map_rcu_gpb.cpp
    tree/hdr_ellenbintree_map_rcu_gpi.cpp
    tree/hdr_ellenbintree_map_rcu_gpt.cpp
    tree/hdr_ellenbintree_map_rcu_shb.cpp
    tree/hdr_ellenbintree_map_rcu_sht.cpp
    tree/hdr_ellenbintree_set_hp.cpp
    tree/hdr_ellenbintree_set_dhp.cpp
    tree/hdr_ellenbintree_set_rcu_gpb.cpp
    tree/hdr_ellenbintree_set_rcu_gpi.cpp
    tree/hdr_ellenbintree_set_rcu_gpt.cpp
    tree/hdr_ellenbintree_set_rcu_shb.cpp
    tree/hdr_ellenbintree_set_rcu_sht.cpp
    tree/hdr_bronson_avltree_map_rcu_gpb.cpp
    tree/hdr_bronson_avltree_map_rcu_gpb_pool_monitor.cpp
    tree/hdr_bronson_avltree_map_rcu_gpi.cpp
    tree/hdr_bronson_avltree_map_rcu_gpi_pool_monitor.cpp
    tree/hdr_bronson_avltree_map_rcu_gpt.cpp
    tree/hdr_bronson_avltree_map_rcu_gpt_pool_monitor.cpp
    tree/hdr_bronson_avltree_map_rcu_shb.cpp
    tree/hdr_bronson_avltree_map_rcu_shb_pool_monitor.cpp
    tree/hdr_bronson_avltree_map_rcu_sht.cpp
    tree/hdr_bronson_avltree_map_rcu_sht_pool_monitor.cpp
   )

set(CDS_TESTHDR_MISC
    misc/cxx11_atomic_class.cpp
    misc/cxx11_atomic_func.cpp
    misc/find_option.cpp
    misc/allocator_test.cpp
    misc/michael_allocator.cpp
    misc/hash_tuple.cpp
    misc/bitop_st.cpp
    misc/split_bitstring.cpp
    misc/permutation_generator.cpp
    misc/thread_init_fini.cpp)

set(CDS_TESTHDR_OFFSETOF_TREE
    tree/hdr_intrusive_ellen_bintree_hp_member.cpp
    tree/hdr_intrusive_ellen_bintree_dhp_member.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_gpb_member.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_gpi_member.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_gpt_member.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_shb_member.cpp
    tree/hdr_intrusive_ellen_bintree_rcu_sht_member.cpp)

set(CDS_TESTHDR_OFFSETOF_SOURCES
	${CDS_TESTHDR_OFFSETOF_TREE})

add_library(test-hdr-offsetof OBJECT ${CDS_TESTHDR_OFFSETOF_SOURCES})

if(CMAKE_COMPILER_IS_GNUCC)
  set_property(TARGET test-hdr-offsetof APPEND_STRING PROPERTY COMPILE_FLAGS -Wno-invalid-offsetof)
endif(CMAKE_COMPILER_IS_GNUCC)

set(CDS_TESTHDR_SOURCES
    ${CDS_TESTHDR_TREE}
    ${CDS_TESTHDR_MISC})

add_executable(${PACKAGE_NAME} ${CDS_TESTHDR_SOURCES} $<TARGET_OBJECTS:test-hdr-offsetof> $<TARGET_OBJECTS:${TEST_COMMON}>)
target_link_libraries(${PACKAGE_NAME} ${CDS_SHARED_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(PACKAGE_NAME unit-tree)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof")
 
set(CDSGTEST_TREE_SOURCES
    ../main.cpp
    bronson_avltree_map_rcu_gpb.cpp
    bronson_avltree_map_rcu_gpi.cpp
    bronson_avltree_map_rcu_gpt.cpp
    bronson_avltree_map_rcu_shb.cpp
    bronson_avltree_map_rcu_sht.cpp
    bronson_avltree_map_ptr_rcu_gpb.cpp
    bronson_avltree_map_ptr_rcu_gpi.cpp
    bronson_avltree_map_ptr_rcu_gpt.cpp
    bronson_avltree_map_ptr_rcu_shb.cpp
    bronson_avltree_map_ptr_rcu_sht.cpp
    ellen_bintree_update_desc_pool.cpp
    ellen_bintree_map_hp.cpp
    ellen_bintree_map_dhp.cpp
    ellen_bintree_map_rcu_gpb.cpp
    ellen_bintree_map_rcu_gpi.cpp
    ellen_bintree_map_rcu_gpt.cpp
    ellen_bintree_map_rcu_shb.cpp
    ellen_bintree_map_rcu_sht.cpp
    ellen_bintree_set_dhp.cpp
    ellen_bintree_set_hp.cpp
    ellen_bintree_set_rcu_gpb.cpp
    ellen_bintree_set_rcu_gpi.cpp
    ellen_bintree_set_rcu_gpt.cpp
    ellen_bintree_set_rcu_shb.cpp
    ellen_bintree_set_rcu_sht.cpp
    intrusive_ellenbintree_hp.cpp
    intrusive_ellenbintree_dhp.cpp
    intrusive_ellenbintree_rcu_gpb.cpp
    intrusive_ellenbintree_rcu_gpi.cpp
    intrusive_ellenbintree_rcu_gpt.cpp
    intrusive_ellenbintree_rcu_shb.cpp
    intrusive_ellenbintree_rcu_sht.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_TREE_SOURCES})
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
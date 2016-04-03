set(PACKAGE_NAME unit-map)

set(CDSGTEST_MAP_SOURCES
    ../main.cpp
    feldman_hashmap_hp.cpp
    feldman_hashmap_dhp.cpp
    feldman_hashset_rcu_gpb.cpp
    feldman_hashset_rcu_gpi.cpp
    feldman_hashset_rcu_gpt.cpp
    feldman_hashset_rcu_shb.cpp
    feldman_hashset_rcu_sht.cpp
    michael_lazy_hp.cpp
    michael_lazy_dhp.cpp
    michael_lazy_nogc.cpp
    michael_lazy_rcu_gpb.cpp
    michael_lazy_rcu_gpi.cpp
    michael_lazy_rcu_gpt.cpp
    michael_lazy_rcu_shb.cpp
    michael_lazy_rcu_sht.cpp
    michael_michael_hp.cpp
    michael_michael_dhp.cpp
    michael_michael_nogc.cpp
    michael_michael_rcu_gpb.cpp
    michael_michael_rcu_gpi.cpp
    michael_michael_rcu_gpt.cpp
    michael_michael_rcu_shb.cpp
    michael_michael_rcu_sht.cpp
    skiplist_hp.cpp
    skiplist_dhp.cpp
    skiplist_nogc.cpp
    skiplist_rcu_gpb.cpp
    skiplist_rcu_gpi.cpp
    skiplist_rcu_gpt.cpp
    skiplist_rcu_shb.cpp
    skiplist_rcu_sht.cpp
    split_lazy_hp.cpp
    split_lazy_dhp.cpp
    split_lazy_nogc.cpp
    split_lazy_rcu_gpb.cpp
    split_lazy_rcu_gpi.cpp
    split_lazy_rcu_gpt.cpp
    split_lazy_rcu_shb.cpp
    split_lazy_rcu_sht.cpp
    split_michael_hp.cpp
    split_michael_dhp.cpp
    split_michael_nogc.cpp
    split_michael_rcu_gpb.cpp
    split_michael_rcu_gpi.cpp
    split_michael_rcu_gpt.cpp
    split_michael_rcu_shb.cpp
    split_michael_rcu_sht.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_MAP_SOURCES})
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
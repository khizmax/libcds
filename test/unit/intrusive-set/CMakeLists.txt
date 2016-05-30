set(PACKAGE_NAME unit-intrusive-set)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof")

set(CDSGTEST_SET_SOURCES
    ../main.cpp
    intrusive_feldman_hashset_hp.cpp
    intrusive_feldman_hashset_dhp.cpp
    intrusive_feldman_hashset_rcu_gpb.cpp
    intrusive_feldman_hashset_rcu_gpi.cpp
    intrusive_feldman_hashset_rcu_gpt.cpp
    intrusive_feldman_hashset_rcu_shb.cpp
    intrusive_feldman_hashset_rcu_sht.cpp
    intrusive_michael_lazy_hp.cpp
    intrusive_michael_lazy_dhp.cpp
    intrusive_michael_lazy_nogc.cpp
    intrusive_michael_lazy_rcu_gpb.cpp
    intrusive_michael_lazy_rcu_gpi.cpp
    intrusive_michael_lazy_rcu_gpt.cpp
    intrusive_michael_lazy_rcu_shb.cpp
    intrusive_michael_lazy_rcu_sht.cpp
    intrusive_michael_michael_hp.cpp
    intrusive_michael_michael_dhp.cpp
    intrusive_michael_michael_nogc.cpp
    intrusive_michael_michael_rcu_gpb.cpp
    intrusive_michael_michael_rcu_gpi.cpp
    intrusive_michael_michael_rcu_gpt.cpp
    intrusive_michael_michael_rcu_shb.cpp
    intrusive_michael_michael_rcu_sht.cpp
    intrusive_skiplist_hp.cpp
    intrusive_skiplist_dhp.cpp
    intrusive_skiplist_nogc.cpp
    intrusive_skiplist_rcu_gpb.cpp
    intrusive_skiplist_rcu_gpi.cpp
    intrusive_skiplist_rcu_gpt.cpp
    intrusive_skiplist_rcu_shb.cpp
    intrusive_skiplist_rcu_sht.cpp
    intrusive_split_lazy_hp.cpp
    intrusive_split_lazy_dhp.cpp
    intrusive_split_lazy_nogc.cpp
    intrusive_split_lazy_rcu_gpb.cpp
    intrusive_split_lazy_rcu_gpi.cpp
    intrusive_split_lazy_rcu_gpt.cpp
    intrusive_split_lazy_rcu_shb.cpp
    intrusive_split_lazy_rcu_sht.cpp
    intrusive_split_michael_hp.cpp
    intrusive_split_michael_dhp.cpp
    intrusive_split_michael_nogc.cpp
    intrusive_split_michael_rcu_gpb.cpp
    intrusive_split_michael_rcu_gpi.cpp
    intrusive_split_michael_rcu_gpt.cpp
    intrusive_split_michael_rcu_shb.cpp
    intrusive_split_michael_rcu_sht.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_SET_SOURCES})
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
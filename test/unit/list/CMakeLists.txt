set(PACKAGE_NAME unit-list)

set(CDSGTEST_LIST_SOURCES
    ../main.cpp
    intrusive_lazy_hp.cpp
    intrusive_lazy_dhp.cpp
    intrusive_lazy_nogc.cpp
    intrusive_lazy_rcu_gpb.cpp
    intrusive_lazy_rcu_gpi.cpp
    intrusive_lazy_rcu_gpt.cpp
    intrusive_lazy_rcu_shb.cpp
    intrusive_lazy_rcu_sht.cpp
    intrusive_michael_hp.cpp
    intrusive_michael_dhp.cpp
    intrusive_michael_nogc.cpp
    intrusive_michael_rcu_gpb.cpp
    intrusive_michael_rcu_gpi.cpp
    intrusive_michael_rcu_gpt.cpp
    intrusive_michael_rcu_shb.cpp
    intrusive_michael_rcu_sht.cpp
    kv_lazy_hp.cpp
    kv_lazy_dhp.cpp
    kv_lazy_nogc.cpp
    kv_lazy_rcu_gpb.cpp
    kv_lazy_rcu_gpi.cpp
    kv_lazy_rcu_gpt.cpp
    kv_lazy_rcu_shb.cpp
    kv_lazy_rcu_sht.cpp
    kv_michael_hp.cpp
    kv_michael_dhp.cpp
    kv_michael_nogc.cpp
    kv_michael_rcu_gpb.cpp
    kv_michael_rcu_gpi.cpp
    kv_michael_rcu_gpt.cpp
    kv_michael_rcu_shb.cpp
    kv_michael_rcu_sht.cpp
    lazy_hp.cpp
    lazy_dhp.cpp
    lazy_nogc.cpp
    lazy_rcu_gpb.cpp
    lazy_rcu_gpi.cpp
    lazy_rcu_gpt.cpp
    lazy_rcu_shb.cpp
    lazy_rcu_sht.cpp
    michael_hp.cpp
    michael_dhp.cpp
    michael_nogc.cpp
    michael_rcu_gpb.cpp
    michael_rcu_gpi.cpp
    michael_rcu_gpt.cpp
    michael_rcu_shb.cpp
    michael_rcu_sht.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_LIST_SOURCES})
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# IterableList
set(UNIT_LIST_ITERABLE unit-list-iterable)
set(UNIT_LIST_ITERABLE_SOURCES
    ../main.cpp
    iterable_hp.cpp
    iterable_dhp.cpp
    kv_iterable_hp.cpp
    kv_iterable_dhp.cpp
)
add_executable(${UNIT_LIST_ITERABLE} ${UNIT_LIST_ITERABLE_SOURCES})
target_link_libraries(${UNIT_LIST_ITERABLE} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_LIST_ITERABLE} COMMAND ${UNIT_LIST_ITERABLE} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# LazyList
set(UNIT_LIST_LAZY unit-list-lazy)
set(UNIT_LIST_LAZY_SOURCES
    ../main.cpp
    kv_lazy_hp.cpp
    kv_lazy_dhp.cpp
    kv_lazy_nogc.cpp
    kv_lazy_rcu_gpb.cpp
    kv_lazy_rcu_gpi.cpp
    kv_lazy_rcu_gpt.cpp
    kv_lazy_rcu_shb.cpp
    kv_lazy_rcu_sht.cpp
    lazy_hp.cpp
    lazy_dhp.cpp
    lazy_nogc.cpp
    lazy_rcu_gpb.cpp
    lazy_rcu_gpi.cpp
    lazy_rcu_gpt.cpp
    lazy_rcu_shb.cpp
    lazy_rcu_sht.cpp
)
add_executable(${UNIT_LIST_LAZY} ${UNIT_LIST_LAZY_SOURCES})
target_link_libraries(${UNIT_LIST_LAZY} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_LIST_LAZY} COMMAND ${UNIT_LIST_LAZY} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# MichaelList
set(UNIT_LIST_MICHAEL unit-list-michael)
set(UNIT_LIST_MICHAEL_SOURCES
    ../main.cpp
    kv_michael_hp.cpp
    kv_michael_dhp.cpp
    kv_michael_nogc.cpp
    kv_michael_rcu_gpb.cpp
    kv_michael_rcu_gpi.cpp
    kv_michael_rcu_gpt.cpp
    kv_michael_rcu_shb.cpp
    kv_michael_rcu_sht.cpp
    michael_hp.cpp
    michael_dhp.cpp
    michael_nogc.cpp
    michael_rcu_gpb.cpp
    michael_rcu_gpi.cpp
    michael_rcu_gpt.cpp
    michael_rcu_shb.cpp
    michael_rcu_sht.cpp
)
add_executable(${UNIT_LIST_MICHAEL} ${UNIT_LIST_MICHAEL_SOURCES})
target_link_libraries(${UNIT_LIST_MICHAEL} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_LIST_MICHAEL} COMMAND ${UNIT_LIST_MICHAEL} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})


add_custom_target( unit-list
    DEPENDS
        ${UNIT_LIST_ITERABLE}
        ${UNIT_LIST_LAZY}
        ${UNIT_LIST_MICHAEL}
)


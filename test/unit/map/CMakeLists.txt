include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# FeldmanHashMap unit test
set(UNIT_MAP_FELDMAN unit-map-feldman)
set(UNIT_MAP_FELDMAN_SOURCES 
    ../main.cpp
    feldman_hashmap_hp.cpp
    feldman_hashmap_dhp.cpp
    feldman_hashset_rcu_gpb.cpp
    feldman_hashset_rcu_gpi.cpp
    feldman_hashset_rcu_gpt.cpp
    feldman_hashset_rcu_shb.cpp
    feldman_hashset_rcu_sht.cpp
)
add_executable(${UNIT_MAP_FELDMAN} ${UNIT_MAP_FELDMAN_SOURCES})
target_link_libraries(${UNIT_MAP_FELDMAN}
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_MAP_FELDMAN} COMMAND ${UNIT_MAP_FELDMAN} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# MichaelHashMap<MichaelList> unit test
set(UNIT_MAP_MICHAEL unit-map-michael)
set(UNIT_MAP_MICHAEL_SOURCES 
    ../main.cpp
    michael_michael_hp.cpp
    michael_michael_dhp.cpp
    michael_michael_nogc.cpp
    michael_michael_rcu_gpb.cpp
    michael_michael_rcu_gpi.cpp
    michael_michael_rcu_gpt.cpp
    michael_michael_rcu_shb.cpp
    michael_michael_rcu_sht.cpp
)
add_executable(${UNIT_MAP_MICHAEL} ${UNIT_MAP_MICHAEL_SOURCES})
target_link_libraries(${UNIT_MAP_MICHAEL}
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_MAP_MICHAEL} COMMAND ${UNIT_MAP_MICHAEL} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# MichaelHashMap<IterableList> unit test
set(UNIT_MAP_MICHAEL_ITERABLE unit-map-michael-iterable)
set(UNIT_MAP_MICHAEL_ITERABLE_SOURCES 
    ../main.cpp
    michael_iterable_hp.cpp
    michael_iterable_dhp.cpp
)
add_executable(${UNIT_MAP_MICHAEL_ITERABLE} ${UNIT_MAP_MICHAEL_ITERABLE_SOURCES})
target_link_libraries(${UNIT_MAP_MICHAEL_ITERABLE}
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_MAP_MICHAEL_ITERABLE} COMMAND ${UNIT_MAP_MICHAEL_ITERABLE} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# MichaelHashMap<LazyList> unit test
set(UNIT_MAP_MICHAEL_LAZY unit-map-michael-lazy)
set(UNIT_MAP_MICHAEL_LAZY_SOURCES 
    ../main.cpp
    michael_lazy_hp.cpp
    michael_lazy_dhp.cpp
    michael_lazy_nogc.cpp
    michael_lazy_rcu_gpb.cpp
    michael_lazy_rcu_gpi.cpp
    michael_lazy_rcu_gpt.cpp
    michael_lazy_rcu_shb.cpp
    michael_lazy_rcu_sht.cpp
)
add_executable(${UNIT_MAP_MICHAEL_LAZY} ${UNIT_MAP_MICHAEL_LAZY_SOURCES})
target_link_libraries(${UNIT_MAP_MICHAEL_LAZY}
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_MAP_MICHAEL_LAZY} COMMAND ${UNIT_MAP_MICHAEL_LAZY} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# SkipListMap unit test
set(UNIT_MAP_SKIP_LIST unit-map-skip)
set(UNIT_MAP_SKIP_LIST_SOURCES 
    ../main.cpp
    skiplist_hp.cpp
    skiplist_dhp.cpp
    skiplist_nogc.cpp
    skiplist_rcu_gpb.cpp
    skiplist_rcu_gpi.cpp
    skiplist_rcu_gpt.cpp
    skiplist_rcu_shb.cpp
    skiplist_rcu_sht.cpp
)
add_executable(${UNIT_MAP_SKIP_LIST} ${UNIT_MAP_SKIP_LIST_SOURCES})
target_link_libraries(${UNIT_MAP_SKIP_LIST}
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_MAP_SKIP_LIST} COMMAND ${UNIT_MAP_SKIP_LIST} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# SplitListMap<MichaelList> unit test
set(UNIT_MAP_SPLIT_MICHAEL unit-map-split-michael)
set(UNIT_MAP_SPLIT_MICHAEL_SOURCES 
    ../main.cpp
    split_michael_hp.cpp
    split_michael_dhp.cpp
    split_michael_nogc.cpp
    split_michael_rcu_gpb.cpp
    split_michael_rcu_gpi.cpp
    split_michael_rcu_gpt.cpp
    split_michael_rcu_shb.cpp
    split_michael_rcu_sht.cpp
)
add_executable(${UNIT_MAP_SPLIT_MICHAEL} ${UNIT_MAP_SPLIT_MICHAEL_SOURCES})
target_link_libraries(${UNIT_MAP_SPLIT_MICHAEL}
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_MAP_SPLIT_MICHAEL} COMMAND ${UNIT_MAP_SPLIT_MICHAEL} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# SplitListMap<IterableList> unit test
set(UNIT_MAP_SPLIT_ITERABLE unit-map-split-iterable)
set(UNIT_MAP_SPLIT_ITERABLE_SOURCES 
    ../main.cpp
    split_iterable_hp.cpp
    split_iterable_dhp.cpp
)
add_executable(${UNIT_MAP_SPLIT_ITERABLE} ${UNIT_MAP_SPLIT_ITERABLE_SOURCES})
target_link_libraries(${UNIT_MAP_SPLIT_ITERABLE}
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_MAP_SPLIT_ITERABLE} COMMAND ${UNIT_MAP_SPLIT_ITERABLE} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# SplitListMap<LazyList> unit test
set(UNIT_MAP_SPLIT_LAZY unit-map-split-lazy)
set(UNIT_MAP_SPLIT_LAZY_SOURCES 
    ../main.cpp
    split_lazy_hp.cpp
    split_lazy_dhp.cpp
    split_lazy_nogc.cpp
    split_lazy_rcu_gpb.cpp
    split_lazy_rcu_gpi.cpp
    split_lazy_rcu_gpt.cpp
    split_lazy_rcu_shb.cpp
    split_lazy_rcu_sht.cpp
)
add_executable(${UNIT_MAP_SPLIT_LAZY} ${UNIT_MAP_SPLIT_LAZY_SOURCES})
target_link_libraries(${UNIT_MAP_SPLIT_LAZY}
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_MAP_SPLIT_LAZY} COMMAND ${UNIT_MAP_SPLIT_LAZY} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})



add_custom_target( unit-map
    DEPENDS
        ${UNIT_MAP_FELDMAN}
        ${UNIT_MAP_MICHAEL}
        ${UNIT_MAP_MICHAEL_ITERABLE}
        ${UNIT_MAP_MICHAEL_LAZY}
        ${UNIT_MAP_SKIP_LIST}
        ${UNIT_MAP_SPLIT_MICHAEL}
        ${UNIT_MAP_SPLIT_ITERABLE}
        ${UNIT_MAP_SPLIT_LAZY}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# FeldmanHashSet
set(UNIT_SET_FELDMAN unit-set-feldman)
set(UNIT_SET_FELDMAN_SOURCES
    ../main.cpp
    feldman_hashset_hp.cpp
    feldman_hashset_dhp.cpp
    feldman_hashset_rcu_gpb.cpp
    feldman_hashset_rcu_gpi.cpp
    feldman_hashset_rcu_gpt.cpp
    feldman_hashset_rcu_shb.cpp
    feldman_hashset_rcu_sht.cpp
)
add_executable(${UNIT_SET_FELDMAN} ${UNIT_SET_FELDMAN_SOURCES})
target_link_libraries(${UNIT_SET_FELDMAN} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_SET_FELDMAN} COMMAND ${UNIT_SET_FELDMAN} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# MichaelSet<MichaelList>
set(UNIT_SET_MICHAEL unit-set-michael)
set(UNIT_SET_MICHAEL_SOURCES
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
add_executable(${UNIT_SET_MICHAEL} ${UNIT_SET_MICHAEL_SOURCES})
target_link_libraries(${UNIT_SET_MICHAEL} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_SET_MICHAEL} COMMAND ${UNIT_SET_MICHAEL} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# MichaelSet<IterableList>
set(UNIT_SET_MICHAEL_ITERABLE unit-set-michael-iterable)
set(UNIT_SET_MICHAEL_ITERABLE_SOURCES
    ../main.cpp
    michael_iterable_hp.cpp
    michael_iterable_dhp.cpp
)
add_executable(${UNIT_SET_MICHAEL_ITERABLE} ${UNIT_SET_MICHAEL_ITERABLE_SOURCES})
target_link_libraries(${UNIT_SET_MICHAEL_ITERABLE} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_SET_MICHAEL_ITERABLE} COMMAND ${UNIT_SET_MICHAEL_ITERABLE} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# MichaelSet<LazyList>
set(UNIT_SET_MICHAEL_LAZY unit-set-michael-lazy)
set(UNIT_SET_MICHAEL_LAZY_SOURCES
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
add_executable(${UNIT_SET_MICHAEL_LAZY} ${UNIT_SET_MICHAEL_LAZY_SOURCES})
target_link_libraries(${UNIT_SET_MICHAEL_LAZY} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_SET_MICHAEL_LAZY} COMMAND ${UNIT_SET_MICHAEL_LAZY} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# SkipListSet
set(UNIT_SET_SKIP unit-set-skip)
set(UNIT_SET_SKIP_SOURCES
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
add_executable(${UNIT_SET_SKIP} ${UNIT_SET_SKIP_SOURCES})
target_link_libraries(${UNIT_SET_SKIP} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_SET_SKIP} COMMAND ${UNIT_SET_SKIP} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# SplitListSet<MichaelList>
set(UNIT_SET_SPLIT_MICHAEL unit-set-split-michael)
set(UNIT_SET_SPLIT_MICHAEL_SOURCES
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
add_executable(${UNIT_SET_SPLIT_MICHAEL} ${UNIT_SET_SPLIT_MICHAEL_SOURCES})
target_link_libraries(${UNIT_SET_SPLIT_MICHAEL} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_SET_SPLIT_MICHAEL} COMMAND ${UNIT_SET_SPLIT_MICHAEL} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# SplitListSet<LazyList>
set(UNIT_SET_SPLIT_LAZY unit-set-split-lazy)
set(UNIT_SET_SPLIT_LAZY_SOURCES
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
add_executable(${UNIT_SET_SPLIT_LAZY} ${UNIT_SET_SPLIT_LAZY_SOURCES})
target_link_libraries(${UNIT_SET_SPLIT_LAZY} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_SET_SPLIT_LAZY} COMMAND ${UNIT_SET_SPLIT_LAZY} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# SplitListSet<IterableList>
set(UNIT_SET_SPLIT_ITERABLE unit-set-split-iterable)
set(UNIT_SET_SPLIT_ITERABLE_SOURCES
    ../main.cpp
    split_iterable_hp.cpp
    split_iterable_dhp.cpp
)
add_executable(${UNIT_SET_SPLIT_ITERABLE} ${UNIT_SET_SPLIT_ITERABLE_SOURCES})
target_link_libraries(${UNIT_SET_SPLIT_ITERABLE} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_SET_SPLIT_ITERABLE} COMMAND ${UNIT_SET_SPLIT_ITERABLE} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})


add_custom_target( unit-set
    DEPENDS
        ${UNIT_SET_FELDMAN}
        ${UNIT_SET_MICHAEL}
        ${UNIT_SET_MICHAEL_ITERABLE}
        ${UNIT_SET_MICHAEL_LAZY}
        ${UNIT_SET_SKIP_LIST}
        ${UNIT_SET_SPLIT_MICHAEL}
        ${UNIT_SET_SPLIT_ITERABLE}
        ${UNIT_SET_SPLIT_LAZY}
)

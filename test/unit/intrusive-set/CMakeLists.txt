include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof")


# intrusive::FeldmanHashSet
set(UNIT_ISET_FELDMAN unit-iset-feldman)
set(UNIT_ISET_FELDMAN_SOURCES
    ../main.cpp
    intrusive_feldman_hashset_hp.cpp
    intrusive_feldman_hashset_dhp.cpp
    intrusive_feldman_hashset_rcu_gpb.cpp
    intrusive_feldman_hashset_rcu_gpi.cpp
    intrusive_feldman_hashset_rcu_gpt.cpp
    intrusive_feldman_hashset_rcu_shb.cpp
    intrusive_feldman_hashset_rcu_sht.cpp
)
add_executable(${UNIT_ISET_FELDMAN} ${UNIT_ISET_FELDMAN_SOURCES})
target_link_libraries(${UNIT_ISET_FELDMAN} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ISET_FELDMAN} COMMAND ${UNIT_ISET_FELDMAN} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# intrusive::MichaelHashSet<MichaelList>
set(UNIT_ISET_MICHAEL unit-iset-michael)
set(UNIT_ISET_MICHAEL_SOURCES
    ../main.cpp
    intrusive_michael_michael_hp.cpp
    intrusive_michael_michael_dhp.cpp
    intrusive_michael_michael_nogc.cpp
    intrusive_michael_michael_rcu_gpb.cpp
    intrusive_michael_michael_rcu_gpi.cpp
    intrusive_michael_michael_rcu_gpt.cpp
    intrusive_michael_michael_rcu_shb.cpp
    intrusive_michael_michael_rcu_sht.cpp
)
add_executable(${UNIT_ISET_MICHAEL} ${UNIT_ISET_MICHAEL_SOURCES})
target_link_libraries(${UNIT_ISET_MICHAEL} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ISET_MICHAEL} COMMAND ${UNIT_ISET_MICHAEL} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# intrusive::MichaelHashSet<LazyList>
set(UNIT_ISET_MICHAEL_LAZY unit-iset-michael-lazy)
set(UNIT_ISET_MICHAEL_LAZY_SOURCES
    ../main.cpp
    intrusive_michael_lazy_hp.cpp
    intrusive_michael_lazy_dhp.cpp
    intrusive_michael_lazy_nogc.cpp
    intrusive_michael_lazy_rcu_gpb.cpp
    intrusive_michael_lazy_rcu_gpi.cpp
    intrusive_michael_lazy_rcu_gpt.cpp
    intrusive_michael_lazy_rcu_shb.cpp
    intrusive_michael_lazy_rcu_sht.cpp
)
add_executable(${UNIT_ISET_MICHAEL_LAZY} ${UNIT_ISET_MICHAEL_LAZY_SOURCES})
target_link_libraries(${UNIT_ISET_MICHAEL_LAZY} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ISET_MICHAEL_LAZY} COMMAND ${UNIT_ISET_MICHAEL_LAZY} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# intrusive::MichaelHashSet<IterableList>
set(UNIT_ISET_MICHAEL_ITERABLE unit-iset-michael-iterable)
set(UNIT_ISET_MICHAEL_ITERABLE_SOURCES
    ../main.cpp
    intrusive_michael_iterable_dhp.cpp
    intrusive_michael_iterable_hp.cpp
)
add_executable(${UNIT_ISET_MICHAEL_ITERABLE} ${UNIT_ISET_MICHAEL_ITERABLE_SOURCES})
target_link_libraries(${UNIT_ISET_MICHAEL_ITERABLE} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ISET_MICHAEL_ITERABLE} COMMAND ${UNIT_ISET_MICHAEL_ITERABLE} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# intrusive::SkipListSet
set(UNIT_ISET_SKIP unit-iset-skip)
set(UNIT_ISET_SKIP_SOURCES
    ../main.cpp
    intrusive_skiplist_hp.cpp
    intrusive_skiplist_dhp.cpp
    intrusive_skiplist_nogc.cpp
    intrusive_skiplist_rcu_gpb.cpp
    intrusive_skiplist_rcu_gpi.cpp
    intrusive_skiplist_rcu_gpt.cpp
    intrusive_skiplist_rcu_shb.cpp
    intrusive_skiplist_rcu_sht.cpp
)
add_executable(${UNIT_ISET_SKIP} ${UNIT_ISET_SKIP_SOURCES})
target_link_libraries(${UNIT_ISET_SKIP} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ISET_SKIP} COMMAND ${UNIT_ISET_SKIP} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# intrusive::SplitListSet<MichaelList>
set(UNIT_ISET_SPLIT_MICHAEL unit-iset-split-michael)
set(UNIT_ISET_SPLIT_MICHAEL_SOURCES
    ../main.cpp
    intrusive_split_michael_hp.cpp
    intrusive_split_michael_dhp.cpp
    intrusive_split_michael_nogc.cpp
    intrusive_split_michael_rcu_gpb.cpp
    intrusive_split_michael_rcu_gpi.cpp
    intrusive_split_michael_rcu_gpt.cpp
    intrusive_split_michael_rcu_shb.cpp
    intrusive_split_michael_rcu_sht.cpp
)
add_executable(${UNIT_ISET_SPLIT_MICHAEL} ${UNIT_ISET_SPLIT_MICHAEL_SOURCES})
target_link_libraries(${UNIT_ISET_SPLIT_MICHAEL} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ISET_SPLIT_MICHAEL} COMMAND ${UNIT_ISET_SPLIT_MICHAEL} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# intrusive::SplitListSet<LazyList>
set(UNIT_ISET_SPLIT_LAZY unit-iset-split-lazy)
set(UNIT_ISET_SPLIT_LAZY_SOURCES
    ../main.cpp
    intrusive_split_lazy_hp.cpp
    intrusive_split_lazy_dhp.cpp
    intrusive_split_lazy_nogc.cpp
    intrusive_split_lazy_rcu_gpb.cpp
    intrusive_split_lazy_rcu_gpi.cpp
    intrusive_split_lazy_rcu_gpt.cpp
    intrusive_split_lazy_rcu_shb.cpp
    intrusive_split_lazy_rcu_sht.cpp
)
add_executable(${UNIT_ISET_SPLIT_LAZY} ${UNIT_ISET_SPLIT_LAZY_SOURCES})
target_link_libraries(${UNIT_ISET_SPLIT_LAZY} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ISET_SPLIT_LAZY} COMMAND ${UNIT_ISET_SPLIT_LAZY} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# intrusive::SplitListSet<IterableList>
set(UNIT_ISET_SPLIT_ITERABLE unit-iset-split-iterable)
set(UNIT_ISET_SPLIT_ITERABLE_SOURCES
    ../main.cpp
    intrusive_split_iterable_hp.cpp
    intrusive_split_iterable_dhp.cpp
)
add_executable(${UNIT_ISET_SPLIT_ITERABLE} ${UNIT_ISET_SPLIT_ITERABLE_SOURCES})
target_link_libraries(${UNIT_ISET_SPLIT_ITERABLE} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ISET_SPLIT_ITERABLE} COMMAND ${UNIT_ISET_SPLIT_ITERABLE} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})



add_custom_target( unit-iset
    DEPENDS
        ${UNIT_ISET_FELDMAN}
        ${UNIT_ISET_MICHAEL}
        ${UNIT_ISET_MICHAEL_ITERABLE}
        ${UNIT_ISET_MICHAEL_LAZY}
        ${UNIT_ISET_SKIP_LIST}
        ${UNIT_ISET_SPLIT_MICHAEL}
        ${UNIT_ISET_SPLIT_ITERABLE}
        ${UNIT_ISET_SPLIT_LAZY}
)



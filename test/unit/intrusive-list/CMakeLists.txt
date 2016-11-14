include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# intrusive::MichaelList
set(UNIT_ILIST_MICHAEL unit-ilist-michael)
set(UNIT_ILIST_MICHAEL_SOURCES
    ../main.cpp
    intrusive_michael_hp.cpp
    intrusive_michael_dhp.cpp
    intrusive_michael_nogc.cpp
    intrusive_michael_rcu_gpb.cpp
    intrusive_michael_rcu_gpi.cpp
    intrusive_michael_rcu_gpt.cpp
    intrusive_michael_rcu_shb.cpp
    intrusive_michael_rcu_sht.cpp
)
add_executable(${UNIT_ILIST_MICHAEL} ${UNIT_ILIST_MICHAEL_SOURCES})
target_link_libraries(${UNIT_ILIST_MICHAEL} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ILIST_MICHAEL} COMMAND ${UNIT_ILIST_MICHAEL} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# intrusive::LazyList
set(UNIT_ILIST_LAZY unit-ilist-lazy)
set(UNIT_ILIST_LAZY_SOURCES
    ../main.cpp
    intrusive_lazy_hp.cpp
    intrusive_lazy_dhp.cpp
    intrusive_lazy_nogc.cpp
    intrusive_lazy_rcu_gpb.cpp
    intrusive_lazy_rcu_gpi.cpp
    intrusive_lazy_rcu_gpt.cpp
    intrusive_lazy_rcu_shb.cpp
    intrusive_lazy_rcu_sht.cpp
)
add_executable(${UNIT_ILIST_LAZY} ${UNIT_ILIST_LAZY_SOURCES})
target_link_libraries(${UNIT_ILIST_LAZY} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ILIST_LAZY} COMMAND ${UNIT_ILIST_LAZY} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# intrusive::IterableList
set(UNIT_ILIST_ITERABLE unit-ilist-iterable)
set(UNIT_ILIST_ITERABLE_SOURCES
    ../main.cpp
    intrusive_iterable_hp.cpp
    intrusive_iterable_dhp.cpp
)
add_executable(${UNIT_ILIST_ITERABLE} ${UNIT_ILIST_ITERABLE_SOURCES})
target_link_libraries(${UNIT_ILIST_ITERABLE} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
add_test(NAME ${UNIT_ILIST_ITERABLE} COMMAND ${UNIT_ILIST_ITERABLE} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})


add_custom_target( unit-ilist
    DEPENDS
        ${UNIT_ILIST_ITERABLE}
        ${UNIT_ILIST_LAZY}
        ${UNIT_ILIST_MICHAEL}
)

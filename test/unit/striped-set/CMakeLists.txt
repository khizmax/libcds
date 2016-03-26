set(PACKAGE_NAME unit-striped-set)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -DCDSUNIT_ENABLE_BOOST_CONTAINER")

set(CDSGTEST_SET_SOURCES
    ../main.cpp
    cuckoo_set.cpp
    intrusive_boost_avl_set.cpp
    intrusive_boost_list.cpp
    intrusive_boost_set.cpp
    intrusive_boost_sg_set.cpp
    intrusive_boost_slist.cpp
    intrusive_boost_splay_set.cpp
    intrusive_boost_treap_set.cpp
    intrusive_boost_unordered_set.cpp
    intrusive_cuckoo_set.cpp
    set_boost_flatset.cpp
    set_boost_list.cpp
    set_boost_set.cpp
    set_boost_slist.cpp
    set_boost_stable_vector.cpp
    set_boost_unordered_set.cpp
    set_boost_vector.cpp
    set_std_list.cpp
    set_std_set.cpp
    set_std_unordered_set.cpp
    set_std_vector.cpp
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
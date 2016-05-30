set(PACKAGE_NAME unit-striped-map)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCDSUNIT_ENABLE_BOOST_CONTAINER")

set(CDSGTEST_STRIPED_MAP_SOURCES
    ../main.cpp
    cuckoo_map.cpp
    map_boost_flat_map.cpp
    map_boost_list.cpp
    map_boost_map.cpp
    map_boost_slist.cpp
    map_boost_unordered_map.cpp
    map_std_list.cpp
    map_std_map.cpp
    map_std_unordered_map.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_STRIPED_MAP_SOURCES})
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
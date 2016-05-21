set(PACKAGE_NAME stress-map-find-int)

set(CDSSTRESS_MAP_FIND_INT_SOURCES
    ../../main.cpp
    map_find_int.cpp
    map_find_int_bronsonavltree.cpp
    map_find_int_cuckoo.cpp
    map_find_int_ellentree.cpp
    map_find_int_feldman_hashset.cpp
    map_find_int_michael.cpp
    map_find_int_skip.cpp
    map_find_int_split.cpp
    map_find_int_std.cpp
    map_find_int_striped.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_MAP_FIND_INT_SOURCES} $<TARGET_OBJECTS:${CDSSTRESS_FRAMEWORK_LIBRARY}>)
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
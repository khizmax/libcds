set(PACKAGE_NAME stress-map-insdel-int)

set(CDSSTRESS_MAP_INSDEL_INT_SOURCES
    ../../main.cpp
    map_insdel_int.cpp
    map_insdel_int_bronsonavltree.cpp
    map_insdel_int_cuckoo.cpp
    map_insdel_int_ellentree.cpp
    map_insdel_int_feldman_hashset.cpp
    map_insdel_int_michael.cpp
    map_insdel_int_skip.cpp
    map_insdel_int_split.cpp
    map_insdel_int_std.cpp
    map_insdel_int_striped.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_MAP_INSDEL_INT_SOURCES} $<TARGET_OBJECTS:${CDSSTRESS_FRAMEWORK_LIBRARY}>)
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
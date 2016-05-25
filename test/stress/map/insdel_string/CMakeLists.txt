set(PACKAGE_NAME stress-map-insdel-string)

set(CDSSTRESS_MAP_INSDEL_STRING_SOURCES
    ../../main.cpp
    map_insdel_string.cpp
    map_insdel_string_bronsonavltree.cpp
    map_insdel_string_cuckoo.cpp
    map_insdel_string_ellentree.cpp
    map_insdel_string_feldman_hashset.cpp
    map_insdel_string_michael.cpp
    map_insdel_string_skip.cpp
    map_insdel_string_split.cpp
    map_insdel_string_std.cpp
    map_insdel_string_striped.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_MAP_INSDEL_STRING_SOURCES} $<TARGET_OBJECTS:${CDSSTRESS_FRAMEWORK_LIBRARY}>)
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
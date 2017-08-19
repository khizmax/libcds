set(PACKAGE_NAME stress-map-del3)

set(CDSSTRESS_MAP_DEL3_SOURCES
    ../../main.cpp
    map_del3.cpp
    map_del3_bronsonavltree.cpp
    map_del3_cuckoo.cpp
    map_del3_ellentree.cpp
    map_del3_feldman_hashmap.cpp
    map_del3_michael.cpp
    map_del3_skip.cpp
    map_del3_split.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_MAP_DEL3_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
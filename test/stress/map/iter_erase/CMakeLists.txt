set(PACKAGE_NAME stress-map-iter-erase)

set(CDSSTRESS_MAP_ITER_ERASE_SOURCES
    ../../main.cpp
    map_iter_erase.cpp
    map_iter_erase_feldman_hashmap.cpp
    map_iter_erase_michael.cpp
    map_iter_erase_split.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_MAP_ITER_ERASE_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(PACKAGE_NAME stress-map-minmax)

set(CDSSTRESS_MAP_MINMAX_SOURCES
    ../../main.cpp
    map_minmax.cpp
    map_minmax_bronsonavltree.cpp
    map_minmax_ellentree.cpp
    map_minmax_skip.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_MAP_MINMAX_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
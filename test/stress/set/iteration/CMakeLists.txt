set(PACKAGE_NAME stress-set-iteration)

set(CDSSTRESS_SET_ITERATION_SOURCES
    ../../main.cpp
    set_iteration.cpp
    set_iteration_feldman_hashset.cpp
    set_iteration_michael.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_SET_ITERATION_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
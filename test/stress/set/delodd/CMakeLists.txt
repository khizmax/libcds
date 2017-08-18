set(PACKAGE_NAME stress-set-delodd)

set(CDSSTRESS_SET_DELODD_SOURCES
    ../../main.cpp
    set_delodd.cpp
    set_delodd_cuckoo.cpp
    set_delodd_ellentree.cpp
    set_delodd_feldman_hashset.cpp
    set_delodd_michael.cpp
    set_delodd_skip.cpp
    set_delodd_split.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_SET_DELODD_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
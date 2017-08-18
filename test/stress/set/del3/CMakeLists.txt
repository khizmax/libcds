set(PACKAGE_NAME stress-set-del3)

set(CDSSTRESS_SET_DEL3_SOURCES
    ../../main.cpp
    set_del3.cpp
    set_del3_cuckoo.cpp
    set_del3_ellentree.cpp
    set_del3_feldman_hashset.cpp
    set_del3_michael.cpp
    set_del3_skip.cpp
    set_del3_split.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_SET_DEL3_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
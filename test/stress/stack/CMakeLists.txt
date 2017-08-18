set(PACKAGE_NAME stress-stack)

set(CDSSTRESS_STACK_SOURCES
    ../main.cpp
    intrusive_push_pop.cpp
    intrusive_push_pop_fcstack.cpp
    push.cpp
    push_pop.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_STACK_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
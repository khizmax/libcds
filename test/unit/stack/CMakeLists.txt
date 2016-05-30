set(PACKAGE_NAME unit-stack)

set(CDSGTEST_STACK_SOURCES
    ../main.cpp
    fcstack.cpp
    intrusive_fcstack.cpp
    intrusive_treiber_stack_dhp.cpp
    intrusive_treiber_stack_hp.cpp
    treiber_stack_dhp.cpp
    treiber_stack_hp.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_STACK_SOURCES})
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
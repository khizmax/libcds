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

add_executable(${PACKAGE_NAME} ${CDSSTRESS_SET_ITERATION_SOURCES} $<TARGET_OBJECTS:${CDSSTRESS_FRAMEWORK_LIBRARY}>)
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
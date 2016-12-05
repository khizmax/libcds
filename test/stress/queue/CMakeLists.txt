set(PACKAGE_NAME stress-queue)

set(CDSSTRESS_QUEUE_SOURCES
    ../main.cpp
    bounded_queue_fulness.cpp
    intrusive_push_pop.cpp
    pop.cpp
    push.cpp
    push_pop.cpp
    random.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_QUEUE_SOURCES} $<TARGET_OBJECTS:${CDSSTRESS_FRAMEWORK_LIBRARY}>)
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
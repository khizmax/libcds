set(PACKAGE_NAME cdsu-queue)

set(CDSUNIT_QUEUE_SOURCES
    queue_pop.cpp
    queue_push.cpp
    queue_random.cpp
    queue_reader_writer.cpp
    intrusive_queue_reader_writer.cpp)

add_executable(${PACKAGE_NAME} ${CDSUNIT_QUEUE_SOURCES} $<TARGET_OBJECTS:${TEST_COMMON}>)
target_link_libraries(${PACKAGE_NAME} ${CDS_SHARED_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME})
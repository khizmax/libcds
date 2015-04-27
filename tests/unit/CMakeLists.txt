set(PACKAGE_NAME cdsu-misc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CDSUNIT_MISC_SOURCES
    alloc/hoard_threadtest.cpp
    alloc/larson.cpp
    alloc/linux_scale.cpp
    alloc/michael_allocator.cpp
    alloc/random.cpp
    lock/spinlock.cpp)

add_executable(${PACKAGE_NAME} ${CDSUNIT_MAP_SOURCES} $<TARGET_OBJECTS:${TEST_COMMON}>)
target_link_libraries(${PACKAGE_NAME} ${CDS_SHARED_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/map2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pqueue)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/queue)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/set2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stack)
set(TEST_COMMON test-common)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES cppunit/test_main.cpp
            cppunit/thread.cpp
            unit/michael_alloc.cpp
            unit/ellen_bintree_update_desc_pool.cpp)

add_library(${TEST_COMMON} OBJECT ${SOURCES})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test-hdr)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unit)
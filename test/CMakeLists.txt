
find_package(GTest REQUIRED) 

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIRS}
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unit)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stress)

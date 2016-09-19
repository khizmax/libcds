
find_package(GTest REQUIRED) 
message("GTest: ${GTEST_INCLUDE_DIRS}")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_LANG_CXX11") 
 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unit)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stress)

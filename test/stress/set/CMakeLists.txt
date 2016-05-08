
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCDSUNIT_USE_URCU")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/delodd)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_find)

add_custom_target( stress-set
    DEPENDS
        stress-set-delodd
        stress-set-insdelfind
)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCDSUNIT_USE_URCU")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/delodd)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/del3)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/find_string)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_func)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_string)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_item_int)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdelfind)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/minmax)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/iter_erase)

add_custom_target( stress-map
    DEPENDS
        stress-map-delodd
        stress-map-del3
        stress-map-find-string
        stress-map-insdel-func
        stress-map-insdel-string
        stress-map-insdel-item-int
        stress-map-insdelfind
        stress-map-minmax
        stress-map-iter-erase
)

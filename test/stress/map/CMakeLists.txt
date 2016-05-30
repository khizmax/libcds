
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCDSUNIT_USE_URCU")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/delodd)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/find_int)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/find_string)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_func)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_int)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_string)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_item_int)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdelfind)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insfind_int)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_find)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_func)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/insdel_string)

add_custom_target( stress-map
    DEPENDS
        stress-map-delodd
        stress-map-find-int
        stress-map-find-string
        stress-map-insdel-func
        stress-map-insdel-int
        stress-map-insdel-string
        stress-map-insdel-item-int
        stress-map-insdelfind
        stress-map-insfind-int
)

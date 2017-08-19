set(PACKAGE_NAME stress-map-insdel-item-int)

set(CDSSTRESS_MAP_INSDEL_ITEM_INT_SOURCES
    ../../main.cpp
    map_insdel_item_int.cpp
    map_insdel_item_int_bronsonavltree.cpp
    map_insdel_item_int_cuckoo.cpp
    map_insdel_item_int_ellentree.cpp
    map_insdel_item_int_feldman_hashset.cpp
    map_insdel_item_int_michael.cpp
    map_insdel_item_int_skip.cpp
    map_insdel_item_int_split.cpp
    map_insdel_item_int_striped.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_MAP_INSDEL_ITEM_INT_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
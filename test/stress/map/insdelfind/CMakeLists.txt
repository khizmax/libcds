set(MAP_INSDELFIND_HP stress-map-insdelfind-hp)
set(MAP_INSDELFIND_RCU stress-map-insdelfind-rcu)

set(CDSSTRESS_MAP_INSDELFIND_HP_SOURCES
    ../../main.cpp
    map_insdelfind.cpp
    map_insdelfind_cuckoo.cpp
    map_insdelfind_ellentree_hp.cpp
    map_insdelfind_feldman_hashset_hp.cpp
    map_insdelfind_michael_hp.cpp
    map_insdelfind_skip_hp.cpp
    map_insdelfind_split_hp.cpp
    map_insdelfind_std.cpp
    map_insdelfind_striped.cpp
)

set(CDSSTRESS_MAP_INSDELFIND_RCU_SOURCES
    ../../main.cpp
    map_insdelfind.cpp
    map_insdelfind_bronsonavltree.cpp
    map_insdelfind_ellentree_rcu.cpp
    map_insdelfind_feldman_hashset_rcu.cpp
    map_insdelfind_michael_rcu.cpp
    map_insdelfind_skip_rcu.cpp
    map_insdelfind_split_rcu.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${MAP_INSDELFIND_HP} ${CDSSTRESS_MAP_INSDELFIND_HP_SOURCES})
target_link_libraries(${MAP_INSDELFIND_HP} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})
add_test(NAME ${MAP_INSDELFIND_HP} COMMAND ${MAP_INSDELFIND_HP} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_executable(${MAP_INSDELFIND_RCU} ${CDSSTRESS_MAP_INSDELFIND_RCU_SOURCES})
target_link_libraries(${MAP_INSDELFIND_RCU} ${CDS_TEST_LIBRARIES} ${CDSSTRESS_FRAMEWORK_LIBRARY})
add_test(NAME ${MAP_INSDELFIND_RCU} COMMAND ${MAP_INSDELFIND_RCU} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_custom_target( stress-map-insdelfind
    DEPENDS
        stress-map-insdelfind-hp
        stress-map-insdelfind-rcu
)

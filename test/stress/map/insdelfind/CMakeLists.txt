set(PACKAGE_NAME stress-map-insdelfind)

set(CDSSTRESS_MAP_INSDELFIND_SOURCES
    ../../main.cpp
    map_insdelfind.cpp
    map_insdelfind_bronsonavltree.cpp
    map_insdelfind_cuckoo.cpp
    map_insdelfind_ellentree.cpp
    map_insdelfind_feldman_hashset.cpp
    map_insdelfind_michael.cpp
    map_insdelfind_skip.cpp
    map_insdelfind_split.cpp
    map_insdelfind_std.cpp
    map_insdelfind_striped.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_MAP_INSDELFIND_SOURCES} $<TARGET_OBJECTS:${CDSSTRESS_FRAMEWORK_LIBRARY}>)
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(PACKAGE_NAME stress-map-insfind-int)

set(CDSSTRESS_MAP_INSFIND_INT_SOURCES
    ../../main.cpp
    map_insfind.cpp
    map_insfind_bronsonavltree.cpp
    map_insfind_cuckoo.cpp
    map_insfind_ellentree.cpp
    map_insfind_feldman_hashset.cpp
    map_insfind_michael.cpp
    map_insfind_skip.cpp
    map_insfind_split.cpp
    map_insfind_std.cpp
    map_insfind_striped.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${PACKAGE_NAME} ${CDSSTRESS_MAP_INSFIND_INT_SOURCES} $<TARGET_OBJECTS:${CDSSTRESS_FRAMEWORK_LIBRARY}>)
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
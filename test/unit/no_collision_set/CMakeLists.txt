set(PACKAGE_NAME unit-nc-set)

# FeldmanHashSet
set(CDSGTEST_NC_SET_SOURCES
    ../main.cpp
#    intrusive_nc_feldman_hashset_hp.cpp
#    intrusive_nc_feldman_hashset_dhp.cpp
    nc_feldman_hashset_hp.cpp
    nc_feldman_hashset_dhp.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_NC_SET_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES})

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

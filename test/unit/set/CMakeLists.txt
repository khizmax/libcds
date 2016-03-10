set(PACKAGE_NAME unit-set)

set(CDSGTEST_SET_SOURCES
    ../main.cpp
    intrusive_michael_lazy_hp.cpp
    intrusive_michael_lazy_dhp.cpp
    intrusive_michael_michael_hp.cpp
    intrusive_michael_michael_dhp.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_SET_SOURCES})
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(PACKAGE_NAME unit-misc)

set(CDSGTEST_MISC_SOURCES
    ../main.cpp
    bitop.cpp
    cxx11_atomic_class.cpp
    cxx11_atomic_func.cpp
    find_option.cpp
    hash_tuple.cpp
    permutation_generator.cpp
    split_bitstring.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_MISC_SOURCES})
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
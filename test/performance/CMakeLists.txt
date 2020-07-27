add_executable(HP_overhead HP_overhead.cpp)
target_link_libraries(HP_overhead ${CDS_TEST_LIBRARIES})

add_test(NAME HP_overhead COMMAND HP_overhead WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
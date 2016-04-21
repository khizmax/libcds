set(PACKAGE_NAME test-hdr)

set(CDS_TESTHDR_MISC
    misc/cxx11_atomic_class.cpp
    misc/cxx11_atomic_func.cpp
    misc/find_option.cpp
    misc/allocator_test.cpp
    misc/michael_allocator.cpp
    misc/hash_tuple.cpp
    misc/bitop_st.cpp
    misc/split_bitstring.cpp
    misc/permutation_generator.cpp
    misc/thread_init_fini.cpp)

set(CDS_TESTHDR_SOURCES
    ${CDS_TESTHDR_MISC})

add_executable(${PACKAGE_NAME} ${CDS_TESTHDR_SOURCES} $<TARGET_OBJECTS:test-hdr-offsetof> $<TARGET_OBJECTS:${TEST_COMMON}>)
target_link_libraries(${PACKAGE_NAME} ${CDS_SHARED_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
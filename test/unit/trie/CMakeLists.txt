set(PACKAGE_NAME unit-trie)

set(CDSGTEST_TRIE_SOURCES
        ../main.cpp
        trie.cpp
)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_TRIE_SOURCES})
target_link_libraries(${PACKAGE_NAME} ${CDS_TEST_LIBRARIES})
strip_binary(${PACKAGE_NAME})
add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
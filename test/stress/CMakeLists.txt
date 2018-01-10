set(CDSSTRESS_FRAMEWORK_LIBRARY stress-framework)

set(CDSSTRESS_FRAMEWORK_SOURCES
    framework/city.cpp
    framework/config.cpp
    framework/ellen_bintree_update_desc_pool.cpp
    framework/stress_test.cpp
)

add_custom_command(
    OUTPUT ${EXECUTABLE_OUTPUT_PATH}/dictionary.txt
    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/test/stress/data/text.txt
    COMMAND perl -X ${PROJECT_SOURCE_DIR}/test/stress/data/split.pl ${PROJECT_SOURCE_DIR}/test/stress/data/text.txt ${EXECUTABLE_OUTPUT_PATH}/dictionary.txt
)
set_property(SOURCE framework/stress_test.cpp APPEND PROPERTY OBJECT_DEPENDS ${EXECUTABLE_OUTPUT_PATH}/dictionary.txt)

add_library(${CDSSTRESS_FRAMEWORK_LIBRARY} ${CDSSTRESS_FRAMEWORK_SOURCES})
target_link_libraries(${CDSSTRESS_FRAMEWORK_LIBRARY} PUBLIC ${CDS_SHARED_LIBRARY})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stack)

add_custom_target( stress-all
    DEPENDS
        stress-stack
)

file(GLOB CONF_FILES ${PROJECT_SOURCE_DIR}/test/stress/data/*.conf)
file(COPY ${CONF_FILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

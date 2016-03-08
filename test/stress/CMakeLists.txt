set(CDSSTRESS_FRAMEWORK_LIBRARY stress-framework)

set(CDSSTRESS_FRAMEWORK_SOURCES
    framework/city.cpp
    framework/config.cpp
    framework/michael_alloc.cpp
    framework/stress_test.cpp
)

add_library(${CDSSTRESS_FRAMEWORK_LIBRARY} OBJECT ${CDSSTRESS_FRAMEWORK_SOURCES}) 

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pqueue)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/queue)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stack)

file(GLOB CONF_FILES ${PROJECT_SOURCE_DIR}/test/stress/data/*.conf)
file(COPY ${CONF_FILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

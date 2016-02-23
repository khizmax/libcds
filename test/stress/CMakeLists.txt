set(CDSSTRESS_FRAMEWORK_LIBRARY stress-framework)

set(CDSSTRESS_FRAMEWORK_SOURCES
    framework/city.cpp
    framework/config.cpp
    framework/stress_test.cpp
)

add_library(${CDSSTRESS_FRAMEWORK_LIBRARY} OBJECT ${CDSSTRESS_FRAMEWORK_SOURCES}) 

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stack)

file(GLOB CONF_FILES ${PROJECT_SOURCE_DIR}/test/stress/data/*.conf)
file(COPY ${CONF_FILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

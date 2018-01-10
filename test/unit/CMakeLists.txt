add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stack)

add_custom_target(unit-all
    DEPENDS 
        unit-stack
)

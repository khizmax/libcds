set(PACKAGE_NAME cdsu-set)

set(CDSUNIT_SET_SOURCES
    set_insdel_func.cpp
    set_insdel_func_cuckoo.cpp
    set_insdel_func_ellentree.cpp
    set_insdel_func_refinable.cpp
    set_insdel_func_skip.cpp
    set_insdel_func_split.cpp
    set_insdel_func_striped.cpp
    set_insdel_string.cpp
    set_insdel_string_michael.cpp
    set_insdel_string_cuckoo.cpp
    set_insdel_string_ellentree.cpp
    set_insdel_string_refinable.cpp
    set_insdel_string_skip.cpp
    set_insdel_string_split.cpp
    set_insdel_string_striped.cpp
    set_insdel_string_std.cpp
    set_insdelfind.cpp
    set_insdelfind_michael.cpp
    set_insdelfind_cuckoo.cpp
    set_insdelfind_ellentree.cpp
    set_insdelfind_refinable.cpp
    set_insdelfind_skip.cpp
    set_insdelfind_split.cpp
    set_insdelfind_striped.cpp
    set_insdelfind_std.cpp
    set_delodd.cpp
    set_delodd_cuckoo.cpp
    set_delodd_michael.cpp
    set_delodd_ellentree.cpp
    set_delodd_skip.cpp
    set_delodd_split.cpp
)

add_executable(${PACKAGE_NAME} ${CDSUNIT_SET_SOURCES} $<TARGET_OBJECTS:${TEST_COMMON}>)
target_link_libraries(${PACKAGE_NAME} ${CDS_SHARED_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME})
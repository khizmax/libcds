set(EXE_SET_INSDELFIND_HP stress-set-insdelfind-hp)
set(EXE_SET_INSDELFIND_RCU stress-set-insdelfind-rcu)

set(CDSSTRESS_SET_INSDELFIND_HP_SOURCES
    ../../main.cpp
    set_insdelfind.cpp
    set_insdelfind_cuckoo.cpp
    set_insdelfind_ellentree_hp.cpp
    set_insdelfind_feldman_hashset_hp.cpp
    set_insdelfind_michael_hp.cpp
    set_insdelfind_skip_hp.cpp
    set_insdelfind_split_hp.cpp
    set_insdelfind_std.cpp
    set_insdelfind_striped.cpp
)

set(CDSSTRESS_SET_INSDELFIND_RCU_SOURCES
    ../../main.cpp
    set_insdelfind.cpp
    set_insdelfind_ellentree_rcu.cpp
    set_insdelfind_feldman_hashset_rcu.cpp
    set_insdelfind_michael_rcu.cpp
    set_insdelfind_skip_rcu.cpp
    set_insdelfind_split_rcu.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable(${EXE_SET_INSDELFIND_HP} ${CDSSTRESS_SET_INSDELFIND_HP_SOURCES} $<TARGET_OBJECTS:${CDSSTRESS_FRAMEWORK_LIBRARY}>)
target_link_libraries(${EXE_SET_INSDELFIND_HP} ${CDS_TEST_LIBRARIES})
add_test(NAME ${EXE_SET_INSDELFIND_HP} COMMAND ${EXE_SET_INSDELFIND_HP} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_executable(${EXE_SET_INSDELFIND_RCU} ${CDSSTRESS_SET_INSDELFIND_RCU_SOURCES} $<TARGET_OBJECTS:${CDSSTRESS_FRAMEWORK_LIBRARY}>)
target_link_libraries(${EXE_SET_INSDELFIND_RCU} ${CDS_TEST_LIBRARIES})
add_test(NAME ${EXE_SET_INSDELFIND_RCU} COMMAND ${EXE_SET_INSDELFIND_RCU} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_custom_target( stress-set-insdelfind
    DEPENDS
        stress-set-insdelfind-hp
        stress-set-insdelfind-rcu
)


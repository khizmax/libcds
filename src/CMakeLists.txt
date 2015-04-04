set(SOURCES hp_gc.cpp
            init.cpp
            dhp_gc.cpp
            urcu_gp.cpp
            urcu_sh.cpp
            michael_heap.cpp
            topology_hpux.cpp
            topology_linux.cpp
            topology_osx.cpp
            dllmain.cpp)

add_library(${CDS_SHARED_LIBRARY} SHARED ${SOURCES})
target_link_libraries(${CDS_SHARED_LIBRARY} PRIVATE ${Boost_LIBRARIES})

install(TARGETS ${CDS_SHARED_LIBRARY} DESTINATION lib)
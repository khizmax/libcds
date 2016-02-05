set(PACKAGE_NAME unit-queue)

set(CDSGTEST_QUEUE_SOURCES
    ../main.cpp
    basket_queue_hp.cpp
    basket_queue_dhp.cpp
    fcqueue.cpp
    moirqueue_hp.cpp
    moirqueue_dhp.cpp
    msqueue_hp.cpp
    msqueue_dhp.cpp
    optimistic_queue_hp.cpp
    optimistic_queue_dhp.cpp
    rwqueue.cpp
    segmented_queue_hp.cpp
    segmented_queue_dhp.cpp
    tsigas_queue.cpp
    vyukov_mpmc_queue.cpp
    intrusive_basket_queue_hp.cpp
    intrusive_basket_queue_dhp.cpp
    intrusive_fcqueue.cpp
    intrusive_msqueue_hp.cpp
    intrusive_msqueue_dhp.cpp
    intrusive_moirqueue_hp.cpp
    intrusive_moirqueue_dhp.cpp
    intrusive_optqueue_hp.cpp
    intrusive_optqueue_dhp.cpp
    intrusive_segmented_queue_hp.cpp
    intrusive_segmented_queue_dhp.cpp
    intrusive_tsigas_queue.cpp
    intrusive_vyukov_queue.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${PACKAGE_NAME} ${CDSGTEST_QUEUE_SOURCES})
target_link_libraries(${PACKAGE_NAME} 
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME ${PACKAGE_NAME} COMMAND ${PACKAGE_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
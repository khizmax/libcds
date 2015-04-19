cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0016 NEW)

project(cds)

set(PROJECT_VERSION 2.1.0)

# Options
option(WITH_TESTS "Build unit tests" OFF)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(CDS_SHARED_LIBRARY ${PROJECT_NAME})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Default build type to Debug" FORCE)
endif()

message("Build type -- ${CMAKE_BUILD_TYPE}")

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

include_directories(${CMAKE_SOURCE_DIR})

# Component names for separate distribution in rpms, debs etc.
set(LIBRARIES_COMPONENT lib)
set(HEADERS_COMPONENT devel)

set(SOURCES src/hp_gc.cpp
            src/init.cpp
            src/dhp_gc.cpp
            src/urcu_gp.cpp
            src/urcu_sh.cpp
            src/michael_heap.cpp
            src/topology_hpux.cpp
            src/topology_linux.cpp
            src/topology_osx.cpp
            src/dllmain.cpp)

add_library(${CDS_SHARED_LIBRARY} SHARED ${SOURCES})
target_link_libraries(${CDS_SHARED_LIBRARY} PRIVATE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${CDS_SHARED_LIBRARY} DESTINATION lib COMPONENT ${LIBRARIES_COMPONENT})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/cds DESTINATION include COMPONENT ${HEADERS_COMPONENT})

if(WITH_TESTS)
  enable_testing()
  add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif(WITH_TESTS)

### FOR PACKAGING in RPM, TGZ...###############################################################################
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/install/description.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Library of concurrent data structures")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}")
set(DEPLOY_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}")

# TGZ specific
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# RPM specific
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/install/post_install_script.sh")
set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/install/post_uninstall_script.sh")
#set(CPACK_RPM_CHANGELOG_FILE ${CHANGELOG_INCLUDE})
set(CPACK_RPM_PACKAGE_LICENSE GPL)
set(CPACK_RPM_PACKAGE_GROUP "System Environment/Base")
set(CPACK_RPM_PACKAGE_REQUIRES "boost >= 1.50")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION ${CPACK_PACKAGING_INSTALL_PREFIX})
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /usr/local)

# DEB specific
set(CPACK_DEBIAN_PACKAGE_DEPENDS "boost (>= 1.50)")

# Components grouping for Mac OS X and Windows installers
set(CPACK_COMPONENT_${LIBRARIES_COMPONENT}_GROUP "Runtime")
set(CPACK_COMPONENT_${HEADERS_COMPONENT}_GROUP "Development")
set(CPACK_COMPONENT_${LIBRARIES_COMPONENT}_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_${HEADERS_COMPONENT}_DISPLAY_NAME "C++ Headers")

set(CPACK_COMPONENT_${HEADERS_COMPONENT}_DEPENDS ${LIBRARIES_COMPONENT})

set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION "All of the tools you'll ever need to develop lock-free oriented software with libcds")
set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION "Only libcds library for runtime")

include(CPack)
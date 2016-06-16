cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0016 NEW)

project(cds)

set(PROJECT_VERSION 2.2.0)

# Options
option(WITH_TESTS "Build unit tests" OFF)
option(WITH_TESTS_COVERAGE "Analyze test coverage using gcov (only for gcc)" OFF)
option(WITH_BOOST_ATOMIC "Use boost atomics (only for boost >= 1.54)" OFF)
option(WITH_ASAN "Build AddressSantinizer instrumented code" OFF)
option(WITH_TSAN "Build ThreadSantinizer instrumented code" OFF)

find_package(Threads REQUIRED)
find_package(Boost 1.50 COMPONENTS system thread REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

if(WITH_BOOST_ATOMIC)
  if(${Boost_MINOR_VERSION} GREATER 53)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCDS_USE_BOOST_ATOMIC")
    message(STATUS "Boost version allows using of boost.atomic: activated")
  endif()
endif(WITH_BOOST_ATOMIC)

if(WITH_ASAN)
  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -fPIC -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1 -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -pie")
  else()
    message(WARNING "Compiler does not support AddressSanitizer")
  endif()
endif(WITH_ASAN)

if(WITH_TSAN)
  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -fPIC -fsanitize=thread -g -DCDS_THREAD_SANITIZER_ENABLED -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O1 -fPIC -fsanitize=thread -g -DCDS_THREAD_SANITIZER_ENABLED -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread -pie")
  else()
    message(WARNING "Compiler does not support ThreadSanitizer")
  endif()
endif(WITH_TSAN)

if(WITH_TESTS_COVERAGE)
  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
    message(STATUS "Test coverage analysis: activated")
  else()
	message(WARNING "Compiler is not GNU gcc! Test coverage couldn't be analyzed")
  endif()  
endif(WITH_TESTS_COVERAGE)

set(CDS_SHARED_LIBRARY ${PROJECT_NAME})
set(CDS_STATIC_LIBRARY ${PROJECT_NAME}-s)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(CDS_BIN_DIR)
    set(EXECUTABLE_OUTPUT_PATH ${CDS_BIN_DIR})
    set(LIBRARY_OUTPUT_PATH ${CDS_BIN_DIR})
else()
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
endif()
message(STATUS "Binary output path: ${EXECUTABLE_OUTPUT_PATH}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Default build type to Debug" FORCE)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fno-strict-aliasing")
# gcc 4.8: disable noise -Wunused-local-typedefs
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs -Wno-unused-parameter")  
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

message("Build type -- ${CMAKE_BUILD_TYPE}")
message("Compiler version: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  message("Compiler flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
else()
  message("Compiler flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

include_directories(${PROJECT_SOURCE_DIR})

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
set_target_properties( ${CDS_SHARED_LIBRARY} PROPERTIES VERSION ${PROJECT_VERSION})
add_library(${CDS_STATIC_LIBRARY} STATIC ${SOURCES})
target_link_libraries(${CDS_SHARED_LIBRARY} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${CDS_STATIC_LIBRARY} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${CDS_SHARED_LIBRARY} DESTINATION lib COMPONENT ${LIBRARIES_COMPONENT})
install(TARGETS ${CDS_STATIC_LIBRARY} DESTINATION lib COMPONENT ${LIBRARIES_COMPONENT})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/cds DESTINATION include COMPONENT ${HEADERS_COMPONENT})

if(WITH_TESTS)
  enable_testing()
  add_subdirectory(${PROJECT_SOURCE_DIR}/test)
  message(STATUS "Build tests: activated")
endif(WITH_TESTS)

### FOR PACKAGING in RPM, TGZ, DEB, NSYS...###############################################################################
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_CONTACT "Max Khizhinsky <libcds-user@lists.sourceforge.net>")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "cds")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/build/cmake/description.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Library of concurrent data structures")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}")
set(DEPLOY_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}")

# TGZ specific
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# RPM specific
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_PACKAGE_RELEASE ${CPACK_PACKAGE_RELEASE})
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${PROJECT_SOURCE_DIR}/build/cmake/post_install_script.sh")
set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${PROJECT_SOURCE_DIR}/build/cmake/post_uninstall_script.sh")
set(CPACK_RPM_PACKAGE_URL https://github.com/khizmax/libcds)
set(CPACK_RPM_PACKAGE_LICENSE GPL)
set(CPACK_RPM_PACKAGE_GROUP "System Environment/Base")
set(CPACK_RPM_PACKAGE_REQUIRES "boost >= 1.50")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION ${CPACK_PACKAGING_INSTALL_PREFIX})
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /usr/local)
set(CPACK_RPM_devel_PACKAGE_REQUIRES "boost >= 1.50, cds-lib = ${PROJECT_VERSION}")

# DEB specific
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "boost (>= 1.50)")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/khizmax/libcds")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${PROJECT_SOURCE_DIR}/build/cmake/post_install_script.sh;;${PROJECT_SOURCE_DIR}/build/cmake/post_uninstall_script.sh;" )

# NSYS specific
set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_NSIS_CONTACT ${CPACK_PACKAGE_CONTACT})
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)

# Components grouping for Mac OS X and Windows installers
set(CPACK_COMPONENT_${LIBRARIES_COMPONENT}_GROUP "Runtime")
set(CPACK_COMPONENT_${HEADERS_COMPONENT}_GROUP "Development")
set(CPACK_COMPONENT_${LIBRARIES_COMPONENT}_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_${HEADERS_COMPONENT}_DISPLAY_NAME "C++ Headers")
set(CPACK_COMPONENT_${HEADERS_COMPONENT}_DEPENDS ${LIBRARIES_COMPONENT})
set(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION "All of the tools you'll ever need to develop lock-free oriented software with libcds")
set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION "Only libcds library for runtime")

include(CPack)

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

cmake_policy(SET CMP0016 NEW)
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/cmake ${CMAKE_MODULE_PATH})
include(TargetArch)
include(CheckIncludeFileCXX)

project(cds)

set(PROJECT_VERSION 2.3.3)

# Options
option(WITH_TESTS "Build unit tests" OFF)
option(WITH_TESTS_COVERAGE "Analyze test coverage using gcov (only for gcc)" OFF)
option(WITH_BOOST_ATOMIC "Use boost atomics (only for boost >= 1.54)" OFF)
option(WITH_ASAN "Build ASan+UBSan instrumented code" OFF)
option(WITH_TSAN "Build TSan instrumented code" OFF)
option(ENABLE_UNIT_TEST "Enable unit test" ON)
option(ENABLE_STRESS_TEST "Enable stress test" ON)
set(CMAKE_TARGET_ARCHITECTURE "" CACHE string "Target build architecture")

find_package(Threads)

if(NOT CMAKE_TARGET_ARCHITECTURE)
    target_architecture(CMAKE_TARGET_ARCHITECTURE)
endif()

if(APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DARWIN_C_SOURCE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DARWIN_C_SOURCE")
endif()

if(WITH_BOOST_ATOMIC)
    if(TARGET boost::atomic)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCDS_USE_BOOST_ATOMIC")
        link_libraries(boost::atomic)
    else()
        find_package(Boost 1.53 COMPONENTS atomic)
        if(Boost_FOUND)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCDS_USE_BOOST_ATOMIC")
            message(STATUS "Boost version allows using of boost.atomic: activated")
        endif()
    endif()
endif(WITH_BOOST_ATOMIC)

if(WITH_ASAN)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -fPIC -fsanitize=address,undefined -g -DCDS_ADDRESS_SANITIZER_ENABLED -fno-omit-frame-pointer -fno-optimize-sibling-calls")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1 -fsanitize=address,undefined -g -DCDS_ASAN_ENABLED -fno-omit-frame-pointer -fno-optimize-sibling-calls")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined -pie")
    else()
        message(WARNING "Compiler does not support AddressSanitizer")
    endif()
endif(WITH_ASAN)

if(WITH_TSAN)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
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

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    string(REGEX MATCHALL "-std=[^ ]+" cxx_std_found ${CMAKE_CXX_FLAGS} " dummy@rg")
    if(cxx_std_found)
        message("C++ std: ${cxx_std_found}")
    else()
        list(APPEND LIBCDS_PUBLIC_CXX_FLAGS "-std=c++11")
        message("C++ std: -std=c++11 (default)")
    endif()

    list(APPEND LIBCDS_PRIVATE_CXX_FLAGS "-Wall" "-Wextra" "-pedantic")

    if(CMAKE_TARGET_ARCHITECTURE STREQUAL "x86_64")
        list(APPEND LIBCDS_PUBLIC_CXX_FLAGS "-mcx16")
        set(LIB_SUFFIX "64")

        # GCC-7: 128-bit atomics support is implemented via libatomic on amd64
        #        see https://gcc.gnu.org/ml/gcc/2017-01/msg00167.html
        # Maybe, it will be changed in future
        if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0.0" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -latomic")
        endif()
    endif()
endif()


if(CMAKE_SYSTEM_NAME STREQUAL "AIX")
    set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> -q -c ${CMAKE_STATIC_LINKER_FLAGS} -o <TARGET> <OBJECTS>")
    list(APPEND LIBCDS_PRIVATE_CXX_FLAGS "-Wl,-G")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-brtl")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

CHECK_INCLUDE_FILE_CXX(linux/membarrier.h CDS_HAVE_LINUX_MEMBARRIER_H CMAKE_CXX_FLAGS)

message("Build type -- ${CMAKE_BUILD_TYPE}")
message("Compiler version: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message("System: ${CMAKE_SYSTEM_NAME} version: ${CMAKE_SYSTEM_VERSION}")
message("Target architecture: ${CMAKE_TARGET_ARCHITECTURE}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Compiler flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ${LIBCDS_PUBLIC_CXX_FLAGS} ${LIBCDS_PRIVATE_CXX_FLAGS}")
else()
    message("Compiler flags: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} ${LIBCDS_PUBLIC_CXX_FLAGS} ${LIBCDS_PRIVATE_CXX_FLAGS}")
endif()
message("Exe flags: ${CMAKE_EXE_LINKER_FLAGS}")

# Component names for separate distribution in rpms, debs etc.
set(LIBRARIES_COMPONENT lib)
set(HEADERS_COMPONENT devel)

set(SOURCES src/init.cpp
            src/hp.cpp
            src/dhp.cpp
            src/urcu_gp.cpp
            src/urcu_sh.cpp
            src/thread_data.cpp
            src/topology_hpux.cpp
            src/topology_linux.cpp
            src/topology_osx.cpp
            src/dllmain.cpp)

add_library(${CDS_SHARED_LIBRARY} SHARED ${SOURCES})
set_target_properties(${CDS_SHARED_LIBRARY} PROPERTIES VERSION ${PROJECT_VERSION}
        DEBUG_POSTFIX "_d")
if(MINGW)
    set_target_properties(${CDS_SHARED_LIBRARY} PROPERTIES DEFINE_SYMBOL CDS_BUILD_LIB)
endif()

add_library(${CDS_STATIC_LIBRARY} STATIC ${SOURCES})
set_target_properties(${CDS_STATIC_LIBRARY} PROPERTIES DEBUG_POSTFIX "_d")
if(MINGW)
    target_compile_definitions(${CDS_STATIC_LIBRARY} PRIVATE CDS_BUILD_STATIC_LIB)
endif()

target_link_libraries(${CDS_SHARED_LIBRARY} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${CDS_STATIC_LIBRARY} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${CDS_SHARED_LIBRARY} INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
                                                            $<INSTALL_INTERFACE:include>)
target_include_directories(${CDS_STATIC_LIBRARY} INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
                                                            $<INSTALL_INTERFACE:include>)
target_compile_options(${CDS_SHARED_LIBRARY} PUBLIC "${LIBCDS_PUBLIC_CXX_FLAGS}")
target_compile_options(${CDS_STATIC_LIBRARY} PUBLIC "${LIBCDS_PUBLIC_CXX_FLAGS}")
target_compile_options(${CDS_SHARED_LIBRARY} PRIVATE "${LIBCDS_PRIVATE_CXX_FLAGS}")
target_compile_options(${CDS_STATIC_LIBRARY} PRIVATE "${LIBCDS_PRIVATE_CXX_FLAGS}")

install(TARGETS ${CDS_SHARED_LIBRARY} EXPORT LibCDSConfig LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT ${LIBRARIES_COMPONENT} NAMELINK_SKIP RUNTIME DESTINATION lib${LIB_SUFFIX})
install(TARGETS ${CDS_SHARED_LIBRARY} EXPORT LibCDSConfig LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT ${HEADERS_COMPONENT} NAMELINK_ONLY)
install(TARGETS ${CDS_STATIC_LIBRARY} EXPORT LibCDSConfig DESTINATION lib${LIB_SUFFIX} COMPONENT ${LIBRARIES_COMPONENT})
install(EXPORT LibCDSConfig FILE LibCDSConfig.cmake NAMESPACE LibCDS:: DESTINATION lib/cmake/LibCDS)
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
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${PROJECT_SOURCE_DIR}/build/cmake/post_install_script.sh;;${PROJECT_SOURCE_DIR}/build/cmake/post_uninstall_script.sh;")

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

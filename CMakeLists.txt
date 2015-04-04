cmake_minimum_required(VERSION 2.8.10)
cmake_policy(SET CMP0016 NEW)

project(cds)

set(PROJECT_VERSION 2.1.0)

find_package(Boost COMPONENTS system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(CDS_SHARED_LIBRARY ${PROJECT_NAME})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories(${CMAKE_SOURCE_DIR})

enable_testing()

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

### FOR PACKAGING #####################################################################################
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_RELEASE 1)

set(CPACK_RPM_PACKAGE_SUMMARY "Library of concurrent data structures")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/install/description.txt")
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/install/post_install_script.sh")
set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/install/post_uninstall_script.sh")
#set(CPACK_RPM_CHANGELOG_FILE ${CHANGELOG_INCLUDE})
set(CPACK_RPM_PACKAGE_LICENSE GPL)
set(CPACK_RPM_PACKAGE_GROUP "System Environment/Base")
set(CPACK_RPM_PACKAGE_REQUIRES "boost >= 1.50")

set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION ${CPACK_PACKAGING_INSTALL_PREFIX})
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /usr/local)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}")
set(DEPLOY_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}")

include(CPack)

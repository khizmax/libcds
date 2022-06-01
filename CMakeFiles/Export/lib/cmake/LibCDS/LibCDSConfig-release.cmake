#----------------------------------------------------------------
# Generated CMake target import file for configuration "RELEASE".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LibCDS::cds" for configuration "RELEASE"
set_property(TARGET LibCDS::cds APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LibCDS::cds PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libcds.so.2.3.3"
  IMPORTED_SONAME_RELEASE "libcds.so.2.3.3"
  )

list(APPEND _IMPORT_CHECK_TARGETS LibCDS::cds )
list(APPEND _IMPORT_CHECK_FILES_FOR_LibCDS::cds "${_IMPORT_PREFIX}/lib64/libcds.so.2.3.3" )

# Import target "LibCDS::cds-s" for configuration "RELEASE"
set_property(TARGET LibCDS::cds-s APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(LibCDS::cds-s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libcds-s.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS LibCDS::cds-s )
list(APPEND _IMPORT_CHECK_FILES_FOR_LibCDS::cds-s "${_IMPORT_PREFIX}/lib64/libcds-s.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

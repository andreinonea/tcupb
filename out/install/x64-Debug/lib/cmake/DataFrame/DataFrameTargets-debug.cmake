#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DataFrame::DataFrame" for configuration "Debug"
set_property(TARGET DataFrame::DataFrame APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(DataFrame::DataFrame PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/DataFrame.lib"
  )

list(APPEND _cmake_import_check_targets DataFrame::DataFrame )
list(APPEND _cmake_import_check_files_for_DataFrame::DataFrame "${_IMPORT_PREFIX}/lib/DataFrame.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

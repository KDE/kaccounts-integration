# Find QJSON - JSON handling library for Qt
#
# This module defines
#  QJSON_FOUND - whether the qsjon library was found
#  QJSON_LIBRARIES - the qjson library
#  QJSON_INCLUDE_DIR - the include path of the qjson library
#

if (QJSON_INCLUDE_DIR AND QJSON_LIBRARIES)

  # Already in cache
  set (QJSON_FOUND TRUE)

else (QJSON_INCLUDE_DIR AND QJSON_LIBRARIES)

  if (NOT WIN32)
    set (_pc_qjson_string "QJson")
    if (QJSON_FIND_VERSION_EXACT)
      set (_pc_qjson_string "${_pc_qjson_string}=")
    else (QJSON_FIND_VERSION_EXACT)
      set (_pc_qjson_string "${_pc_qjson_string}>=")
    endif (QJSON_FIND_VERSION_EXACT)
    if (QJSON_FIND_VERSION_COUNT GREATER 0)
      set (_pc_qjson_string "${_pc_qjson_string}${QJSON_FIND_VERSION}")
    else (QJSON_FIND_VERSION_COUNT GREATER 0)
      set (_pc_qjson_string "${_pc_qjson_string}0.5")
    endif (QJSON_FIND_VERSION_COUNT GREATER 0)
    # use pkg-config to get the values of QJSON_INCLUDE_DIRS
    # and QJSON_LIBRARY_DIRS to add as hints to the find commands.
    include (FindPkgConfig)
    pkg_check_modules (PC_QJSON ${_pc_qjson_string})
    set (QJSON_VERSION "${PC_QJSON_VERSION}")
  endif (NOT WIN32)

  find_library (QJSON_LIBRARIES
    NAMES
    qjson
    PATHS
    ${PC_QJSON_LIBRARY_DIRS}
    ${LIB_INSTALL_DIR}
    ${KDE4_LIB_DIR}
  )

  find_path (QJSON_INCLUDE_DIR
    NAMES
    qjson/parser.h
    PATHS
    ${PC_QJSON_INCLUDE_DIRS}
    ${INCLUDE_INSTALL_DIR}
    ${KDE4_INCLUDE_DIR}
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(QJSON
                                    REQUIRED_VARS QJSON_LIBRARIES QJSON_INCLUDE_DIR
                                    VERSION_VAR QJSON_VERSION
  )

endif (QJSON_INCLUDE_DIR AND QJSON_LIBRARIES)

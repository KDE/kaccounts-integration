find_package(PkgConfig REQUIRED)

pkg_check_modules(PC_LIBACCOUNTS_GLIB libaccounts-glib)

find_library(LIBACCOUNTSGLIB_LIBRARY NAMES accounts-glib
             HINTS ${PC_LIBACCOUNTS_GLIB_LIBDIR} ${PC_LIBACCOUNTS_GLIB_LIBRARY_DIRS} )

set(LIBACCOUNTSGLIB_LIBRARIES ${LIBACCOUNTSGLIB_LIBRARY})
set(LIBACCOUNTSGLIB_FOUND ${PC_LIBACCOUNTS_GLIB_FOUND})

string(ASCII "${PC_LIBACCOUNTS_GLIB_VERSION}" LIBACCOUNTS_GLIB_VERSION_STRING)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LibAccountsGlib
                                  FOUND_VAR LIBACCOUNTSGLIB_FOUND
                                  REQUIRED_VARS LIBACCOUNTSGLIB_LIBRARY
                                  VERSION_VAR PC_LIBACCOUNTS_GLIB_VERSION)

mark_as_advanced(LIBACCOUNTSGLIB_LIBRARY)

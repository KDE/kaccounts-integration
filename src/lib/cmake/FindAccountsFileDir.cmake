find_package(PkgConfig REQUIRED)

execute_process(
    COMMAND "${PKG_CONFIG_EXECUTABLE}" --variable=prefix libaccounts-glib
    OUTPUT_VARIABLE _pkgconfig_invoke_result
    RESULT_VARIABLE _pkgconfig_failed
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE)

if (_pkgconfig_failed)
    message(FATAL_ERROR "Couldn't find the prefix for libaccounts-glib")
else()
    set(ACCOUNTS_PREFIX_DIR ${_pkgconfig_invoke_result})
endif()

execute_process(
    COMMAND "${PKG_CONFIG_EXECUTABLE}" --variable=providerfilesdir libaccounts-glib
    OUTPUT_VARIABLE _pkgconfig_invoke_result
    RESULT_VARIABLE _pkgconfig_failed
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE)

if (_pkgconfig_failed)
    message(FATAL_ERROR "Couldn't find the providerfilesdir for libaccounts-glib")
else()
    if (NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        string(REPLACE ${ACCOUNTS_PREFIX_DIR} ${CMAKE_INSTALL_PREFIX} _pkgconfig_invoke_result "${_pkgconfig_invoke_result}")
    endif()
    set(ACCOUNTS_PROVIDERS_DIR ${_pkgconfig_invoke_result})
endif()

execute_process(
    COMMAND "${PKG_CONFIG_EXECUTABLE}" --variable=servicefilesdir libaccounts-glib
    OUTPUT_VARIABLE _pkgconfig_invoke_result
    RESULT_VARIABLE _pkgconfig_failed
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE)

if (_pkgconfig_failed)
    message(FATAL_ERROR "Couldn't find the servicefilesdir for libaccounts-glib")
else()
    if (NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        string(REPLACE ${ACCOUNTS_PREFIX_DIR} ${CMAKE_INSTALL_PREFIX} _pkgconfig_invoke_result "${_pkgconfig_invoke_result}")
    endif()
    set(ACCOUNTS_SERVICES_DIR ${_pkgconfig_invoke_result})
endif()

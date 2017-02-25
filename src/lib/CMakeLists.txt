project (kaccounts-support-library)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_FULL_CMAKEPACKAGEDIR}/KAccounts")

include(ECMGenerateHeaders)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(CMakePackageConfigHelpers)
include(WriteBasicConfigVersionFile)
include(KDECMakeSettings)

ecm_setup_version(${KACCOUNTS_VERSION}
  VARIABLE_PREFIX KACCOUNTS
  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kaccounts_version.h"
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KAccountsConfigVersion.cmake"
  SOVERSION ${KACCOUNTS_SOVERSION})

set (kaccountslib_SRCS
     kaccountsuiplugin.cpp
     kaccountsdplugin.cpp
     getcredentialsjob.cpp
     core.cpp
)

ecm_generate_headers(kaccountslib_HEADERS
  HEADER_NAMES
  Core
  GetCredentialsJob
  KAccountsUiPlugin
  KAccountsDPlugin
  REQUIRED_HEADERS kaccountslib_HEADERS
)

add_library (kaccounts SHARED
             ${kaccountslib_SRCS}
)

target_link_libraries (kaccounts
    PUBLIC
        KF5::CoreAddons
        ${ACCOUNTSQT_LIBRARIES}
        Qt5::Xml
        Qt5::Gui
    PRIVATE
        ${SIGNONQT_LIBRARIES}
)

target_include_directories(kaccounts INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KAccounts>" PUBLIC "${ACCOUNTSQT_INCLUDE_DIRS}" PRIVATE "${SIGNONQT_INCLUDE_DIRS}")
set_target_properties(kaccounts PROPERTIES VERSION   ${KACCOUNTS_VERSION}
                                           SOVERSION ${KACCOUNTS_SOVERSION}
                                           EXPORT_NAME KAccounts
)

ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KAccountsConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KAccountsConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
  "KAccountsMacros.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KAccountsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KAccountsConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(TARGETS kaccounts EXPORT KAccountsTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(EXPORT KAccountsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KAccountsTargets.cmake ) #NAMESPACE KF5::

install (FILES
        ${kaccountslib_HEADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/kaccounts_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KAccounts COMPONENT Devel
)

add_subdirectory(cmake)

project (kaccounts-support-library)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KAccounts")

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


add_library (kaccounts SHARED
             ${kaccountslib_SRCS}
)

target_link_libraries (kaccounts Qt5::Core
                                 Qt5::Xml
                                 KF5::CoreAddons
                                 ${ACCOUNTSQT_LIBRARIES}
                                 ${SIGNONQT_LIBRARIES}
)

target_include_directories(kaccounts INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KAccounts>")
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
  "${CMAKE_CURRENT_BINARY_DIR}/KAccountsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KAccountsConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(TARGETS kaccounts EXPORT KAccountsTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(EXPORT KAccountsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KAccountsTargets.cmake ) #NAMESPACE KF5::

install (FILES
        core.h
        getcredentialsjob.h
        kaccountsuiplugin.h
        kaccountsdplugin.h
        kaccounts_export.h
        ${CMAKE_CURRENT_BINARY_DIR}/kaccounts_version.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/KAccounts COMPONENT Devel
)
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KAccounts")

ecm_setup_version(${KACCOUNTS_VERSION}
  VARIABLE_PREFIX KACCOUNTS
  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kaccounts_version.h"
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KAccountsConfigVersion.cmake"
  SOVERSION ${KACCOUNTS_SOVERSION})

set (kaccountslib_SRCS
     accountsmodel.cpp
     core.cpp
     kaccountsdplugin.cpp
     kaccountsuiplugin.cpp
     providersmodel.cpp
     servicesmodel.cpp
     uipluginsmanager.cpp

     accountservicetogglejob.cpp
     changeaccountdisplaynamejob.cpp
     createaccountjob.cpp
     getcredentialsjob.cpp
     removeaccountjob.cpp

     accountsmodel.h
     core.h
     kaccountsdplugin.h
     kaccountsuiplugin.h
     providersmodel.h
     servicesmodel.h
     uipluginsmanager.h

     accountservicetogglejob.h
     changeaccountdisplaynamejob.h
     createaccountjob.h
     getcredentialsjob.h
     removeaccountjob.h
)

ecm_generate_headers(kaccountslib_HEADERS
  HEADER_NAMES
  AccountsModel
  Core
  KAccountsUiPlugin
  KAccountsDPlugin
  ProvidersModel
  ServicesModel

  AccountServiceToggleJob
  ChangeAccountDisplayNameJob
  CreateAccountJob
  GetCredentialsJob
  RemoveAccountJob
  REQUIRED_HEADERS kaccountslib_HEADERS
)

add_library(kaccounts ${kaccountslib_SRCS})

target_compile_definitions(kaccounts
    PRIVATE
    ACCOUNTSQT5_VERSION_MAJOR=${AccountsQt5_VERSION_MAJOR}
    ACCOUNTSQT5_VERSION_MINOR=${AccountsQt5_VERSION_MINOR}
)

generate_export_header(kaccounts BASE_NAME kaccounts)
target_link_libraries (kaccounts
    PUBLIC
        KF5::CoreAddons
        KF5::I18n
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

configure_package_config_file(
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
        ${CMAKE_CURRENT_BINARY_DIR}/kaccounts_export.h
        ${kaccountslib_HEADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/kaccounts_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KAccounts COMPONENT Devel
)

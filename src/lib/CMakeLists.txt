if(QT_MAJOR_VERSION STREQUAL "5")
  set(KACCOUNTS_SUFFIX "")
else()
  set(KACCOUNTS_SUFFIX "6")
endif()

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KAccounts${KACCOUNTS_SUFFIX}")

ecm_setup_version(${KACCOUNTS_VERSION}
  VARIABLE_PREFIX KACCOUNTS
  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kaccounts_version.h"
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KAccounts${KACCOUNTS_SUFFIX}ConfigVersion.cmake"
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

# For the Qt5 build keep the old header layout for compatibility
if(QT_MAJOR_VERSION STREQUAL "5")
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
else()
  ecm_generate_headers(kaccountslib_CamelCase_HEADERS
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
    PREFIX KAccounts
    REQUIRED_HEADERS kaccountslib_HEADERS
  )
endif()

add_library(kaccounts ${kaccountslib_SRCS})

target_compile_definitions(kaccounts
    PRIVATE
    ACCOUNTSQT5_VERSION_MAJOR=${AccountsQt5_VERSION_MAJOR}
    ACCOUNTSQT5_VERSION_MINOR=${AccountsQt5_VERSION_MINOR}
)

generate_export_header(kaccounts BASE_NAME kaccounts)
target_link_libraries (kaccounts
    PUBLIC
        KF${QT_MAJOR_VERSION}::CoreAddons
        KF${QT_MAJOR_VERSION}::I18n
        ${ACCOUNTSQT_LIBRARIES}
        Qt::Xml
        Qt::Gui
    PRIVATE
        ${SIGNONQT_LIBRARIES}
)

target_include_directories(kaccounts
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KAccounts${KACCOUNTS_SUFFIX}>"
)

target_include_directories(kaccounts SYSTEM
    PUBLIC "${ACCOUNTSQT_INCLUDE_DIRS}"
    PRIVATE "${SIGNONQT_INCLUDE_DIRS}"
)
set_target_properties(kaccounts PROPERTIES VERSION   ${KACCOUNTS_VERSION}
                                           SOVERSION ${KACCOUNTS_SOVERSION}
                                           EXPORT_NAME KAccounts${KACCOUNTS_SUFFIX}
                                           LIBRARY_OUTPUT_NAME kaccounts${KACCOUNTS_SUFFIX}
)

ecm_qt_declare_logging_category(kaccounts HEADER debug.h IDENTIFIER KACCOUNTS_LIB_LOG CATEGORY_NAME org.kde.kaccounts.lib
    DESCRIPTION "KAccounts Library"
    EXPORT KAccounts
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KAccountsConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KAccounts${KACCOUNTS_SUFFIX}Config.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
  "KAccountsMacros.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KAccounts${KACCOUNTS_SUFFIX}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KAccounts${KACCOUNTS_SUFFIX}ConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(TARGETS kaccounts EXPORT KAccounts${KACCOUNTS_SUFFIX}Targets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(EXPORT KAccounts${KACCOUNTS_SUFFIX}Targets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KAccounts${KACCOUNTS_SUFFIX}Targets.cmake ) #NAMESPACE KF${QT_MAJOR_VERSION}::

if(QT_MAJOR_VERSION STREQUAL "5")
  install (FILES
        ${CMAKE_CURRENT_BINARY_DIR}/kaccounts_export.h
        ${kaccountslib_HEADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/kaccounts_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KAccounts COMPONENT Devel
  )

else()
  install (FILES
          ${CMAKE_CURRENT_BINARY_DIR}/kaccounts_export.h
          ${CMAKE_CURRENT_BINARY_DIR}/kaccounts_version.h
          DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KAccounts${KACCOUNTS_SUFFIX} COMPONENT Devel
  )

  install (FILES
          ${kaccountslib_HEADERS}
          DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KAccounts${KACCOUNTS_SUFFIX}/kaccounts COMPONENT Devel
  )

  install (FILES
          ${kaccountslib_CamelCase_HEADERS}
          DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KAccounts${KACCOUNTS_SUFFIX}/KAccounts COMPONENT Devel
  )
endif()

project (owncloud-plugin-kaccounts)

include_directories (${CMAKE_CURRENT_BUILD_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set (owncloud_plugin_kaccounts_SRCS
     owncloud.cpp
     qmlhelper.cpp
     pages/basicinfo.cpp
     pages/connecting.cpp
     pages/oservices.cpp
)

ki18n_wrap_ui(owncloud_plugin_kaccounts_SRCS
              pages/basicinfo.ui
              pages/connecting.ui
              pages/oservices.ui
)

add_library (owncloud_plugin_kaccounts MODULE
             ${owncloud_plugin_kaccounts_SRCS}
)

target_link_libraries (owncloud_plugin_kaccounts
                       Qt5::Core
                       Qt5::Widgets
                       KF5::KIOCore
                       KF5::KIOWidgets
                       KF5::CoreAddons
                       KF5::WidgetsAddons
                       KF5::IconThemes
                       KF5::I18n
                       KF5::Declarative
                       kaccounts
                       ${ACCOUNTSQT_LIBRARIES}
                       ${SIGNONQT_LIBRARIES}
)

# Install:
install (TARGETS owncloud_plugin_kaccounts
         DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/ui
)
kpackage_install_package(package org.kde.kaccounts.owncloud genericqml)

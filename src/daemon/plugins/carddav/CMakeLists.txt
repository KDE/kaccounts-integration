project(kaccounts-carddav-plugin)

include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set(kaccounts_carddav_plugin_SRCS
    carddav-plugin.cpp

    carddav.cpp
    auth.cpp
    replyparser.cpp
    requestgenerator.cpp
    syncer.cpp
)

add_library(kaccounts_carddav_plugin MODULE ${kaccounts_carddav_plugin_SRCS})

target_link_libraries(kaccounts_carddav_plugin
                      Qt5::Core
                      Qt5::Network
                      Qt5::Gui
                      Qt5::Xml
                      KF5::CoreAddons
                      KF5::Contacts
                      KF5::ConfigCore
                      kaccounts
                      ${ACCOUNTSQT_LIBRARIES}
                      ${SIGNONQT_LIBRARIES}
)

# Install:
install (TARGETS kaccounts_carddav_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/daemonplugins
)



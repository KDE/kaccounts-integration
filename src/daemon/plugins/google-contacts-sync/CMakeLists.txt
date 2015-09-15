project(google-contacts-plugin)

include_directories (${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
)

set(google_contacts_plugin_SRCS
    google-contacts-plugin.cpp
)

add_library(kaccounts_google_contacts_plugin MODULE ${google_contacts_plugin_SRCS})

target_link_libraries(kaccounts_google_contacts_plugin
                      Qt5::Core
                      Qt5::DBus
                      Qt5::Xml
                      KF5::ConfigCore
                      KF5::CoreAddons
                      KF5::GAPICore
                      KF5::GAPIContacts
                      KF5::Contacts
                      kaccounts
                      ${ACCOUNTSQT_LIBRARIES}
                      ${SIGNONQT_LIBRARIES}
)

# Install:
install (TARGETS kaccounts_google_contacts_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/daemonplugins
)

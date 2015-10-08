include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(accounts_daemon_SRCS
    daemon.cpp
)

add_library(kded_accounts MODULE
    ${accounts_daemon_SRCS}
)
kcoreaddons_desktop_to_json(kded_accounts accounts.desktop)

target_link_libraries(kded_accounts
    Qt5::Core
    Qt5::Xml
    Qt5::Widgets
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::Wallet
    KF5::KIOCore
    KF5::I18n
    KF5::DBusAddons
    kaccounts
)

install(TARGETS kded_accounts DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES accounts.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)

add_subdirectory(plugins)

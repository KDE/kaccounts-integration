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
    KF5::DBusAddons
    kaccounts
)

set_target_properties(kded_accounts PROPERTIES OUTPUT_NAME accounts)
install(TARGETS kded_accounts DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kded)

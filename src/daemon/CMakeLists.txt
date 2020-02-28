include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

kcoreaddons_add_plugin(
    kded_accounts
    SOURCES daemon.cpp
    JSON kded_accounts.json
    INSTALL_NAMESPACE "kf5/kded"
)

target_link_libraries(kded_accounts
    Qt5::Core
    KF5::DBusAddons
    kaccounts
)

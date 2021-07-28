kcoreaddons_add_plugin(
    kded_accounts
    SOURCES kded_accounts.cpp
    INSTALL_NAMESPACE "kf5/kded"
)

target_link_libraries(kded_accounts
    Qt5::Core
    KF5::DBusAddons
    kaccounts
)

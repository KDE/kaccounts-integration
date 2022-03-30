kcoreaddons_add_plugin(
    kded_accounts
    SOURCES kded_accounts.cpp kded_accounts.h
    INSTALL_NAMESPACE "kf5/kded"
)

target_link_libraries(kded_accounts
    Qt::Core
    KF5::DBusAddons
    kaccounts
)

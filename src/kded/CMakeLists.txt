kcoreaddons_add_plugin(
    kded_accounts
    SOURCES kded_accounts.cpp kded_accounts.h
    INSTALL_NAMESPACE "kf${QT_MAJOR_VERSION}/kded"
)

target_link_libraries(kded_accounts
    Qt::Core
    KF${QT_MAJOR_VERSION}::DBusAddons
    kaccounts
)

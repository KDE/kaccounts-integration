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

ecm_qt_declare_logging_category(kded_accounts HEADER debug.h IDENTIFIER KACCOUNTS_KDED_LOG CATEGORY_NAME org.kde.kaccounts.kded
    DESCRIPTION "KAccounts KDED Module"
    EXPORT KAccounts
)

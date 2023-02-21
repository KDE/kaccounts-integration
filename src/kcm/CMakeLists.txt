kcoreaddons_add_plugin(kcm_kaccounts SOURCES accounts.cpp accounts.h INSTALL_NAMESPACE "plasma/kcms/systemsettings")

target_link_libraries(kcm_kaccounts
    Qt::Core
    KF5::CoreAddons
    KF5::I18n
    KF5::QuickAddons
    )

kpackage_install_package(package kcm_kaccounts kcms)
kcmutils_generate_desktop_file(kcm_kaccounts)

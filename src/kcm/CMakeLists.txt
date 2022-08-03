add_library(kcm_kaccounts MODULE accounts.cpp accounts.h)

target_link_libraries(kcm_kaccounts
    Qt::Core
    KF5::CoreAddons
    KF5::I18n
    KF5::QuickAddons
    )

install(TARGETS kcm_kaccounts DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/systemsettings)

kpackage_install_package(package kcm_kaccounts kcms)

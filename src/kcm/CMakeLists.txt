kcoreaddons_add_plugin(kcm_kaccounts SOURCES accounts.cpp accounts.h INSTALL_NAMESPACE "plasma/kcms/systemsettings")

target_link_libraries(kcm_kaccounts
    Qt::Core
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::I18n
    )

if(QT_MAJOR_VERSION STREQUAL "5")
    target_link_libraries(kcm_kaccounts KF5::QuickAddons)
else()
    target_link_libraries(kcm_kaccounts KF6::KCMUtils)
endif()

kpackage_install_package(package kcm_kaccounts kcms)
kcmutils_generate_desktop_file(kcm_kaccounts)

set(accountsettings_SRCS
    accounts.cpp
)

add_library(kcm_kaccounts MODULE ${accountsettings_SRCS})

target_link_libraries(kcm_kaccounts
    Qt5::Core
    KF5::CoreAddons
    KF5::I18n
    KF5::QuickAddons
    )
kcoreaddons_desktop_to_json(kcm_kaccounts "kcm_kaccounts.desktop")

install(TARGETS kcm_kaccounts DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)

install(FILES kcm_kaccounts.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
kpackage_install_package(package kcm_kaccounts kcms)

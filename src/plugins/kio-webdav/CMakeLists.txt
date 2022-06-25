set(kio-webdav_SRCS
    kioservices.cpp
    kioservices.h
)

kcoreaddons_add_plugin(kaccounts_kio_webdav_plugin
    SOURCES ${kio-webdav_SRCS}
    INSTALL_NAMESPACE "kaccounts/daemonplugins"
    )

target_link_libraries(kaccounts_kio_webdav_plugin
    Qt::Core
    Qt::Widgets
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::Wallet
    KF5::KIOCore
    KF5::I18n
    KF5::DBusAddons
    kaccounts
    ${ACCOUNTSQT_LIBRARIES}
    ${SIGNONQT_LIBRARIES}
    QCoro::Core
)

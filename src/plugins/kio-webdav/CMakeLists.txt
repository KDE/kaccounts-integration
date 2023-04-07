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
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::ConfigCore
    KF${QT_MAJOR_VERSION}::Wallet
    KF${QT_MAJOR_VERSION}::KIOCore
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::DBusAddons
    kaccounts
    ${ACCOUNTSQT_LIBRARIES}
    ${SIGNONQT_LIBRARIES}
    QCoro::Core
)

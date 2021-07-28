set(kio-webdav_SRCS
    kioservices.cpp
    createnetattachjob.cpp
    removenetattachjob.cpp
    createkioservice.cpp
    removekioservice.cpp)

kcoreaddons_add_plugin(kaccounts_kio_webdav_plugin
    SOURCES ${kio-webdav_SRCS}
    INSTALL_NAMESPACE "kaccounts/daemonplugins"
    )

target_link_libraries(kaccounts_kio_webdav_plugin
    Qt5::Core
    Qt5::Widgets
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::Wallet
    KF5::KIOCore
    KF5::I18n
    KF5::DBusAddons
    kaccounts
    ${ACCOUNTSQT_LIBRARIES}
    ${SIGNONQT_LIBRARIES}
)


set(kio-webdav_SRCS
    kioservices.cpp
    createnetattachjob.cpp
    removenetattachjob.cpp
    createkioservice.cpp
    removekioservice.cpp)

add_library(kaccounts_kio_webdav_plugin MODULE ${kio-webdav_SRCS})

target_link_libraries(kaccounts_kio_webdav_plugin
    Qt5::Core
#     Qt5::Xml
#     Qt5::Widgets
#     KF5::CoreAddons
#     KF5::ConfigCore
#     KF5::Wallet
#     KF5::KIOCore
#     KF5::I18n
    KF5::DBusAddons
    kaccounts
    ${ACCOUNTSQT_LIBRARIES}
    ${SIGNONQT_LIBRARIES}
)

install (TARGETS kaccounts_carddav_plugin
         DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/daemonplugins
)

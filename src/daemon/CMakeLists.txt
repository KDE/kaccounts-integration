include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(accounts_daemon_SRCS
    daemon.cpp
    kio/kioservices.cpp
    kio/createnetattachjob.cpp
    kio/removenetattachjob.cpp
    kio/createkioservice.cpp
    kio/removekioservice.cpp
)

add_library(kded_accounts MODULE
    ${accounts_daemon_SRCS}
)
kcoreaddons_desktop_to_json(kded_accounts accounts.desktop)

target_link_libraries(kded_accounts
    Qt5::Core
    Qt5::Xml
    Qt5::Widgets
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::Wallet
    KF5::KIOCore
    KF5::I18n
    KF5::DBusAddons
    kaccounts
)

install(TARGETS kded_accounts DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES accounts.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)

####################################################################################

if (KF5Akonadi_FOUND)
    set(akonadi_plugin_SRCS
        akonadi/akonadiservices.cpp
        akonadi/akonadiaccounts.cpp
        akonadi/jobs/abstractakonadijob.cpp
        akonadi/jobs/createresourcejob.cpp
        akonadi/jobs/removeresourcejob.cpp
        akonadi/jobs/changesettingsjob.cpp
        akonadi/jobs/fetchsettingsjob.cpp
        akonadi/jobs/enableservicejob.cpp
        akonadi/jobs/dbussettingsinterfacejob.cpp
        akonadi/jobs/lookupakonadiservices.cpp
        akonadi/jobs/removeakonadiservicesjob.cpp)

    add_library(kaccounts_akonadi_plugin MODULE ${akonadi_plugin_SRCS})

    target_link_libraries(kaccounts_akonadi_plugin
        kaccounts
        Qt5::DBus
        KF5::ConfigCore
        KF5::AkonadiCore)

    # Install:
    install (TARGETS kaccounts_akonadi_plugin
            DESTINATION ${PLUGIN_INSTALL_DIR}/kaccounts/daemonplugins
)

endif()

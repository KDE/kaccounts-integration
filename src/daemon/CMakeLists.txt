# find_package(Akonadi REQUIRED)
# find_package(KdepimLibs REQUIRED)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${AKONADI_INCLUDE_DIR})

set(accounts_daemon_SRCS
    daemon.cpp
#     akonadi/akonadiservices.cpp
#     akonadi/akonadiaccounts.cpp
#     akonadi/jobs/abstractakonadijob.cpp
#     akonadi/jobs/createresourcejob.cpp
#     akonadi/jobs/removeresourcejob.cpp
#     akonadi/jobs/changesettingsjob.cpp
#     akonadi/jobs/fetchsettingsjob.cpp
#     akonadi/jobs/enableservicejob.cpp
#     akonadi/jobs/dbussettingsinterfacejob.cpp
#     akonadi/jobs/lookupakonadiservices.cpp
#     akonadi/jobs/removeakonadiservicesjob.cpp
    kio/kioservices.cpp
    kio/createnetattachjob.cpp
    kio/removenetattachjob.cpp
    kio/createkioservice.cpp
    kio/removekioservice.cpp
)

add_library(kded_accounts MODULE
    ${accounts_daemon_SRCS}
)
kservice_desktop_to_json(kded_accounts accounts.desktop)

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

#     ${KDEPIMLIBS_AKONADI_LIBS}
    ${ACCOUNTSQT_LIBRARIES}
    ${SIGNONQT_LIBRARIES}
)

install(TARGETS kded_accounts DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES accounts.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)

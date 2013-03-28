find_package(Akonadi REQUIRED)
find_package(KdepimLibs REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${AKONADI_INCLUDE_DIR})

set(accounts_daemon_SRCS
    daemon.cpp
    akonadi/akonadiaccounts.cpp
    akonadi/jobs/abstractakonadijob.cpp
    akonadi/jobs/createresourcejob.cpp
    akonadi/jobs/removeresourcejob.cpp
    akonadi/jobs/changesettingsjob.cpp
    akonadi/jobs/fetchsettingsjob.cpp
    akonadi/jobs/enableservicejob.cpp
    akonadi/jobs/dbussettingsinterfacejob.cpp
    akonadi/jobs/lookupakonadiservices.cpp
    akonadi/jobs/removeakonadiservicesjob.cpp
)

kde4_add_plugin(kded_accounts
    ${accounts_daemon_SRCS}
)

target_link_libraries(kded_accounts
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTXML_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDEPIMLIBS_AKONADI_LIBS}
    ${ACCOUNTSQT_LIBRARIES}
)

install(TARGETS kded_accounts DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES accounts-daemon.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
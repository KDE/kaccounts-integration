find_package(Akonadi REQUIRED)
find_package(KdepimLibs REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${AKONADI_INCLUDE_DIR})

set(accounts_daemon_SRCS
    daemon.cpp
    akonadiaccounts.cpp
    jobs/createresourcejob.cpp
    jobs/removeresourcejob.cpp
    jobs/changesettingsjob.cpp
    jobs/fetchsettingsjob.cpp
    jobs/enableservicejob.cpp
    jobs/dbussettingsinterfacejob.cpp
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
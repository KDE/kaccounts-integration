add_subdirectory(daemon)

set(webaccounts_SRCS
    webaccounts.cpp
    create.cpp
    accountwidget.cpp
    models/accountsmodel.cpp
    models/modeltest.cpp
    jobs/createaccount.cpp
    jobs/getcredentialsjob.cpp
    jobs/ownclouddialog.cpp
)

qt4_automoc(${webaccounts_SRCS})

kde4_add_ui_files(webaccounts_SRCS kcm.ui types.ui services.ui owncloudDialog.ui)

kde4_add_plugin(webaccounts ${webaccounts_SRCS})

target_link_libraries(webaccounts
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${KDE4_KDEUI_LIBRARY}
    ${ACCOUNTSQT_LIBRARIES}
    ${SIGNONQT_LIBRARIES}
)

install(TARGETS webaccounts DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES webaccounts.desktop DESTINATION ${SERVICES_INSTALL_DIR})

add_subdirectory(daemon)

set(kdeaccounts_SRCS
    webaccounts.cpp
    create.cpp
    accountwidget.cpp
    models/accountsmodel.cpp
    models/modeltest.cpp
    jobs/createaccount.cpp
    jobs/getcredentialsjob.cpp
    jobs/ownclouddialog.cpp
    jobs/checkowncloudhostjob.cpp
)

qt4_automoc(${kdeaccounts_SRCS})

kde4_add_ui_files(kdeaccounts_SRCS kcm.ui types.ui services.ui owncloudDialog.ui)

kde4_add_plugin(kcm_kdeaccounts ${kdeaccounts_SRCS})

target_link_libraries(kcm_kdeaccounts
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${KDE4_KDEUI_LIBRARY}
    ${KDE4_KIO_LIBRARY}
    ${ACCOUNTSQT_LIBRARIES}
    ${SIGNONQT_LIBRARIES}
    ${QJSON_LIBRARIES}
    ${QJSON_LIBRARY}
)

install(TARGETS kcm_kdeaccounts DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kcm_kdeaccounts.desktop DESTINATION ${SERVICES_INSTALL_DIR})

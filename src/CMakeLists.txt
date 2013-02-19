

set(webaccounts_SRCS
    webaccounts.cpp
    create.cpp
    accountwidget.cpp
    models/accountsmodel.cpp
    models/modeltest.cpp
    providerconfig.cpp
    jobs/removeakonadiresource.cpp

    google/google.cpp
    google/pages/credentials.cpp
    google/pages/oauth.cpp
    google/pages/services.cpp
    google/pages/serviceoption.cpp
    google/pages/wallet.cpp
    google/jobs/createcalendar.cpp
    google/jobs/createtask.cpp
    google/jobs/createchat.cpp
    google/jobs/createmail.cpp
    google/jobs/createcontact.cpp
    google/jobs/removechat.cpp
    google/jobs/removeemail.cpp
    google/jobs/removecalendar.cpp
    google/jobs/removetask.cpp

    facebook/facebook.cpp
    facebook/faccountwidget.cpp
    facebook/pages/fcredentials.cpp
    facebook/pages/foauth.cpp
    facebook/pages/fservices.cpp
    facebook/jobs/fcreatechat.cpp
    facebook/jobs/fcreatepim.cpp

    owncloud/owncloud.cpp
    owncloud/oaccountwidget.cpp
    owncloud/pages/basicinfo.cpp
    owncloud/pages/connecting.cpp
    owncloud/pages/oservices.cpp

    owncloud/jobs/ocreatefile.cpp
    owncloud/jobs/ocreatecontact.cpp
    owncloud/jobs/ocreatecalendar.cpp
    owncloud/jobs/oremovecalendar.cpp
    owncloud/jobs/oremovecontact.cpp
    owncloud/jobs/oremovefile.cpp

    runnerid/runnerid.cpp
    runnerid/pages/rcredentials.cpp
    runnerid/pages/rservices.cpp
)

qt4_add_dbus_interface(webaccounts_SRCS
    google/jobs/org.kde.Akonadi.GoogleCalendar.Settings.xml
    google_calendar_settings
)
qt4_add_dbus_interface(webaccounts_SRCS
    google/jobs/org.kde.Akonadi.Imap.Settings.xml
    akonadi_imap_settings
)
qt4_add_dbus_interface(webaccounts_SRCS
    google/jobs/org.kde.Akonadi.GoogleContacts.Settings.xml
    google_contact_settings
)
qt4_add_dbus_interface(webaccounts_SRCS
    facebook/jobs/org.kde.Akonadi.Facebook.Settings.xml
    facebook_settings
)
qt4_add_dbus_interface(webaccounts_SRCS
    owncloud/jobs/org.kde.Akonadi.davGroupware.Settings.xml
    davGroupware_settings
)

qt4_automoc(${webaccounts_SRCS})

kde4_add_ui_files(webaccounts_SRCS
    kcm.ui
    types.ui
    google/pages/credentials.ui
    google/pages/oauth.ui
    google/pages/services.ui
    google/pages/serviceoption.ui
    google/pages/wallet.ui

    facebook/pages/foauth.ui
    owncloud/pages/basicinfo.ui
    owncloud/pages/connecting.ui)

kde4_add_plugin(webaccounts ${webaccounts_SRCS})

target_link_libraries(webaccounts ${KDE4_KIO_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
${KDEPIMLIBS_AKONADI_LIBS} ${KDEPIMLIBS_MAILTRANSPORT_LIBS} ${LibKGAPI_LIBRARY} ${TELEPATHY_QT4_LIBRARIES}
${KTP_LIBRARIES} ${LIBKFACEBOOK_LIBRARY} ${QJSON_LIBRARIES} ${QJSON_LIBRARY} ${KDEPIMLIBS_KPIMUTILS_LIBS}
${ACCOUNTSQT_LIBRARIES} ${SIGNONQT_LIBRARIES})

install(TARGETS webaccounts DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES webaccounts.desktop DESTINATION ${SERVICES_INSTALL_DIR})

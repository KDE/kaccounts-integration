

set(webaccounts_SRCS
    webaccounts.cpp
    create.cpp
    accountwidget.cpp
    jobs/removeakonadiresource.cpp

    google/google.cpp
    google/pages/credentials.cpp
    google/pages/oauth.cpp
    google/pages/services.cpp
    google/pages/serviceoption.cpp
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
    facebook/fcredentials.cpp
    facebook/foauth.cpp
    facebook/fservices.cpp
    jobs/fcreatechat.cpp
    jobs/fcreatepim.cpp

    owncloud/owncloud.cpp
    owncloud/basicinfo.cpp
    owncloud/connecting.cpp
    owncloud/oservices.cpp
    jobs/ocreatefile.cpp
    jobs/ocreatecontact.cpp
    jobs/ocreatecalendar.cpp
    jobs/oremovecalendar.cpp
    jobs/oremovecontact.cpp
    jobs/oremovefile.cpp
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
    jobs/org.kde.Akonadi.Facebook.Settings.xml
    facebook_settings
)
qt4_add_dbus_interface(webaccounts_SRCS
    jobs/org.kde.Akonadi.davGroupware.Settings.xml
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

    owncloud/basicinfo.ui
    owncloud/connecting.ui)

kde4_add_plugin(webaccounts ${webaccounts_SRCS})

target_link_libraries(webaccounts ${KDE4_KIO_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
${KDEPIMLIBS_AKONADI_LIBS} ${KDEPIMLIBS_MAILTRANSPORT_LIBS} ${LIBKGOOGLE_LIBRARY} ${TELEPATHY_QT4_LIBRARIES}
${KTP_LIBRARIES} ${LIBKFACEBOOK_LIBRARY} ${QJSON_LIBRARIES} ${QJSON_LIBRARY} ${KDEPIMLIBS_KPIMUTILS_LIBS})

install(TARGETS webaccounts DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES webaccounts.desktop DESTINATION ${SERVICES_INSTALL_DIR})
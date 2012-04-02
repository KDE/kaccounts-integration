

set(webaccounts_SRCS
    webaccounts.cpp
    create.cpp
    accountwidget.cpp
    google/google.cpp
    google/credentials.cpp
    google/oauth.cpp
    google/services.cpp
    google/serviceoption.cpp
    jobs/createcalendar.cpp
    jobs/createtask.cpp
    jobs/createchat.cpp
    jobs/createmail.cpp
    jobs/createcontact.cpp
)

qt4_add_dbus_interface(webaccounts_SRCS
    jobs/org.kde.Akonadi.GoogleCalendar.Settings.xml
    google_calendar_settings
)
qt4_add_dbus_interface(webaccounts_SRCS
    jobs/org.kde.Akonadi.Imap.Settings.xml
    akonadi_imap_settings
)
qt4_add_dbus_interface(webaccounts_SRCS
    jobs/org.kde.Akonadi.GoogleContacts.Settings.xml
    google_contact_settings
)

qt4_automoc(${webaccounts_SRCS})

kde4_add_ui_files(webaccounts_SRCS kcm.ui types.ui google/google.ui google/oauth.ui google/services.ui google/serviceoption.ui)

kde4_add_plugin(webaccounts ${webaccounts_SRCS})
message (WARNING ${KDEPIMLIBS_AKONADI_LIBS})
target_link_libraries(webaccounts ${KDE4_KIO_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDEPIMLIBS_AKONADI_LIBS} ${KDEPIMLIBS_MAILTRANSPORT_LIBS} ${LIBKGOOGLE_LIBRARY} ${TELEPATHY_QT4_LIBRARIES} ${KTP_LIBRARIES})

install(TARGETS webaccounts DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES webaccounts.desktop DESTINATION ${SERVICES_INSTALL_DIR})
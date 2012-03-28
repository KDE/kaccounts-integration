

set(webaccounts_SRCS
    webaccounts.cpp
    create.cpp
    accountwidget.cpp
    google/google.cpp
    google/credentials.cpp
    google/oauth.cpp
    google/services.cpp
    google/serviceoption.cpp
)

qt4_automoc(${webaccounts_SRCS})

kde4_add_ui_files(webaccounts_SRCS kcm.ui types.ui google/google.ui google/oauth.ui google/services.ui google/serviceoption.ui)

kde4_add_plugin(webaccounts ${webaccounts_SRCS})

target_link_libraries(webaccounts ${KDE4_KIO_LIBS} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${LIBKGOOGLE_LIBRARY})

install(TARGETS webaccounts DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES webaccounts.desktop DESTINATION ${SERVICES_INSTALL_DIR})
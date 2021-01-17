add_library(kaccountsdeclarativeplugin SHARED kaccountsdeclarativeplugin.cpp)

target_link_libraries(kaccountsdeclarativeplugin Qt5::Qml
                                                 KF5::I18n
                                                 kaccounts
                                                 )

install(TARGETS kaccountsdeclarativeplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kaccounts)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kaccounts)

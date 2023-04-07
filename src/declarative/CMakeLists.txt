ecm_add_qml_module(kaccountsdeclarativeplugin URI "org.kde.kaccounts" VERSION 1.2)

target_sources(kaccountsdeclarativeplugin PRIVATE kaccountsdeclarativeplugin.cpp kaccountsdeclarativeplugin.h)

target_link_libraries(kaccountsdeclarativeplugin Qt::Qml
                                                 KF${QT_MAJOR_VERSION}::I18n
                                                 kaccounts
                                                 )

ecm_finalize_qml_module(kaccountsdeclarativeplugin DESTINATION ${KDE_INSTALL_QMLDIR})

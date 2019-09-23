include_directories(${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(kaccountsdeclarativeplugin SHARED kaccountsdeclarativeplugin.cpp
                            ../jobs/createaccount.cpp
                            ../lib/uipluginsmanager.cpp)

target_link_libraries(kaccountsdeclarativeplugin Qt5::Qml
                                                 KF5::I18n
                                                 kaccounts
                                                 ${SIGNONQT_LIBRARIES})

target_include_directories(kaccountsdeclarativeplugin PRIVATE "${SIGNONQT_INCLUDE_DIRS}")

install(TARGETS kaccountsdeclarativeplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kaccounts)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kaccounts)

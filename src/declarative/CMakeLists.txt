add_library(kaccountsdeclarativeplugin SHARED kaccountsdeclarativeplugin.cpp
                            ../jobs/createaccount.cpp ../jobs/ownclouddialog.cpp ../jobs/checkowncloudhostjob.cpp
                            ../uipluginsmanager.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(kaccountsdeclarativeplugin Qt5::Qml Qt5::Widgets
                      KF5::CoreAddons KF5::I18n KF5::ConfigCore KF5::KIOCore KF5::WidgetsAddons KF5::IconThemes
                      kaccounts ${SIGNONQT_LIBRARIES})
target_include_directories(kaccountsdeclarativeplugin PRIVATE "${SIGNONQT_INCLUDE_DIRS}")

install(TARGETS kaccountsdeclarativeplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kaccounts)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kaccounts)

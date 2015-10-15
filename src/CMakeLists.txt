add_subdirectory(daemon)
add_subdirectory(lib)
add_subdirectory(declarative)

set(kaccounts_SRCS
    kaccounts.cpp
    create.cpp
    accountwidget.cpp
    uipluginsmanager.cpp
    models/accountsmodel.cpp
    jobs/createaccount.cpp
)

ki18n_wrap_ui(kaccounts_SRCS kcm.ui types.ui services.ui)

add_library(kcm_kaccounts MODULE ${kaccounts_SRCS})
kcoreaddons_desktop_to_json(kcm_kaccounts kcm_kaccounts.desktop)

target_link_libraries(kcm_kaccounts
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
    KF5::I18n
    KF5::KCMUtils
    ${SIGNONQT_LIBRARIES}
    kaccounts
)
target_include_directories(kcm_kaccounts PRIVATE "${SIGNONQT_INCLUDE_DIRS}")


install(TARGETS kcm_kaccounts DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kcm_kaccounts.desktop DESTINATION ${SERVICES_INSTALL_DIR})

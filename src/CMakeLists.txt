add_subdirectory(daemon)
add_subdirectory(lib)

set(kaccounts_SRCS
    kaccounts.cpp
    create.cpp
    accountwidget.cpp
    uipluginsmanager.cpp
    models/accountsmodel.cpp
#     models/modeltest.cpp
    jobs/createaccount.cpp
    jobs/ownclouddialog.cpp
    jobs/checkowncloudhostjob.cpp
)

ki18n_wrap_ui(kaccounts_SRCS kcm.ui types.ui services.ui owncloudDialog.ui)

add_library(kcm_kaccounts MODULE ${kaccounts_SRCS})
kcoreaddons_desktop_to_json(kcm_kaccounts kcm_kaccounts.desktop)

target_link_libraries(kcm_kaccounts
    Qt5::Core
    Qt5::Gui
    KF5::KIOCore
    KF5::I18n
    KF5::KCMUtils
    KF5::IconThemes
    ${ACCOUNTSQT_LIBRARIES}
    ${SIGNONQT_LIBRARIES}
    kaccounts
)


install(TARGETS kcm_kaccounts DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kcm_kaccounts.desktop DESTINATION ${SERVICES_INSTALL_DIR})

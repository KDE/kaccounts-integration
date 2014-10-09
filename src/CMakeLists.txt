add_subdirectory(daemon)
add_subdirectory(lib)

set(kdeaccounts_SRCS
    webaccounts.cpp
    create.cpp
    accountwidget.cpp
    models/accountsmodel.cpp
#     models/modeltest.cpp
    jobs/createaccount.cpp
    jobs/ownclouddialog.cpp
    jobs/checkowncloudhostjob.cpp
)

ki18n_wrap_ui(kdeaccounts_SRCS kcm.ui types.ui services.ui owncloudDialog.ui)

add_library(kcm_kdeaccounts MODULE ${kdeaccounts_SRCS})
kservice_desktop_to_json(kcm_kdeaccounts kcm_kdeaccounts.desktop)

target_link_libraries(kcm_kdeaccounts
    Qt5::Core
    Qt5::Gui
    KF5::KIOCore
    KF5::I18n
    KF5::KCMUtils
    KF5::IconThemes
    ${ACCOUNTSQT_LIBRARIES}
    ${SIGNONQT_LIBRARIES}
    ${QJSON_LIBRARIES}
    ${QJSON_LIBRARY}
    kaccounts
)


install(TARGETS kcm_kdeaccounts DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kcm_kdeaccounts.desktop DESTINATION ${SERVICES_INSTALL_DIR})

kcmutils_add_qml_kcm(kcm_kaccounts SOURCES accounts.cpp accounts.h)

target_link_libraries(kcm_kaccounts PRIVATE
    Qt::Core
    KF6::CoreAddons
    KF6::I18n
    KF6::KCMUtils
)

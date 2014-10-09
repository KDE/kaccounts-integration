project (kaccounts-support-library)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set (kaccountslib_SRCS
     kaccountsuiplugin.cpp
     kaccountsdplugin.cpp
     getcredentialsjob.cpp
     core.cpp
)


add_library (kaccounts SHARED
             ${kaccountslib_SRCS}
)

target_link_libraries (kaccounts Qt5::Core
                                 Qt5::Xml
                                 KF5::CoreAddons
                                 ${ACCOUNTSQT_LIBRARIES}
                                 ${SIGNONQT_LIBRARIES}
)

# Install:
install (TARGETS kaccounts
         ${INSTALL_TARGETS_DEFAULT_ARGS}
)


install (FILES
        core.h
        getcredentialsjob.h
        kaccountsuiplugin.h
        kaccountsdplugin.h
        kaccounts_export.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/KAccounts COMPONENT Devel
)
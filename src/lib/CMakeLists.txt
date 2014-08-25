project (kaccounts-support-library)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set (kaccountslib_SRCS
     kaccountsuiplugin.cpp
     kaccountsdplugin.cpp
)


add_library (kaccounts SHARED
             ${kaccountslib_SRCS}
)

target_link_libraries (kaccounts Qt5::Core)

# Install:
install (TARGETS kaccounts
         ${INSTALL_TARGETS_DEFAULT_ARGS}
)


install (FILES
        kaccountsuiplugin.h
        kaccountsdplugin.h
        kaccounts_export.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/KAccounts COMPONENT Devel
)
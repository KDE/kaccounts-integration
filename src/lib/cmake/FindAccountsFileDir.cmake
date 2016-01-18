# The provider and service files used with KAccounts REQUIRE
# an env variable set (see https://bugs.kde.org/347219), so
# we might as well just read it directly as an install path
# if those are not set, read CMAKE_INSTALL_PREFIX and add
# the subpath of share/kaccounts/*

if(ENV{AG_PROVIDERS} AND ENV{AG_SERVICES})
    set(ACCOUNTS_PROVIDERS_DIR $ENV{AG_PROVIDERS})
    set(ACCOUNTS_SERVICES_DIR $ENV{AG_SERVICES})
else()
    set(ACCOUNTS_PROVIDERS_DIR ${CMAKE_INSTALL_PREFIX}/share/kaccounts/providers)
    set(ACCOUNTS_SERVICES_DIR ${CMAKE_INSTALL_PREFIX}/share/kaccounts/services)

    message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n\n\
    Provider files will be installed to ${ACCOUNTS_PROVIDERS_DIR} \n\
    Service files will be installed to ${ACCOUNTS_SERVICES_DIR} \n\
    \n\
    Please ensure your system has these environment variables set:\n\n\
    AG_PROVIDERS=${ACCOUNTS_PROVIDERS_DIR}
    AG_SERVICES=${ACCOUNTS_SERVICES_DIR}\n\n\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
endif()

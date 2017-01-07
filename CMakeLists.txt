project(KAccounts)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

enable_testing()

set(KDE_APPLICATIONS_VERSION_MAJOR "16")
set(KDE_APPLICATIONS_VERSION_MINOR "12")
set(KDE_APPLICATIONS_VERSION_MICRO "1")
set(KACCOUNTS_VERSION "${KDE_APPLICATIONS_VERSION_MAJOR}.${KDE_APPLICATIONS_VERSION_MINOR}.${KDE_APPLICATIONS_VERSION_MICRO}")
set(KACCOUNTS_SOVERSION "1")
set(REQUIRED_QT_VERSION "5.2.0")
set(REQUIRED_KF5_VERSION "5.4.0")
set(ACCOUNTSQT_DEP_VERSION "1.13")
set(SIGNONQT_DEP_VERSION "8.55")
set(ACCOUNTSGLIB_DEP_VERSION "1.21")

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Widgets)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED KCMUtils I18n CoreAddons DBusAddons Declarative)

find_package(AccountsQt5 ${ACCOUNTSQT_DEP_VERSION} CONFIG)
set_package_properties(AccountsQt5 PROPERTIES DESCRIPTION "Accounts management library for Qt applications"
   URL "https://gitlab.com/accounts-sso/libaccounts-qt"
   TYPE REQUIRED
   PURPOSE "Required for building this module")

find_package(SignOnQt5 ${SIGNONQT_DEP_VERSION} CONFIG)
set_package_properties(SignOnQt5 PROPERTIES DESCRIPTION "D-Bus service which performs user authentication on behalf of its clients"
   URL "https://gitlab.com/accounts-sso/signond"
   TYPE REQUIRED
   PURPOSE "Required for building this module")

find_package(LibAccountsGlib ${ACCOUNTSGLIB_DEP_VERSION})
set_package_properties(LibAccountsGlib PROPERTIES DESCRIPTION "GLib-based client library for the accounts database"
   URL "https://gitlab.com/accounts-sso/libaccounts-glib"
   TYPE REQUIRED
   PURPOSE "Required for building this module")

add_definitions (-fexceptions
                 -DQT_NO_KEYWORDS)

add_definitions(-DTRANSLATION_DOMAIN=\"kaccounts-integration\")

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(FeatureSummary)

include_directories(${ACCOUNTSQT_INCLUDE_DIRS} ${SIGNONQT_INCLUDE_DIRS})

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

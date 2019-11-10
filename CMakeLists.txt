project(KAccounts)

cmake_minimum_required(VERSION 2.8.12)

set(QT_REQUIRED_VERSION "5.7.0")
set(KF5_MIN_VERSION "5.4.0")


find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

enable_testing()

set(KDE_APPLICATIONS_VERSION_MAJOR "20")
set(KDE_APPLICATIONS_VERSION_MINOR "03")
set(KDE_APPLICATIONS_VERSION_MICRO "70")
set(KACCOUNTS_VERSION "${KDE_APPLICATIONS_VERSION_MAJOR}.${KDE_APPLICATIONS_VERSION_MINOR}.${KDE_APPLICATIONS_VERSION_MICRO}")
set(KACCOUNTS_SOVERSION "1")
set(ACCOUNTSQT_DEP_VERSION "1.13")
set(SIGNONQT_DEP_VERSION "8.55")
set(ACCOUNTSGLIB_DEP_VERSION "1.21")

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED KCMUtils I18n CoreAddons DBusAddons Declarative)

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

add_definitions (
                 -DQT_NO_KEYWORDS)

add_definitions(-DTRANSLATION_DOMAIN=\"kaccounts-integration\")
add_definitions(-DQT_NO_FOREACH)
if (EXISTS "${CMAKE_SOURCE_DIR}/.git" AND AccountsQt5_VERSION VERSION_GREATER 1.15)
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
   add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x060000)
endif()

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(FeatureSummary)

kde_enable_exceptions()

include_directories(${ACCOUNTSQT_INCLUDE_DIRS} ${SIGNONQT_INCLUDE_DIRS})

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

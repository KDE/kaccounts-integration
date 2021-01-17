cmake_minimum_required(VERSION 3.16)

set(RELEASE_SERVICE_VERSION_MAJOR "21")
set(RELEASE_SERVICE_VERSION_MINOR "03")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(KACCOUNTS_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(KAccounts LANGUAGES CXX VERSION ${KACCOUNTS_VERSION})

set(QT_REQUIRED_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.78.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(KACCOUNTS_SOVERSION "2")
set(ACCOUNTSQT_DEP_VERSION "1.13")
set(SIGNONQT_DEP_VERSION "8.55")
set(ACCOUNTSGLIB_DEP_VERSION "1.21")

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED KCMUtils I18n CoreAddons DBusAddons Declarative Wallet KIO)

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

add_definitions (-DTRANSLATION_DOMAIN=\"kaccounts-integration\" -DQT_NO_KEYWORDS -DQT_NO_FOREACH)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(FeatureSummary)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(KDEClangFormat)

if(KF5_VERSION VERSION_GREATER_EQUAL 5.79)
    include(KDEGitCommitHooks)
endif()

add_subdirectory(src)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

if(KF5_VERSION VERSION_GREATER_EQUAL 5.79)
    kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

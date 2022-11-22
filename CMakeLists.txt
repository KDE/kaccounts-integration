cmake_minimum_required(VERSION 3.16)

set(RELEASE_SERVICE_VERSION_MAJOR "22")
set(RELEASE_SERVICE_VERSION_MINOR "11")
set(RELEASE_SERVICE_VERSION_MICRO "90")
set(KACCOUNTS_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(KAccounts LANGUAGES CXX VERSION ${KACCOUNTS_VERSION})

set(QT_REQUIRED_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.97.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(FeatureSummary)
include(ECMQmlModule)

set(KACCOUNTS_SOVERSION "2")
set(ACCOUNTSQT_DEP_VERSION "1.13")
set(SIGNONQT_DEP_VERSION "8.55")
set(ACCOUNTSGLIB_DEP_VERSION "1.21")

find_package(Qt${QT_MAJOR_VERSION} ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED KCMUtils I18n CoreAddons DBusAddons Declarative Wallet KIO)

find_package(AccountsQt${QT_MAJOR_VERSION} ${ACCOUNTSQT_DEP_VERSION} CONFIG)
set_package_properties(AccountsQt${QT_MAJOR_VERSION} PROPERTIES DESCRIPTION "Accounts management library for Qt applications"
   URL "https://gitlab.com/accounts-sso/libaccounts-qt"
   TYPE REQUIRED
   PURPOSE "Required for building this module")

find_package(SignOnQt${QT_MAJOR_VERSION} ${SIGNONQT_DEP_VERSION} CONFIG)
set_package_properties(SignOnQt${QT_MAJOR_VERSION} PROPERTIES DESCRIPTION "D-Bus service which performs user authentication on behalf of its clients"
   URL "https://gitlab.com/accounts-sso/signond"
   TYPE REQUIRED
   PURPOSE "Required for building this module")

add_definitions (-DTRANSLATION_DOMAIN=\"kaccounts-integration\")

add_subdirectory(src)

ki18n_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

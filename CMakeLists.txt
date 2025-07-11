cmake_minimum_required(VERSION 3.16)

set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "11")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(KACCOUNTS_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(KAccounts LANGUAGES CXX VERSION ${KACCOUNTS_VERSION})

set(QT_REQUIRED_VERSION "6.5.0")
set(KF_MIN_VERSION "6.1.0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# make KDE CI pass the right flag when building for Qt5
if($ENV{CI_JOB_NAME} MATCHES ".*qt515")
   set(KF6_COMPAT_BUILD ON)
endif()

option(KF6_COMPAT_BUILD "Enable this to get a build to support KF5 applications" OFF)

if(KF6_COMPAT_BUILD)
   set(QT_REQUIRED_VERSION "5.15.2")
   set(KF_MIN_VERSION "5.97.0")
   set(QT_MAJOR_VERSION "5")
endif()

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
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
include(ECMDeprecationSettings)
include(ECMQtDeclareLoggingCategory)

set(KACCOUNTS_SOVERSION "2")
set(ACCOUNTSQT_DEP_VERSION "1.16")
set(SIGNONQT_DEP_VERSION "8.55")
set(ACCOUNTSGLIB_DEP_VERSION "1.21")

find_package(Qt${QT_MAJOR_VERSION} ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Widgets Qml)
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED KCMUtils I18n CoreAddons DBusAddons Wallet KIO)

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

find_package(QCoro${QT_MAJOR_VERSION} REQUIRED COMPONENTS Core)

qcoro_enable_coroutines()

add_definitions(-DTRANSLATION_DOMAIN=\"kaccounts-integration\")

ecm_set_disabled_deprecation_versions(QT 5.13.0
     KF 5.101.0
)

add_subdirectory(src)

if(NOT KF6_COMPAT_BUILD)
   ki18n_install(po)
endif()

ecm_qt_install_logging_categories(
    EXPORT KAccounts
    FILE kaccounts.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

project(KAccounts)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

enable_testing()

set(KDE_APPLICATIONS_VERSION_MAJOR "15")
set(KDE_APPLICATIONS_VERSION_MINOR "08")
set(KDE_APPLICATIONS_VERSION_MICRO "1")
set(KACCOUNTS_VERSION "${KDE_APPLICATIONS_VERSION_MAJOR}.${KDE_APPLICATIONS_VERSION_MINOR}.${KDE_APPLICATIONS_VERSION_MICRO}")
set(KACCOUNTS_SOVERSION "1")
set(REQUIRED_QT_VERSION "5.2.0")
set(REQUIRED_KF5_VERSION "5.4.0")
set(ACCOUNTSQT_DEP_VERSION "1.10")
set(SIGNONQT_DEP_VERSION "8.55")

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Widgets Qml)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED KCMUtils KIO I18n WidgetsAddons CoreAddons IconThemes Config Wallet DBusAddons)
find_package(KF5Akonadi) #Akonadi-qt5 is at version 4.75.0 right now, so cannot be up here^
find_package(AccountsQt5 ${ACCOUNTSQT_DEP_VERSION} REQUIRED CONFIG)
find_package(SignOnQt5 ${SIGNONQT_DEP_VERSION} REQUIRED CONFIG)

add_definitions (-fexceptions
                 -DQT_NO_KEYWORDS)

add_definitions(-DTRANSLATION_DOMAIN=\"kaccounts-integration\")

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)
include(KDECompilerSettings)
include(ECMInstallIcons)
include(FeatureSummary)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_subdirectory(src)
add_subdirectory(tests)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

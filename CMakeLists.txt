project(webaccounts)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(IS_KTP_INTERNAL_MODULE TRUE)
enable_testing()

find_package(Qt5 5.2.0 CONFIG REQUIRED Core Widgets)
find_package(KF5 5.0.0 REQUIRED KCMUtils KIO I18n WidgetsAddons)
# find_package(KdepimLibs REQUIRED)
# find_package(Akonadi REQUIRED)
# find_package(QJSON REQUIRED)
find_package(AccountsQt 1.10 REQUIRED CONFIG)
find_package(SignOnQt 8.55 REQUIRED CONFIG)

add_definitions (-fexceptions
                 -DQT_NO_KEYWORDS)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)
include(KDECompilerSettings)
include(ECMInstallIcons)
include(FeatureSummary)

include_directories(
#     ${CMAKE_CURRENT_BINARY_DIR}
#     ${CMAKE_CURRENT_BINARY_DIR}/src
#     ${KDE4_INCLUDES}
    ${ACCOUNTSQT_INCLUDE_DIRS}
    ${SIGNONQT_INCLUDE_DIRS}
#     ${KDEPIMLIBS_INCLUDE_DIRS}
#     ${AKONADI_INCLUDE_DIRS}
)

add_subdirectory(src)
# add_subdirectory(tests)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

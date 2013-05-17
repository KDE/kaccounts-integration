project(webaccounts)

cmake_minimum_required(VERSION 2.6)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(IS_KTP_INTERNAL_MODULE TRUE)
enable_testing()

find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(QJSON REQUIRED)
find_package(AccountsQt REQUIRED)
find_package(SignOnQt REQUIRED)

add_definitions (${KDE4_DEFINITIONS}
                 -fexceptions
                 -DQT_NO_KEYWORDS
)

include(KDE4Defaults)
include(MacroOptionalAddSubdirectory)

include_directories(${QT_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${KDE4_INCLUDES}
    ${ACCOUNTSQT_INCLUDE_DIRS}
    ${SIGNONQT_INCLUDE_DIRS}
)

add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(tests)
project(webaccounts)

cmake_minimum_required(VERSION 2.6)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(IS_KTP_INTERNAL_MODULE TRUE)

find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(Akonadi REQUIRED)
find_package(LibKGAPI REQUIRED)
find_package(LibKFacebook REQUIRED)
find_package(KdepimLibs 4.7.41 REQUIRED)
find_package (TelepathyQt4 0.8.9 REQUIRED)
find_package(QJSON)
find_package (KTp REQUIRED)
find_package(AccountsQt REQUIRED)
find_package(SignOnQt REQUIRED)

add_definitions (${KDE4_DEFINITIONS}
                 -fexceptions
                 -DQT_NO_KEYWORDS
)

include(KDE4Defaults)
include(MacroOptionalAddSubdirectory)

MESSAGE(WARNING ${LIBKFACEBOOK_INCLUDE_DIR})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/src 
${KDE4_INCLUDES} ${LibKGAPI_INCLUDE_DIR} ${AKONADI_INCLUDE_DIR} ${TELEPATHY_QT4_INCLUDE_DIR} 
${KTP_INCLUDE_DIR} ${ACCOUNTSQT_INCLUDE_DIRS} ${SIGNONQT_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(icons)

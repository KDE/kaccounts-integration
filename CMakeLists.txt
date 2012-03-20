project(webaccounts)

cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)

include(KDE4Defaults)
include(MacroOptionalAddSubdirectory)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
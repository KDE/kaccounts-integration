find_package(KdepimLibs REQUIRED)

add_definitions("-DTEST_DATA=\\\"${CMAKE_CURRENT_SOURCE_DIR}/providers/\\\"")

set(testname testproviderconfig)
qt4_generate_moc(${testname}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${testname}.moc ../src/providerconfig.cpp)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp ${testname}.moc ../src/providerconfig.cpp)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY})
add_test(${testname} ${CMAKE_CURRENT_BINARY_DIR}/${testname})


set(testname testdbussettingsjob)
qt4_generate_moc(${testname}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${testname}.moc ../src/daemon/dbussettingspathjob.cpp)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp ${testname}.moc ../src/daemon/dbussettingspathjob.cpp)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS})
add_test(${testname} ${CMAKE_CURRENT_BINARY_DIR}/${testname})

set(testname testfetchsettingsjob)
qt4_generate_moc(${testname}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${testname}.moc ../src/daemon/fetchsettingsjob.cpp ../src/daemon/dbussettingspathjob.cpp)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp ${testname}.moc ../src/daemon/fetchsettingsjob.cpp ../src/daemon/dbussettingspathjob.cpp)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS})
add_test(${testname} ${CMAKE_CURRENT_BINARY_DIR}/${testname})

set(testname testchangesettingsjob)
qt4_generate_moc(${testname}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${testname}.moc ../src/daemon/changesettingsjob.cpp ../src/daemon/dbussettingspathjob.cpp)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp ${testname}.moc ../src/daemon/changesettingsjob.cpp ../src/daemon/dbussettingspathjob.cpp)

target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS} ${KDEPIMLIBS_AKONADI_LIBS})
add_test(${testname} ${CMAKE_CURRENT_BINARY_DIR}/${testname})
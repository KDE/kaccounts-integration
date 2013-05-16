find_package(KdepimLibs REQUIRED)

add_definitions("-DTEST_DATA=\\\"${CMAKE_CURRENT_SOURCE_DIR}/providers/\\\"")

set(testname testproviderconfig)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp ../src/providerconfig.cpp)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY})

set(testname testdbussettingsjob)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp
    ../src/daemon/akonadi/jobs/dbussettingsinterfacejob.cpp
    ../src/daemon/akonadi/jobs/abstractakonadijob.cpp fakeresource.cpp
)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS})

set(testname testfetchsettingsjob)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp
    ../src/daemon/akonadi/jobs/fetchsettingsjob.cpp
    ../src/daemon/akonadi/jobs/abstractakonadijob.cpp
    ../src/daemon/akonadi/jobs/dbussettingsinterfacejob.cpp
    fakeresource.cpp
)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS})

set(testname testchangesettingsjob)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp
    ../src/daemon/akonadi/jobs/changesettingsjob.cpp
    ../src/daemon/akonadi/jobs/abstractakonadijob.cpp
    ../src/daemon/akonadi/jobs/dbussettingsinterfacejob.cpp
    fakeresource.cpp)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS} ${KDEPIMLIBS_AKONADI_LIBS})

set(testname testenableservicejob)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname}
    ${testname}.cpp
    ../src/daemon/akonadi/jobs/abstractakonadijob.cpp
    ../src/daemon/akonadi/jobs/fetchsettingsjob.cpp
    ../src/daemon/akonadi/jobs/enableservicejob.cpp
    ../src/daemon/akonadi/jobs/changesettingsjob.cpp
    ../src/daemon/akonadi/jobs/dbussettingsinterfacejob.cpp
    fakeresource.cpp
)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS} ${KDEPIMLIBS_AKONADI_LIBS})

set(testname testakonadiaccounts)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp ../src/daemon/akonadi/akonadiaccounts.cpp)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS})

set(testname testnetattachjobs)
set(testname testcreatenetattachjob)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp
    ../src/daemon/kio/createnetattachjob.cpp
)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS} ${ACCOUNTSQT_LIBRARIES} ${KDE4_KIO_LIBS})

set(testname testremovenetattachjob)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_unit_test(${testname} ${testname}.cpp
    ../src/daemon/kio/removenetattachjob.cpp
)
target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDE4_KDECORE_LIBS} ${ACCOUNTSQT_LIBRARIES} ${KDE4_KIO_LIBS})
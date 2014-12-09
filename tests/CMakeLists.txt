include(ECMMarkAsTest)

find_package(Qt5 5.2.0 CONFIG REQUIRED Test)

add_definitions("-DTEST_DATA=\"${CMAKE_CURRENT_SOURCE_DIR}/data/\"")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(COMMON_LINK_LIBS Qt5::Test Qt5::Core Qt5::Xml Qt5::DBus KF5::CoreAddons ${ACCOUNTSQT_LIBRARIES})

set(testname testproviderconfig)
add_executable(${testname} ${testname}.cpp ../src/providerconfig.cpp)
add_test(kaccounts-${testname} ${testname})
ecm_mark_as_test(${testname})
target_link_libraries(${testname} ${COMMON_LINK_LIBS} KF5::ConfigCore)

#=========================================================================
if (KF5Akonadi_FOUND)

    set(testname testdbussettingsjob)
    add_executable(${testname} ${testname}.cpp
        ../src/daemon/akonadi/jobs/dbussettingsinterfacejob.cpp
        ../src/daemon/akonadi/jobs/abstractakonadijob.cpp fakeresource.cpp
    )
    add_test(kaccounts-${testname} ${testname})
    ecm_mark_as_test(${testname})

    target_link_libraries(${testname} ${COMMON_LINK_LIBS})

    #=========================================================================

    set(testname testfetchsettingsjob)
    add_executable(${testname} ${testname}.cpp
        ../src/daemon/akonadi/jobs/fetchsettingsjob.cpp
        ../src/daemon/akonadi/jobs/abstractakonadijob.cpp
        ../src/daemon/akonadi/jobs/dbussettingsinterfacejob.cpp
        fakeresource.cpp
    )
    add_test(kaccounts-${testname} ${testname})
    ecm_mark_as_test(${testname})
    target_link_libraries(${testname} ${COMMON_LINK_LIBS})

    #=========================================================================

    set(testname testchangesettingsjob)
    add_executable(${testname} ${testname}.cpp
        ../src/daemon/akonadi/jobs/changesettingsjob.cpp
        ../src/daemon/akonadi/jobs/abstractakonadijob.cpp
        ../src/daemon/akonadi/jobs/dbussettingsinterfacejob.cpp
        fakeresource.cpp)
    add_test(kaccounts-${testname} ${testname})
    ecm_mark_as_test(${testname})
    target_link_libraries(${testname} ${COMMON_LINK_LIBS})

    #=========================================================================

    set(testname testenableservicejob)
    add_executable(${testname}
        ${testname}.cpp
        ../src/daemon/akonadi/jobs/abstractakonadijob.cpp
        ../src/daemon/akonadi/jobs/fetchsettingsjob.cpp
        ../src/daemon/akonadi/jobs/enableservicejob.cpp
        ../src/daemon/akonadi/jobs/changesettingsjob.cpp
        ../src/daemon/akonadi/jobs/dbussettingsinterfacejob.cpp
        fakeresource.cpp
    )
    add_test(kaccounts-${testname} ${testname})
    ecm_mark_as_test(${testname})
    target_link_libraries(${testname} ${COMMON_LINK_LIBS})

    #=========================================================================

    set(testname testakonadiaccounts)
    add_executable(${testname} ${testname}.cpp ../src/daemon/akonadi/akonadiaccounts.cpp)
    ecm_mark_as_test(${testname})
    target_link_libraries(${testname} ${COMMON_LINK_LIBS} KF5::ConfigCore)

endif()
#=========================================================================

set(testname testnetattachjobs)
add_executable(${testname} ${testname}.cpp
    ../src/daemon/kio/createnetattachjob.cpp
    ../src/daemon/kio/removenetattachjob.cpp
)
add_test(kaccounts-${testname} ${testname})
ecm_mark_as_test(${testname})
target_link_libraries(${testname} ${COMMON_LINK_LIBS}  KF5::ConfigCore KF5::KIOCore Qt5::Widgets KF5::Wallet)

include(ECMMarkAsTest)

find_package(Qt5 5.2.0 CONFIG REQUIRED Test)

add_definitions("-DTEST_DATA=\"${CMAKE_CURRENT_SOURCE_DIR}/data/\"")
include_directories(${CMAKE_CURRENT_BINARY_DIR} SYSTEM ${ACCOUNTSQT_INCLUDE_DIRS} ${SIGNONQT_INCLUDE_DIRS})
set(COMMON_LINK_LIBS Qt5::Test Qt5::Core Qt5::Xml Qt5::DBus KF5::CoreAddons ${ACCOUNTSQT_LIBRARIES})

set(testname testnetattachjobs)
add_executable(${testname} ${testname}.cpp
    ../createnetattachjob.cpp
    ../removenetattachjob.cpp
)
add_test(kaccounts-${testname} ${testname})
ecm_mark_as_test(${testname})
target_link_libraries(${testname} ${COMMON_LINK_LIBS}  KF5::ConfigCore KF5::KIOCore Qt5::Widgets KF5::Wallet)

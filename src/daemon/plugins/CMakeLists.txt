include_directories(${CMAKE_SOURCE_DIR}/src/lib)

if (KF5Contacts_FOUND AND KF5GAPI_FOUND AND Qt5WebKitWidgets_FOUND)
    add_subdirectory(google-contacts-sync)
endif()

if (KF5Contacts_FOUND)
    add_subdirectory(carddav)
endif()

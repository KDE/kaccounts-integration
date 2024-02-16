add_definitions(-DTRANSLATION_DOMAIN="kaccounts-integration")

add_subdirectory(lib)
add_subdirectory(declarative)

if(NOT KF6_COMPAT_BUILD)
    add_subdirectory(plugins)
    add_subdirectory(kcm)
    add_subdirectory(kded)
endif()

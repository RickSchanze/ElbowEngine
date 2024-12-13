project(Engine)

add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(ThirdParty)
add_subdirectory(Resource)

if (BUILD_TEST)
    enable_testing()
    add_subdirectory(Test)
endif ()

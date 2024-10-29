project(Engine)

add_subdirectory(Core)
add_subdirectory(Function)
add_subdirectory(Platform)
add_subdirectory(Tool)
add_subdirectory(Resource)
add_subdirectory(ThirdParty)

if (BUILD_TEST)
    enable_testing()
    add_subdirectory(Test)
endif ()

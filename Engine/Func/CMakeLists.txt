set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(Func)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("/wd4244")
endif ()
add_library(${PROJECT_NAME}
        Logcat.h
        World/Actor.cpp
        World/Actor.h
        World/ITick.cpp
        World/ITick.h
        World/WorldClock.cpp
        World/WorldClock.h
        Render/RenderPipeline.cpp
        Render/RenderPipeline.h
        Render/RenderContext.cpp
        Render/RenderContext.h
        Render/Misc.cpp
        Render/Misc.h
        Render/FixedBasicTestRenderPipeline.cpp
        Render/FixedBasicTestRenderPipeline.h
        World/Transform.cpp
        World/Transform.h
        World/Component.cpp
        World/Component.h
        World/SceneComponent.cpp
        World/SceneComponent.h
        Camera/CameraComponent.cpp
        Camera/CameraComponent.h
        Camera/Camera.cpp
        Camera/Camera.h
        Render/GlobalObjectInstancedDataBuffer.cpp
        Render/GlobalObjectInstancedDataBuffer.h
        Render/RenderTexture.cpp
        Render/RenderTexture.h
        Camera/ACameraHolder.cpp
        Camera/ACameraHolder.h
        UI/Widget/Text.cpp
        UI/Widget/Text.h
        UI/Widget/Widget.cpp
        UI/Widget/Widget.h
        UI/Overlay.cpp
        UI/Overlay.h
)

add_library(Engine::Func ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/..)

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core -WHOLEARCHIVE:$<TARGET_FILE:Engine::Core>)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Platform -WHOLEARCHIVE:$<TARGET_FILE:Engine::Platform>)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Resource -WHOLEARCHIVE:$<TARGET_FILE:Engine::Resource>)

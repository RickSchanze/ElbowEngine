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
)

add_library(Engine::Func ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/..)

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core -WHOLEARCHIVE:$<TARGET_FILE:Engine::Core>)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Platform -WHOLEARCHIVE:$<TARGET_FILE:Engine::Platform>)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Resource -WHOLEARCHIVE:$<TARGET_FILE:Engine::Resource>)

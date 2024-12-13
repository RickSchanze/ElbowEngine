cmake_minimum_required(VERSION 3.27)
set(VCPKG_TARGET_TRIPLET x64-windows)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/Software/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(ElbowEngine)

set(CMAKE_CXX_STANDARD 23)
set(ELBOWENGINE_MAKE_WORK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(${CMAKE_SOURCE_DIR}/Generated)

set(ENABLE_TEST OFF CACHE BOOL "Enable test")

add_subdirectory(Engine)

add_executable(ElbowEngine main.cpp
)

# WHOLEARCHIEVE确保RTTR注册一定会先于main执行
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core -WHOLEARCHIVE:$<TARGET_FILE:Engine::Core>)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Platform -WHOLEARCHIVE:$<TARGET_FILE:Engine::Platform>)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Resource -WHOLEARCHIVE:$<TARGET_FILE:Engine::Resource>)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

include_directories(Engine/ThirdParty)
include_directories(Generated)
include_directories("C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.20348.0\\um")

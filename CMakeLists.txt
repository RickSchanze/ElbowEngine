cmake_minimum_required(VERSION 3.27)
set(VCPKG_TARGET_TRIPLET x64-windows)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/Vcpkg/scripts/buildsystems/vcpkg.cmake")

project(ElbowEngine)

option(ENABLE_TEST "Enable test" ON)
option(USE_IMGUI "Enable imgui" ON)
option(ENABLE_PROFILING "Enable profiling" OFF)
option(WITH_EDITOR "Enable editor" ON)

if (ENABLE_TEST)
    add_compile_definitions(ENABLE_TEST)
endif ()

if (USE_IMGUI)
    add_compile_definitions(USE_IMGUI)
endif ()

if (ENABLE_PROFILING)
    add_compile_definitions(ENABLE_PROFILING)
endif ()

if (WITH_EDITOR)
    add_compile_definitions(WITH_EDITOR)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(REFLECTION_EXE ${CMAKE_SOURCE_DIR}/CodeGenerator/bin/CodeGenerator.exe)
set(PYTHON_EXE "C:/Users/Echo/AppData/Local/Programs/Python/Python312/python.exe")

execute_process(COMMAND ${PYTHON_EXE} ${CMAKE_SOURCE_DIR}/CodeGen/main.py --includes ${CMAKE_BINARY_DIR}/vcpkg_installed/x86-windows/include)

set(ELBOWENGINE_MAKE_WORK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
    add_compile_options("/source-charset:utf-8" "/execution-charset:utf-8")
    add_compile_options("/wd4996")
    add_compile_options("/wd4244")
    add_compile_options("/wd4312")
endif ()

include_directories(${CMAKE_SOURCE_DIR}/Generated)

set(ENABLE_TEST OFF CACHE BOOL "Enable test")

add_subdirectory(Engine)

add_executable(ElbowEngine main.cpp)





# WHOLEARCHIEVE确保RTTR注册一定会先于main执行
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Function -WHOLEARCHIVE:$<TARGET_FILE:Engine::Function>)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Tool -WHOLEARCHIVE:$<TARGET_FILE:Engine::Tool>)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Platform -WHOLEARCHIVE:$<TARGET_FILE:Engine::Platform>)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

include_directories(Engine/ThirdParty)
include_directories(Generated)
include_directories("C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.20348.0\\um")

# 设置第三方库include和链接路径
# slang
set(SLANG_INCLUDE "D:\\Library\\slang\\include" CACHE STRING "库Slang的包含路径, 此库用于Shader编译")
set(SLANG_LIB "D:\\Library\\slang\\lib" CACHE STRING "库Slang的链接路径, 此库用于Shader编译")

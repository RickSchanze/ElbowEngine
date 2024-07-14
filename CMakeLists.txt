cmake_minimum_required(VERSION 3.27)
set(VCPKG_TARGET_TRIPLET x64-windows)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/Vcpkg/scripts/buildsystems/vcpkg.cmake")

project(ElbowEngine)
set(CMAKE_CXX_STANDARD 20)
set(REFLECTION_EXE ${CMAKE_SOURCE_DIR}/CodeGenerator/bin/CodeGenerator.exe)
set(PYTHON_EXE "C:/Users/Echo/AppData/Local/Programs/Python/Python312/python.exe")
execute_process(COMMAND ${PYTHON_EXE} ${CMAKE_SOURCE_DIR}/CodeGenerator/CodeGenerator.py
        --reflection-tool-path=${REFLECTION_EXE}
        --output-files-path=${CMAKE_SOURCE_DIR}/Generated
        --process-files-path ${CMAKE_SOURCE_DIR}/Engine
        --include-path ${CMAKE_SOURCE_DIR}/Engine ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/include ${CMAKE_SOURCE_DIR}/Generated
        ${CMAKE_SOURCE_DIR}/Engine/Core ${CMAKE_SOURCE_DIR}/Engine/Function ${CMAKE_SOURCE_DIR}/Engine/Platform ${CMAKE_SOURCE_DIR}/Engine/Tool
        --config-path ${CMAKE_SOURCE_DIR}/CodeGenerator/Config.json
)

if (MSVC)
    add_compile_options("/source-charset:utf-8" "/execution-charset:utf-8")
    add_compile_options("/wd4996")
    add_compile_options("/wd4244")
    add_compile_options("/wd4312")
endif ()

include_directories(${CMAKE_SOURCE_DIR}/Generated)
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

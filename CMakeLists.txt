cmake_minimum_required(VERSION 3.27)
project(ElbowEngine)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options("/source-charset:utf-8" "/execution-charset:utf-8")
    add_compile_options("/wd4996")
    add_compile_options("/wd4244")
    add_compile_options("/wd4312")
endif ()

add_subdirectory(Engine)

add_executable(ElbowEngine main.cpp)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(ElbowEngine PRIVATE RTTR::Core)

cmake_minimum_required(VERSION 3.30)
set(VULKAN_PATH "C:/VulkanSDK/1.3.296.0" CACHE PATH "Vulkan SDK path")
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/Software/vcpkg/scripts/buildsystems/vcpkg.cmake")

# 设置使用 Ninja 和 Clang
set(CMAKE_GENERATOR "Ninja")
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(ElbowEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wno-inconsistent-missing-override)
add_subdirectory(Engine)
add_executable(ElbowEngine main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Platform)
target_link_libraries(${PROJECT_NAME} PRIVATE Resource)
target_link_libraries(${PROJECT_NAME} PRIVATE Func)

set(ENV{VULKAN_SDK} ${VULKAN_PATH})
find_package(Vulkan REQUIRED)

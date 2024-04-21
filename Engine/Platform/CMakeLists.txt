project(Platform)

set(CMAKE_CXX_STANDARD 20)

set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.280.0")

if (MSVC)
    add_compile_options("/source-charset:utf-8" "/execution-charset:utf-8")
    add_compile_options("/wd4996")
    add_compile_options("/wd4244")
    add_compile_options("/wd4312")
endif ()


add_library(Platform
        RHI/Vulkan/Exception.h
        RHI/Vulkan/Exception.cpp
        RHI/Vulkan/Instance.cpp
        RHI/Vulkan/Instance.h
        RHI/Vulkan/ValidationLayer.h
        RHI/Vulkan/ValidationLayer.cpp
        RHI/Vulkan/VulkanCommon.h
        RHI/Vulkan/Interface/IRHIResource.h
        RHI/Vulkan/Application.h
        RHI/Vulkan/Application.cpp
        RHI/Vulkan/PhysicalDevice.cpp
        RHI/Vulkan/PhysicalDevice.h
        Window/GLFWWindow.h
        Window/GLFWWindow.cpp
        Window/WindowCommon.h
        RHI/Vulkan/LogicalDevice.cpp
        RHI/Vulkan/LogicalDevice.h
        RHI/Vulkan/VulkanQueue.cpp
        RHI/Vulkan/VulkanQueue.h
)

add_library(Engine::Platform ALIAS Platform)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

target_include_directories(${PROJECT_NAME} PUBLIC "C:/VulkanSDK/1.3.280.0/Include")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

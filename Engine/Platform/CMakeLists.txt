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
        RHI/Vulkan/Render/LogicalDevice.cpp
        RHI/Vulkan/Render/LogicalDevice.h
        RHI/Vulkan/Render/SwapChain.cpp
        RHI/Vulkan/Render/SwapChain.h
        RHI/Vulkan/VulkanContext.h
        RHI/Vulkan/VulkanContext.cpp
        RHI/Vulkan/Resource/Image.cpp
        RHI/Vulkan/Resource/Image.h
        RHI/Vulkan/Render/RenderPass.cpp
        RHI/Vulkan/Render/RenderPass.h
        RHI/Vulkan/VulkanStringify.cpp
        RHI/Vulkan/VulkanStringify.h
        RHI/Vulkan/Resource/ImageView.cpp
        RHI/Vulkan/Resource/ImageView.h
        RHI/Vulkan/Render/Shader.cpp
        RHI/Vulkan/Render/Shader.h
        RHI/Vulkan/Render/GraphicsPipeline.cpp
        RHI/Vulkan/Render/GraphicsPipeline.h
        RHI/Vulkan/Render/ShaderProgram.cpp
        RHI/Vulkan/Render/ShaderProgram.h
        RHI/Vulkan/Render/CommandPool.cpp
        RHI/Vulkan/Render/CommandPool.h
        RHI/Vulkan/Render/CommandList.h
        RHI/Vulkan/Render/CommandList.cpp
        RHI/Vulkan/Render/Framebuffer.h
        RHI/Vulkan/Render/Framebuffer.cpp
        RHI/Vulkan/Interface/IGraphicsPipeline.h
        RHI/Vulkan/Resource/VulkanModel.h
        RHI/Vulkan/Resource/VulkanModel.cpp
)

if (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_compile_definitions(Platform PRIVATE "RELWITHDEBINFO")
endif ()

add_library(Engine::Platform ALIAS Platform)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core Engine::Function Engine::Resource)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(spirv_cross_reflect CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spirv-cross-reflect)

find_package(spirv_cross_core CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spirv-cross-core)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME}  PRIVATE imgui::imgui)

target_include_directories(${PROJECT_NAME} PUBLIC "C:/VulkanSDK/1.3.280.0/Include")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

project(Platform)

set(CMAKE_CXX_STANDARD 20)

set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.280.0")
set(SPIRV_CROSS_LIB "C:/VulkanSDK/1.3.280.0/Lib/spirv-cross-core.lib")
set(SPIRV_REFLECT_LIB "C:/VulkanSDK/1.3.280.0/Lib/spirv-cross-reflect.lib")

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
        RHI/Vulkan/VulkanQueue.cpp
        RHI/Vulkan/VulkanQueue.h
        RHI/Vulkan/Render/SwapChain.cpp
        RHI/Vulkan/Render/SwapChain.h
        RHI/Vulkan/Render/VulkanRenderer.cpp
        RHI/Vulkan/Render/VulkanRenderer.h
        RHI/Vulkan/Render/Image.cpp
        RHI/Vulkan/Render/Image.h
        RHI/Vulkan/Render/RenderPass.cpp
        RHI/Vulkan/Render/RenderPass.h
        RHI/Vulkan/Interface/IRenderPassProducer.h
        RHI/Vulkan/VulkanStringify.cpp
        RHI/Vulkan/VulkanStringify.h
        RHI/Vulkan/Render/ImageView.cpp
        RHI/Vulkan/Render/ImageView.h
        RHI/Vulkan/Render/Shader.cpp
        RHI/Vulkan/Render/Shader.h
        RHI/Vulkan/Render/GraphicsPipeline.cpp
        RHI/Vulkan/Render/GraphicsPipeline.h
        RHI/Vulkan/Render/ShaderProgram.cpp
        RHI/Vulkan/Render/ShaderProgram.h
        RHI/Vulkan/Render/CommandProducer.cpp
        RHI/Vulkan/Render/CommandProducer.h
)

if (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_compile_definitions(Platform PRIVATE "RELWITHDEBINFO")
endif ()

add_library(Engine::Platform ALIAS Platform)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SPIRV_CROSS_LIB})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SPIRV_REFLECT_LIB})

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core Engine::Function)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

target_include_directories(${PROJECT_NAME} PUBLIC "C:/VulkanSDK/1.3.280.0/Include")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

find_package(unofficial-spirv-reflect CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::spirv-reflect)

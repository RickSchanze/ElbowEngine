project(Platform)

set(CMAKE_CXX_STANDARD 20)

set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.280.0")

add_library(Platform
        RHI/Vulkan/Core.h
        RHI/Vulkan/Core.cpp
        RHI/Vulkan/Exception.h
        RHI/Vulkan/Exception.cpp
)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

target_include_directories(${PROJECT_NAME} PUBLIC "C:/VulkanSDK/1.3.280.0/Include")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

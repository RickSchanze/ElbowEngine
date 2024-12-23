project(Platform)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Zc:preprocessor")
endif ()


add_library(Platform
        PlatformEvents.h
        FileSystem/Path.cpp
        FileSystem/Path.h
        FileSystem/Folder.cpp
        FileSystem/Folder.h
        FileSystem/File.cpp
        FileSystem/File.h
        PlatformLogcat.h
        RHI/GfxContext.cpp
        RHI/GfxContext.h
        RHI/Vulkan/GfxContext_Vulkan.cpp
        RHI/Vulkan/GfxContext_Vulkan.h
        Config/PlatformConfig.cpp
        Config/PlatformConfig.h
        Window/Window.cpp
        Window/Window.h
        Window/Window_GLFW.cpp
        Window/Window_GLFW.h
        PlatformDefines.h
        Window/WindowManager.cpp
        Window/WindowManager.h
        RHI/Surface.cpp
        RHI/Surface.h
        RHI/IResource.cpp
        RHI/IResource.h
        RHI/Enums.cpp
        RHI/Enums.h
        RHI/Vulkan/Enums_Vulkan.cpp
        RHI/Vulkan/Enums_Vulkan.h
        RHI/Image.cpp
        RHI/Image.h
        RHI/ImageView.cpp
        RHI/ImageView.h
        RHI/Vulkan/ImageView_Vulkan.cpp
        RHI/Vulkan/ImageView_Vulkan.h
        RHI/Vulkan/Image_Vulkan.cpp
        RHI/Vulkan/Image_Vulkan.h
        RHI/Buffer.cpp
        RHI/Buffer.h
        RHI/Vulkan/Buffer_Vulkan.cpp
        RHI/Vulkan/Buffer_Vulkan.h
        RHI/CommandBuffer.cpp
        RHI/CommandBuffer.h
        RHI/Commands.h
        RHI/Vulkan/CommandBuffer_Vulkan.cpp
        RHI/Vulkan/CommandBuffer_Vulkan.h
        RHI/SyncPrimitives.h
        RHI/Vulkan/SyncPrimitives_Vulkan.cpp
        RHI/Vulkan/SyncPrimitives_Vulkan.h
        RHI/GfxCommandHelper.cpp
        RHI/GfxCommandHelper.h
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    target_compile_definitions(Platform PRIVATE "RELWITHDEBINFO")
endif ()

add_library(Engine::Platform ALIAS Platform)

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(${PROJECT_NAME} PRIVATE IconFontCppHeaders)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/..)

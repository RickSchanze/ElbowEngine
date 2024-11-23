project(Platform)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
#    add_compile_options("/source-charset:utf-8" "/execution-charset:utf-8")
    add_compile_options("/wd4996")
    add_compile_options("/wd4244")
    add_compile_options("/wd4312")
    add_compile_options("/permissive-")
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
        RHI/IRHIResource.cpp
        RHI/IRHIResource.h
        RHI/RHIEnums.cpp
        RHI/RHIEnums.h
        RHI/Vulkan/RHIEnums_Vulkan.cpp
        RHI/Vulkan/RHIEnums_Vulkan.h
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    target_compile_definitions(Platform PRIVATE "RELWITHDEBINFO")
endif ()

# SLANG
target_include_directories(${PROJECT_NAME} PRIVATE ${SLANG_INCLUDE})
target_link_directories(${PROJECT_NAME} PRIVATE ${SLANG_LIB})

add_library(Engine::Platform ALIAS Platform)

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(spirv_cross_reflect CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spirv-cross-reflect)

find_package(spirv_cross_core CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spirv-cross-core)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(${PROJECT_NAME} PRIVATE IconFontCppHeaders)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/..)

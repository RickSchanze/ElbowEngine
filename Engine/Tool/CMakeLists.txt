project(Tool)

add_library(${PROJECT_NAME}
        EngineApplication.h
        EngineApplication.cpp
        ToolCommon.h
        UI/Window/WindowBase.cpp
        UI/Window/WindowBase.h
        UI/Window/WindowManager.cpp
        UI/Window/WindowManager.h
        UI/Window/DebugWindow.cpp
        UI/Window/DebugWindow.h
        UI/Window/OutlineWindow.cpp
        UI/Window/OutlineWindow.h
        UI/Window/DetailWindow.cpp
        UI/Window/DetailWindow.h
        UI/Drawer/PropertyDrawer.cpp
        UI/Drawer/PropertyDrawer.h
)

add_library(Engine::Tool ALIAS ${PROJECT_NAME})

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Engine::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Engine::Platform)
target_link_libraries(${PROJECT_NAME} PUBLIC Engine::Function)

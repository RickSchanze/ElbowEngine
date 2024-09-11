project(Tool)

set(WINDOW_FILE
        Editor/Window/DebugWindow.h
        Editor/Window/DebugWindow.cpp
        Editor/Window/OutlineWindow.cpp
        Editor/Window/OutlineWindow.h
        Editor/Window/DetailWindow.cpp
        Editor/Window/DetailWindow.h
        Editor/Window/ImGuiDemoWindow.cpp
        Editor/Window/ImGuiDemoWindow.h
        Editor/Window/ViewportWindow.h
        Editor/Window/ViewportWindow.cpp
        Editor/Window/SceneViewportWindow.h
        Editor/Window/SceneViewportWindow.cpp
        Editor/Window/LightSettingWindow.h
        Editor/Window/LightSettingWindow.cpp
        Editor/Window/ConsoleWindow.h
        Editor/Window/ConsoleWindow.cpp
)

set(WIDGET_FILE
        Editor/Widgets/WidgetBase.h
        Editor/Widgets/WidgetBase.cpp
        Editor/Widgets/Button.h
        Editor/Widgets/Button.cpp
        Editor/Widgets/ToggleButton.h
        Editor/Widgets/ToggleButton.cpp
)

add_library(${PROJECT_NAME}
        EngineApplication.h
        EngineApplication.cpp
        ToolCommon.h
        Editor/Window/WindowBase.cpp
        Editor/Window/WindowBase.h
        Editor/Window/WindowManager.cpp
        Editor/Window/WindowManager.h
        ${WINDOW_FILE}
        ${WIDGET_FILE}
        Editor/Drawer/PropertyDrawer.cpp
        Editor/Drawer/PropertyDrawer.h
        EditorStyle/ImGuiStyle.h
        EditorStyle/ImGuiStyle.cpp
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
target_link_libraries(${PROJECT_NAME} PUBLIC Engine::Resource)
target_link_libraries(${PROJECT_NAME} PUBLIC IconFontCppHeaders)

target_include_directories(${PROJECT_NAME} PRIVATE ${ELBOWENGINE_MAKE_WORK_DIR}/Generated/Engine/Tool)

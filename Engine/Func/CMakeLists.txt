project(Func)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE FUNC_HEADER_FILES *.hpp)
file(GLOB_RECURSE FUNC_CPP_FILES *.cpp)

add_library(Func
        ${FUNC_CPP_FILES} ${FUNC_HEADER_FILES}
        UI/ImGuiWindow.cpp
        UI/ImGuiWindow.hpp
        UI/Window.cpp
        UI/Window.hpp
        UI/UIManager.cpp
        UI/UIManager.hpp
        UI/ImGuiDemoWindow.cpp
        UI/ImGuiDemoWindow.hpp
        UI/GlobalDockingWindow.cpp
        UI/GlobalDockingWindow.hpp
        UI/ViewportWindow.cpp
        UI/ViewportWindow.hpp
        UI/LayoutUtility.cpp
        UI/LayoutUtility.hpp
        UI/ImGuiDrawer.cpp
        UI/ImGuiDrawer.hpp
        UI/InspectorWindow.cpp
        UI/InspectorWindow.hpp
        World/Scene/Scene.cpp
        World/Scene/Scene.hpp
        UI/DetailWindow.cpp
        UI/DetailWindow.hpp
)

target_link_libraries(Func PRIVATE Core)
target_link_libraries(Func PRIVATE Platform)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

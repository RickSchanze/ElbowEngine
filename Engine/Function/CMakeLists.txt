project(Function)

add_library(${PROJECT_NAME}
        Component/Component.cpp
        Component/Component.h
        GameObject/GameObject.cpp
        GameObject/GameObject.h
        FunctionCommon.h
        Input/Input.cpp
        Input/Input.h
        Component/Camera.cpp
        Component/Camera.h
        Component/Transform.cpp
        Component/Transform.h
        Component/Mesh/Mesh.h
        Component/Mesh/Mesh.cpp
        Component/Mesh/StaticMesh.h
        Component/Mesh/StaticMesh.cpp
        Component/Interface/IDetailGUIDrawer.h
        Component/Interface/IDetailGUIDrawer.cpp
        Component/Light/Light.h
        Component/Light/Light.cpp
        Component/Script/SpaceCircle.h
        Component/Script/SpaceCircle.cpp
        Render/RenderContext.h
        Render/RenderContext.cpp
        Render/LiteForwardRenderPipeline.h
        Render/LiteForwardRenderPipeline.cpp
        Render/RenderPipeline.h
        Render/RenderPipeline.cpp
        Render/Event.h
        Render/Material.h
        Render/Material.cpp
        Render/Shaders/StandardForwardShader.h
        Render/Shaders/StandardForwardShader.cpp
        Render/RenderPasses/PointLightShadowPass.h
        Render/RenderPasses/PointLightShadowPass.cpp
        Render/RenderPasses/SimpleObjectShadingPass.h
        Render/RenderPasses/SimpleObjectShadingPass.cpp
        ImGui/ImGuiHelper.h
        ImGui/ImGuiHelper.cpp
)

add_library(Engine::Function ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core Engine::Resource Engine::Platform)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)


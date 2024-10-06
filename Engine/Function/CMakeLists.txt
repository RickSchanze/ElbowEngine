project(Function)

set(SHADER_FILE
        Render/Shaders/StandardForwardShader.h
        Render/Shaders/StandardForwardShader.cpp
        Render/Shaders/PointLightShadowPassShader.h
        Render/Shaders/PointLightShadowPassShader.cpp
        Render/Shaders/SkyboxShader.h
        Render/Shaders/SkyboxShader.cpp
        Render/Shaders/SkySphereShader.h
        Render/Shaders/SkySphereShader.cpp
)

set(RENDER_PASS_FILE
        Render/RenderPasses/PointLightShadowPass.h
        Render/RenderPasses/PointLightShadowPass.cpp
        Render/RenderPasses/SimpleObjectShadingPass.h
        Render/RenderPasses/SimpleObjectShadingPass.cpp
        Render/RenderPasses/SkyboxPass.h
        Render/RenderPasses/SkyboxPass.cpp
)

set(MATERIAL_FILE
        Render/Materials/Material.h
        Render/Materials/Material.cpp
        Render/Materials/SkyboxMaterial.h
        Render/Materials/SkyboxMaterial.cpp
        Render/RenderPasses/PostImageLayoutTransitionPass.cpp
        Render/RenderPasses/PostImageLayoutTransitionPass.h
        Render/Shaders/InfiniteGridShader.cpp
        Render/Shaders/InfiniteGridShader.h
        World/TickManager.cpp
        World/TickManager.h
        World/ITickable.cpp
        World/ITickable.h
)

add_library(${PROJECT_NAME}
        Component/Component.cpp
        Component/Component.h
        GameObject/GameObject.cpp
        GameObject/GameObject.h
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
        Component/Script/Autorotation.h
        Component/Script/Autorotation.cpp
        Render/RenderContext.h
        Render/RenderContext.cpp
        Render/LiteForwardRenderPipeline.h
        Render/LiteForwardRenderPipeline.cpp
        Render/RenderPipeline.h
        Render/RenderPipeline.cpp
        Render/Event.h
        ${SHADER_FILE}
        ${RENDER_PASS_FILE}
        ${MATERIAL_FILE}
        ImGui/ImGuiHelper.h
        ImGui/ImGuiHelper.cpp
)

add_library(Engine::Function ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core Engine::Resource Engine::Platform)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

target_link_libraries(${PROJECT_NAME} PRIVATE IconFontCppHeaders)

target_include_directories(${PROJECT_NAME} PRIVATE ${ELBOWENGINE_MAKE_WORK_DIR}/Generated/Engine/Function)


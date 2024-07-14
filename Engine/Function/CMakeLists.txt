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
        Component/Mesh/Mesh.h
        Component/Mesh/Mesh.cpp
        Component/Mesh/StaticMesh.h
        Component/Mesh/StaticMesh.cpp
        Render/RenderContext.h
        Render/RenderContext.cpp
        Render/LiteForwardRenderPipeline.h
        Render/LiteForwardRenderPipeline.cpp
        Render/RenderPipeline.h
        Render/RenderPipeline.cpp
        Render/Event.h
)

add_library(Engine::Function ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core Engine::Resource Engine::Platform)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)


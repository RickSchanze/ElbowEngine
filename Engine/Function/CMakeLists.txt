project(Function)

add_library(${PROJECT_NAME}
        ../Core/Path/Exception.h
        ../Core/Path/Exception.cpp
        ../Core/Path/Path.cpp
        ../Core/Path/Path.h
        Component/Component.cpp
        Component/Component.h
        GameObject/GameObject.cpp
        GameObject/GameObject.h
        FunctionCommon.h
        Component/Transform.cpp
        Component/Transform.h
        Input/Input.cpp
        Input/Input.h
        Component/Camera.cpp
        Component/Camera.h
        Render/RenderPipeline.h
        Render/RenderPipeline.cpp
)

add_library(Engine::Function ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)


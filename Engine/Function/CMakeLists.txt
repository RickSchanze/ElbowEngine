project(Function)

add_library(${PROJECT_NAME}
        Path/Exception.h
        Path/Exception.cpp
        Path/Path.cpp
        Path/Path.h
        Component/ComponentBase.cpp
        Component/ComponentBase.h
        GameObject/GameObject.cpp
        GameObject/GameObject.h
        FunctionCommon.h
        Component/TransformComponent.cpp
        Component/TransformComponent.h
        Input/Input.cpp
        Input/Input.h
        Component/Camera.cpp
        Component/Camera.h
)

add_library(Engine::Function ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core Engine::Tool)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)


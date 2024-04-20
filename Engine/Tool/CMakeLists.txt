project(Tool)

add_library(${PROJECT_NAME}
        EngineApplication.h
        EngineApplication.cpp
        ToolCommon.h
)

add_library(Engine::Tool ALIAS ${PROJECT_NAME})

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Engine::Core Engine::Platform)

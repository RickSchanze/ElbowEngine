project(Resource)

add_library(${PROJECT_NAME}
        Texture.h
        Texture.cpp
        ResourceCommon.h
        Model.h
        Model.cpp
        Interface/IResource.h
        Interface/IRHIResourceContainer.h
)

add_library(Engine::Resource ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Stb REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Engine::Core
        Engine::Function
        Engine::Platform
)

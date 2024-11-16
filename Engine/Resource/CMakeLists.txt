project(Resource)

add_library(${PROJECT_NAME}
        Texture.h
        Texture.cpp
        Mesh.h
        Mesh.cpp
        ResourceManager.h
        ResourceManager.cpp
        Interface/IResource.h
        Interface/IRHIResourceContainer.h
        Logcat.h
        Shader.cpp
        Shader.h
        Interface/IResource.cpp
)

add_library(Engine::Resource ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

find_package(Stb REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

find_package(tinyexr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::tinyexr::tinyexr)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Engine::Core
        Engine::Platform
)

# SLANG
target_include_directories(${PROJECT_NAME} PRIVATE ${SLANG_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SLANG_LIB}/slang.lib)
message(STATUS "SLANG_LIB: ${SLANG_LIB}")


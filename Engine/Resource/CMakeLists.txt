project(Resource)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("/Zc:__cplusplus")
endif ()


add_library(Resource
        AssetType.cpp
        AssetType.h
        AssetDataBase.h
        AssetDataBase.cpp
        Logcat.h
        Project.h
        Project.cpp
        SQLHelper.cpp
        SQLHelper.h
        Assets/Mesh/MeshMeta.cpp
        Assets/Mesh/MeshMeta.h
        Assets/Mesh/Mesh.cpp
        Assets/Mesh/Mesh.h
        Assets/Asset.cpp
        Assets/Asset.h
        Assets/Shader/ShaderMeta.cpp
        Assets/Shader/ShaderMeta.h
        Assets/Shader/Shader.cpp
        Assets/Shader/Shader.h
        Assets/Shader/SlangShaderLoader.cpp
        Assets/Shader/SlangShaderLoader.h
        Config/ResourceConfig.cpp
        Config/ResourceConfig.h
        Assets/Texture/Texture2D.cpp
        Assets/Texture/Texture2D.h
        Assets/Texture/Texture2DMeta.cpp
        Assets/Texture/Texture2DMeta.h
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    target_compile_definitions(Platform PRIVATE "RELWITHDEBINFO")
endif ()

# SLANG
target_include_directories(${PROJECT_NAME} PRIVATE ${SLANG_INCLUDE})
target_link_directories(${PROJECT_NAME} PRIVATE ${SLANG_LIB})

add_library(Engine::Resource ALIAS Resource)

target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Platform)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/..)

find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME}  PRIVATE assimp::assimp)

# 查找 shader-slang 的头文件和链接库
find_path(SLANG_INCLUDE_DIR slang.h) # 查找 shader-slang 的头文件
find_library(SLANG_LIBRARY NAMES slang) # 查找 shader-slang 的库文件

# 如果找到 shader-slang，设置头文件和链接库
if (SLANG_INCLUDE_DIR AND SLANG_LIBRARY)
    message(STATUS "Found shader-slang")
    add_library(slang STATIC IMPORTED)
    set_target_properties(slang PROPERTIES
            IMPORTED_LOCATION ${SLANG_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${SLANG_INCLUDE_DIR})
else()
    message(FATAL_ERROR "shader-slang not found!")
endif()

# 链接 shader-slang 库
target_link_libraries(${PROJECT_NAME} PRIVATE slang)

project(Resource)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_compile_options("/wd4996")
    add_compile_options("/wd4244")
    add_compile_options("/wd4312")
    add_compile_options("/permissive-")
endif ()


add_library(Resource
        AssetType.cpp
        AssetType.h
        AssetDataBase.h
        AssetDataBase.cpp
        Logcat.h
        Project.h
        Project.cpp
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

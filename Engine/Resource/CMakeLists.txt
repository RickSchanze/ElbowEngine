project(Resource)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE RESOURCE_HEADER_FILES *.hpp)
file(GLOB_RECURSE RESOURCE_CPP_FILES *.cpp)

add_library(Resource
        ${RESOURCE_CPP_FILES} ${RESOURCE_HEADER_FILES}
)

target_include_directories(Resource PUBLIC ../)

target_link_libraries(${PROJECT_NAME} PRIVATE Core Platform)

find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

find_package(Stb REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

find_package(Freetype REQUIRED)
target_link_libraries(${PROJECT_NAME}  PRIVATE Freetype::Freetype) # since CMake 3.10

find_package(utf8cpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE utf8::cpp utf8cpp::utf8cpp)

find_package(tinyexr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::tinyexr::tinyexr)

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

find_package(Stb REQUIRED)
target_include_directories(${PROJECT_NAME}  PRIVATE ${Stb_INCLUDE_DIR})

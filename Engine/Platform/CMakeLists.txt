project(Platform)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE PLATFORM_HEADER_FILES *.hpp)
file(GLOB_RECURSE PLATFORM_CPP_FILES *.cpp)

get_property(GEN_INCLUDE_DIR GLOBAL PROPERTY ${PROJECT_NAME}_GEN_INCLUDE_DIR)
get_property(GEN_SOURCES GLOBAL PROPERTY ${PROJECT_NAME}_GEN_SOURCES)

add_library(Platform
        ${PLATFORM_CPP_FILES} ${PLATFORM_HEADER_FILES} ${GEN_SOURCES}
)

# 添加生成的头文件目录
target_include_directories(${PROJECT_NAME} PUBLIC
        ${GEN_INCLUDE_DIR}
)

target_include_directories(Platform PUBLIC ../)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

target_include_directories(${PROJECT_NAME} PUBLIC ${VULKAN_PATH}/Include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

target_link_libraries(${PROJECT_NAME} PRIVATE Core)
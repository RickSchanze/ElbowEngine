project(Platform)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE PLATFORM_HEADER_FILES *.hpp)
file(GLOB_RECURSE PLATFORM_CPP_FILES *.cpp)

add_library(Platform
        ${PLATFORM_CPP_FILES} ${PLATFORM_HEADER_FILES}
)

target_include_directories(Platform PUBLIC ../)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

target_include_directories(${PROJECT_NAME} PUBLIC ${VULKAN_PATH}/Include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

target_link_libraries(${PROJECT_NAME} PRIVATE Core)
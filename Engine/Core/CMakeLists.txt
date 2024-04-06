project(Core)

add_library(${PROJECT_NAME}
        Object/Object.h
        Object/Object.cpp
        Log/Logger.h
        Log/Logger.cpp
        Utils/StringUtils.h
        Utils/StringUtils.cpp
        CoreDef.h
)

add_library(Engine::Core ALIAS ${PROJECT_NAME})

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/..)


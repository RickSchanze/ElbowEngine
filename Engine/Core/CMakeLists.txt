set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(Core)

add_library(${PROJECT_NAME}
        Object/Object.h
        Object/Object.cpp
        Object/ObjectManager.h
        Object/ObjectManager.cpp
        Log/Logger.h
        Log/Logger.cpp
        Utils/StringUtils.h
        Utils/StringUtils.cpp
        Utils/ReflUtils.h
        Utils/ReflUtils.cpp
        Exception/Exception.h
        Exception/Exception.cpp
        Singleton/Singleton.h
        Serialization/Interfaces.h
        Serialization/YamlSerializer.h
        Serialization/YamlSerializer.cpp
        Serialization/YamlDeserializer.h
        Serialization/YamlDeserializer.cpp
        CoreDef.h
        CoreGlobal.h
        CoreGlobal.cpp
        CoreTypeTraits.h
        Math/MathTypes.cpp
        Math/MathTypes.h
        Event/Event.h
        Misc/Guid.cpp
        Misc/Guid.h
        Math/Math.cpp
        Math/Math.h
)

add_library(Engine::Core ALIAS ${PROJECT_NAME})

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME}  PRIVATE spdlog::spdlog_header_only)

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(cpptrace CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE cpptrace::cpptrace)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})


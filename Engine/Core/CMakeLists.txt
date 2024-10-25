set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(Core)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_library(${PROJECT_NAME}
        Object/Object.h
        Object/Object.cpp
        Object/ObjectManager.h
        Object/ObjectManager.cpp
        Log/Logger.h
        Log/Logger.cpp
        Log/LogEvent.h
        Log/LogEvent.cpp
        Log/LogRecorder.h
        Log/LogRecorder.cpp
        Utils/StringUtils.h
        Utils/StringUtils.cpp
        Utils/ReflUtils.h
        Utils/ReflUtils.cpp
        Utils/ContainerUtils.h
        Utils/ContainerUtils.cpp
        Utils/MemoryUtils.cpp
        Utils/MemoryUtils.h
        Utils/HashUtils.h
        Utils/HashUtils.cpp
        Utils/PathUtils.h
        Utils/PathUtils.cpp
        Utils/TimeUtils.h
        Utils/TimeUtils.cpp
        Exception/Exception.h
        Exception/Exception.cpp
        Singleton/Singleton.h
        CoreDef.h
        CoreGlobal.h
        CoreGlobal.cpp
        CoreEvents.h
        CoreTypeTraits.h
        Math/MathTypes.cpp
        Math/MathTypes.h
        Misc/Guid.cpp
        Misc/Guid.h
        Misc/Vertex.h
        Misc/Vertex.cpp
        Math/Math.cpp
        Math/Math.h
        Path/Path.h
        Path/Path.cpp
        Path/Exception.h
        Path/Exception.cpp
        Interface/IStringify.h
        CachedString.h
        CachedString.cpp
        Profiler/ProfileMacro.cpp
        Profiler/ProfileMacro.h
        Async/Coroutine/Task.cpp
        Async/Coroutine/Task.h
        Async/Coroutine/Promise.cpp
        Async/Coroutine/Promise.h
        Async/Coroutine/Awaiter.cpp
        Async/Coroutine/Awaiter.h
        Async/Coroutine/IExecutor.h
        Async/Coroutine/Result.cpp
        Async/Coroutine/Result.h
        Async/Coroutine/CoroutineExecutorManager.cpp
        Async/Coroutine/CoroutineExecutorManager.h
        Async/Coroutine/MainThreadExecutor.cpp
        Async/Coroutine/MainThreadExecutor.h
        Async/Coroutine/CoroutineCommon.h
        Async/Coroutine/TaskAwaiter.cpp
        Async/Coroutine/TaskAwaiter.h
        Async/Coroutine/Coroutine.h
        Event/Delegate.h
        Event/Event.h
        Serialization/Archive.h
        Serialization/YamlArchive.cpp
        Serialization/YamlArchive.h
        Serialization/ISerializer.h
        Serialization/Archive.cpp
)

add_library(Engine::Core ALIAS ${PROJECT_NAME})

find_package(rttr CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE RTTR::Core)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog_header_only)

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(cpptrace CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE cpptrace::cpptrace)

find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::Crypto)

find_package(directx-headers CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectX-Headers)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC Tracy::TracyClient)

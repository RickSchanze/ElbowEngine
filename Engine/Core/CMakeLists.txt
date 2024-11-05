set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(Core)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_library(${PROJECT_NAME}
        Object/Object.h
        Object/Object.cpp
        Log/Logger.h
        Log/Logger.cpp
        Utils/ReflUtils.h
        Utils/ReflUtils.cpp
        Utils/ContainerUtils.h
        Utils/ContainerUtils.cpp
        Utils/MemoryUtils.cpp
        Utils/MemoryUtils.h
        Utils/TimeUtils.h
        Utils/TimeUtils.cpp
        Singleton/Singleton.h
        CoreDef.h
        CoreGlobal.h
        CoreGlobal.cpp
        CoreEvents.h
        CoreTypeTraits.h
        Math/MathTypes.cpp
        Math/MathTypes.h
        Misc/Vertex.h
        Misc/Vertex.cpp
        Math/Math.cpp
        Math/Math.h
        FileSystem/Paths.cpp
        FileSystem/Paths.h
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
        Base/EString.cpp
        Base/EString.h
        FileSystem/File.cpp
        FileSystem/File.h
        FileSystem/FileSystemEnum.h
        Base/UniquePtr.h
        Log/CoreLogCategory.h
        Base/CallStackFrame.h
        Base/CallStackFrame.cpp
        Base/CoreTypeDef.h
        Log/LogEvent.h
        Log/LogRecorder.cpp
        Log/LogRecorder.h
        Object/Setting.cpp
        Object/Setting.h
        Reflection/Reflection.cpp
        Reflection/Reflection.h
        Base/Base.h
        Reflection/MetaInfoManager.cpp
        Reflection/MetaInfoManager.h
        Base/Ref.h
        Reflection/ContainerView.cpp
        Reflection/ContainerView.h
        Reflection/Any.h
        Reflection/ITypeGetter.h
        Reflection/MetaInfoMacro.h
        Core.h
)

add_library(Engine::Core ALIAS ${PROJECT_NAME})

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

find_package(ICU REQUIRED COMPONENTS uc i18n)
target_link_libraries(${PROJECT_NAME}  PRIVATE ICU::uc ICU::i18n)

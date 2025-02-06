set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(Core)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("/wd4244")
endif ()
add_library(${PROJECT_NAME}
        Object/Object.h
        Object/Object.cpp
        Log/Logger.h
        Log/Logger.cpp
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
        Math/Math.cpp
        Math/Math.h
        Profiler/ProfileMacro.cpp
        Profiler/ProfileMacro.h
        Event/Delegate.h
        Event/Event.h
        Serialization/Archive.h
        Serialization/YamlArchive.cpp
        Serialization/YamlArchive.h
        Serialization/Archive.cpp
        Base/EString.cpp
        Base/EString.h
        Base/UniquePtr.h
        Log/CoreLogCategory.h
        Base/CallStackFrame.h
        Base/CallStackFrame.cpp
        Base/CoreTypeDef.h
        Log/LogEvent.h
        Log/LogRecorder.cpp
        Log/LogRecorder.h
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
        Base/Interface.h
        Reflection/Any.cpp
        Reflection/CtorManager.cpp
        Reflection/CtorManager.h
        Utils/HashUtils.cpp
        Utils/HashUtils.h
        Config/IConfig.cpp
        Config/IConfig.h
        Config/CoreConfig.cpp
        Config/CoreConfig.h
        Config/ConfigManager.cpp
        Config/ConfigManager.h
        CoreDef.cpp
        Base/UniquePtr.cpp
        Singleton/MManager.cpp
        Singleton/MManager.h
        Reflection/ITypeGetter.cpp
        Base/TagInvoke.h
        Base/TypeTraits.h
        Async/Execution/Concept.h
        Async/Execution/Just.h
        Async/Execution/Then.h
        Async/Execution/NullReceiver.cpp
        Async/Execution/NullReceiver.h
        Async/Execution/NullScheduler.h
        Async/Execution/Repeat.h
        Async/ThreadSafeQueue.cpp
        Async/ThreadSafeQueue.h
        Async/ITask.h
        Async/ThreadCluster.cpp
        Async/ThreadCluster.h
        Async/ThreadManager.cpp
        Async/ThreadManager.h
        Async/ThreadUtils.cpp
        Async/ThreadUtils.h
        Assert.h
        Async/Execution/WhenAll.h
        Async/Execution/StartAsync.h
        Base/Exception.cpp
        Base/Exception.h
        Reflection/SharedAny.cpp
        Reflection/SharedAny.h
        Base/Optional.h
        Object/ObjectRegistry.cpp
        Object/ObjectRegistry.h
        Base/FlatMap.h
        Object/ObjectPtr.h
        Object/ObjectPtr.cpp
        Object/PersistentObject.cpp
        Object/PersistentObject.h
        Features.h
        Memory/MemoryPool.cpp
        Memory/MemoryPool.h
        Memory/MemoryManager.cpp
        Memory/MemoryManager.h
        Memory/FrameAllocator.cpp
        Memory/FrameAllocator.h
        Base/Ranges.cpp
        Base/Ranges.h
        Async/Execution/SyncGroup.cpp
        Async/Execution/SyncGroup.h
        Math/MathFunctional.cpp
        Math/MathFunctional.h
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

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/..)

target_link_libraries(${PROJECT_NAME} PUBLIC Tracy::TracyClient)

find_package(ICU REQUIRED COMPONENTS uc i18n)
target_link_libraries(${PROJECT_NAME} PRIVATE ICU::uc ICU::i18n)

find_package(xxHash CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE xxHash::xxhash)

find_package(range-v3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE range-v3::meta range-v3::concepts range-v3::range-v3)

find_package(mimalloc CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:mimalloc-static>,mimalloc-static,mimalloc>)

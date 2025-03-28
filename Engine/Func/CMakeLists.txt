project(Func)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE FUNC_HEADER_FILES *.hpp)
file(GLOB_RECURSE FUNC_CPP_FILES *.cpp)

add_library(Func
        ${FUNC_CPP_FILES} ${FUNC_HEADER_FILES}
        Render/GlobalObjectInstancedDataBuffer.cpp
        Render/GlobalObjectInstancedDataBuffer.hpp
        Render/RenderTexture.cpp
        Render/RenderTexture.hpp
        Render/RenderContext.cpp
        Render/RenderContext.hpp
        Render/RenderPipeline.cpp
        Render/RenderPipeline.hpp
        Render/Camera/Camera.cpp
        Render/Camera/Camera.hpp
        Render/Camera/CameraComponent.cpp
        Render/Camera/CameraComponent.hpp
        Render/Camera/ACameraHolder.cpp
        Render/Camera/ACameraHolder.hpp
        Render/ElbowEngineRenderPipeline.cpp
        Render/ElbowEngineRenderPipeline.hpp
)

target_link_libraries(Func PRIVATE Core)
target_link_libraries(Func PRIVATE Platform)

project(Func)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE FUNC_HEADER_FILES *.hpp)
file(GLOB_RECURSE FUNC_CPP_FILES *.cpp)

add_library(Func
        ${FUNC_CPP_FILES} ${FUNC_HEADER_FILES}
        Render/Light/LightManager.cpp
        Render/Light/LightManager.hpp
        Render/Light/PointLightComponent.cpp
        Render/Light/PointLightComponent.hpp
)

target_link_libraries(Func PRIVATE Core)
target_link_libraries(Func PRIVATE Platform)

project(Core)

add_library(${PROJECT_NAME} STATIC
        Object/Object.h
        Object/Object.cpp
        ../../CodeGenerator/Test.generated.hpp
)

add_library(Engine::Core ALIAS ${PROJECT_NAME})

find_package(rttr CONFIG REQUIRED)
target_link_libraries(Core PRIVATE RTTR::Core)


cmake_minimum_required(VERSION 3.28)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Echo/Vcpkg/scripts/buildsystems/vcpkg.cmake")
project(CodeGenerator)

if (MSVC)
    add_compile_options("/source-charset:utf-8" "/execution-charset:utf-8")
    add_compile_options("/wd4996")
    add_compile_options("/wd4244")
    add_compile_options("/wd4312")
    add_compile_options("/wd4291")
endif ()

set(CMAKE_CXX_STANDARD 20)
add_executable(CodeGenerator main.cpp ReflectedEntityFinder.h ReflectedEntityFinder.cpp)

find_package(LLVM CONFIG REQUIRED)
find_package(Clang REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
add_definitions(${LLVM_DEFINITIONS})
target_include_directories(CodeGenerator PRIVATE ${LLVM_INCLUDE_DIRS})

# Find the libraries that correspond to the LLVM components that we wish to use
llvm_map_components_to_libnames(llvm_libs Support Core)

# Link against LLVM libraries
target_link_libraries(CodeGenerator PRIVATE ${llvm_libs})

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(CodeGenerator PRIVATE JsonCpp::JsonCpp)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(CodeGenerator PRIVATE RTTR::Core)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CodeGenerator PRIVATE clangAST clangASTMatchers clangBasic clangFrontend clangTooling)

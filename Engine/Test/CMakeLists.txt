project(Test)
enable_testing()
add_executable(${PROJECT_NAME}
        Core/Reflection/TestArchiveSerialization.cpp
        Core/Reflection/TestArchiveSerialization.h
        Core/Reflection/TestMetaInfoGenerate.h
        Core/Reflection/TestMetaInfoGenerate.cpp
)
find_package(GTest CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Core)

# 自动发现测试
include(GoogleTest)
gtest_add_tests(TARGET ${PROJECT_NAME} SOURCES )

target_compile_options(${PROJECT_NAME} $<$CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>)
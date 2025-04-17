set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "[EHT] 启动代码生成任务")
set(CODE_GENERATOR "C:/Users/Echo/Documents/Projects/ElbowEngine/EHT/EHT/bin/Release/net9.0/win-x64/EHT.exe")
execute_process(
        COMMAND ${CODE_GENERATOR} --output ${CMAKE_CURRENT_BINARY_DIR} --input ${CMAKE_CURRENT_SOURCE_DIR} --include ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/include
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        RESULT_VARIABLE generator_result
)
message(STATUS "[EHT] 代码生成结束")
message(STATUS "[EHT] 返回值: " ${generator_result})

set(SUBPROJECTS Core Platform Resource Func)
# 预处理生成文件路径 ------------------------------------------------------
foreach (subproj IN LISTS SUBPROJECTS)
    # 生成的头文件目录（绝对路径）
    set(gen_include_dir "${CMAKE_CURRENT_BINARY_DIR}/Generated/Include/${subproj}")

    # 生成的源文件目录（绝对路径）
    set(gen_source_dir "${CMAKE_CURRENT_BINARY_DIR}/Generated/Source/${subproj}")

    # 验证头文件目录存在
    if (NOT EXISTS ${gen_include_dir})
        message(FATAL_ERROR "生成头文件目录缺失: ${gen_include_dir}")
    endif ()

    if (NOT EXISTS ${gen_source_dir})
        message(FATAL_ERROR "生成源文件目录缺失: ${gen_include_dir}")
    endif ()

    # 收集生成的源文件（带校验）
    file(GLOB_RECURSE gen_sources "${gen_source_dir}/*.cpp")
    if (NOT gen_sources)
        message(WARNING "未找到生成的源文件: ${gen_source_dir}/*.cpp")
    endif ()

    # 存储到全局属性
    set_property(GLOBAL PROPERTY ${subproj}_GEN_INCLUDE_DIR ${gen_include_dir})
    set_property(GLOBAL PROPERTY ${subproj}_GEN_SOURCES ${gen_sources})
endforeach ()

add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(Resource)
add_subdirectory(Func)

project(Func)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE FUNC_HEADER_FILES *.hpp)
file(GLOB_RECURSE FUNC_CPP_FILES *.cpp)

get_property(GEN_INCLUDE_DIR GLOBAL PROPERTY Func_GEN_INCLUDE_DIR)
get_property(GEN_SOURCES GLOBAL PROPERTY ${PROJECT_NAME}_GEN_SOURCES)

add_library(Func
        ${FUNC_CPP_FILES} ${FUNC_HEADER_FILES} ${GEN_SOURCES}
)

# 添加生成的头文件目录
target_include_directories(${PROJECT_NAME} PUBLIC
        ${GEN_INCLUDE_DIR}
)

target_link_libraries(Func PRIVATE Core)
target_link_libraries(Func PRIVATE Platform)
target_link_libraries(Func PRIVATE Resource)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)




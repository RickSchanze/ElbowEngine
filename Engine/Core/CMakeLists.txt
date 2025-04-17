project(Core)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE CORE_HEADER_FILES *.hpp)
file(GLOB_RECURSE CORE_CPP_FILES *.cpp)

file(GLOB_RECURSE FUNC_HEADER_FILES *.hpp)
file(GLOB_RECURSE FUNC_CPP_FILES *.cpp)

get_property(GEN_INCLUDE_DIR GLOBAL PROPERTY ${PROJECT_NAME}_GEN_INCLUDE_DIR)
get_property(GEN_SOURCES GLOBAL PROPERTY ${PROJECT_NAME}_GEN_SOURCES)

add_library(Core
        ${CORE_CPP_FILES} ${CORE_HEADER_FILES} ${GEN_SOURCES})

# 添加生成的头文件目录
target_include_directories(${PROJECT_NAME} PUBLIC
        ${GEN_INCLUDE_DIR}
)



find_package(mimalloc CONFIG REQUIRED)
target_link_libraries(Core PRIVATE $<IF:$<TARGET_EXISTS:mimalloc-static>,mimalloc-static,mimalloc>)

find_package(utf8cpp CONFIG REQUIRED)
target_link_libraries(Core PRIVATE utf8::cpp utf8cpp::utf8cpp)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(Core PRIVATE spdlog::spdlog)

find_package(Tracy CONFIG REQUIRED)
target_link_libraries(Core PUBLIC Tracy::TracyClient)

find_package(unofficial-concurrentqueue CONFIG REQUIRED)
target_link_libraries(Core PRIVATE unofficial::concurrentqueue::concurrentqueue)

find_package(xxHash CONFIG REQUIRED)
target_link_libraries(Core PRIVATE xxHash::xxhash)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)

target_include_directories(Core PUBLIC ../)
